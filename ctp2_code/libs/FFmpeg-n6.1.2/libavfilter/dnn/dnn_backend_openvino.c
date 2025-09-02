/*
 * Copyright (c) 2020
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * DNN OpenVINO backend implementation.
 */

#include "dnn_io_proc.h"
#include "libavformat/avio.h"
#include "libavutil/avassert.h"
#include "libavutil/cpu.h"
#include "libavutil/opt.h"
#include "libavutil/avstring.h"
#include "libavutil/detection_bbox.h"
#include "../internal.h"
#include "safe_queue.h"
#if HAVE_OPENVINO2
#include <openvino/c/openvino.h>
#else
#include <c_api/ie_c_api.h>
#endif
#include "dnn_backend_common.h"

typedef struct OVOptions{
    char *device_type;
    int nireq;
    uint8_t async;
    int batch_size;
    int input_resizable;
    DNNLayout layout;
    float scale;
    float mean;
} OVOptions;

typedef struct OVContext {
    const AVClass *class;
    OVOptions options;
} OVContext;

typedef struct OVModel{
    OVContext ctx;
    DNNModel *model;
#if HAVE_OPENVINO2
    ov_core_t *core;
    ov_model_t *ov_model;
    ov_compiled_model_t *compiled_model;
    ov_output_const_port_t* input_port;
    ov_preprocess_input_info_t* input_info;
    ov_output_const_port_t* output_port;
    ov_preprocess_output_info_t* output_info;
    ov_preprocess_prepostprocessor_t* preprocess;
#else
    ie_core_t *core;
    ie_network_t *network;
    ie_executable_network_t *exe_network;
    const char *all_input_names;
    const char *all_output_names;
#endif
    SafeQueue *request_queue;   // holds OVRequestItem
    Queue *task_queue;          // holds TaskItem
    Queue *lltask_queue;     // holds LastLevelTaskItem
} OVModel;

// one request for one call to openvino
typedef struct OVRequestItem {
    LastLevelTaskItem **lltasks;
    uint32_t lltask_count;
#if HAVE_OPENVINO2
    ov_infer_request_t *infer_request;
    ov_callback_t callback;
#else
    ie_complete_call_back_t callback;
    ie_infer_request_t *infer_request;
#endif
} OVRequestItem;

#define APPEND_STRING(generated_string, iterate_string)                                            \
    generated_string = generated_string ? av_asprintf("%s %s", generated_string, iterate_string) : \
                                          av_asprintf("%s", iterate_string);

#define OFFSET(x) offsetof(OVContext, x)
#define FLAGS AV_OPT_FLAG_FILTERING_PARAM
static const AVOption dnn_openvino_options[] = {
    { "device", "device to run model", OFFSET(options.device_type), AV_OPT_TYPE_STRING, { .str = "CPU" }, 0, 0, FLAGS },
    DNN_BACKEND_COMMON_OPTIONS
    { "batch_size",  "batch size per request", OFFSET(options.batch_size),  AV_OPT_TYPE_INT,    { .i64 = 1 },     1, 1000, FLAGS},
    { "input_resizable", "can input be resizable or not", OFFSET(options.input_resizable), AV_OPT_TYPE_BOOL,   { .i64 = 0 },     0, 1, FLAGS },
    { "layout", "input layout of model", OFFSET(options.layout), AV_OPT_TYPE_INT, { .i64 = DL_NONE}, DL_NONE, DL_NHWC, FLAGS, "layout" },
        { "none",  "none", 0, AV_OPT_TYPE_CONST, { .i64 = DL_NONE }, 0, 0, FLAGS, "layout"},
        { "nchw",  "nchw", 0, AV_OPT_TYPE_CONST, { .i64 = DL_NCHW }, 0, 0, FLAGS, "layout"},
        { "nhwc",  "nhwc", 0, AV_OPT_TYPE_CONST, { .i64 = DL_NHWC }, 0, 0, FLAGS, "layout"},
    { "scale", "Add scale preprocess operation. Divide each element of input by specified value.", OFFSET(options.scale), AV_OPT_TYPE_FLOAT, { .dbl = 0 }, INT_MIN, INT_MAX, FLAGS},
    { "mean",  "Add mean preprocess operation. Subtract specified value from each element of input.", OFFSET(options.mean),  AV_OPT_TYPE_FLOAT, { .dbl = 0 }, INT_MIN, INT_MAX, FLAGS},
    { NULL }
};

AVFILTER_DEFINE_CLASS(dnn_openvino);

#if HAVE_OPENVINO2
static const struct {
    ov_status_e status;
    int         av_err;
    const char *desc;
} ov2_errors[] = {
    { OK,                     0,                  "success"                },
    { GENERAL_ERROR,          AVERROR_EXTERNAL,   "general error"          },
    { NOT_IMPLEMENTED,        AVERROR(ENOSYS),    "not implemented"        },
    { NETWORK_NOT_LOADED,     AVERROR_EXTERNAL,   "network not loaded"     },
    { PARAMETER_MISMATCH,     AVERROR(EINVAL),    "parameter mismatch"     },
    { NOT_FOUND,              AVERROR_EXTERNAL,   "not found"              },
    { OUT_OF_BOUNDS,          AVERROR(EOVERFLOW), "out of bounds"          },
    { UNEXPECTED,             AVERROR_EXTERNAL,   "unexpected"             },
    { REQUEST_BUSY,           AVERROR(EBUSY),     "request busy"           },
    { RESULT_NOT_READY,       AVERROR(EBUSY),     "result not ready"       },
    { NOT_ALLOCATED,          AVERROR(ENODATA),   "not allocated"          },
    { INFER_NOT_STARTED,      AVERROR_EXTERNAL,   "infer not started"      },
    { NETWORK_NOT_READ,       AVERROR_EXTERNAL,   "network not read"       },
    { INFER_CANCELLED,        AVERROR(ECANCELED), "infer cancelled"        },
    { INVALID_C_PARAM,        AVERROR(EINVAL),    "invalid C parameter"    },
    { UNKNOWN_C_ERROR,        AVERROR_UNKNOWN,    "unknown C error"        },
    { NOT_IMPLEMENT_C_METHOD, AVERROR(ENOSYS),    "not implement C method" },
    { UNKNOW_EXCEPTION,       AVERROR_UNKNOWN,    "unknown exception"      },
};

static int ov2_map_error(ov_status_e status, const char **desc)
{
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(ov2_errors); i++) {
        if (ov2_errors[i].status == status) {
            if (desc)
                *desc = ov2_errors[i].desc;
            return ov2_errors[i].av_err;
        }
    }
    if (desc)
        *desc = "unknown error";
    return AVERROR_UNKNOWN;
}
#endif

