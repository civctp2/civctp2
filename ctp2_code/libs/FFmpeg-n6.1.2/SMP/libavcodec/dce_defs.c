/** libavcodec DCE definitions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "config.h"
#include "stdint.h"
#include "config_components.h"

#if !((HAVE_6REGS && HAVE_MMX_INLINE)||(HAVE_6REGS && HAVE_MMXEXT_INLINE))
#include "libavcodec/x86/vc1dsp.h"
#endif
#if !(ARCH_X86_64)
#include "libavcodec/x86/simple_idct.h"
#endif
#if !(ARCH_X86_64)
#include "libavcodec/x86/hevcdsp.h"
#endif
#if !(HAVE_SSE2_INLINE)
#include "libavcodec/x86/fdct.h"
#endif

#if !(HAVE_SSE2_INLINE)
void ff_fdct_sse2(int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_flac_decorrelate_indep8_16_avx(uint8_t **out, int32_t **in, int channels, int len, int shift) {return;}
#endif
#if !(ARCH_X86_64)
void ff_flac_decorrelate_indep8_16_ssse3(uint8_t **out, int32_t **in, int channels, int len, int shift) {return;}
#endif
#if !(ARCH_X86_64)
void ff_flac_decorrelate_indep8_32_avx(uint8_t **out, int32_t **in, int channels, int len, int shift) {return;}
#endif
#if !(ARCH_X86_64)
void ff_flac_decorrelate_indep8_32_ssse3(uint8_t **out, int32_t **in, int channels, int len, int shift) {return;}
#endif
void ff_flac_enc_lpc_16_sse4(int32_t * param1, const int32_t * param2, int param3, int param4, const int32_t * param5,int param6) {return;}
#if !(ARCH_X86_64)
void ff_hevc_h_loop_filter_luma_10_avx(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_h_loop_filter_luma_10_sse2(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_h_loop_filter_luma_10_ssse3(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_h_loop_filter_luma_12_avx(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_h_loop_filter_luma_12_sse2(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_h_loop_filter_luma_12_ssse3(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_h_loop_filter_luma_8_avx(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_h_loop_filter_luma_8_sse2(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_h_loop_filter_luma_8_ssse3(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_idct_16x16_10_avx(int16_t *coeffs, int col_limit) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_idct_16x16_10_sse2(int16_t *coeffs, int col_limit) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_idct_16x16_8_avx(int16_t *coeffs, int col_limit) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_idct_16x16_8_sse2(int16_t *coeffs, int col_limit) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_idct_32x32_10_avx(int16_t *coeffs, int col_limit) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_idct_32x32_10_sse2(int16_t *coeffs, int col_limit) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_idct_32x32_8_avx(int16_t *coeffs, int col_limit) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_idct_32x32_8_sse2(int16_t *coeffs, int col_limit) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_h8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_hv8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_epel_v8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_pel_pixels8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_h8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_hv8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_qpel_v8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_h8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_hv8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_epel_v8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_pel_pixels8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_h8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_hv8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_bi_w_qpel_v8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, const int16_t *src2, int height, int denom, int wx0, int wx1, int ox0, int ox1, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h12_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h12_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h12_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h16_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h16_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h16_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h16_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h24_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h24_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h24_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h24_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h32_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h32_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h32_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h32_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h32_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h48_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h48_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h48_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h48_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h48_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h4_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h4_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h4_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h64_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h64_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h64_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h64_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h64_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h6_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h6_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h6_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h8_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h8_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_h8_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv12_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv12_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv12_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv16_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv16_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv16_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv16_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv24_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv24_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv24_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv24_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv32_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv32_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv32_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv32_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv32_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv48_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv48_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv48_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv48_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv48_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv4_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv4_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv4_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv64_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv64_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv64_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv64_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv64_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv6_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv6_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv6_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv8_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv8_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_hv8_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v12_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v12_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v12_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v16_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v16_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v16_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v16_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v24_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v24_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v24_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v24_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v32_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v32_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v32_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v32_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v32_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v48_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v48_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v48_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v48_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v48_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v4_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v4_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v4_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v64_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v64_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v64_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v64_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v64_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v6_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v6_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v6_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v8_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v8_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_epel_v8_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels12_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels12_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels12_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels16_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels16_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels16_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels16_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels24_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels24_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels24_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels24_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels32_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels32_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels32_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels32_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels32_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels48_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels48_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels48_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels48_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels48_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels4_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels4_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels4_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels64_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels64_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels64_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels64_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels64_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels6_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels6_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels6_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels8_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels8_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_pel_pixels8_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h12_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h12_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h12_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h16_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h16_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h16_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h16_8_avx512icl(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h16_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h24_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h24_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h24_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h24_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h32_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h32_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h32_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h32_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h32_8_avx512icl(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h32_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h48_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h48_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h48_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h48_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h48_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h4_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h4_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h4_8_avx512icl(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h4_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h64_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h64_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h64_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h64_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h64_8_avx512icl(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h64_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h8_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h8_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h8_8_avx512icl(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_h8_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv12_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv12_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv12_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv16_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv16_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv16_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv16_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv24_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv24_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv24_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv24_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv32_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv32_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv32_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv32_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv48_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv48_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv48_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv48_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv4_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv4_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv4_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv64_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv64_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv64_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv64_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv8_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv8_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv8_8_avx512icl(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_hv8_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v12_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v12_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v12_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v16_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v16_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v16_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v16_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v24_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v24_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v24_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v24_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v32_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v32_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v32_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v32_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v32_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v48_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v48_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v48_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v48_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v48_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v4_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v4_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v4_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v64_10_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v64_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v64_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v64_8_avx2(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v64_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v8_10_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v8_12_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_qpel_v8_8_sse4(int16_t *dst, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_h8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_hv8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_epel_v8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels128_8_avx2(uint8_t *dst, ptrdiff_t dststride,const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels32_8_avx2(uint8_t *dst, ptrdiff_t dststride,const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels48_8_avx2(uint8_t *dst, ptrdiff_t dststride,const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels64_8_avx2(uint8_t *dst, ptrdiff_t dststride,const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_pel_pixels96_8_avx2(uint8_t *dst, ptrdiff_t dststride,const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my,int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_h8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_hv8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v16_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v24_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v32_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v32_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v48_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v48_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v64_10_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v64_8_avx2(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_qpel_v8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_h8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_hv8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_epel_v8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels6_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels6_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels6_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_pel_pixels8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_h8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_hv8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v12_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v12_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v12_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v16_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v16_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v16_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v24_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v24_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v24_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v32_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v32_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v32_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v48_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v48_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v48_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v4_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v4_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v4_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v64_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v64_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v64_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v8_10_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v8_12_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_put_hevc_uni_w_qpel_v8_8_sse4(uint8_t *_dst, ptrdiff_t _dststride, const uint8_t *_src, ptrdiff_t _srcstride, int height, int denom, int wx, int ox, intptr_t mx, intptr_t my, int width) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_v_loop_filter_luma_10_avx(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_v_loop_filter_luma_10_sse2(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_v_loop_filter_luma_10_ssse3(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_v_loop_filter_luma_12_avx(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_v_loop_filter_luma_12_sse2(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_v_loop_filter_luma_12_ssse3(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_v_loop_filter_luma_8_avx(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_v_loop_filter_luma_8_sse2(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_hevc_v_loop_filter_luma_8_ssse3(uint8_t *pix, ptrdiff_t stride, int beta, const int *tc, const uint8_t *no_p, const uint8_t *no_q) {return;}
#endif
#if !(ARCH_X86_64)
void ff_mlp_rematrix_channel_avx2_bmi2(int32_t *samples, const int32_t *coeffs, const uint8_t *bypassed_lsbs, const int8_t *noise_buffer, int index, unsigned int dest_ch, uint16_t blockpos, unsigned int maxchan, int matrix_noise_shift, int access_unit_size_pow2, int32_t mask) {return;}
#endif
#if !(ARCH_X86_64)
void ff_mlp_rematrix_channel_sse4(int32_t *samples, const int32_t *coeffs, const uint8_t *bypassed_lsbs, const int8_t *noise_buffer, int index, unsigned int dest_ch, uint16_t blockpos, unsigned int maxchan, int matrix_noise_shift, int access_unit_size_pow2, int32_t mask) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct10_avx(int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct10_put_avx(uint8_t *dest, ptrdiff_t line_size, int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct10_put_sse2(uint8_t *dest, ptrdiff_t line_size, int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct10_sse2(int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct12_avx(int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct12_put_avx(uint8_t *dest, ptrdiff_t line_size, int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct12_put_sse2(uint8_t *dest, ptrdiff_t line_size, int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct12_sse2(int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct8_add_avx(uint8_t *dest, ptrdiff_t line_size, int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct8_add_sse2(uint8_t *dest, ptrdiff_t line_size, int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct8_avx(int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct8_put_avx(uint8_t *dest, ptrdiff_t line_size, int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct8_put_sse2(uint8_t *dest, ptrdiff_t line_size, int16_t *block) {return;}
#endif
#if !(ARCH_X86_64)
void ff_simple_idct8_sse2(int16_t *block) {return;}
#endif
#if !(HAVE_6REGS && HAVE_MMX_INLINE)
void ff_vc1dsp_init_mmx(VC1DSPContext *dsp) {return;}
#endif
#if !(HAVE_6REGS && HAVE_MMXEXT_INLINE)
void ff_vc1dsp_init_mmxext(VC1DSPContext *dsp) {return;}
#endif
#if !(ARCH_X86_64)
void ff_vp9_iadst_iadst_16x16_add_avx2(uint8_t *dst, ptrdiff_t stride, int16_t *block, int eob) {return;}
#endif
#if !(ARCH_X86_64)
void ff_vp9_iadst_idct_16x16_add_avx2(uint8_t *dst, ptrdiff_t stride, int16_t *block, int eob) {return;}
#endif
#if !(ARCH_X86_64)
void ff_vp9_idct_iadst_16x16_add_avx2(uint8_t *dst, ptrdiff_t stride, int16_t *block, int eob) {return;}
#endif
#if !(ARCH_X86_64)
void ff_vp9_idct_idct_16x16_add_avx2(uint8_t *dst, ptrdiff_t stride, int16_t *block, int eob) {return;}
#endif
#if !(ARCH_X86_64)
void ff_vp9_idct_idct_32x32_add_avx2(uint8_t *dst, ptrdiff_t stride, int16_t *block, int eob) {return;}
#endif
