//
// The implementation of aui_SDLMovie is a stripped version of ffplay.c.
// ffplay.c is part of ffmpeg (https://github.com/FFmpeg/FFmpeg).
// The version used is 3.4 with some version 4.2 modifications.
//

#include "c3.h"
#include "aui_sdlmovie.h"

#if defined(__AUI_USE_SDL__)

#if defined(USE_SDL_FFMPEG)

extern "C" {
	#include <libavformat/avformat.h>
	#include <libavutil/time.h>
    #include <libswresample/swresample.h>
	#include "libswscale/swscale.h"
}
#include <SDL2/SDL.h>
#include "profileDB.h"
#include "aui_sdlsurface.h"

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25
#define EXTERNAL_CLOCK_MIN_FRAMES 2
#define EXTERNAL_CLOCK_MAX_FRAMES 10

/* Minimum SDL audio buffer size, in samples. */
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
/* Calculate actual buffer size keeping in mind not cause too frequent audio callbacks */
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.04
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* external clock speed adjustment constants for realtime sources based on buffer fullness */
#define EXTERNAL_CLOCK_SPEED_MIN  0.900
#define EXTERNAL_CLOCK_SPEED_MAX  1.010
#define EXTERNAL_CLOCK_SPEED_STEP 0.001

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

/* polls for possible required screen refresh at least this often, should be less than 1/fps */
#define REFRESH_RATE 0.01

#define CURSOR_HIDE_DELAY 1000000

static unsigned sws_flags = SWS_BICUBIC;

typedef struct MyAVPacketList {
	AVPacket pkt;
	struct MyAVPacketList *next;
	int serial;
} MyAVPacketList;

typedef struct PacketQueue {
	MyAVPacketList *first_pkt, *last_pkt;
	int nb_packets;
	int size;
	int64_t duration;
	int abort_request;
	int serial;
	SDL_mutex *mutex;
	SDL_cond *cond;
} PacketQueue;

#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

typedef struct AudioParams {
	int freq;
	int channels;
	int64_t channel_layout;
	enum AVSampleFormat fmt;
	int frame_size;
	int bytes_per_sec;
} AudioParams;

typedef struct Clock {
	double pts;           /* clock base */
	double pts_drift;     /* clock base minus time at which we updated the clock */
	double last_updated;
	double speed;
	int serial;           /* clock is based on a packet with this serial */
	int paused;
	int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;

/* Common struct for handling all types of decoded data and allocated render buffers. */
typedef struct Frame {
	AVFrame *frame;
	int serial;
	double pts;           /* presentation timestamp for the frame */
	double duration;      /* estimated duration of the frame */
	int64_t pos;          /* byte position of the frame in the input file */
	int width;
	int height;
	int format;
	AVRational sar;
	int uploaded;
	int flip_v;
} Frame;

typedef struct FrameQueue {
	Frame queue[FRAME_QUEUE_SIZE];
	int rindex;
	int windex;
	int size;
	int max_size;
	int keep_last;
	int rindex_shown;
	SDL_mutex *mutex;
	SDL_cond *cond;
	PacketQueue *pktq;
} FrameQueue;

enum {
	AV_SYNC_AUDIO_MASTER, /* default choice */
	AV_SYNC_VIDEO_MASTER,
	AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};

typedef struct Decoder {
	AVPacket pkt;
	PacketQueue *queue;
	AVCodecContext *avctx;
	int pkt_serial;
	int finished;
	int packet_pending;
	SDL_cond *empty_queue_cond;
	int64_t start_pts;
	AVRational start_pts_tb;
	int64_t next_pts;
	AVRational next_pts_tb;
	SDL_Thread *decoder_tid;
} Decoder;

typedef struct VideoState {
	SDL_Thread *read_tid;
	int abort_request;
	int force_refresh;
	int paused;
	int last_paused;
	int queue_attachments_req;
	int read_pause_return;
	AVFormatContext *ic;
	int realtime;

	Clock audclk;
	Clock vidclk;
	Clock extclk;

	FrameQueue pictq;
	FrameQueue sampq;

	Decoder auddec;
	Decoder viddec;

	int audio_stream;

	int av_sync_type;

	double audio_clock;
	int audio_clock_serial;
	double audio_diff_cum; /* used for AV difference average computation */
	double audio_diff_avg_coef;
	double audio_diff_threshold;
	int audio_diff_avg_count;
	AVStream *audio_st;
	PacketQueue audioq;
	int audio_hw_buf_size;
	uint8_t *audio_buf;
	uint8_t *audio_buf1;
	unsigned int audio_buf_size; /* in bytes */
	unsigned int audio_buf1_size;
	int audio_buf_index; /* in bytes */
	int audio_write_buf_size;
	int audio_volume;
	int muted;
	struct AudioParams audio_src;
	struct AudioParams audio_tgt;
	struct SwrContext *swr_ctx;
	int frame_drops_early;
	int frame_drops_late;

	SDL_Texture *vid_texture;

	double frame_timer;
	double frame_last_filter_delay;
	int video_stream;
	AVStream *video_st;
	PacketQueue videoq;
	double max_frame_duration;      // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity
	struct SwsContext *img_convert_ctx;
	struct SwsContext *sub_convert_ctx;
	int eof;

	char *filename;
	int width, height, xleft, ytop;
	int step;

	SDL_cond *continue_read_thread;
} VideoState;

/* options specified by the user */
static int default_width  = 640;
static int default_height = 480;
static int screen_width  = 0;
static int screen_height = 0;
static int av_sync_type = AV_SYNC_AUDIO_MASTER;
static int infinite_buffer = -1;
static int64_t cursor_last_shown;
static int cursor_hidden = 0;

/* current context */
static int is_full_screen;

static AVPacket flush_pkt;

static SDL_Renderer *renderer = NULL;
static SDL_Texture *background = NULL;
static SDL_AudioDeviceID audio_dev;

static const struct TextureFormatEntry {
	enum AVPixelFormat format;
	int texture_fmt;
} sdl_texture_format_map[] = {
		{ AV_PIX_FMT_RGB8,           SDL_PIXELFORMAT_RGB332 },
		{ AV_PIX_FMT_RGB444,         SDL_PIXELFORMAT_RGB444 },
		{ AV_PIX_FMT_RGB555,         SDL_PIXELFORMAT_RGB555 },
		{ AV_PIX_FMT_BGR555,         SDL_PIXELFORMAT_BGR555 },
		{ AV_PIX_FMT_RGB565,         SDL_PIXELFORMAT_RGB565 },
		{ AV_PIX_FMT_BGR565,         SDL_PIXELFORMAT_BGR565 },
		{ AV_PIX_FMT_RGB24,          SDL_PIXELFORMAT_RGB24 },
		{ AV_PIX_FMT_BGR24,          SDL_PIXELFORMAT_BGR24 },
		{ AV_PIX_FMT_0RGB32,         SDL_PIXELFORMAT_RGB888 },
		{ AV_PIX_FMT_0BGR32,         SDL_PIXELFORMAT_BGR888 },
		{ AV_PIX_FMT_NE(RGB0, 0BGR), SDL_PIXELFORMAT_RGBX8888 },
		{ AV_PIX_FMT_NE(BGR0, 0RGB), SDL_PIXELFORMAT_BGRX8888 },
		{ AV_PIX_FMT_RGB32,          SDL_PIXELFORMAT_ARGB8888 },
		{ AV_PIX_FMT_RGB32_1,        SDL_PIXELFORMAT_RGBA8888 },
		{ AV_PIX_FMT_BGR32,          SDL_PIXELFORMAT_ABGR8888 },
		{ AV_PIX_FMT_BGR32_1,        SDL_PIXELFORMAT_BGRA8888 },
		{ AV_PIX_FMT_YUV420P,        SDL_PIXELFORMAT_IYUV },
		{ AV_PIX_FMT_YUYV422,        SDL_PIXELFORMAT_YUY2 },
		{ AV_PIX_FMT_UYVY422,        SDL_PIXELFORMAT_UYVY },
		{ AV_PIX_FMT_NONE,           SDL_PIXELFORMAT_UNKNOWN },
};

static int packet_queue_put_private(PacketQueue *q, AVPacket *pkt)
{
	MyAVPacketList *pkt1;

	if (q->abort_request)
		return -1;

	pkt1 = (MyAVPacketList*) av_malloc(sizeof(MyAVPacketList));
	if (!pkt1)
		return -1;
	pkt1->pkt = *pkt;
	pkt1->next = NULL;
	if (pkt == &flush_pkt)
		q->serial++;
	pkt1->serial = q->serial;

	if (!q->last_pkt)
		q->first_pkt = pkt1;
	else
		q->last_pkt->next = pkt1;
	q->last_pkt = pkt1;
	q->nb_packets++;
	q->size += pkt1->pkt.size + sizeof(*pkt1);
	q->duration += pkt1->pkt.duration;
	/* XXX: should duplicate packet data in DV case */
	SDL_CondSignal(q->cond);
	return 0;
}

static int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
	int ret;

	SDL_LockMutex(q->mutex);
	ret = packet_queue_put_private(q, pkt);
	SDL_UnlockMutex(q->mutex);

	if (pkt != &flush_pkt && ret < 0)
		av_packet_unref(pkt);

	return ret;
}

static int packet_queue_put_nullpacket(PacketQueue *q, int stream_index)
{
	AVPacket pkt1, *pkt = &pkt1;
	av_init_packet(pkt);
	pkt->data = NULL;
	pkt->size = 0;
	pkt->stream_index = stream_index;
	return packet_queue_put(q, pkt);
}