#if HAVE_OPENVINO2
static DNNDataType precision_to_datatype(ov_element_type_e precision)
#else
static DNNDataType precision_to_datatype(precision_e precision)
#endif
{
    switch (precision)
    {
#if HAVE_OPENVINO2
    case F32:
#else
    case FP32:
#endif
        return DNN_FLOAT;
    case U8:
        return DNN_UINT8;
    default:
        av_assert0(!"not supported yet.");
        return DNN_FLOAT;
    }
}

static int get_datatype_size(DNNDataType dt)
{
    switch (dt)
    {
    case DNN_FLOAT:
        return sizeof(float);
    case DNN_UINT8:
        return sizeof(uint8_t);
    default:
        av_assert0(!"not supported yet.");
        return 1;
    }
}

static int fill_model_input_ov(OVModel *ov_model, OVRequestItem *request)
{
    DNNData input;
    LastLevelTaskItem *lltask;
    TaskItem *task;
    OVContext *ctx = &ov_model->ctx;
#if HAVE_OPENVINO2
    int64_t* dims;
    ov_status_e status;
    ov_tensor_t* tensor = NULL;
    ov_shape_t input_shape = {0};
    ov_element_type_e precision;
    void *input_data_ptr = NULL;
#else
    dimensions_t dims;
    precision_e precision;
    ie_blob_buffer_t blob_buffer;
    IEStatusCode status;
    ie_blob_t *input_blob = NULL;
#endif

    memset(&input, 0, sizeof(input));
    lltask = ff_queue_peek_front(ov_model->lltask_queue);
    av_assert0(lltask);
    task = lltask->task;

#if HAVE_OPENVINO2
    if (!ov_model_is_dynamic(ov_model->ov_model)) {
        if (ov_model->input_port) {
            ov_output_const_port_free(ov_model->input_port);
            ov_model->input_port = NULL;
        }
        status = ov_model_const_input_by_name(ov_model->ov_model, task->input_name, &ov_model->input_port);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to get input port shape.\n");
            return ov2_map_error(status, NULL);
        }
        status = ov_const_port_get_shape(ov_model->input_port, &input_shape);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to get input port shape.\n");
            return ov2_map_error(status, NULL);
        }
        dims = input_shape.dims;
        status = ov_port_get_element_type(ov_model->input_port, &precision);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to get input port data type.\n");
            ov_shape_free(&input_shape);
            return ov2_map_error(status, NULL);
        }
    } else {
        avpriv_report_missing_feature(ctx, "Do not support dynamic model.");
        return AVERROR(ENOSYS);
    }
    input.height = dims[1];
    input.width = dims[2];
    input.channels = dims[3];
    input.dt = precision_to_datatype(precision);
    input.data = av_malloc(input.height * input.width * input.channels * get_datatype_size(input.dt));
    if (!input.data) {
        ov_shape_free(&input_shape);
        return AVERROR(ENOMEM);
    }
    input_data_ptr = input.data;
#else
    status = ie_infer_request_get_blob(request->infer_request, task->input_name, &input_blob);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get input blob with name %s\n", task->input_name);
        return DNN_GENERIC_ERROR;
    }

    status |= ie_blob_get_dims(input_blob, &dims);
    status |= ie_blob_get_precision(input_blob, &precision);
    if (status != OK) {
        ie_blob_free(&input_blob);
        av_log(ctx, AV_LOG_ERROR, "Failed to get input blob dims/precision\n");
        return DNN_GENERIC_ERROR;
    }

    status = ie_blob_get_buffer(input_blob, &blob_buffer);
    if (status != OK) {
        ie_blob_free(&input_blob);
        av_log(ctx, AV_LOG_ERROR, "Failed to get input blob buffer\n");
        return DNN_GENERIC_ERROR;
    }
    input.height = dims.dims[2];
    input.width = dims.dims[3];
    input.channels = dims.dims[1];
    input.data = blob_buffer.buffer;
    input.dt = precision_to_datatype(precision);
#endif
    // all models in openvino open model zoo use BGR as input,
    // change to be an option when necessary.
    input.order = DCO_BGR;
    // We use preprocess_steps to scale input data, so disable scale and mean here.
    input.scale = 1;
    input.mean = 0;

    for (int i = 0; i < ctx->options.batch_size; ++i) {
        lltask = ff_queue_pop_front(ov_model->lltask_queue);
        if (!lltask) {
            break;
        }
        request->lltasks[i] = lltask;
        request->lltask_count = i + 1;
        task = lltask->task;
        switch (ov_model->model->func_type) {
        case DFT_PROCESS_FRAME:
            if (task->do_ioproc) {
                if (ov_model->model->frame_pre_proc != NULL) {
                    ov_model->model->frame_pre_proc(task->in_frame, &input, ov_model->model->filter_ctx);
                } else {
                    ff_proc_from_frame_to_dnn(task->in_frame, &input, ctx);
                }
            }
            break;
        case DFT_ANALYTICS_DETECT:
            ff_frame_to_dnn_detect(task->in_frame, &input, ctx);
            break;
        case DFT_ANALYTICS_CLASSIFY:
            ff_frame_to_dnn_classify(task->in_frame, &input, lltask->bbox_index, ctx);
            break;
        default:
            av_assert0(!"should not reach here");
            break;
        }
#if HAVE_OPENVINO2
        status = ov_tensor_create_from_host_ptr(precision, input_shape, input.data, &tensor);
        ov_shape_free(&input_shape);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to create tensor from host prt.\n");
            return ov2_map_error(status, NULL);
        }
        status = ov_infer_request_set_input_tensor(request->infer_request, tensor);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to Set an input tensor for the model.\n");
            return ov2_map_error(status, NULL);
        }
#endif
        input.data = (uint8_t *)input.data
                     + input.width * input.height * input.channels * get_datatype_size(input.dt);
    }
#if HAVE_OPENVINO2
    av_freep(&input_data_ptr);
#else
    ie_blob_free(&input_blob);
#endif

    return 0;
}

