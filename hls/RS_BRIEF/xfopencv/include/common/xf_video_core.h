/***************************************************************************
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
may be used to endorse or promote products derived from this software 
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

/*
 * HLS Video Core Structures Header File
 */

#ifndef ___XF__VIDEO_CORE__
#define ___XF__VIDEO_CORE__

namespace xf {

/* Video Format Types defined on 8-bit boundaries */
typedef struct{
  unsigned char Y;
  char UV;
}yuv422_8;

typedef struct{
  unsigned char Y;
  char U;
  char V;
}yuv444_8;

typedef struct{
  unsigned char R;
  unsigned char G;
  unsigned char B;
}rgb_8;

typedef yuv422_8 yuv420_8;

typedef struct{
  char A;
  unsigned char Y;
  char UV;
}yuva422_8;

typedef struct{
  char A;
  unsigned char Y;
  char U;
  char V;
}yuva444_8;

typedef struct{
  char A;
  unsigned char R;
  unsigned char G;
  unsigned char B;
}rgba_8;

typedef struct{
  char AUV;
  unsigned char Y;
}yuva420_8;

typedef struct{
  unsigned char D;
  unsigned char Y;
  char UV;
}yuvd422_8;

typedef struct{
  unsigned char D;
  unsigned char Y;
  char U;
  char V;
}yuvd444_8;

typedef struct{
  unsigned char D;
  unsigned char R;
  unsigned char G;
  unsigned char B;
}rgbd_8;

typedef yuvd422_8 yuvd420_8;

typedef struct{
  unsigned char CMY;
}bayer_8;


typedef struct{
  unsigned char Y;
}luma_8;
/* convertors for different pixel types*/

/* yuv422_8 and yuv420_8 */
template<typename T>
yuv422_8 Scalar_to_yuv422_8(Scalar<2, T> scl) {
#pragma HLS inline
    yuv422_8 pix;
    pix.Y = (unsigned char)scl.val[0];
    pix.UV = (char)scl.val[1];
    return pix;
}
template<typename T>
Scalar<2, T> yuv422_8_to_Scalar(yuv422_8 pix) {
#pragma HLS inline
    Scalar<2, T> scl;
    scl.val[0] = (T)pix.Y;
    scl.val[1] = (T)pix.UV;
    return scl;
}

/* yuv444_8 */
template<typename T>
yuv444_8 Scalar_to_yuv444_8(Scalar<3, T> scl) {
#pragma HLS inline
    yuv444_8 pix;
    pix.Y = (unsigned char)scl.val[0];
    pix.U = (char)scl.val[1];
    pix.V = (char)scl.val[2];
    return pix;
}
template<typename T>
Scalar<3, T> yuv444_8_to_Scalar(yuv444_8 pix) {
#pragma HLS inline
    Scalar<3, T> scl;
    scl.val[0] = (T)pix.Y;
    scl.val[1] = (T)pix.U;
    scl.val[2] = (T)pix.V;
    return scl;
}

/* rgb_8 */
template<typename T>
rgb_8 Scalar_to_rgb_8(Scalar<3, T> scl) {
#pragma HLS inline
    rgb_8 pix;
    pix.R = (unsigned char)scl.val[0];
    pix.G = (unsigned char)scl.val[1];
    pix.B = (unsigned char)scl.val[2];
    return pix;
}
template<typename T>
Scalar<3, T> rgb_8_to_Scalar(rgb_8 pix) {
#pragma HLS inline
    Scalar<3, T> scl;
    scl.val[0] = (T)pix.R;
    scl.val[1] = (T)pix.G;
    scl.val[2] = (T)pix.B;
    return scl;
}

/* yuva422_8 */
template<typename T>
yuva422_8 Scalar_to_yuva422_8(Scalar<3, T> scl) {
#pragma HLS inline
    yuva422_8 pix;
    pix.A = (char)scl.val[0];
    pix.Y = (unsigned char)scl.val[1];
    pix.UV = (char)scl.val[2];
    return pix;
}
template<typename T>
Scalar<3, T> yuva422_8_to_Scalar(yuva422_8 pix) {
#pragma HLS inline
    Scalar<3, T> scl;
    scl.val[0] = (T)pix.A;
    scl.val[1] = (T)pix.Y;
    scl.val[2] = (T)pix.UV;
    return scl;
}

/* yuva444_8 */
template<typename T>
yuva444_8 Scalar_to_yuva444_8(Scalar<4, T> scl) {
#pragma HLS inline
    yuva444_8 pix;
    pix.A = (char)scl.val[0];
    pix.Y = (unsigned char)scl.val[1];
    pix.U = (char)scl.val[2];
    pix.V = (char)scl.val[3];
    return pix;
}
template<typename T>
Scalar<4, T> yuva444_8_to_Scalar(yuva444_8 pix) {
#pragma HLS inline
    Scalar<4, T> scl;
    scl.val[0] = (T)pix.A;
    scl.val[1] = (T)pix.Y;
    scl.val[2] = (T)pix.U;
    scl.val[3] = (T)pix.V;
    return scl;
}

/* rgba_8 */
template<typename T>
rgba_8 Scalar_to_rgba_8(Scalar<4, T> scl) {
#pragma HLS inline
    rgba_8 pix;
    pix.A = (char)scl.val[0];
    pix.R = (unsigned char)scl.val[1];
    pix.G = (unsigned char)scl.val[2];
    pix.B = (unsigned char)scl.val[3];
    return pix;
}
template<typename T>
Scalar<4, T> rgba_8_to_Scalar(rgba_8 pix) {
#pragma HLS inline
    Scalar<4, T> scl;
    scl.val[0] = (T)pix.A;
    scl.val[1] = (T)pix.R;
    scl.val[2] = (T)pix.G;
    scl.val[3] = (T)pix.B;
    return scl;
}

/* yuva420_8 */
template<typename T>
yuva420_8 Scalar_to_yuva420_8(Scalar<2, T> scl) {
#pragma HLS inline
    yuva420_8 pix;
    pix.AUV = (char)scl.val[0];
    pix.Y = (unsigned char)scl.val[1];
    return pix;
}
template<typename T>
Scalar<2, T> yuva420_8_to_Scalar(yuva420_8 pix) {
#pragma HLS inline
    Scalar<2, T> scl;
    scl.val[0] = (T)pix.AUV;
    scl.val[1] = (T)pix.Y;
    return scl;
}

/* yuvd422_8 and yuvd420_8*/
template<typename T>
yuvd422_8 Scalar_to_yuvd422_8(Scalar<3, T> scl) {
#pragma HLS inline
    yuvd422_8 pix;
    pix.D = (unsigned char)scl.val[0];
    pix.Y = (unsigned char)scl.val[1];
    pix.UV = (char)scl.val[2];
    return pix;
}
template<typename T>
Scalar<3, T> yuvd422_8_to_Scalar(yuvd422_8 pix) {
#pragma HLS inline
    Scalar<3, T> scl;
    scl.val[0] = (T)pix.D;
    scl.val[1] = (T)pix.Y;
    scl.val[2] = (T)pix.UV;
    return scl;
}

/* yuvd444_8 */
template<typename T>
yuvd444_8 Scalar_to_yuvd444_8(Scalar<4, T> scl) {
#pragma HLS inline
    yuvd444_8 pix;
    pix.D = (unsigned char)scl.val[0];
    pix.Y = (unsigned char)scl.val[1];
    pix.U = (char)scl.val[2];
    pix.V = (char)scl.val[3];
    return pix;
}
template<typename T>
Scalar<4, T> yuvd444_8_to_Scalar(yuvd444_8 pix) {
#pragma HLS inline
    Scalar<4, T> scl;
    scl.val[0] = (T)pix.D;
    scl.val[1] = (T)pix.Y;
    scl.val[2] = (T)pix.U;
    scl.val[3] = (T)pix.V;
    return scl;
}

/* rgbd_8 */
template<typename T>
rgbd_8 Scalar_to_rgbd_8(Scalar<4, T> scl) {
#pragma HLS inline
    rgbd_8 pix;
    pix.D = (unsigned char)scl.val[0];
    pix.R = (unsigned char)scl.val[1];
    pix.G = (unsigned char)scl.val[2];
    pix.B = (unsigned char)scl.val[3];
    return pix;
}
template<typename T>
Scalar<4, T> rgbd_8_to_Scalar(rgbd_8 pix) {
#pragma HLS inline
    Scalar<4, T> scl;
    scl.val[0] = (T)pix.D;
    scl.val[1] = (T)pix.R;
    scl.val[2] = (T)pix.G;
    scl.val[3] = (T)pix.B;
    return scl;
}

/* bayer_8 */
template<typename T>
bayer_8 Scalar_to_bayer_8(Scalar<1, T> scl) {
#pragma HLS inline
    bayer_8 pix;
    pix.CMY = (unsigned char)scl.val[0];
    return pix;
}
template<typename T>
Scalar<1, T> bayer_8_to_Scalar(bayer_8 pix) {
#pragma HLS inline
    Scalar<1, T> scl;
    scl.val[0] = (T)pix.CMY;
    return scl;
}

/* luma_8 */
template<typename T>
luma_8 Scalar_to_luma_8(Scalar<1, T> scl) {
#pragma HLS inline
    luma_8 pix;
    pix.Y = (unsigned char)scl.val[0];
    return pix;
}
template<typename T>
Scalar<1, T> luma_8_to_Scalar(luma_8 pix) {
#pragma HLS inline
    Scalar<1, T> scl;
    scl.val[0] = (T)pix.Y;
    return scl;
}




} // namespace xf


#endif