/* packet queue handling */
static int packet_queue_init(PacketQueue *q)
{
	memset(q, 0, sizeof(PacketQueue));
	q->mutex = SDL_CreateMutex();
	if (!q->mutex) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
        return AVERROR(ENOMEM);
	}
	q->cond = SDL_CreateCond();
	if (!q->cond) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
        return AVERROR(ENOMEM);
	}
	q->abort_request = 1;
	return 0;
}

static void packet_queue_flush(PacketQueue *q)
{
	MyAVPacketList *pkt, *pkt1;

	SDL_LockMutex(q->mutex);
	for (pkt = q->first_pkt; pkt; pkt = pkt1) {
		pkt1 = pkt->next;
		av_packet_unref(&pkt->pkt);
		av_freep(&pkt);
	}
	q->last_pkt = NULL;
	q->first_pkt = NULL;
	q->nb_packets = 0;
	q->size = 0;
	q->duration = 0;
	SDL_UnlockMutex(q->mutex);
}

static void packet_queue_destroy(PacketQueue *q)
{
	packet_queue_flush(q);
	SDL_DestroyMutex(q->mutex);
	SDL_DestroyCond(q->cond);
}

static void packet_queue_abort(PacketQueue *q)
{
	SDL_LockMutex(q->mutex);

	q->abort_request = 1;

	SDL_CondSignal(q->cond);

	SDL_UnlockMutex(q->mutex);
}

static void packet_queue_start(PacketQueue *q)
{
	SDL_LockMutex(q->mutex);
	q->abort_request = 0;
	packet_queue_put_private(q, &flush_pkt);
	SDL_UnlockMutex(q->mutex);
}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block, int *serial)
{
	MyAVPacketList *pkt1;
	int ret;

	SDL_LockMutex(q->mutex);

	for (;;) {
		if (q->abort_request) {
			ret = -1;
			break;
		}

		pkt1 = q->first_pkt;
		if (pkt1) {
			q->first_pkt = pkt1->next;
			if (!q->first_pkt)
				q->last_pkt = NULL;
			q->nb_packets--;
			q->size -= pkt1->pkt.size + sizeof(*pkt1);
			q->duration -= pkt1->pkt.duration;
			*pkt = pkt1->pkt;
			if (serial)
				*serial = pkt1->serial;
			av_free(pkt1);
			ret = 1;
			break;
		} else if (!block) {
			ret = 0;
			break;
		} else {
			SDL_CondWait(q->cond, q->mutex);
		}
	}
	SDL_UnlockMutex(q->mutex);
	return ret;
}

static void decoder_init(Decoder *d, AVCodecContext *avctx, PacketQueue *queue, SDL_cond *empty_queue_cond) {
	memset(d, 0, sizeof(Decoder));
	d->avctx = avctx;
	d->queue = queue;
	d->empty_queue_cond = empty_queue_cond;
	d->start_pts = AV_NOPTS_VALUE;
	d->pkt_serial = -1;
}

static int decoder_decode_frame(Decoder *d, AVFrame *frame) {
	int ret = AVERROR(EAGAIN);

	for (;;) {
		AVPacket pkt;

		if (d->queue->serial == d->pkt_serial) {
			do {
				if (d->queue->abort_request)
					return -1;

				switch (d->avctx->codec_type) {
					case AVMEDIA_TYPE_VIDEO:
						ret = avcodec_receive_frame(d->avctx, frame);
						if (ret >= 0) {
							frame->pts = frame->best_effort_timestamp;
						}
						break;
					case AVMEDIA_TYPE_AUDIO:
						ret = avcodec_receive_frame(d->avctx, frame);
						if (ret >= 0) {
							AVRational tb = (AVRational){1, frame->sample_rate};
							if (frame->pts != AV_NOPTS_VALUE)
								frame->pts = av_rescale_q(frame->pts, av_codec_get_pkt_timebase(d->avctx), tb);
							else if (d->next_pts != AV_NOPTS_VALUE)
								frame->pts = av_rescale_q(d->next_pts, d->next_pts_tb, tb);
							if (frame->pts != AV_NOPTS_VALUE) {
								d->next_pts = frame->pts + frame->nb_samples;
								d->next_pts_tb = tb;
							}
						}
						break;
				}
				if (ret == AVERROR_EOF) {
					d->finished = d->pkt_serial;
					avcodec_flush_buffers(d->avctx);
					return 0;
				}
				if (ret >= 0)
					return 1;
			} while (ret != AVERROR(EAGAIN));
		}

		do {
			if (d->queue->nb_packets == 0)
				SDL_CondSignal(d->empty_queue_cond);
			if (d->packet_pending) {
				av_packet_move_ref(&pkt, &d->pkt);
				d->packet_pending = 0;
			} else {
				if (packet_queue_get(d->queue, &pkt, 1, &d->pkt_serial) < 0)
					return -1;
			}
			// Added from ffmpeg 4
			if (d->queue->serial == d->pkt_serial)
				break;
			av_packet_unref(&pkt);
		} while (1);

		if (pkt.data == flush_pkt.data) {
			avcodec_flush_buffers(d->avctx);
			d->finished = 0;
			d->next_pts = d->start_pts;
			d->next_pts_tb = d->start_pts_tb;
		} else {
			if (avcodec_send_packet(d->avctx, &pkt) == AVERROR(EAGAIN)) {
				av_log(d->avctx, AV_LOG_ERROR, "Receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
				d->packet_pending = 1;
				av_packet_move_ref(&d->pkt, &pkt);
			}
			av_packet_unref(&pkt);
		}
	}
}

static void decoder_destroy(Decoder *d) {
	av_packet_unref(&d->pkt);
	avcodec_free_context(&d->avctx);
}

static void frame_queue_unref_item(Frame *vp)
{
	av_frame_unref(vp->frame);
}

static int frame_queue_init(FrameQueue *f, PacketQueue *pktq, int max_size, int keep_last)
{
	int i;
	memset(f, 0, sizeof(FrameQueue));
	if (!(f->mutex = SDL_CreateMutex())) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
        return AVERROR(ENOMEM);
	}
	if (!(f->cond = SDL_CreateCond())) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
        return AVERROR(ENOMEM);
	}
	f->pktq = pktq;
	f->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
	f->keep_last = !!keep_last;
	for (i = 0; i < f->max_size; i++)
		if (!(f->queue[i].frame = av_frame_alloc()))
            return AVERROR(ENOMEM);
	return 0;
}

static void frame_queue_destroy(FrameQueue *f)
{
	int i;
	for (i = 0; i < f->max_size; i++) {
		Frame *vp = &f->queue[i];
		frame_queue_unref_item(vp);
		av_frame_free(&vp->frame);
	}
	SDL_DestroyMutex(f->mutex);
	SDL_DestroyCond(f->cond);
}

static void frame_queue_signal(FrameQueue *f)
{
	SDL_LockMutex(f->mutex);
	SDL_CondSignal(f->cond);
	SDL_UnlockMutex(f->mutex);
}