static void infer_completion_callback(void *args)
{
    OVRequestItem *request = args;
    LastLevelTaskItem *lltask = request->lltasks[0];
    TaskItem *task = lltask->task;
    OVModel *ov_model = task->model;
    SafeQueue *requestq = ov_model->request_queue;
    DNNData output;
    OVContext *ctx = &ov_model->ctx;
#if HAVE_OPENVINO2
    size_t* dims;
    ov_status_e status;
    ov_tensor_t *output_tensor;
    ov_shape_t output_shape = {0};
    ov_element_type_e precision;

    memset(&output, 0, sizeof(output));
    status = ov_infer_request_get_output_tensor_by_index(request->infer_request, 0, &output_tensor);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR,
               "Failed to get output tensor.");
        return;
    }

    status = ov_tensor_data(output_tensor, &output.data);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR,
               "Failed to get output data.");
        return;
    }

    status = ov_tensor_get_shape(output_tensor, &output_shape);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get output port shape.\n");
        return;
    }
    dims = output_shape.dims;

    status = ov_port_get_element_type(ov_model->output_port, &precision);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get output port data type.\n");
        ov_shape_free(&output_shape);
        return;
    }
    output.channels = dims[1];
    output.height   = dims[2];
    output.width    = dims[3];
    av_assert0(request->lltask_count <= dims[0]);
    ov_shape_free(&output_shape);
#else
    IEStatusCode status;
    dimensions_t dims;
    ie_blob_t *output_blob = NULL;
    ie_blob_buffer_t blob_buffer;
    precision_e precision;
    status = ie_infer_request_get_blob(request->infer_request, task->output_names[0], &output_blob);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR,
               "output \"%s\" may not correct, all output(s) are: \"%s\"\n",
               task->output_names[0], ov_model->all_output_names);
        return;
    }

    status = ie_blob_get_buffer(output_blob, &blob_buffer);
    if (status != OK) {
        ie_blob_free(&output_blob);
        av_log(ctx, AV_LOG_ERROR, "Failed to access output memory\n");
        return;
    }

    status |= ie_blob_get_dims(output_blob, &dims);
    status |= ie_blob_get_precision(output_blob, &precision);
    if (status != OK) {
        ie_blob_free(&output_blob);
        av_log(ctx, AV_LOG_ERROR, "Failed to get dims or precision of output\n");
        return;
    }
    output.data     = blob_buffer.buffer;
    output.channels = dims.dims[1];
    output.height   = dims.dims[2];
    output.width    = dims.dims[3];
    av_assert0(request->lltask_count <= dims.dims[0]);
#endif
    output.dt       = precision_to_datatype(precision);
    output.layout   = ctx->options.layout;
    output.scale    = ctx->options.scale;
    output.mean     = ctx->options.mean;

    av_assert0(request->lltask_count >= 1);
    for (int i = 0; i < request->lltask_count; ++i) {
        task = request->lltasks[i]->task;

        switch (ov_model->model->func_type) {
        case DFT_PROCESS_FRAME:
            if (task->do_ioproc) {
                if (ov_model->model->frame_post_proc != NULL) {
                    ov_model->model->frame_post_proc(task->out_frame, &output, ov_model->model->filter_ctx);
                } else {
                    ff_proc_from_dnn_to_frame(task->out_frame, &output, ctx);
                }
            } else {
                task->out_frame->width = output.width;
                task->out_frame->height = output.height;
            }
            break;
        case DFT_ANALYTICS_DETECT:
            if (!ov_model->model->detect_post_proc) {
                av_log(ctx, AV_LOG_ERROR, "detect filter needs to provide post proc\n");
                return;
            }
            ov_model->model->detect_post_proc(task->in_frame, &output, 1, ov_model->model->filter_ctx);
            break;
        case DFT_ANALYTICS_CLASSIFY:
            if (!ov_model->model->classify_post_proc) {
                av_log(ctx, AV_LOG_ERROR, "classify filter needs to provide post proc\n");
                return;
            }
            ov_model->model->classify_post_proc(task->in_frame, &output, request->lltasks[i]->bbox_index, ov_model->model->filter_ctx);
            break;
        default:
            av_assert0(!"should not reach here");
            break;
        }

        task->inference_done++;
        av_freep(&request->lltasks[i]);
        output.data = (uint8_t *)output.data
                      + output.width * output.height * output.channels * get_datatype_size(output.dt);
    }
#if !HAVE_OPENVINO2
    ie_blob_free(&output_blob);
#endif
    request->lltask_count = 0;
    if (ff_safe_queue_push_back(requestq, request) < 0) {
#if HAVE_OPENVINO2
        ov_infer_request_free(request->infer_request);
#else
        ie_infer_request_free(&request->infer_request);
#endif
        av_freep(&request);
        av_log(ctx, AV_LOG_ERROR, "Failed to push back request_queue.\n");
        return;
    }
}

static void dnn_free_model_ov(DNNModel **model)
{
    OVModel *ov_model;

    if (!model || !*model)
        return;

    ov_model = (*model)->model;
    while (ff_safe_queue_size(ov_model->request_queue) != 0) {
        OVRequestItem *item = ff_safe_queue_pop_front(ov_model->request_queue);
        if (item && item->infer_request) {
#if HAVE_OPENVINO2
            ov_infer_request_free(item->infer_request);
#else
            ie_infer_request_free(&item->infer_request);
#endif
        }
        av_freep(&item->lltasks);
        av_freep(&item);
    }
    ff_safe_queue_destroy(ov_model->request_queue);

    while (ff_queue_size(ov_model->lltask_queue) != 0) {
        LastLevelTaskItem *item = ff_queue_pop_front(ov_model->lltask_queue);
        av_freep(&item);
    }
    ff_queue_destroy(ov_model->lltask_queue);

    while (ff_queue_size(ov_model->task_queue) != 0) {
        TaskItem *item = ff_queue_pop_front(ov_model->task_queue);
        av_frame_free(&item->in_frame);
        av_frame_free(&item->out_frame);
        av_freep(&item);
    }
    ff_queue_destroy(ov_model->task_queue);
#if HAVE_OPENVINO2
    if (ov_model->input_port)
        ov_output_const_port_free(ov_model->input_port);
    if (ov_model->output_port)
        ov_output_const_port_free(ov_model->output_port);
    if (ov_model->preprocess)
        ov_preprocess_prepostprocessor_free(ov_model->preprocess);
    if (ov_model->compiled_model)
        ov_compiled_model_free(ov_model->compiled_model);
    if (ov_model->ov_model)
        ov_model_free(ov_model->ov_model);
    if (ov_model->core)
        ov_core_free(ov_model->core);
#else
    if (ov_model->exe_network)
        ie_exec_network_free(&ov_model->exe_network);
    if (ov_model->network)
        ie_network_free(&ov_model->network);
    if (ov_model->core)
        ie_core_free(&ov_model->core);
    av_free(ov_model->all_output_names);
    av_free(ov_model->all_input_names);
#endif
    av_opt_free(&ov_model->ctx);
    av_freep(&ov_model);
    av_freep(model);
}


