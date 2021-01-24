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

#ifndef _XF_INRANGE_HPP_
#define _XF_INRANGE_HPP_

#ifndef __cplusplus
#error C++ is needed to include this header
#endif

typedef unsigned short  uint16_t;
typedef unsigned char  uchar;

#include "hls_stream.h"
#include "common/xf_common.h"
#include "common/xf_utility.h"

namespace xf{

/**
 * inRange Kernel: Thresholds an input image and produces an output boolean image
 * Input   : src, upper_thresh and lower_thresh
 * Output  : dst
 */
template<int SRC_T, int ROWS, int COLS,int DEPTH, int NPC, int WORDWIDTH_SRC,
int WORDWIDTH_DST, int COLS_TRIP>
void xFinRangeKernel(xf::Mat<SRC_T, ROWS, COLS, NPC> & _src_mat, xf::Mat<SRC_T, ROWS, COLS, NPC> & _dst_mat,
		unsigned char lower_thresh,unsigned char upper_thresh, unsigned short height, unsigned short width)
{
	XF_SNAME(WORDWIDTH_SRC) val_src;
	XF_SNAME(WORDWIDTH_DST) val_dst;
	XF_PTNAME(DEPTH)  p;//,out;
	XF_PTNAME(DEPTH)  _lower_thresh=(XF_PTNAME(DEPTH))lower_thresh;
	XF_PTNAME(DEPTH)  _upper_thresh=(XF_PTNAME(DEPTH))upper_thresh;
	ap_uint<13> i, j, k;
	rowLoop:
	for(i = 0; i < height; i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS
#pragma HLS LOOP_FLATTEN off

		colLoop:
		for(j = 0; j < width; j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=COLS_TRIP max=COLS_TRIP
#pragma HLS pipeline

			val_src = (XF_SNAME(WORDWIDTH_SRC)) (_src_mat.data[i*width+j]);  //reading the source stream _src into val_src

			for(k = 0; k < (XF_WORDDEPTH(WORDWIDTH_SRC));k += XF_PIXELDEPTH(DEPTH))
			{
#pragma HLS unroll
				p=val_src.range(k+(XF_PIXELDEPTH(DEPTH)-1),k);

				val_dst.range(k + (XF_PIXELDEPTH(DEPTH)-1), k) = (( p >= _lower_thresh) && ( p <= _upper_thresh))? (ap_uint<8>)255 : (ap_uint<8>)0;

			}

			_dst_mat.data[i*width+j] = (val_dst);  //writing the val_dst into output stream _dst
		}
	}
}


#pragma SDS data access_pattern("src.data":SEQUENTIAL, "dst.data":SEQUENTIAL)
#pragma SDS data copy("src.data"[0:"src.size"], "dst.data"[0:"dst.size"])
#pragma SDS data mem_attribute("src.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("dst.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
/* inRange API call*/
template<int SRC_T, int ROWS, int COLS,int NPC=1>
void inRange(xf::Mat<SRC_T, ROWS, COLS, NPC> & src,unsigned char lower_thresh,unsigned char upper_thresh,xf::Mat<SRC_T, ROWS, COLS, NPC> & dst)
{

	unsigned short width = src.cols >> XF_BITSHIFT(NPC);
	unsigned short height = src.rows;

	assert((SRC_T == XF_8UC1) && "Type must be XF_8UC1");
	assert(((NPC == XF_NPPC1) || (NPC == XF_NPPC8) ) &&
			"NPC must be XF_NPPC1, XF_NPPC8");

	assert(((lower_thresh >= 0) && (lower_thresh <= 255)) &&
			"lower_thresh must be with the range of 0 to 255");

	assert(((upper_thresh >= 0) && (upper_thresh <= 255)) &&
			"lower_thresh must be with the range of 0 to 255");

	assert(((height <= ROWS ) && (width <= COLS)) && "ROWS and COLS should be greater than input image");

#pragma HLS INLINE OFF

	xFinRangeKernel<SRC_T, ROWS,COLS,XF_DEPTH(SRC_T,NPC),NPC,XF_WORDWIDTH(SRC_T,NPC),XF_WORDWIDTH(SRC_T,NPC),(COLS>>XF_BITSHIFT(NPC))>
	(src,dst,lower_thresh,upper_thresh,height,width);

}
}

#endif