static Frame *frame_queue_peek(FrameQueue *f)
{
	return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

static Frame *frame_queue_peek_next(FrameQueue *f)
{
	return &f->queue[(f->rindex + f->rindex_shown + 1) % f->max_size];
}

static Frame *frame_queue_peek_last(FrameQueue *f)
{
	return &f->queue[f->rindex];
}

static Frame *frame_queue_peek_writable(FrameQueue *f)
{
	/* wait until we have space to put a new frame */
	SDL_LockMutex(f->mutex);
	while (f->size >= f->max_size &&
		   !f->pktq->abort_request) {
		SDL_CondWait(f->cond, f->mutex);
	}
	SDL_UnlockMutex(f->mutex);

	if (f->pktq->abort_request)
		return NULL;

	return &f->queue[f->windex];
}

static Frame *frame_queue_peek_readable(FrameQueue *f)
{
	/* wait until we have a readable a new frame */
	SDL_LockMutex(f->mutex);
	while (f->size - f->rindex_shown <= 0 &&
		   !f->pktq->abort_request) {
		SDL_CondWait(f->cond, f->mutex);
	}
	SDL_UnlockMutex(f->mutex);

	if (f->pktq->abort_request)
		return NULL;

	return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

static void frame_queue_push(FrameQueue *f)
{
	if (++f->windex == f->max_size)
		f->windex = 0;
	SDL_LockMutex(f->mutex);
	f->size++;
	SDL_CondSignal(f->cond);
	SDL_UnlockMutex(f->mutex);
}

static void frame_queue_next(FrameQueue *f)
{
	if (f->keep_last && !f->rindex_shown) {
		f->rindex_shown = 1;
		return;
	}
	frame_queue_unref_item(&f->queue[f->rindex]);
	if (++f->rindex == f->max_size)
		f->rindex = 0;
	SDL_LockMutex(f->mutex);
	f->size--;
	SDL_CondSignal(f->cond);
	SDL_UnlockMutex(f->mutex);
}

/* return the number of undisplayed frames in the queue */
static int frame_queue_nb_remaining(FrameQueue *f)
{
	return f->size - f->rindex_shown;
}

static void decoder_abort(Decoder *d, FrameQueue *fq)
{
	packet_queue_abort(d->queue);
	frame_queue_signal(fq);
	SDL_WaitThread(d->decoder_tid, NULL);
	d->decoder_tid = NULL;
	packet_queue_flush(d->queue);
}

static int realloc_texture(SDL_Texture **texture, Uint32 new_format, int new_width, int new_height, SDL_BlendMode blendmode, int init_texture)
{
	Uint32 format;
	int access, w, h;
	// test on texture added from ffmpeg 4
	if (!*texture || SDL_QueryTexture(*texture, &format, &access, &w, &h) < 0 || new_width != w || new_height != h || new_format != format) {
		void *pixels;
		int pitch;
		if (*texture)
			SDL_DestroyTexture(*texture);
		if (!(*texture = SDL_CreateTexture(renderer, new_format, SDL_TEXTUREACCESS_STREAMING, new_width, new_height)))
			return -1;
		if (SDL_SetTextureBlendMode(*texture, blendmode) < 0)
			return -1;
		if (init_texture) {
			if (SDL_LockTexture(*texture, NULL, &pixels, &pitch) < 0)
				return -1;
			memset(pixels, 0, pitch * new_height);
			SDL_UnlockTexture(*texture);
		}
		av_log(NULL, AV_LOG_VERBOSE, "Created %dx%d texture with %s.\n", new_width, new_height, SDL_GetPixelFormatName(new_format));
	}
	return 0;
}

static void calculate_display_rect(SDL_Rect *rect,
								   int scr_xleft, int scr_ytop, int scr_width, int scr_height,
                                   int pic_width, int pic_height, AVRational pic_sar)
{
	float aspect_ratio;
	int width, height, x, y;

	if (pic_sar.num == 0)
		aspect_ratio = 0;
	else
		aspect_ratio = av_q2d(pic_sar);

	if (aspect_ratio <= 0.0)
		aspect_ratio = 1.0;
	aspect_ratio *= (float) pic_width / (float) pic_height;

	/* XXX: we suppose the screen has a 1.0 pixel ratio */
	height = scr_height;
	width = lrint(height * aspect_ratio) & ~1;
	if (width > scr_width) {
		width = scr_width;
		height = lrint(width / aspect_ratio) & ~1;
	}
	x = (scr_width - width) / 2;
	y = (scr_height - height) / 2;
	rect->x = scr_xleft + x;
	rect->y = scr_ytop + y;
	rect->w = FFMAX(width, 1);
	rect->h = FFMAX(height, 1);
}

static void get_sdl_pix_fmt_and_blendmode(int format, Uint32 *sdl_pix_fmt, SDL_BlendMode *sdl_blendmode)
{
	int i;
	*sdl_blendmode = SDL_BLENDMODE_NONE;
	*sdl_pix_fmt = SDL_PIXELFORMAT_UNKNOWN;
	if (format == AV_PIX_FMT_RGB32   ||
		format == AV_PIX_FMT_RGB32_1 ||
		format == AV_PIX_FMT_BGR32   ||
		format == AV_PIX_FMT_BGR32_1)
		*sdl_blendmode = SDL_BLENDMODE_BLEND;
	for (i = 0; i < FF_ARRAY_ELEMS(sdl_texture_format_map) - 1; i++) {
		if (format == sdl_texture_format_map[i].format) {
			*sdl_pix_fmt = sdl_texture_format_map[i].texture_fmt;
			return;
		}
	}
}

static int upload_texture(SDL_Texture **tex, AVFrame *frame, struct SwsContext **img_convert_ctx) {
	int ret = 0;
	Uint32 sdl_pix_fmt;
	SDL_BlendMode sdl_blendmode;
	get_sdl_pix_fmt_and_blendmode(frame->format, &sdl_pix_fmt, &sdl_blendmode);
	if (realloc_texture(tex, sdl_pix_fmt == SDL_PIXELFORMAT_UNKNOWN ? SDL_PIXELFORMAT_ARGB8888 : sdl_pix_fmt, frame->width, frame->height, sdl_blendmode, 0) < 0)
		return -1;
	switch (sdl_pix_fmt) {
		case SDL_PIXELFORMAT_UNKNOWN:
			*img_convert_ctx = sws_getCachedContext(*img_convert_ctx,
				frame->width, frame->height, (AVPixelFormat) frame->format, frame->width, frame->height,
													AV_PIX_FMT_BGRA, sws_flags, NULL, NULL, NULL);
			if (*img_convert_ctx != NULL) {
				uint8_t *pixels[4];
				int pitch[4];
				if (!SDL_LockTexture(*tex, NULL, (void **)pixels, pitch)) {
					sws_scale(*img_convert_ctx, (const uint8_t * const *)frame->data, frame->linesize,
							  0, frame->height, pixels, pitch);
					SDL_UnlockTexture(*tex);
				}
			} else {
				av_log(NULL, AV_LOG_FATAL, "Cannot initialize the conversion context\n");
				ret = -1;
			}
			break;
		case SDL_PIXELFORMAT_IYUV:
			if (frame->linesize[0] > 0 && frame->linesize[1] > 0 && frame->linesize[2] > 0) {
				ret = SDL_UpdateYUVTexture(*tex, NULL, frame->data[0], frame->linesize[0],
										   frame->data[1], frame->linesize[1],
										   frame->data[2], frame->linesize[2]);
			} else if (frame->linesize[0] < 0 && frame->linesize[1] < 0 && frame->linesize[2] < 0) {
				ret = SDL_UpdateYUVTexture(*tex, NULL, frame->data[0] + frame->linesize[0] * (frame->height                    - 1), -frame->linesize[0],
										   frame->data[1] + frame->linesize[1] * (AV_CEIL_RSHIFT(frame->height, 1) - 1), -frame->linesize[1],
										   frame->data[2] + frame->linesize[2] * (AV_CEIL_RSHIFT(frame->height, 1) - 1), -frame->linesize[2]);
			} else {
				av_log(NULL, AV_LOG_ERROR, "Mixed negative and positive linesizes are not supported.\n");
				return -1;
			}
			break;
		default:
			if (frame->linesize[0] < 0) {
				ret = SDL_UpdateTexture(*tex, NULL, frame->data[0] + frame->linesize[0] * (frame->height - 1), -frame->linesize[0]);
			} else {
				ret = SDL_UpdateTexture(*tex, NULL, frame->data[0], frame->linesize[0]);
			}
			break;
	}
	return ret;
}

static void set_sdl_yuv_conversion_mode(AVFrame *frame)
{
#if SDL_VERSION_ATLEAST(2,0,8)
	SDL_YUV_CONVERSION_MODE mode = SDL_YUV_CONVERSION_AUTOMATIC;
	if (frame && (frame->format == AV_PIX_FMT_YUV420P || frame->format == AV_PIX_FMT_YUYV422 || frame->format == AV_PIX_FMT_UYVY422)) {
		if (frame->color_range == AVCOL_RANGE_JPEG)
			mode = SDL_YUV_CONVERSION_JPEG;
		else if (frame->colorspace == AVCOL_SPC_BT709)
			mode = SDL_YUV_CONVERSION_BT709;
		else if (frame->colorspace == AVCOL_SPC_BT470BG || frame->colorspace == AVCOL_SPC_SMPTE170M || frame->colorspace == AVCOL_SPC_SMPTE240M)
			mode = SDL_YUV_CONVERSION_BT601;
	}
	SDL_SetYUVConversionMode(mode);
#endif
}

static void video_image_display(VideoState *is)
{
	Frame *vp;
	SDL_Rect rect;

	vp = frame_queue_peek_last(&is->pictq);

	calculate_display_rect(&rect, is->xleft, is->ytop, is->width, is->height, vp->width, vp->height, vp->sar);

	if (!vp->uploaded) {
		if (upload_texture(&is->vid_texture, vp->frame, &is->img_convert_ctx) < 0)
			return;
		vp->uploaded = 1;
		vp->flip_v = vp->frame->linesize[0] < 0;
	}

	// copied from ffmpeg 4
	set_sdl_yuv_conversion_mode(vp->frame);
	SDL_RenderCopyEx(renderer, is->vid_texture, NULL, &rect, 0, NULL, (SDL_RendererFlip) (vp->flip_v ? SDL_FLIP_VERTICAL : 0));
	// copied from ffmpeg 4
	set_sdl_yuv_conversion_mode(NULL);
}

static void stream_component_close(VideoState *is, int stream_index)
{
	AVFormatContext *ic = is->ic;
	AVCodecParameters *codecpar;

	if (stream_index < 0 || stream_index >= ic->nb_streams)
		return;
	codecpar = ic->streams[stream_index]->codecpar;

	switch (codecpar->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			decoder_abort(&is->auddec, &is->sampq);
			SDL_CloseAudioDevice(audio_dev);
			decoder_destroy(&is->auddec);
			swr_free(&is->swr_ctx);
			av_freep(&is->audio_buf1);
			is->audio_buf1_size = 0;
			is->audio_buf = NULL;
			break;
		case AVMEDIA_TYPE_VIDEO:
			decoder_abort(&is->viddec, &is->pictq);
			decoder_destroy(&is->viddec);
			break;
		default:
			break;
	}

	ic->streams[stream_index]->discard = AVDISCARD_ALL;
	switch (codecpar->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			is->audio_st = NULL;
			is->audio_stream = -1;
			break;
		case AVMEDIA_TYPE_VIDEO:
			is->video_st = NULL;
			is->video_stream = -1;
			break;
		default:
			break;
	}
}

static void stream_close(VideoState *is)
{
	/* XXX: use a special url_shutdown call to abort parse cleanly */
	is->abort_request = 1;
	SDL_WaitThread(is->read_tid, NULL);

	/* close each stream */
	if (is->audio_stream >= 0)
		stream_component_close(is, is->audio_stream);
	if (is->video_stream >= 0)
		stream_component_close(is, is->video_stream);
	avformat_close_input(&is->ic);

	packet_queue_destroy(&is->videoq);
	packet_queue_destroy(&is->audioq);

	/* free all pictures */
	frame_queue_destroy(&is->pictq);
	frame_queue_destroy(&is->sampq);
	SDL_DestroyCond(is->continue_read_thread);
	sws_freeContext(is->img_convert_ctx);
	sws_freeContext(is->sub_convert_ctx);
	av_free(is->filename);
	if (is->vid_texture)
		SDL_DestroyTexture(is->vid_texture);
	av_free(is);
}

static void set_default_window_size(int width, int height, AVRational sar)
{
	SDL_Rect rect;
	int max_width  = screen_width  ? screen_width  : INT_MAX;
	int max_height = screen_height ? screen_height : INT_MAX;
	if (max_width == INT_MAX && max_height == INT_MAX)
		max_height = height;
	calculate_display_rect(&rect, 0, 0, max_width, max_height, width, height, sar);
	default_width  = rect.w;
	default_height = rect.h;
}

static int video_open(VideoState *is)
{
	int w,h;

	w = screen_width ? screen_width : default_width;
	h = screen_height ? screen_height : default_height;

	is->width  = w;
	is->height = h;

	return 0;
}

/* display the current picture, if any */
static void video_display(VideoState *is)
{
	if (!is->width)
		video_open(is);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	if (!is_full_screen) {
		SDL_RenderCopy(renderer, background, NULL, NULL);
	}
	if (is->video_st)
		video_image_display(is);
	SDL_RenderPresent(renderer);
}

static double get_clock(Clock *c)
{
	if (*c->queue_serial != c->serial)
		return NAN;
	if (c->paused) {
		return c->pts;
	} else {
		double time = av_gettime_relative() / 1000000.0;
		return c->pts_drift + time - (time - c->last_updated) * (1.0 - c->speed);
	}
}

static void set_clock_at(Clock *c, double pts, int serial, double time)
{
	c->pts = pts;
	c->last_updated = time;
	c->pts_drift = c->pts - time;
	c->serial = serial;
}

static void set_clock(Clock *c, double pts, int serial)
{
	double time = av_gettime_relative() / 1000000.0;
	set_clock_at(c, pts, serial, time);
}

static void set_clock_speed(Clock *c, double speed)
{
	set_clock(c, get_clock(c), c->serial);
	c->speed = speed;
}

static void init_clock(Clock *c, int *queue_serial)
{
	c->speed = 1.0;
	c->paused = 0;
	c->queue_serial = queue_serial;
	set_clock(c, NAN, -1);
}

static void sync_clock_to_slave(Clock *c, Clock *slave)
{
	double clock = get_clock(c);
	double slave_clock = get_clock(slave);
	if (!isnan(slave_clock) && (isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))
		set_clock(c, slave_clock, slave->serial);
}

static int get_master_sync_type(VideoState *is) {
	if (is->av_sync_type == AV_SYNC_VIDEO_MASTER) {
		if (is->video_st)
			return AV_SYNC_VIDEO_MASTER;
		else
			return AV_SYNC_AUDIO_MASTER;
	} else if (is->av_sync_type == AV_SYNC_AUDIO_MASTER) {
		if (is->audio_st)
			return AV_SYNC_AUDIO_MASTER;
		else
			return AV_SYNC_EXTERNAL_CLOCK;
	} else {
		return AV_SYNC_EXTERNAL_CLOCK;
	}
}

/* get the current master clock value */
static double get_master_clock(VideoState *is)
{
	double val;

	switch (get_master_sync_type(is)) {
		case AV_SYNC_VIDEO_MASTER:
			val = get_clock(&is->vidclk);
			break;
		case AV_SYNC_AUDIO_MASTER:
			val = get_clock(&is->audclk);
			break;
		default:
			val = get_clock(&is->extclk);
			break;
	}
	return val;
}

static void check_external_clock_speed(VideoState *is) {
	if (is->video_stream >= 0 && is->videoq.nb_packets <= EXTERNAL_CLOCK_MIN_FRAMES ||
		is->audio_stream >= 0 && is->audioq.nb_packets <= EXTERNAL_CLOCK_MIN_FRAMES) {
		set_clock_speed(&is->extclk, FFMAX(EXTERNAL_CLOCK_SPEED_MIN, is->extclk.speed - EXTERNAL_CLOCK_SPEED_STEP));
	} else if ((is->video_stream < 0 || is->videoq.nb_packets > EXTERNAL_CLOCK_MAX_FRAMES) &&
			   (is->audio_stream < 0 || is->audioq.nb_packets > EXTERNAL_CLOCK_MAX_FRAMES)) {
		set_clock_speed(&is->extclk, FFMIN(EXTERNAL_CLOCK_SPEED_MAX, is->extclk.speed + EXTERNAL_CLOCK_SPEED_STEP));
	} else {
		double speed = is->extclk.speed;
		if (speed != 1.0)
			set_clock_speed(&is->extclk, speed + EXTERNAL_CLOCK_SPEED_STEP * (1.0 - speed) / fabs(1.0 - speed));
	}
}

/* pause or resume the video */
static void stream_toggle_pause(VideoState *is)
{
	if (is->paused) {
		is->frame_timer += av_gettime_relative() / 1000000.0 - is->vidclk.last_updated;
		if (is->read_pause_return != AVERROR(ENOSYS)) {
			is->vidclk.paused = 0;
		}
		set_clock(&is->vidclk, get_clock(&is->vidclk), is->vidclk.serial);
	}
	set_clock(&is->extclk, get_clock(&is->extclk), is->extclk.serial);
	is->paused = is->audclk.paused = is->vidclk.paused = is->extclk.paused = !is->paused;
}

static void toggle_pause(VideoState *is)
{
	stream_toggle_pause(is);
	is->step = 0;
}

static void step_to_next_frame(VideoState *is)
{
	/* if the stream is paused unpause it, then step */
	if (is->paused)
		stream_toggle_pause(is);
	is->step = 1;
}

static double compute_target_delay(double delay, VideoState *is)
{
	double sync_threshold, diff = 0;

	/* update delay to follow master synchronisation source */
	if (get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) {
		/* if video is slave, we try to correct big delays by
		   duplicating or deleting a frame */
		diff = get_clock(&is->vidclk) - get_master_clock(is);

		/* skip or repeat frame. We take into account the
		   delay to compute the threshold. I still don't know
		   if it is the best guess */
		sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
		if (!isnan(diff) && fabs(diff) < is->max_frame_duration) {
			if (diff <= -sync_threshold)
				delay = FFMAX(0, delay + diff);
			else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
				delay = delay + diff;
			else if (diff >= sync_threshold)
				delay = 2 * delay;
		}
	}

	av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\n",
		   delay, -diff);

	return delay;
}

static double vp_duration(VideoState *is, Frame *vp, Frame *nextvp) {
	if (vp->serial == nextvp->serial) {
		double duration = nextvp->pts - vp->pts;
		if (isnan(duration) || duration <= 0 || duration > is->max_frame_duration)
			return vp->duration;
		else
			return duration;
	} else {
		return 0.0;
	}
}

static void update_video_pts(VideoState *is, double pts, int64_t pos, int serial) {
	/* update current video pts */
	set_clock(&is->vidclk, pts, serial);
	sync_clock_to_slave(&is->extclk, &is->vidclk);
}

/* called to display each frame */
static void video_refresh(void *opaque, double *remaining_time)
{
	VideoState *is = (VideoState*) opaque;
	double time;

	Frame *sp, *sp2;

	if (!is->paused && get_master_sync_type(is) == AV_SYNC_EXTERNAL_CLOCK && is->realtime)
		check_external_clock_speed(is);

	if (is->video_st) {
		retry:
		if (frame_queue_nb_remaining(&is->pictq) == 0) {
			// nothing to do, no picture to display in the queue
		} else {
			double last_duration, duration, delay;
			Frame *vp, *lastvp;

			/* dequeue the picture */
			lastvp = frame_queue_peek_last(&is->pictq);
			vp = frame_queue_peek(&is->pictq);

			if (vp->serial != is->videoq.serial) {
				frame_queue_next(&is->pictq);
				goto retry;
			}

			if (lastvp->serial != vp->serial)
				is->frame_timer = av_gettime_relative() / 1000000.0;

			if (is->paused)
				goto display;

			/* compute nominal last_duration */
			last_duration = vp_duration(is, lastvp, vp);
			delay = compute_target_delay(last_duration, is);

			time= av_gettime_relative()/1000000.0;
			if (time < is->frame_timer + delay) {
				*remaining_time = FFMIN(is->frame_timer + delay - time, *remaining_time);
				goto display;
			}

			is->frame_timer += delay;
			if (delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
				is->frame_timer = time;

			SDL_LockMutex(is->pictq.mutex);
			if (!isnan(vp->pts))
				update_video_pts(is, vp->pts, vp->pos, vp->serial);
			SDL_UnlockMutex(is->pictq.mutex);

			if (frame_queue_nb_remaining(&is->pictq) > 1) {
				Frame *nextvp = frame_queue_peek_next(&is->pictq);
				duration = vp_duration(is, vp, nextvp);
				if(!is->step && (get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) && time > is->frame_timer + duration){
					is->frame_drops_late++;
					frame_queue_next(&is->pictq);
					goto retry;
				}
			}

			frame_queue_next(&is->pictq);
			is->force_refresh = 1;

			if (is->step && !is->paused)
				stream_toggle_pause(is);
		}
		display:
		/* display picture */
		if (is->force_refresh && is->pictq.rindex_shown)
			video_display(is);
	}
	is->force_refresh = 0;
}

static int queue_picture(VideoState *is, AVFrame *src_frame, double pts, double duration, int64_t pos, int serial)
{
	Frame *vp;

#if defined(DEBUG_SYNC)
	printf("frame_type=%c pts=%0.3f\n",
           av_get_picture_type_char(src_frame->pict_type), pts);
#endif

	if (!(vp = frame_queue_peek_writable(&is->pictq)))
		return -1;

	vp->sar = src_frame->sample_aspect_ratio;
	vp->uploaded = 0;

	vp->width = src_frame->width;
	vp->height = src_frame->height;
	vp->format = src_frame->format;

	vp->pts = pts;
	vp->duration = duration;
	vp->pos = pos;
	vp->serial = serial;

	set_default_window_size(vp->width, vp->height, vp->sar);

	av_frame_move_ref(vp->frame, src_frame);
	frame_queue_push(&is->pictq);
	return 0;
}

static int get_video_frame(VideoState *is, AVFrame *frame)
{
	int got_picture;

	if ((got_picture = decoder_decode_frame(&is->viddec, frame)) < 0)
		return -1;

	if (got_picture) {
		double dpts = NAN;

		if (frame->pts != AV_NOPTS_VALUE)
			dpts = av_q2d(is->video_st->time_base) * frame->pts;

		frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(is->ic, is->video_st, frame);

		if (get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) {
			if (frame->pts != AV_NOPTS_VALUE) {
				double diff = dpts - get_master_clock(is);
				if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD &&
					diff - is->frame_last_filter_delay < 0 &&
					is->viddec.pkt_serial == is->vidclk.serial &&
					is->videoq.nb_packets) {
					is->frame_drops_early++;
					av_frame_unref(frame);
					got_picture = 0;
				}
			}
		}
	}

	return got_picture;
}

static int audio_thread(void *arg)
{
	VideoState *is = (VideoState*) arg;
	AVFrame *frame = av_frame_alloc();
	Frame *af;
	int got_frame = 0;
	AVRational tb;
	int ret = 0;

	if (!frame)
		return AVERROR(ENOMEM);

	do {
		if ((got_frame = decoder_decode_frame(&is->auddec, frame)) < 0)
			goto the_end;

		if (got_frame) {
			tb = (AVRational){1, frame->sample_rate};

			if (!(af = frame_queue_peek_writable(&is->sampq)))
				goto the_end;

			af->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
			af->pos = frame->pkt_pos;
			af->serial = is->auddec.pkt_serial;
			af->duration = av_q2d((AVRational){frame->nb_samples, frame->sample_rate});

			av_frame_move_ref(af->frame, frame);
			frame_queue_push(&is->sampq);
		}
	} while (ret >= 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF);
the_end:
	av_frame_free(&frame);
	return ret;
}

static int decoder_start(Decoder *d, int (*fn)(void *), const char *thread_name, void* arg)
{
	packet_queue_start(d->queue);
	d->decoder_tid = SDL_CreateThread(fn, thread_name, arg);
	if (!d->decoder_tid) {
		av_log(NULL, AV_LOG_ERROR, "SDL_CreateThread(): %s\n", SDL_GetError());
		return AVERROR(ENOMEM);
	}
	return 0;
}

static int video_thread(void *arg)
{
	VideoState *is = (VideoState*) arg;
	AVFrame *frame = av_frame_alloc();
	double pts;
	double duration;
	int ret;
	AVRational tb = is->video_st->time_base;
	AVRational frame_rate = av_guess_frame_rate(is->ic, is->video_st, NULL);

	if (!frame) {
		return AVERROR(ENOMEM);
	}

	for (;;) {
		ret = get_video_frame(is, frame);
		if (ret < 0)
			goto the_end;
		if (!ret)
			continue;

		duration = (frame_rate.num && frame_rate.den ? av_q2d((AVRational){frame_rate.den, frame_rate.num}) : 0);
		pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
		ret = queue_picture(is, frame, pts, duration, frame->pkt_pos, is->viddec.pkt_serial);
		av_frame_unref(frame);

		if (ret < 0)
			goto the_end;
	}
	the_end:
	av_frame_free(&frame);
	return 0;
}

/* ad-turn the wanted number of samples to get better sync if sync_type is video
 * or external master clock */
static int synchronize_audio(VideoState *is, int nb_samples)
{
	int wanted_nb_samples = nb_samples;

	/* if not master, then we try to remove or add samples to correct the clock */
	if (get_master_sync_type(is) != AV_SYNC_AUDIO_MASTER) {
		double diff, avg_diff;
		int min_nb_samples, max_nb_samples;

		diff = get_clock(&is->audclk) - get_master_clock(is);

		if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD) {
			is->audio_diff_cum = diff + is->audio_diff_avg_coef * is->audio_diff_cum;
			if (is->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
				/* not enough measures to have a correct estimate */
				is->audio_diff_avg_count++;
			} else {
				/* estimate the A-V difference */
				avg_diff = is->audio_diff_cum * (1.0 - is->audio_diff_avg_coef);

				if (fabs(avg_diff) >= is->audio_diff_threshold) {
					wanted_nb_samples = nb_samples + (int)(diff * is->audio_src.freq);
					min_nb_samples = ((nb_samples * (100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100));
					max_nb_samples = ((nb_samples * (100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100));
					wanted_nb_samples = av_clip(wanted_nb_samples, min_nb_samples, max_nb_samples);
				}
				av_log(NULL, AV_LOG_TRACE, "diff=%f adiff=%f sample_diff=%d apts=%0.3f %f\n",
					   diff, avg_diff, wanted_nb_samples - nb_samples,
					   is->audio_clock, is->audio_diff_threshold);
			}
		} else {
			/* too big difference : may be initial PTS errors, so
			   reset A-V filter */
			is->audio_diff_avg_count = 0;
			is->audio_diff_cum       = 0;
		}
	}

	return wanted_nb_samples;
}

/**
 * Decode one audio frame and return its uncompressed size.
 *
 * The processed audio frame is decoded, converted if required, and
 * stored in is->audio_buf, with size in bytes given by the return
 * value.
 */
static int audio_decode_frame(VideoState *is)
{
	int data_size, resampled_data_size;
	int64_t dec_channel_layout;
	av_unused double audio_clock0;
	int wanted_nb_samples;
	Frame *af;

	if (is->paused)
		return -1;

	do {
		if (!(af = frame_queue_peek_readable(&is->sampq)))
			return -1;
		frame_queue_next(&is->sampq);
	} while (af->serial != is->audioq.serial);

	data_size = av_samples_get_buffer_size(NULL, af->frame->channels,
										   af->frame->nb_samples,
                                           (AVSampleFormat) af->frame->format, 1);

	dec_channel_layout =
			(af->frame->channel_layout && af->frame->channels == av_get_channel_layout_nb_channels(af->frame->channel_layout)) ?
			af->frame->channel_layout : av_get_default_channel_layout(af->frame->channels);
	wanted_nb_samples = synchronize_audio(is, af->frame->nb_samples);

	if (af->frame->format        != is->audio_src.fmt            ||
		dec_channel_layout       != is->audio_src.channel_layout ||
		af->frame->sample_rate   != is->audio_src.freq           ||
		(wanted_nb_samples       != af->frame->nb_samples && !is->swr_ctx)) {
		swr_free(&is->swr_ctx);
		is->swr_ctx = swr_alloc_set_opts(NULL,
										 is->audio_tgt.channel_layout, is->audio_tgt.fmt, is->audio_tgt.freq,
										 dec_channel_layout,           (AVSampleFormat) af->frame->format, af->frame->sample_rate,
										 0, NULL);
		if (!is->swr_ctx || swr_init(is->swr_ctx) < 0) {
			av_log(NULL, AV_LOG_ERROR,
				   "Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
				   af->frame->sample_rate, av_get_sample_fmt_name((AVSampleFormat) af->frame->format), af->frame->channels,
				   is->audio_tgt.freq, av_get_sample_fmt_name(is->audio_tgt.fmt), is->audio_tgt.channels);
			swr_free(&is->swr_ctx);
			return -1;
		}
		is->audio_src.channel_layout = dec_channel_layout;
		is->audio_src.channels       = af->frame->channels;
		is->audio_src.freq = af->frame->sample_rate;
		is->audio_src.fmt = (AVSampleFormat) af->frame->format;
	}

	if (is->swr_ctx) {
		const uint8_t **in = (const uint8_t **)af->frame->extended_data;
		uint8_t **out = &is->audio_buf1;
		int out_count = (int64_t)wanted_nb_samples * is->audio_tgt.freq / af->frame->sample_rate + 256;
		int out_size  = av_samples_get_buffer_size(NULL, is->audio_tgt.channels, out_count, is->audio_tgt.fmt, 0);
		int len2;
		if (out_size < 0) {
			av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
			return -1;
		}
		if (wanted_nb_samples != af->frame->nb_samples) {
			if (swr_set_compensation(is->swr_ctx, (wanted_nb_samples - af->frame->nb_samples) * is->audio_tgt.freq / af->frame->sample_rate,
									 wanted_nb_samples * is->audio_tgt.freq / af->frame->sample_rate) < 0) {
				av_log(NULL, AV_LOG_ERROR, "swr_set_compensation() failed\n");
				return -1;
			}
		}
		av_fast_malloc(&is->audio_buf1, &is->audio_buf1_size, out_size);
		if (!is->audio_buf1)
			return AVERROR(ENOMEM);
		len2 = swr_convert(is->swr_ctx, out, out_count, in, af->frame->nb_samples);
		if (len2 < 0) {
			av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
			return -1;
		}
		if (len2 == out_count) {
			av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
			if (swr_init(is->swr_ctx) < 0)
				swr_free(&is->swr_ctx);
		}
		is->audio_buf = is->audio_buf1;
		resampled_data_size = len2 * is->audio_tgt.channels * av_get_bytes_per_sample(is->audio_tgt.fmt);
	} else {
		is->audio_buf = af->frame->data[0];
		resampled_data_size = data_size;
	}

	audio_clock0 = is->audio_clock;
	/* update the audio clock with the pts */
	if (!isnan(af->pts))
		is->audio_clock = af->pts + (double) af->frame->nb_samples / af->frame->sample_rate;
	else
		is->audio_clock = NAN;
	is->audio_clock_serial = af->serial;
#ifdef DEBUG
	{
		static double last_clock;
		printf("audio: delay=%0.3f clock=%0.3f clock0=%0.3f\n",
			   is->audio_clock - last_clock,
			   is->audio_clock, audio_clock0);
		last_clock = is->audio_clock;
	}
#endif
	return resampled_data_size;
}

/* prepare a new audio buffer */
static void sdl_audio_callback(void *opaque, Uint8 *stream, int len)
{
	VideoState *is = (VideoState*) opaque;
	int audio_size, len1;

	int64_t audio_callback_time = av_gettime_relative();

	while (len > 0) {
		if (is->audio_buf_index >= is->audio_buf_size) {
			audio_size = audio_decode_frame(is);
			if (audio_size < 0) {
				/* if error, just output silence */
				is->audio_buf = NULL;
				is->audio_buf_size = SDL_AUDIO_MIN_BUFFER_SIZE / is->audio_tgt.frame_size * is->audio_tgt.frame_size;
			} else {
				is->audio_buf_size = audio_size;
			}
			is->audio_buf_index = 0;
		}
		len1 = is->audio_buf_size - is->audio_buf_index;
		if (len1 > len)
			len1 = len;
		if (!is->muted && is->audio_buf && is->audio_volume == SDL_MIX_MAXVOLUME)
			memcpy(stream, (uint8_t *)is->audio_buf + is->audio_buf_index, len1);
		else {
			memset(stream, 0, len1);
			if (!is->muted && is->audio_buf)
                SDL_MixAudioFormat(stream, (uint8_t *)is->audio_buf + is->audio_buf_index, AUDIO_S16SYS, len1, is->audio_volume);
		}
		len -= len1;
		stream += len1;
		is->audio_buf_index += len1;
	}
	is->audio_write_buf_size = is->audio_buf_size - is->audio_buf_index;
	/* Let's assume the audio driver that is used by SDL has two periods. */
	if (!isnan(is->audio_clock)) {
        set_clock_at(&is->audclk, is->audio_clock - (double)(2 * is->audio_hw_buf_size + is->audio_write_buf_size) / is->audio_tgt.bytes_per_sec, is->audio_clock_serial, audio_callback_time / 1000000.0);
		sync_clock_to_slave(&is->extclk, &is->audclk);
	}
}

static int audio_open(void *opaque, int64_t wanted_channel_layout, int wanted_nb_channels, int wanted_sample_rate, struct AudioParams *audio_hw_params)
{
	SDL_AudioSpec wanted_spec, spec;
	const char *env;
	static const int next_nb_channels[] = {0, 0, 1, 6, 2, 6, 4, 6};
	static const int next_sample_rates[] = {0, 44100, 48000, 96000, 192000};
	int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;

	env = SDL_getenv("SDL_AUDIO_CHANNELS");
	if (env) {
		wanted_nb_channels = atoi(env);
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
	}
	if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
		wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
	}
	wanted_nb_channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
	wanted_spec.channels = wanted_nb_channels;
	wanted_spec.freq = wanted_sample_rate;
	if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
		av_log(NULL, AV_LOG_ERROR, "Invalid sample rate or channel count!\n");
        return -1;
	}
	while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq)
		next_sample_rate_idx--;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.silence = 0;
	wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
	wanted_spec.callback = sdl_audio_callback;
	wanted_spec.userdata = opaque;
	while (!(audio_dev = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | SDL_AUDIO_ALLOW_CHANNELS_CHANGE))) {
		av_log(NULL, AV_LOG_WARNING, "SDL_OpenAudio (%d channels, %d Hz): %s\n",
			   wanted_spec.channels, wanted_spec.freq, SDL_GetError());
		wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
		if (!wanted_spec.channels) {
			wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
			wanted_spec.channels = wanted_nb_channels;
			if (!wanted_spec.freq) {
				av_log(NULL, AV_LOG_ERROR,
					   "No more combinations to try, audio open failed\n");
				return -1;
			}
		}
		wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
	}
	if (spec.format != AUDIO_S16SYS) {
		av_log(NULL, AV_LOG_ERROR,
			   "SDL advised audio format %d is not supported!\n", spec.format);
		return -1;
	}
	if (spec.channels != wanted_spec.channels) {
		wanted_channel_layout = av_get_default_channel_layout(spec.channels);
		if (!wanted_channel_layout) {
			av_log(NULL, AV_LOG_ERROR,
				   "SDL advised channel count %d is not supported!\n", spec.channels);
			return -1;
		}
	}

	audio_hw_params->fmt = AV_SAMPLE_FMT_S16;
	audio_hw_params->freq = spec.freq;
	audio_hw_params->channel_layout = wanted_channel_layout;
	audio_hw_params->channels =  spec.channels;
	audio_hw_params->frame_size = av_samples_get_buffer_size(NULL, audio_hw_params->channels, 1, audio_hw_params->fmt, 1);
	audio_hw_params->bytes_per_sec = av_samples_get_buffer_size(NULL, audio_hw_params->channels, audio_hw_params->freq, audio_hw_params->fmt, 1);
	if (audio_hw_params->bytes_per_sec <= 0 || audio_hw_params->frame_size <= 0) {
		av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
		return -1;
	}
	return spec.size;
}