static int init_model_ov(OVModel *ov_model, const char *input_name, const char *output_name)
{
    int ret = 0;
    OVContext *ctx = &ov_model->ctx;
#if HAVE_OPENVINO2
    ov_status_e status;
    ov_preprocess_input_tensor_info_t* input_tensor_info = NULL;
    ov_preprocess_output_tensor_info_t* output_tensor_info = NULL;
    ov_preprocess_input_model_info_t* input_model_info = NULL;
    ov_model_t *tmp_ov_model;
    ov_layout_t* NHWC_layout = NULL;
    ov_layout_t* NCHW_layout = NULL;
    const char* NHWC_desc = "NHWC";
    const char* NCHW_desc = "NCHW";
    const char* device = ctx->options.device_type;
#else
    IEStatusCode status;
    ie_available_devices_t a_dev;
    ie_config_t config = {NULL, NULL, NULL};
    char *all_dev_names = NULL;
#endif
    // We scale pixel by default when do frame processing.
    if (fabsf(ctx->options.scale) < 1e-6f)
        ctx->options.scale = ov_model->model->func_type == DFT_PROCESS_FRAME ? 255 : 1;
    // batch size
    if (ctx->options.batch_size <= 0) {
        ctx->options.batch_size = 1;
    }
#if HAVE_OPENVINO2
    if (ctx->options.batch_size > 1) {
        avpriv_report_missing_feature(ctx, "Do not support batch_size > 1 for now,"
                                           "change batch_size to 1.\n");
        ctx->options.batch_size = 1;
    }

    status = ov_preprocess_prepostprocessor_create(ov_model->ov_model, &ov_model->preprocess);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to create preprocess for ov_model.\n");
        ret = ov2_map_error(status, NULL);
        goto err;
    }

    status = ov_preprocess_prepostprocessor_get_input_info_by_name(ov_model->preprocess, input_name, &ov_model->input_info);
    status |= ov_preprocess_prepostprocessor_get_output_info_by_name(ov_model->preprocess, output_name, &ov_model->output_info);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get input/output info from preprocess.\n");
        ret = ov2_map_error(status, NULL);
        goto err;
    }

    status = ov_preprocess_input_info_get_tensor_info(ov_model->input_info, &input_tensor_info);
    status |= ov_preprocess_output_info_get_tensor_info(ov_model->output_info, &output_tensor_info);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get tensor info from input/output.\n");
        ret = ov2_map_error(status, NULL);
        goto err;
    }

    //set input layout
    status = ov_layout_create(NHWC_desc, &NHWC_layout);
    status |= ov_layout_create(NCHW_desc, &NCHW_layout);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to create layout for input.\n");
        ret = ov2_map_error(status, NULL);
        goto err;
    }

    status = ov_preprocess_input_tensor_info_set_layout(input_tensor_info, NHWC_layout);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to set input tensor layout\n");
        ret = ov2_map_error(status, NULL);
        goto err;
    }

    status = ov_preprocess_input_info_get_model_info(ov_model->input_info, &input_model_info);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get input model info\n");
        ret = ov2_map_error(status, NULL);
        goto err;
    }
    if (ctx->options.layout == DL_NCHW)
        status = ov_preprocess_input_model_info_set_layout(input_model_info, NCHW_layout);
    else if (ctx->options.layout == DL_NHWC)
        status = ov_preprocess_input_model_info_set_layout(input_model_info, NHWC_layout);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get set input model layout\n");
        ret = ov2_map_error(status, NULL);
        goto err;
    }

    status = ov_preprocess_input_tensor_info_set_element_type(input_tensor_info, U8);
    if (ov_model->model->func_type != DFT_PROCESS_FRAME)
        status |= ov_preprocess_output_set_element_type(output_tensor_info, F32);
    else if (fabsf(ctx->options.scale - 1) > 1e-6f || fabsf(ctx->options.mean) > 1e-6f)
        status |= ov_preprocess_output_set_element_type(output_tensor_info, F32);
    else
        status |= ov_preprocess_output_set_element_type(output_tensor_info, U8);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to set input/output element type\n");
        ret = ov2_map_error(status, NULL);
        goto err;
    }
    // set preprocess steps.
    if (fabsf(ctx->options.scale - 1) > 1e-6f || fabsf(ctx->options.mean) > 1e-6f) {
        ov_preprocess_preprocess_steps_t* input_process_steps = NULL;
        status = ov_preprocess_input_info_get_preprocess_steps(ov_model->input_info, &input_process_steps);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to get preprocess steps\n");
            ret = ov2_map_error(status, NULL);
            goto err;
        }
        status = ov_preprocess_preprocess_steps_convert_element_type(input_process_steps, F32);
        status |= ov_preprocess_preprocess_steps_mean(input_process_steps, ctx->options.mean);
        status |= ov_preprocess_preprocess_steps_scale(input_process_steps, ctx->options.scale);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to set preprocess steps\n");
            ret = ov2_map_error(status, NULL);
            goto err;
        }
        ov_preprocess_preprocess_steps_free(input_process_steps);
    }

    //update model
    if(ov_model->ov_model)
        tmp_ov_model = ov_model->ov_model;
    status = ov_preprocess_prepostprocessor_build(ov_model->preprocess, &ov_model->ov_model);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to update OV model\n");
        ret = ov2_map_error(status, NULL);
        goto err;
    }
    ov_model_free(tmp_ov_model);

    //update output_port
    if (ov_model->output_port) {
        ov_output_const_port_free(ov_model->output_port);
        ov_model->output_port = NULL;
    }
    status = ov_model_const_output_by_name(ov_model->ov_model, output_name, &ov_model->output_port);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get output port.\n");
        goto err;
    }
    //compile network
    status = ov_core_compile_model(ov_model->core, ov_model->ov_model, device, 0, &ov_model->compiled_model);
    if (status != OK) {
        ret = ov2_map_error(status, NULL);
        goto err;
    }
    ov_preprocess_input_model_info_free(input_model_info);
    ov_layout_free(NCHW_layout);
    ov_layout_free(NHWC_layout);
