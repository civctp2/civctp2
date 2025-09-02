/** libavformat DCE definitions
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

#include "libavformat/rtmpcrypt.h"

int ff_rtmpe_compute_secret_key(URLContext *h, const uint8_t *serverdata,
                                const uint8_t *clientdata, int type) {return 0;}
int ff_rtmpe_gen_pub_key(URLContext *h, uint8_t *buf) {return 0;}
int ff_rtmpe_update_keystream(URLContext *h) {return 0;}
void ff_rtmpe_encrypt_sig(URLContext *h, uint8_t *signature,
                          const uint8_t *digest, int type) {return;}