/* open a given stream. Return 0 if OK */
static int stream_component_open(VideoState *is, int stream_index)
{
	AVFormatContext *ic = is->ic;
	AVCodecContext *avctx;
	AVCodec *codec;
	AVDictionary *opts = NULL;
	AVDictionaryEntry *t = NULL;
	int sample_rate, nb_channels;
	int64_t channel_layout;
	int ret = 0;

	if (stream_index < 0 || stream_index >= ic->nb_streams)
        return -1;

	avctx = avcodec_alloc_context3(NULL);
	if (!avctx)
        return AVERROR(ENOMEM);

	ret = avcodec_parameters_to_context(avctx, ic->streams[stream_index]->codecpar);
    if (ret < 0)
		goto fail;
	av_codec_set_pkt_timebase(avctx, ic->streams[stream_index]->time_base);

	codec = avcodec_find_decoder(avctx->codec_id);

	if (!codec) {
        av_log(NULL, AV_LOG_WARNING,
                                      "No codec could be found with id %d\n", avctx->codec_id);
        ret = AVERROR(EINVAL);
		goto fail;
	}

	avctx->codec_id = codec->id;
#if FF_API_EMU_EDGE
	if(codec->capabilities & AV_CODEC_CAP_DR1)
		avctx->flags |= CODEC_FLAG_EMU_EDGE;
#endif

	if ((ret = avcodec_open2(avctx, codec, NULL)) < 0) {
		goto fail;
	}

	is->eof = 0;
	ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
	switch (avctx->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			sample_rate    = avctx->sample_rate;
			nb_channels    = avctx->channels;
			channel_layout = avctx->channel_layout;

			/* prepare audio output */
        if ((ret = audio_open(is, channel_layout, nb_channels, sample_rate, &is->audio_tgt)) < 0)
				goto fail;
			is->audio_hw_buf_size = ret;
			is->audio_src = is->audio_tgt;
			is->audio_buf_size  = 0;
			is->audio_buf_index = 0;

			/* init averaging filter */
			is->audio_diff_avg_coef  = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
			is->audio_diff_avg_count = 0;
			/* since we do not have a precise anough audio FIFO fullness,
			   we correct audio sync only if larger than this threshold */
			is->audio_diff_threshold = (double)(is->audio_hw_buf_size) / is->audio_tgt.bytes_per_sec;

			is->audio_stream = stream_index;
			is->audio_st = ic->streams[stream_index];

			decoder_init(&is->auddec, avctx, &is->audioq, is->continue_read_thread);
			if ((is->ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && !is->ic->iformat->read_seek) {
				is->auddec.start_pts = is->audio_st->start_time;
				is->auddec.start_pts_tb = is->audio_st->time_base;
			}
			if ((ret = decoder_start(&is->auddec, audio_thread, "audio_decoder", is)) < 0)
				goto out;
			SDL_PauseAudioDevice(audio_dev, 0);
			break;
		case AVMEDIA_TYPE_VIDEO:
			is->video_stream = stream_index;
			is->video_st = ic->streams[stream_index];

			decoder_init(&is->viddec, avctx, &is->videoq, is->continue_read_thread);
			if ((ret = decoder_start(&is->viddec, video_thread, "video_decoder", is)) < 0)
				goto out;
			is->queue_attachments_req = 1;
			break;
		default:
			break;
	}
	goto out;

fail:
	avcodec_free_context(&avctx);
out:
	av_dict_free(&opts);

	return ret;
}

static int decode_interrupt_cb(void *ctx)
{
	VideoState *is = (VideoState*) ctx;
	return is->abort_request;
}

static int stream_has_enough_packets(AVStream *st, int stream_id, PacketQueue *queue) {
	return stream_id < 0 ||
		   queue->abort_request ||
		   (st->disposition & AV_DISPOSITION_ATTACHED_PIC) ||
		   queue->nb_packets > MIN_FRAMES && (!queue->duration || av_q2d(st->time_base) * queue->duration > 1.0);
}

static int is_realtime(AVFormatContext *s)
{
	if(   !strcmp(s->iformat->name, "rtp")
		  || !strcmp(s->iformat->name, "rtsp")
		  || !strcmp(s->iformat->name, "sdp")
			)
		return 1;

	if(s->pb && (   !strncmp(s->filename, "rtp:", 4)
					|| !strncmp(s->filename, "udp:", 4)
	)
			)
		return 1;
	return 0;
}

void print_error(const char *filename, int err)
{
	char errbuf[128];
	const char *errbuf_ptr = errbuf;

	if (av_strerror(err, errbuf, sizeof(errbuf)) < 0)
		errbuf_ptr = strerror(AVUNERROR(err));
	av_log(NULL, AV_LOG_ERROR, "%s: %s\n", filename, errbuf_ptr);
}

/* this thread gets the stream from the disk or the network */
static int read_thread(void *arg)
{
	VideoState *is = (VideoState*)arg;
	AVFormatContext *ic = NULL;
	int err, i, ret;
	int st_index[AVMEDIA_TYPE_NB];
	AVPacket pkt1, *pkt = &pkt1;
	int64_t stream_start_time;
	AVDictionaryEntry *t;
	SDL_mutex *wait_mutex = SDL_CreateMutex();
	int scan_all_pmts_set = 0;
	int64_t pkt_ts;

	if (!wait_mutex) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateMutex(): %s\n", SDL_GetError());
        ret = AVERROR(ENOMEM);
		goto fail;
	}

	memset(st_index, -1, sizeof(st_index));
	is->video_stream = -1;
	is->audio_stream = -1;
	is->eof = 0;

	ic = avformat_alloc_context();
	if (!ic) {
		av_log(NULL, AV_LOG_FATAL, "Could not allocate context.\n");
        ret = AVERROR(ENOMEM);
		goto fail;
	}
	ic->interrupt_callback.callback = decode_interrupt_cb;
	ic->interrupt_callback.opaque = is;

	err = avformat_open_input(&ic, is->filename, NULL, NULL);
	if (err < 0) {
		print_error(is->filename, err);
        ret = -1;
		goto fail;
	}
	is->ic = ic;

	av_format_inject_global_side_data(ic);

	if (ic->pb)
		ic->pb->eof_reached = 0;

	is->max_frame_duration = (ic->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;

	is->realtime = is_realtime(ic);

	st_index[AVMEDIA_TYPE_VIDEO] =
			av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO,
								st_index[AVMEDIA_TYPE_VIDEO], -1, NULL, 0);
	st_index[AVMEDIA_TYPE_AUDIO] =
			av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO,
								st_index[AVMEDIA_TYPE_AUDIO],
								st_index[AVMEDIA_TYPE_VIDEO],
								NULL, 0);
	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
		AVStream *st = ic->streams[st_index[AVMEDIA_TYPE_VIDEO]];
		AVCodecParameters *codecpar = st->codecpar;
		AVRational sar = av_guess_sample_aspect_ratio(ic, st, NULL);
		if (codecpar->width)
			set_default_window_size(codecpar->width, codecpar->height, sar);
	}

	/* open the streams */
	if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
		stream_component_open(is, st_index[AVMEDIA_TYPE_AUDIO]);
	}

	ret = -1;
	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
		ret = stream_component_open(is, st_index[AVMEDIA_TYPE_VIDEO]);
	}

	if (is->video_stream < 0 && is->audio_stream < 0) {
		av_log(NULL, AV_LOG_FATAL, "Failed to open file '%s' or configure filtergraph\n",
			   is->filename);
		ret = -1;
		goto fail;
	}

	if (infinite_buffer < 0 && is->realtime)
		infinite_buffer = 1;

	for (;;) {
		if (is->abort_request)
			break;
		if (is->paused != is->last_paused) {
			is->last_paused = is->paused;
			if (is->paused)
				is->read_pause_return = av_read_pause(ic);
			else
				av_read_play(ic);
		}
		if (is->queue_attachments_req) {
			if (is->video_st && is->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC) {
				AVPacket copy = { 0 };
				if ((ret = av_packet_ref(&copy, &is->video_st->attached_pic)) < 0)
					goto fail;
				packet_queue_put(&is->videoq, &copy);
				packet_queue_put_nullpacket(&is->videoq, is->video_stream);
			}
			is->queue_attachments_req = 0;
		}

		/* if the queue are full, no need to read more */
		if (infinite_buffer<1 &&
			(is->audioq.size + is->videoq.size > MAX_QUEUE_SIZE
			 || (stream_has_enough_packets(is->audio_st, is->audio_stream, &is->audioq) &&
				 stream_has_enough_packets(is->video_st, is->video_stream, &is->videoq)))) {
			/* wait 10 ms */
			SDL_LockMutex(wait_mutex);
			SDL_CondWaitTimeout(is->continue_read_thread, wait_mutex, 10);
			SDL_UnlockMutex(wait_mutex);
			continue;
		}
		if (!is->paused &&
			(!is->audio_st || (is->auddec.finished == is->audioq.serial && frame_queue_nb_remaining(&is->sampq) == 0)) &&
			(!is->video_st || (is->viddec.finished == is->videoq.serial && frame_queue_nb_remaining(&is->pictq) == 0))) {
			ret = AVERROR_EOF;
			goto fail;
		}
		ret = av_read_frame(ic, pkt);
		if (ret < 0) {
			if ((ret == AVERROR_EOF || avio_feof(ic->pb)) && !is->eof) {
				if (is->video_stream >= 0)
					packet_queue_put_nullpacket(&is->videoq, is->video_stream);
				if (is->audio_stream >= 0)
					packet_queue_put_nullpacket(&is->audioq, is->audio_stream);
				is->eof = 1;
			}
			if (ic->pb && ic->pb->error)
				break;
			SDL_LockMutex(wait_mutex);
			SDL_CondWaitTimeout(is->continue_read_thread, wait_mutex, 10);
			SDL_UnlockMutex(wait_mutex);
			continue;
		} else {
			is->eof = 0;
		}
		/* check if packet is in play range specified by user, then queue, otherwise discard */
		stream_start_time = ic->streams[pkt->stream_index]->start_time;
		pkt_ts = pkt->pts == AV_NOPTS_VALUE ? pkt->dts : pkt->pts;
		if (pkt->stream_index == is->audio_stream) {
			packet_queue_put(&is->audioq, pkt);
		} else if (pkt->stream_index == is->video_stream
				   && !(is->video_st->disposition & AV_DISPOSITION_ATTACHED_PIC)) {
			packet_queue_put(&is->videoq, pkt);
		} else {
			av_packet_unref(pkt);
		}
	}

	ret = 0;