#else
    if (ctx->options.batch_size > 1) {
        input_shapes_t input_shapes;
        status = ie_network_get_input_shapes(ov_model->network, &input_shapes);
        if (status != OK) {
            ret = DNN_GENERIC_ERROR;
            goto err;
        }
        for (int i = 0; i < input_shapes.shape_num; i++)
            input_shapes.shapes[i].shape.dims[0] = ctx->options.batch_size;
        status = ie_network_reshape(ov_model->network, input_shapes);
        ie_network_input_shapes_free(&input_shapes);
        if (status != OK) {
            ret = DNN_GENERIC_ERROR;
            goto err;
        }
    }

    // The order of dims in the openvino is fixed and it is always NCHW for 4-D data.
    // while we pass NHWC data from FFmpeg to openvino
    status = ie_network_set_input_layout(ov_model->network, input_name, NHWC);
    if (status != OK) {
        if (status == NOT_FOUND) {
            av_log(ctx, AV_LOG_ERROR, "Could not find \"%s\" in model, failed to set input layout as NHWC, "\
                                      "all input(s) are: \"%s\"\n", input_name, ov_model->all_input_names);
        } else{
            av_log(ctx, AV_LOG_ERROR, "Failed to set layout as NHWC for input %s\n", input_name);
        }
        ret = DNN_GENERIC_ERROR;
        goto err;
    }
    status = ie_network_set_output_layout(ov_model->network, output_name, NHWC);
    if (status != OK) {
        if (status == NOT_FOUND) {
            av_log(ctx, AV_LOG_ERROR, "Could not find \"%s\" in model, failed to set output layout as NHWC, "\
                                      "all output(s) are: \"%s\"\n", output_name, ov_model->all_output_names);
        } else{
            av_log(ctx, AV_LOG_ERROR, "Failed to set layout as NHWC for output %s\n", output_name);
        }
        ret = DNN_GENERIC_ERROR;
        goto err;
    }

    // all models in openvino open model zoo use BGR with range [0.0f, 255.0f] as input,
    // we don't have a AVPixelFormat to describe it, so we'll use AV_PIX_FMT_BGR24 and
    // ask openvino to do the conversion internally.
    // the current supported SR model (frame processing) is generated from tensorflow model,
    // and its input is Y channel as float with range [0.0f, 1.0f], so do not set for this case.
    // TODO: we need to get a final clear&general solution with all backends/formats considered.
    if (ov_model->model->func_type != DFT_PROCESS_FRAME) {
        status = ie_network_set_input_precision(ov_model->network, input_name, U8);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to set input precision as U8 for %s\n", input_name);
            ret = DNN_GENERIC_ERROR;
            goto err;
        }
    }

    status = ie_core_load_network(ov_model->core, ov_model->network, ctx->options.device_type, &config, &ov_model->exe_network);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to load OpenVINO model network\n");
        status = ie_core_get_available_devices(ov_model->core, &a_dev);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to get available devices\n");
            ret = DNN_GENERIC_ERROR;
            goto err;
        }
        for (int i = 0; i < a_dev.num_devices; i++) {
            APPEND_STRING(all_dev_names, a_dev.devices[i])
        }
        av_log(ctx, AV_LOG_ERROR,"device %s may not be supported, all available devices are: \"%s\"\n",
               ctx->options.device_type, all_dev_names);
        ret = AVERROR(ENODEV);
        goto err;
    }
#endif
    // create infer_requests for async execution
    if (ctx->options.nireq <= 0) {
        // the default value is a rough estimation
        ctx->options.nireq = av_cpu_count() / 2 + 1;
    }

    ov_model->request_queue = ff_safe_queue_create();
    if (!ov_model->request_queue) {
        ret = AVERROR(ENOMEM);
        goto err;
    }

    for (int i = 0; i < ctx->options.nireq; i++) {
        OVRequestItem *item = av_mallocz(sizeof(*item));
        if (!item) {
            ret = AVERROR(ENOMEM);
            goto err;
        }

#if HAVE_OPENVINO2
        item->callback.callback_func = infer_completion_callback;
#else
        item->callback.completeCallBackFunc = infer_completion_callback;
#endif
        item->callback.args = item;
        if (ff_safe_queue_push_back(ov_model->request_queue, item) < 0) {
            av_freep(&item);
            ret = AVERROR(ENOMEM);
            goto err;
        }

#if HAVE_OPENVINO2
        status = ov_compiled_model_create_infer_request(ov_model->compiled_model, &item->infer_request);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to Creates an inference request object.\n");
            goto err;
        }
#else
        status = ie_exec_network_create_infer_request(ov_model->exe_network, &item->infer_request);
        if (status != OK) {
            ret = DNN_GENERIC_ERROR;
            goto err;
        }
#endif

        item->lltasks = av_malloc_array(ctx->options.batch_size, sizeof(*item->lltasks));
        if (!item->lltasks) {
            ret = AVERROR(ENOMEM);
            goto err;
        }
        item->lltask_count = 0;
    }

    ov_model->task_queue = ff_queue_create();
    if (!ov_model->task_queue) {
        ret = AVERROR(ENOMEM);
        goto err;
    }

    ov_model->lltask_queue = ff_queue_create();
    if (!ov_model->lltask_queue) {
        ret = AVERROR(ENOMEM);
        goto err;
    }

    return 0;

err:
#if HAVE_OPENVINO2
    if (NCHW_layout)
        ov_layout_free(NCHW_layout);
    if (NHWC_layout)
        ov_layout_free(NHWC_layout);
    if (input_model_info)
        ov_preprocess_input_model_info_free(input_model_info);
#endif
    dnn_free_model_ov(&ov_model->model);
    return ret;
}

static int execute_model_ov(OVRequestItem *request, Queue *inferenceq)
{
#if HAVE_OPENVINO2
    ov_status_e status;
#else
    IEStatusCode status;
#endif
    LastLevelTaskItem *lltask;
    int ret = 0;
    TaskItem *task;
    OVContext *ctx;
    OVModel *ov_model;

    if (ff_queue_size(inferenceq) == 0) {
#if HAVE_OPENVINO2
        ov_infer_request_free(request->infer_request);
#else
        ie_infer_request_free(&request->infer_request);
#endif
        av_freep(&request);
        return 0;
    }

    lltask = ff_queue_peek_front(inferenceq);
    task = lltask->task;
    ov_model = task->model;
    ctx = &ov_model->ctx;

    ret = fill_model_input_ov(ov_model, request);
    if (ret != 0) {
        goto err;
    }

#if HAVE_OPENVINO2
    if (task->async) {
        status = ov_infer_request_set_callback(request->infer_request, &request->callback);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to set completion callback for inference\n");
            ret = ov2_map_error(status, NULL);
            goto err;
        }

        status = ov_infer_request_start_async(request->infer_request);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to start async inference\n");
            ret = ov2_map_error(status, NULL);
            goto err;
        }
        return 0;
    } else {
        status = ov_infer_request_infer(request->infer_request);
        if (status != OK) {
            av_log(NULL, AV_LOG_ERROR, "Failed to start synchronous model inference for OV2\n");
            ret = ov2_map_error(status, NULL);
            goto err;
        }
        infer_completion_callback(request);
        return (task->inference_done == task->inference_todo) ? 0 : DNN_GENERIC_ERROR;
    }
#else
    if (task->async) {
        status = ie_infer_set_completion_callback(request->infer_request, &request->callback);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to set completion callback for inference\n");
            ret = DNN_GENERIC_ERROR;
            goto err;
        }
        status = ie_infer_request_infer_async(request->infer_request);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to start async inference\n");
            ret = DNN_GENERIC_ERROR;
            goto err;
        }
        return 0;
    } else {
        status = ie_infer_request_infer(request->infer_request);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to start synchronous model inference\n");
            ret = DNN_GENERIC_ERROR;
            goto err;
        }
        infer_completion_callback(request);
        return (task->inference_done == task->inference_todo) ? 0 : DNN_GENERIC_ERROR;
    }
#endif
err:
    if (ff_safe_queue_push_back(ov_model->request_queue, request) < 0) {
#if HAVE_OPENVINO2
        ov_infer_request_free(request->infer_request);
#else
        ie_infer_request_free(&request->infer_request);
#endif
        av_freep(&request);
    }
    return ret;
}

static int get_input_ov(void *model, DNNData *input, const char *input_name)
{
    OVModel *ov_model = model;
    OVContext *ctx = &ov_model->ctx;
    int input_resizable = ctx->options.input_resizable;

#if HAVE_OPENVINO2
    ov_shape_t input_shape = {0};
    ov_element_type_e precision;
    int64_t* dims;
    ov_status_e status;
    if (!ov_model_is_dynamic(ov_model->ov_model)) {
        status = ov_model_const_input_by_name(ov_model->ov_model, input_name, &ov_model->input_port);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to get input port shape.\n");
            return ov2_map_error(status, NULL);
        }

        status = ov_const_port_get_shape(ov_model->input_port, &input_shape);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to get input port shape.\n");
            return ov2_map_error(status, NULL);
        }
        dims = input_shape.dims;

        status = ov_port_get_element_type(ov_model->input_port, &precision);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to get input port data type.\n");
            return ov2_map_error(status, NULL);
        }
    } else {
        avpriv_report_missing_feature(ctx, "Do not support dynamic model now.");
        return AVERROR(ENOSYS);
    }

    input->channels = dims[1];
    input->height   = input_resizable ? -1 : dims[2];
    input->width    = input_resizable ? -1 : dims[3];
    input->dt       = precision_to_datatype(precision);

    return 0;
#else
    char *model_input_name = NULL;
    IEStatusCode status;
    size_t model_input_count = 0;
    dimensions_t dims;
    precision_e precision;
    status = ie_network_get_inputs_number(ov_model->network, &model_input_count);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get input count\n");
        return DNN_GENERIC_ERROR;
    }
    for (size_t i = 0; i < model_input_count; i++) {
        status = ie_network_get_input_name(ov_model->network, i, &model_input_name);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to get No.%d input's name\n", (int)i);
            return DNN_GENERIC_ERROR;
        }
        if (strcmp(model_input_name, input_name) == 0) {
            ie_network_name_free(&model_input_name);
            status |= ie_network_get_input_dims(ov_model->network, input_name, &dims);
            status |= ie_network_get_input_precision(ov_model->network, input_name, &precision);
            if (status != OK) {
                av_log(ctx, AV_LOG_ERROR, "Failed to get No.%d input's dims or precision\n", (int)i);
                return DNN_GENERIC_ERROR;
            }

            input->channels = dims.dims[1];
            input->height   = input_resizable ? -1 : dims.dims[2];
            input->width    = input_resizable ? -1 : dims.dims[3];
            input->dt       = precision_to_datatype(precision);
            return 0;
        }

        ie_network_name_free(&model_input_name);
    }

    av_log(ctx, AV_LOG_ERROR, "Could not find \"%s\" in model, all input(s) are: \"%s\"\n", input_name, ov_model->all_input_names);
    return AVERROR(EINVAL);
#endif
}

static int contain_valid_detection_bbox(AVFrame *frame)
{
    AVFrameSideData *sd;
    const AVDetectionBBoxHeader *header;
    const AVDetectionBBox *bbox;

    sd = av_frame_get_side_data(frame, AV_FRAME_DATA_DETECTION_BBOXES);
    if (!sd) { // this frame has nothing detected
        return 0;
    }

    if (!sd->size) {
        return 0;
    }

    header = (const AVDetectionBBoxHeader *)sd->data;
    if (!header->nb_bboxes) {
        return 0;
    }

    for (uint32_t i = 0; i < header->nb_bboxes; i++) {
        bbox = av_get_detection_bbox(header, i);
        if (bbox->x < 0 || bbox->w < 0 || bbox->x + bbox->w >= frame->width) {
            return 0;
        }
        if (bbox->y < 0 || bbox->h < 0 || bbox->y + bbox->h >= frame->width) {
            return 0;
        }

        if (bbox->classify_count == AV_NUM_DETECTION_BBOX_CLASSIFY) {
            return 0;
        }
    }

    return 1;
}

static int extract_lltask_from_task(DNNFunctionType func_type, TaskItem *task, Queue *lltask_queue, DNNExecBaseParams *exec_params)
{
    switch (func_type) {
    case DFT_PROCESS_FRAME:
    case DFT_ANALYTICS_DETECT:
    {
        LastLevelTaskItem *lltask = av_malloc(sizeof(*lltask));
        if (!lltask) {
            return AVERROR(ENOMEM);
        }
        task->inference_todo = 1;
        task->inference_done = 0;
        lltask->task = task;
        if (ff_queue_push_back(lltask_queue, lltask) < 0) {
            av_freep(&lltask);
            return AVERROR(ENOMEM);
        }
        return 0;
    }
    case DFT_ANALYTICS_CLASSIFY:
    {
        const AVDetectionBBoxHeader *header;
        AVFrame *frame = task->in_frame;
        AVFrameSideData *sd;
        DNNExecClassificationParams *params = (DNNExecClassificationParams *)exec_params;

        task->inference_todo = 0;
        task->inference_done = 0;

        if (!contain_valid_detection_bbox(frame)) {
            return 0;
        }

        sd = av_frame_get_side_data(frame, AV_FRAME_DATA_DETECTION_BBOXES);
        header = (const AVDetectionBBoxHeader *)sd->data;

        for (uint32_t i = 0; i < header->nb_bboxes; i++) {
            LastLevelTaskItem *lltask;
            const AVDetectionBBox *bbox = av_get_detection_bbox(header, i);

            if (params->target) {
                if (av_strncasecmp(bbox->detect_label, params->target, sizeof(bbox->detect_label)) != 0) {
                    continue;
                }
            }

            lltask = av_malloc(sizeof(*lltask));
            if (!lltask) {
                return AVERROR(ENOMEM);
            }
            task->inference_todo++;
            lltask->task = task;
            lltask->bbox_index = i;
            if (ff_queue_push_back(lltask_queue, lltask) < 0) {
                av_freep(&lltask);
                return AVERROR(ENOMEM);
            }
        }
        return 0;
    }
    default:
        av_assert0(!"should not reach here");
        return AVERROR(EINVAL);
    }
}