fail:
	if (ic && !is->ic)
		avformat_close_input(&ic);

	if (ret != 0) {
		SDL_Event event;
		event.type = SDL_QUIT;
		SDL_PushEvent(&event);
	}
	SDL_DestroyMutex(wait_mutex);
	return 0;
}

static VideoState *stream_open(const char *filename, int startup_volume)
{
	VideoState *is;

	is = (VideoState *) av_mallocz(sizeof(VideoState));
	if (!is)
		return NULL;
	is->filename = av_strdup(filename);
	if (!is->filename)
		goto fail;
	is->ytop    = 0;
	is->xleft   = 0;

	/* start video display */
    if (frame_queue_init(&is->pictq, &is->videoq, VIDEO_PICTURE_QUEUE_SIZE, 1) < 0)
		goto fail;
    if (frame_queue_init(&is->sampq, &is->audioq, SAMPLE_QUEUE_SIZE, 1) < 0)
		goto fail;

    if (packet_queue_init(&is->videoq) < 0 ||
        packet_queue_init(&is->audioq) < 0)
		goto fail;

	if (!(is->continue_read_thread = SDL_CreateCond())) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateCond(): %s\n", SDL_GetError());
		goto fail;
	}

	init_clock(&is->vidclk, &is->videoq.serial);
	init_clock(&is->audclk, &is->audioq.serial);
	init_clock(&is->extclk, &is->extclk.serial);
	is->audio_clock_serial = -1;
	if (startup_volume < 0)
		av_log(NULL, AV_LOG_WARNING, "-volume=%d < 0, setting to 0\n", startup_volume);
	if (startup_volume > 100)
		av_log(NULL, AV_LOG_WARNING, "-volume=%d > 100, setting to 100\n", startup_volume);
	startup_volume = av_clip(startup_volume, 0, 100);
	startup_volume = av_clip(SDL_MIX_MAXVOLUME * startup_volume / 100, 0, SDL_MIX_MAXVOLUME);
	is->audio_volume = startup_volume;
	is->muted = 0;
	is->av_sync_type = av_sync_type;
	return is;