static int get_output_ov(void *model, const char *input_name, int input_width, int input_height,
                                   const char *output_name, int *output_width, int *output_height)
{
#if HAVE_OPENVINO2
    ov_dimension_t dims[4] = {{1, 1}, {1, 1}, {input_height, input_height}, {input_width, input_width}};
    ov_status_e status;
    ov_shape_t input_shape = {0};
    ov_partial_shape_t partial_shape;
#else
    IEStatusCode status;
    input_shapes_t input_shapes;
#endif
    int ret;
    OVModel *ov_model = model;
    OVContext *ctx = &ov_model->ctx;
    TaskItem task;
    OVRequestItem *request;
    DNNExecBaseParams exec_params = {
        .input_name     = input_name,
        .output_names   = &output_name,
        .nb_output      = 1,
        .in_frame       = NULL,
        .out_frame      = NULL,
    };

    if (ov_model->model->func_type != DFT_PROCESS_FRAME) {
        av_log(ctx, AV_LOG_ERROR, "Get output dim only when processing frame.\n");
        return AVERROR(EINVAL);
    }

#if HAVE_OPENVINO2
    if (ctx->options.input_resizable) {
        if (!ov_model_is_dynamic(ov_model->ov_model)) {
            status = ov_partial_shape_create(4, dims, &partial_shape);
            if (status != OK) {
                av_log(ctx, AV_LOG_ERROR, "Failed create partial shape.\n");
                return ov2_map_error(status, NULL);
            }
            status = ov_const_port_get_shape(ov_model->input_port, &input_shape);
            input_shape.dims[2] = input_height;
            input_shape.dims[3] = input_width;
            if (status != OK) {
                av_log(ctx, AV_LOG_ERROR, "Failed create shape for model input resize.\n");
                return ov2_map_error(status, NULL);
            }

            status = ov_shape_to_partial_shape(input_shape, &partial_shape);
            if (status != OK) {
                av_log(ctx, AV_LOG_ERROR, "Failed create partial shape for model input resize.\n");
                return ov2_map_error(status, NULL);
            }

            status = ov_model_reshape_single_input(ov_model->ov_model, partial_shape);
            if (status != OK) {
                av_log(ctx, AV_LOG_ERROR, "Failed to reszie model input.\n");
                return ov2_map_error(status, NULL);
            }
        } else {
            avpriv_report_missing_feature(ctx, "Do not support dynamic model.");
            return AVERROR(ENOTSUP);
        }
    }

    status = ov_model_const_output_by_name(ov_model->ov_model, output_name, &ov_model->output_port);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get output port.\n");
        return ov2_map_error(status, NULL);
    }
    if (!ov_model->compiled_model) {
#else
    if (ctx->options.input_resizable) {
        status = ie_network_get_input_shapes(ov_model->network, &input_shapes);
        input_shapes.shapes->shape.dims[2] = input_height;
        input_shapes.shapes->shape.dims[3] = input_width;
        status |= ie_network_reshape(ov_model->network, input_shapes);
        ie_network_input_shapes_free(&input_shapes);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to reshape input size for %s\n", input_name);
            return DNN_GENERIC_ERROR;
        }
    }
    if (!ov_model->exe_network) {
#endif
        ret = init_model_ov(ov_model, input_name, output_name);
        if (ret != 0) {
            av_log(ctx, AV_LOG_ERROR, "Failed init OpenVINO exectuable network or inference request\n");
            return ret;
        }
    }

    ret = ff_dnn_fill_gettingoutput_task(&task, &exec_params, ov_model, input_height, input_width, ctx);
    if (ret != 0) {
        goto err;
    }

    ret = extract_lltask_from_task(ov_model->model->func_type, &task, ov_model->lltask_queue, NULL);
    if (ret != 0) {
        av_log(ctx, AV_LOG_ERROR, "unable to extract inference from task.\n");
        goto err;
    }

    request = ff_safe_queue_pop_front(ov_model->request_queue);
    if (!request) {
        av_log(ctx, AV_LOG_ERROR, "unable to get infer request.\n");
        ret = AVERROR(EINVAL);
        goto err;
    }

    ret = execute_model_ov(request, ov_model->lltask_queue);
    *output_width = task.out_frame->width;
    *output_height = task.out_frame->height;
err:
    av_frame_free(&task.out_frame);
    av_frame_free(&task.in_frame);
    return ret;
}

static DNNModel *dnn_load_model_ov(const char *model_filename, DNNFunctionType func_type, const char *options, AVFilterContext *filter_ctx)
{
    DNNModel *model = NULL;
    OVModel *ov_model = NULL;
    OVContext *ctx = NULL;
#if HAVE_OPENVINO2
    ov_core_t* core = NULL;
    ov_model_t* ovmodel = NULL;
    ov_status_e status;
#else
    size_t node_count = 0;
    char *node_name = NULL;
    IEStatusCode status;
#endif

    model = av_mallocz(sizeof(DNNModel));
    if (!model){
        return NULL;
    }

    ov_model = av_mallocz(sizeof(OVModel));
    if (!ov_model) {
        av_freep(&model);
        return NULL;
    }
    model->model = ov_model;
    ov_model->model = model;
    ov_model->ctx.class = &dnn_openvino_class;
    ctx = &ov_model->ctx;

    //parse options
    av_opt_set_defaults(ctx);
    if (av_opt_set_from_string(ctx, options, NULL, "=", "&") < 0) {
        av_log(ctx, AV_LOG_ERROR, "Failed to parse options \"%s\"\n", options);
        goto err;
    }

#if HAVE_OPENVINO2
    status = ov_core_create(&core);
    if (status != OK) {
        goto err;
    }
    ov_model->core = core;

    status = ov_core_read_model(core, model_filename, NULL, &ovmodel);
    if (status != OK) {
        ov_version_t ver;
        status = ov_get_openvino_version(&ver);
        av_log(NULL, AV_LOG_ERROR, "Failed to read the network from model file %s,\n"
                                  "Please check if the model version matches the runtime OpenVINO Version:\n",
                                   model_filename);
        if (status == OK) {
            av_log(NULL, AV_LOG_ERROR, "BuildNumber: %s\n", ver.buildNumber);
        }
        ov_version_free(&ver);
        goto err;
    }
    ov_model->ov_model = ovmodel;
#else
    ov_model->all_input_names = NULL;
    ov_model->all_output_names = NULL;

    status = ie_core_create("", &ov_model->core);
    if (status != OK)
        goto err;

    status = ie_core_read_network(ov_model->core, model_filename, NULL, &ov_model->network);
    if (status != OK) {
        ie_version_t ver;
        ver = ie_c_api_version();
        av_log(ctx, AV_LOG_ERROR, "Failed to read the network from model file %s,\n"
                                  "Please check if the model version matches the runtime OpenVINO %s\n",
                                   model_filename, ver.api_version);
        ie_version_free(&ver);
        goto err;
    }

    //get all the input and output names
    status = ie_network_get_inputs_number(ov_model->network, &node_count);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get input count\n");
        goto err;
    }
    for (size_t i = 0; i < node_count; i++) {
        status = ie_network_get_input_name(ov_model->network, i, &node_name);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to get No.%d input's name\n", (int)i);
            goto err;
        }
        APPEND_STRING(ov_model->all_input_names, node_name)
        ie_network_name_free(&node_name);
    }
    status = ie_network_get_outputs_number(ov_model->network, &node_count);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get output count\n");
        goto err;
    }
    for (size_t i = 0; i < node_count; i++) {
        status = ie_network_get_output_name(ov_model->network, i, &node_name);
        if (status != OK) {
            av_log(ctx, AV_LOG_ERROR, "Failed to get No.%d output's name\n", (int)i);
            goto err;
        }
        APPEND_STRING(ov_model->all_output_names, node_name)
        ie_network_name_free(&node_name);
    }