fail:
	stream_close(is);
	return NULL;

}

static void refresh_loop_wait_event(VideoState *is, SDL_Event *event) {
	double remaining_time = 0.0;
	SDL_PumpEvents();
	while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
	if (is_full_screen && !cursor_hidden && av_gettime_relative() - cursor_last_shown > CURSOR_HIDE_DELAY) {
			SDL_ShowCursor(0);
			cursor_hidden = 1;
		}
		if (remaining_time > 0.0)
			av_usleep((int64_t)(remaining_time * 1000000.0));
		remaining_time = REFRESH_RATE;
		if (!is->paused || is->force_refresh)
			video_refresh(is, &remaining_time);
		SDL_PumpEvents();
	}
}

#else  // USE_SDL_FFMPEG

struct VideoState {
};

#endif // USE_SDL_FFMPEG

aui_SDLMovie::aui_SDLMovie(AUI_ERRCODE *retval, const MBCHAR * filename) :
	aui_Movie(),
	m_videoState(NULL),
	m_renderer(NULL),
	m_background(NULL),
	m_windowWidth(0),
	m_windowHeight(0),
	m_logicalWidth(0),
	m_logicalHeight(0)
{
	strncpy(m_filename, filename, sizeof(m_filename));
	m_filename[MAX_PATH] = '\0';
	*retval = AUI_ERRCODE_OK;
}