#endif

    model->get_input = &get_input_ov;
    model->get_output = &get_output_ov;
    model->options = options;
    model->filter_ctx = filter_ctx;
    model->func_type = func_type;

    return model;

err:
    dnn_free_model_ov(&model);
    return NULL;
}

static int dnn_execute_model_ov(const DNNModel *model, DNNExecBaseParams *exec_params)
{
    OVModel *ov_model = model->model;
    OVContext *ctx = &ov_model->ctx;
    OVRequestItem *request;
    TaskItem *task;
    int ret;

    ret = ff_check_exec_params(ctx, DNN_OV, model->func_type, exec_params);
    if (ret != 0) {
        return ret;
    }

#if HAVE_OPENVINO2
    if (!ov_model->compiled_model) {
#else
    if (!ov_model->exe_network) {
#endif
        ret = init_model_ov(ov_model, exec_params->input_name, exec_params->output_names[0]);
        if (ret != 0) {
            av_log(ctx, AV_LOG_ERROR, "Failed init OpenVINO exectuable network or inference request\n");
            return ret;
        }
    }

    task = av_malloc(sizeof(*task));
    if (!task) {
        av_log(ctx, AV_LOG_ERROR, "unable to alloc memory for task item.\n");
        return AVERROR(ENOMEM);
    }

    ret = ff_dnn_fill_task(task, exec_params, ov_model, ctx->options.async, 1);
    if (ret != 0) {
        av_freep(&task);
        return ret;
    }

    if (ff_queue_push_back(ov_model->task_queue, task) < 0) {
        av_freep(&task);
        av_log(ctx, AV_LOG_ERROR, "unable to push back task_queue.\n");
        return AVERROR(ENOMEM);
    }

    ret = extract_lltask_from_task(model->func_type, task, ov_model->lltask_queue, exec_params);
    if (ret != 0) {
        av_log(ctx, AV_LOG_ERROR, "unable to extract inference from task.\n");
        return ret;
    }

    if (ctx->options.async) {
        while (ff_queue_size(ov_model->lltask_queue) >= ctx->options.batch_size) {
            request = ff_safe_queue_pop_front(ov_model->request_queue);
            if (!request) {
                av_log(ctx, AV_LOG_ERROR, "unable to get infer request.\n");
                return AVERROR(EINVAL);
            }

            ret = execute_model_ov(request, ov_model->lltask_queue);
            if (ret != 0) {
                return ret;
            }
        }

        return 0;
    }
    else {
        if (model->func_type == DFT_ANALYTICS_CLASSIFY) {
            // Classification filter has not been completely
            // tested with the sync mode. So, do not support now.
            avpriv_report_missing_feature(ctx, "classify for sync execution");
            return AVERROR(ENOSYS);
        }

        if (ctx->options.batch_size > 1) {
            avpriv_report_missing_feature(ctx, "batch mode for sync execution");
            return AVERROR(ENOSYS);
        }

        request = ff_safe_queue_pop_front(ov_model->request_queue);
        if (!request) {
            av_log(ctx, AV_LOG_ERROR, "unable to get infer request.\n");
            return AVERROR(EINVAL);
        }
        return execute_model_ov(request, ov_model->lltask_queue);
    }
}

static DNNAsyncStatusType dnn_get_result_ov(const DNNModel *model, AVFrame **in, AVFrame **out)
{
    OVModel *ov_model = model->model;
    return ff_dnn_get_result_common(ov_model->task_queue, in, out);
}

static int dnn_flush_ov(const DNNModel *model)
{
    OVModel *ov_model = model->model;
    OVContext *ctx = &ov_model->ctx;
    OVRequestItem *request;
#if HAVE_OPENVINO2
    ov_status_e status;
#else
    IEStatusCode status;
#endif
    int ret;

    if (ff_queue_size(ov_model->lltask_queue) == 0) {
        // no pending task need to flush
        return 0;
    }

    request = ff_safe_queue_pop_front(ov_model->request_queue);
    if (!request) {
        av_log(ctx, AV_LOG_ERROR, "unable to get infer request.\n");
        return AVERROR(EINVAL);
    }

    ret = fill_model_input_ov(ov_model, request);
    if (ret != 0) {
        av_log(ctx, AV_LOG_ERROR, "Failed to fill model input.\n");
        return ret;
    }
#if HAVE_OPENVINO2
    status = ov_infer_request_infer(request->infer_request);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to start sync inference for OV2\n");
        return ov2_map_error(status, NULL);
    }
#else
    status = ie_infer_set_completion_callback(request->infer_request, &request->callback);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to set completion callback for inference\n");
        return DNN_GENERIC_ERROR;
    }
    status = ie_infer_request_infer_async(request->infer_request);
    if (status != OK) {
        av_log(ctx, AV_LOG_ERROR, "Failed to start async inference\n");
        return DNN_GENERIC_ERROR;
    }
#endif

    return 0;
}

const DNNModule ff_dnn_backend_openvino = {
    .load_model     = dnn_load_model_ov,
    .execute_model  = dnn_execute_model_ov,
    .get_result     = dnn_get_result_ov,
    .flush          = dnn_flush_ov,
    .free_model     = dnn_free_model_ov,
};