aui_SDLMovie::~aui_SDLMovie() {
	Assert(!m_videoState);
	m_renderer = NULL;
	m_background = NULL;
}

void aui_SDLMovie::SetContext(SDL_Renderer *renderer, SDL_Texture *background, const int windowWidth,
		const int windowHeight) {
	Assert(renderer);
	Assert(background);
	m_renderer = renderer;
	m_background = background;
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
}

AUI_ERRCODE aui_SDLMovie::Load() {
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_SDLMovie::Unload() {
	return Close();
}

AUI_ERRCODE aui_SDLMovie::Open(uint32 flags, aui_Surface *surface, RECT *rect) {

#if defined(USE_SDL_FFMPEG)
	if (!m_renderer || (!(flags & k_AUI_MOVIE_PLAYFLAG_ONSCREEN) && !m_background)) {
		return AUI_ERRCODE_MOVIEFAILED;
	}

	static SDL_RendererInfo renderer_info = {0};
	SDL_GetRendererInfo(m_renderer, &renderer_info);
	if (!renderer_info.num_texture_formats) {
		av_log(NULL, AV_LOG_FATAL, "Failed to create window or renderer: %s", SDL_GetError());
		Close();
		return AUI_ERRCODE_MOVIEFAILED;
	}

	m_flags = flags;
	renderer = m_renderer;

	av_init_packet(&flush_pkt);
	flush_pkt.data = (uint8_t *)&flush_pkt;

	is_full_screen = m_flags & k_AUI_MOVIE_PLAYFLAG_ONSCREEN;
	SDL_RenderGetLogicalSize(renderer, &m_logicalWidth, &m_logicalHeight);
	if (is_full_screen) {
		screen_width = m_windowWidth;
		screen_height = m_windowHeight;
		SDL_RenderSetLogicalSize(renderer, m_windowWidth, m_windowHeight);
	} else {
		screen_width = rect->right - rect->left;
		screen_height = rect->bottom - rect->top;
		background = m_background;
	}

	// Adjust volume range ctp2 [0..10] -> ffmpeg [0..100]
	m_videoState = stream_open(m_filename, g_theProfileDB->GetSFXVolume() * 10);
	if (m_videoState && !(m_flags & k_AUI_MOVIE_PLAYFLAG_ONSCREEN)) {
		m_videoState->xleft = rect->left;
		m_videoState->ytop = rect->top;
	}
	return m_videoState ? AUI_ERRCODE_OK : AUI_ERRCODE_MOVIEFAILED;
#else // USE_SDL_FFMPEG
	return AUI_ERRCODE_OK;
#endif // USE_SDL_FFMPEG
}

AUI_ERRCODE aui_SDLMovie::Close() {

#if defined(USE_SDL_FFMPEG)
	SDL_ShowCursor(SDL_ENABLE);
	if (m_videoState) {
		Stop();
		if (m_flags & k_AUI_MOVIE_PLAYFLAG_PLAYANDHOLD) {
			GrabLastFrame();
		}
		stream_close(m_videoState);
		m_videoState = NULL;
	}

	if (m_flags & k_AUI_MOVIE_PLAYFLAG_ONSCREEN) {
		SDL_RenderSetLogicalSize(m_renderer, m_logicalWidth, m_logicalHeight);
	}
	renderer = NULL;
	background = NULL;
	m_flags = 0;
	m_isFinished = TRUE;
#endif // USE_SDL_FFMPEG
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_SDLMovie::Play() {

#if defined(USE_SDL_FFMPEG)
	if (m_videoState) {
		m_videoState->read_tid = SDL_CreateThread(read_thread, "read_thread", m_videoState);
		if (!m_videoState->read_tid) {
			av_log(NULL, AV_LOG_FATAL, "SDL_CreateThread(): %s\n", SDL_GetError());
			Stop();
			return AUI_ERRCODE_MOVIEFAILED;
		}
	}
#endif // USE_SDL_FFMPEG
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_SDLMovie::Stop() {
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_SDLMovie::Pause() {

#if defined(USE_SDL_FFMPEG)
	if (m_videoState && !m_videoState->paused) {
		toggle_pause(m_videoState);
	}
#endif // USE_SDL_FFMPEG
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_SDLMovie::Resume() {

#if defined(USE_SDL_FFMPEG)
	if (m_videoState && m_videoState->paused) {
		toggle_pause(m_videoState);
	}
#endif // USE_SDL_FFMPEG
}

AUI_ERRCODE aui_SDLMovie::Process() {

#if defined(USE_SDL_FFMPEG)
	if (m_videoState) {
		/* handle an event sent by the GUI */
		SDL_Event event;

		bool movieFinished = false;
		while (!movieFinished) {
			refresh_loop_wait_event(m_videoState, &event);
			movieFinished = HandleMovieEvent(event);
		}
		Close();
	}
#endif // USE_SDL_FFMPEG
}

BOOL aui_SDLMovie::IsOpen() const {
	return m_videoState != NULL || m_isFinished;
}

BOOL aui_SDLMovie::IsPlaying() const {
#if defined(USE_SDL_FFMPEG)
	return m_videoState && m_videoState->read_tid;
#else // USE_SDL_FFMPEG
	return FALSE;
#endif // USE_SDL_FFMPEG
}

BOOL aui_SDLMovie::IsPaused() const {

#if defined(USE_SDL_FFMPEG)
	return m_videoState && m_videoState->paused;
#else // USE_SDL_FFMPEG
	return FALSE;
#endif // USE_SDL_FFMPEG
}

bool aui_SDLMovie::HandleMovieEvent(SDL_Event &event) {
	bool movieFinished = false;

	switch (event.type) {
		case SDL_QUIT:
			movieFinished = true;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_SPACE:
				case SDLK_q:
					movieFinished = true;
					break;
				case SDLK_p:
					toggle_pause(m_videoState);
					break;
				default:
					break;
			}
			break;
		case SDL_MOUSEMOTION:
			if (cursor_hidden) {
				SDL_ShowCursor(1);
				cursor_hidden = 0;
			} else {
				if ((event.motion.type & SDL_BUTTON_LMASK != 0) &&
					(is_full_screen || InsideMovieArea(event.button.x, event.button.y))) {
					movieFinished = true;
				}
			}
			cursor_last_shown = av_gettime_relative();
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (is_full_screen || InsideMovieArea(event.button.x, event.button.y)) {
				movieFinished = true;
			}
			break;
		default:
			break;
	}
	return movieFinished;
}


bool aui_SDLMovie::InsideMovieArea(int x, int y) {
	return (x > m_videoState->xleft && x < (m_videoState->xleft + m_videoState->width)
		&& y > m_videoState->ytop && y < (m_videoState->ytop + m_videoState->height));
}

void aui_SDLMovie::GrabLastFrame() {
	aui_SDLSurface *sdlSurface = dynamic_cast<aui_SDLSurface *>(GetDestSurface());
	if (sdlSurface) {
		// Undo logical size if needed; to grab unscaled pixels
		if (m_windowWidth != m_logicalWidth) {
			SDL_RenderSetLogicalSize(m_renderer, m_windowWidth, m_windowHeight);
		}

		SDL_Surface *surface = sdlSurface->GetSDLSurface();
		SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(m_renderer, surface);
		if (backgroundTexture) {
			SDL_RenderClear(m_renderer);

			SDL_Rect surfaceRect = { 0, 0, surface->w, surface->h };
			SDL_RenderCopy(m_renderer, backgroundTexture, NULL, &surfaceRect);

			RECT *destRect = GetDestRect();
			SDL_Rect sdlSrcRect = {0, 0, destRect->right - destRect->left, destRect->bottom - destRect->top };
			SDL_Rect sdlDestRect = { destRect->left, destRect->top, sdlSrcRect.w, sdlSrcRect.h };
			SDL_RenderCopy(m_renderer, m_videoState->vid_texture, &sdlSrcRect, &sdlDestRect);
			SDL_RenderReadPixels(m_renderer, &surfaceRect, surface->format->format, surface->pixels, surface->pitch);

			SDL_RenderClear(m_renderer);
			SDL_DestroyTexture(backgroundTexture);
		}

		// Reset logical size if needed
		if (m_windowWidth != m_logicalWidth) {
			SDL_RenderSetLogicalSize(m_renderer, m_logicalWidth, m_logicalHeight);
		}
	}
}
#endif // defined(__AUI_USE_SDL__)
