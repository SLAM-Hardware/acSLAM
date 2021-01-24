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

#ifndef _XF_BOX_FILTER_HPP_
#define _XF_BOX_FILTER_HPP_

#ifndef __cplusplus
#error C++ is needed to include this header
#endif

typedef unsigned short  uint16_t;

typedef unsigned int  uint32_t;

#include "hls_stream.h"
#include "common/xf_common.h"
#include "common/xf_utility.h"



/////  Division factor for various filter sizes  /////
#define XF_DIV_VAL_3x3 	3641   // 1/9  value converted into fixed point Q1.15 format.
#define XF_DIV_VAL_5x5  1311   // 1/25 value converted into fixed point Q1.15 format.
#define XF_DIV_VAL_7x7  669    // 1/49 value converted into fixed point Q1.15 format.

namespace xf{

/**
 * xFApplyMask3x3: apply a 3x3 mask
 *
 * --------------------
 * 		  |1    1 	 1|
 *	    1/9 * |1    1	 1|
 * 		  |1    1	 1|
 * --------------------
 *
 */
template<int DEPTH>
XF_PTNAME(DEPTH) xFApplyMask3x3(XF_PTNAME(DEPTH) _i00, XF_PTNAME(DEPTH) _i01,
		XF_PTNAME(DEPTH) _i02, XF_PTNAME(DEPTH) _i10, XF_PTNAME(DEPTH) _i11,
		XF_PTNAME(DEPTH) _i12, XF_PTNAME(DEPTH) _i20, XF_PTNAME(DEPTH) _i21,
		XF_PTNAME(DEPTH) _i22)
		{
#pragma HLS INLINE off
	XF_PTNAME(DEPTH) res;
	ap_int<20> res_fixed;
	ap_int18_t g00 = (_i00 + _i02);
	ap_int18_t g01 = (_i20 + _i22);
	ap_int18_t g10 = (_i01 + _i10);
	ap_int18_t g11 = (_i12 + _i21);
	ap_int18_t g0  = (g00 + g01);
	ap_int18_t g1  = (g10 + g11);
	ap_int18_t g2  = _i11;
	res_fixed = (g0 + g1 + g2);		
	res  = ((res_fixed* XF_DIV_VAL_3x3) >> 15);   // 1/9 in fixed point format(Q1.15)
	return res;
		}


/**
 * xFComputeMaskValue3x3 function:
 * If PO is enabled then 16 mask_value will be computed, by unrolling the filter_loop.
 * If RO is enabled then 8 mask_value  will be computed, by unrolling the filter_loop.
 */
template<int NPC, int DEPTH>
void xFComputeMaskValues3x3(XF_PTNAME(DEPTH)* _mask_value,
		XF_PTNAME(DEPTH)* _l00_buf, XF_PTNAME(DEPTH)* _l10_buf,
		XF_PTNAME(DEPTH)* _l20_buf)
{
#pragma HLS INLINE
	ap_uint<5> filter_loop = XF_NPIXPERCYCLE(NPC);

	computeMaskValueLoop:
	for(ap_uint<5> j = 0; j < filter_loop ; j++ )
	{
#pragma HLS unroll
		_mask_value[j] = xFApplyMask3x3<DEPTH>( _l00_buf[j], _l00_buf[j+1],
				_l00_buf[j+2], _l10_buf[j], _l10_buf[j+1], _l10_buf[j+2],
				_l20_buf[j], _l20_buf[j+1], _l20_buf[j+2]);
	} // end of computeMaskValueLoop
}

template<int ROWS, int COLS,int DEPTH, int NPC, int WORDWIDTH_SRC, int WORDWIDTH_DST, int COLS_COUNT>
void ProcessBox3x3(hls::stream< XF_SNAME(WORDWIDTH_SRC) > &_src,
		hls::stream< XF_SNAME(WORDWIDTH_DST) > &_dst, XF_SNAME(WORDWIDTH_SRC) buf[3][COLS>>XF_BITSHIFT(NPC)],
		XF_PTNAME(DEPTH) l00_buf[XF_NPIXPERCYCLE(NPC)+2], XF_PTNAME(DEPTH) l10_buf[XF_NPIXPERCYCLE(NPC)+2], XF_PTNAME(DEPTH) l20_buf[XF_NPIXPERCYCLE(NPC)+2],
		XF_PTNAME(DEPTH) mask_value[XF_NPIXPERCYCLE(NPC)], XF_SNAME(WORDWIDTH_SRC) &P0, uint16_t img_width, uint16_t img_height, uint16_t &shift, ap_uint<13> row_ind, ap_uint<2> top, ap_uint<2> mid, ap_uint<2> bottom, ap_uint<13> row)

{
#pragma HLS INLINE
	XF_SNAME(WORDWIDTH_SRC) buf0, buf1, buf2;

	ap_uint<8> npc = XF_NPIXPERCYCLE(NPC),
			buf_size = XF_NPIXPERCYCLE(NPC) + 2;

	colLoop1:
	for(ap_uint<13> col = 0; col < img_width; col++)   // Width of the image
	{
#pragma HLS LOOP_TRIPCOUNT min=COLS_COUNT max=COLS_COUNT
#pragma HLS pipeline
		if(row < img_height)
			buf[row_ind][col] = _src.read();
		else
			buf[bottom][col] = 0;

		buf0 = buf[top][col];
		buf1 = buf[mid][col];
		buf2 = buf[bottom][col];


		if(NPC == XF_NPPC8)
		{
			/*	   Extract the data from the packed pixels       */
			xfExtractPixels<NPC,WORDWIDTH_SRC,DEPTH>(&l00_buf[2],buf0,0);
			xfExtractPixels<NPC,WORDWIDTH_SRC,DEPTH>(&l10_buf[2],buf1,0);
			xfExtractPixels<NPC,WORDWIDTH_SRC,DEPTH>(&l20_buf[2],buf2,0);
		}
		else
		{
			l00_buf[2] = buf0;
			l10_buf[2] = buf1;
			l20_buf[2] = buf2;
		}
		xFComputeMaskValues3x3<NPC,DEPTH>(mask_value, l00_buf, l10_buf, l20_buf);

		if(col == 0)
		{
			shift = 0;
			P0 = 0;

			P0loop1:
			xfPackPixels<NPC, WORDWIDTH_SRC, DEPTH>(&mask_value[0], P0, 1, (npc-1), shift);
		}
		else
		{
			xfPackPixels<NPC, WORDWIDTH_SRC, DEPTH>(&mask_value[0], P0, 0, 1, shift);
			_dst.write(P0);
			shift = 0;
			P0 = 0;
			xfPackPixels<NPC, WORDWIDTH_SRC, DEPTH>(&mask_value[0], P0, 1, (npc-1), shift);

		}
		l00_buf[0] = l00_buf[buf_size-2];
		l00_buf[1] = l00_buf[buf_size-1];

		l10_buf[0] = l10_buf[buf_size-2];
		l10_buf[1] = l10_buf[buf_size-1];

		l20_buf[0] = l20_buf[buf_size-2];
		l20_buf[1] = l20_buf[buf_size-1];
	}  // end of colLoop1
}
/**
 * xFBoxFilter : Compute a Box filter of size 3x3 over a window of the input image.
 * Inputs : _src_mat --> input image of type XF_8U, XF_16U or XF_16S
 * Output : _dst_mat --> output image of input type
 */
template<int ROWS, int COLS,int DEPTH, int NPC, int WORDWIDTH_SRC,
int WORDWIDTH_DST, int COLS_COUNT,bool USE_URAM>
void xFBoxFilter3x3(hls::stream< XF_SNAME(WORDWIDTH_SRC) > &_src,
		hls::stream< XF_SNAME(WORDWIDTH_DST) > &_dst, uint16_t img_height, uint16_t img_width)
{
	ap_uint<13> row_ind = 1;
	ap_uint<13> row, col;
	uint16_t shift = 0;
	ap_uint<2> top, mid, bottom;
	ap_uint<8> buf_size = XF_NPIXPERCYCLE(NPC) + 2;

	XF_PTNAME(DEPTH) mask_value[XF_NPIXPERCYCLE(NPC)];
#pragma HLS ARRAY_PARTITION variable=mask_value complete dim=1

	XF_SNAME(WORDWIDTH_SRC) buf0, buf1, buf2;

	XF_PTNAME(DEPTH) l00_buf[XF_NPIXPERCYCLE(NPC)+2], l10_buf[XF_NPIXPERCYCLE(NPC)+2], l20_buf[XF_NPIXPERCYCLE(NPC)+2];			// Temporary buffers to hold image data from three rows.
#pragma HLS ARRAY_PARTITION variable=l00_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l10_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l20_buf complete dim=1

	XF_SNAME(WORDWIDTH_SRC) P0;
	XF_SNAME(WORDWIDTH_SRC) buf[3][COLS>>XF_BITSHIFT(NPC)];								// Line Buffer to hold image row data
if(USE_URAM)
{
#pragma HLS RESOURCE variable=buf core=RAM_S2P_URAM
#pragma HLS array reshape variable=buf dim=1 factor=3 cyclic	
}
else{
#pragma HLS RESOURCE variable=buf core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1
}

	bufColLoop:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=COLS_COUNT max=COLS_COUNT
#pragma HLS pipeline
		buf[0][col] = 0;
		buf[row_ind][col] = _src.read();
	}
	row_ind++;

	l00_buf[0] = l00_buf[1] = 0;
	l10_buf[0] = l10_buf[1] = 0;
	l20_buf[0] = l20_buf[1] = 0;

	ROWLOOP:
	for(row = 1; row < img_height+1; row++)    								// Height of the image
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS

		// update the row index
		if(row_ind == 2)
		{
			top = 0; mid = 1; bottom = 2;
		}
		else if(row_ind == 0)
		{
			top = 1; mid = 2; bottom = 0;
		}
		else if(row_ind == 1)
		{
			top = 2; mid = 0; bottom = 1;
		}

		l00_buf[0] = l00_buf[1] = 0;
		l10_buf[0] = l10_buf[1] = 0;
		l20_buf[0] = l20_buf[1] = 0;
		P0 = 0;

		ProcessBox3x3<ROWS, COLS, DEPTH, NPC, WORDWIDTH_SRC, WORDWIDTH_DST, COLS_COUNT>
		(_src, _dst, buf, l00_buf, l10_buf, l20_buf, mask_value, P0, img_width, img_height, shift, row_ind, top, mid, bottom, row);

		if(NPC == XF_NPPC1)
		{
			mask_value[0] = xFApplyMask3x3<DEPTH>( l00_buf[buf_size-3],		// Applying Mask
					l00_buf[buf_size-2], 0,
					l10_buf[buf_size-3],
					l10_buf[buf_size-2], 0,
					l20_buf[buf_size-3],
					l20_buf[buf_size-2], 0);					//	Take care of border at last column
		}
		else
		{
			mask_value[0] = xFApplyMask3x3<DEPTH>(l00_buf[buf_size-2],
					l00_buf[buf_size-1], 0,
					l10_buf[buf_size-2],
					l10_buf[buf_size-1], 0,
					l20_buf[buf_size-2],
					l20_buf[buf_size-1], 0);
		}
		xfPackPixels<NPC, WORDWIDTH_SRC, DEPTH>(&mask_value[0], P0, 0, 1, shift);
		_dst.write(P0);
		shift = 0;
		P0 = 0;
		row_ind++;
		if(row_ind == 3)
		{
			row_ind = 0;
		}
	}  // end of rowLoop

}  // end of function


/**
 * xFApplyMask5x5: apply a 5x5 mask
 *
 * ----------------------------
 * 		  |1	1 	1	1 	1|
 *		  |1	1	1	1 	1|
 * 	1/25* |1	1	1	1 	1|
 * 		  |1	1	1	1 	1|
 * 		  |1	1	1	1 	1|
 * ---------------------------
 */
template<int DEPTH_SRC, int  DEPTH_DST>
XF_PTNAME(DEPTH_SRC) xFGradient5x5(
		XF_PTNAME(DEPTH_SRC) D0,  XF_PTNAME(DEPTH_SRC) D1, XF_PTNAME(DEPTH_SRC) D2,  XF_PTNAME(DEPTH_SRC) D3,
		XF_PTNAME(DEPTH_SRC) D4,  XF_PTNAME(DEPTH_SRC) D5, XF_PTNAME(DEPTH_SRC) D6,  XF_PTNAME(DEPTH_SRC) D7,
		XF_PTNAME(DEPTH_SRC) D8,  XF_PTNAME(DEPTH_SRC) D9, XF_PTNAME(DEPTH_SRC) D10, XF_PTNAME(DEPTH_SRC) D11,
		XF_PTNAME(DEPTH_SRC) D12, XF_PTNAME(DEPTH_SRC) D13, XF_PTNAME(DEPTH_SRC) D14, XF_PTNAME(DEPTH_SRC) D15,
		XF_PTNAME(DEPTH_SRC) D16, XF_PTNAME(DEPTH_SRC) D17, XF_PTNAME(DEPTH_SRC) D18, XF_PTNAME(DEPTH_SRC) D19,
		XF_PTNAME(DEPTH_SRC) D20, XF_PTNAME(DEPTH_SRC) D21,	XF_PTNAME(DEPTH_SRC) D22, XF_PTNAME(DEPTH_SRC) D23,
		XF_PTNAME(DEPTH_SRC) D24)
		{
#pragma HLS INLINE off
	XF_PTNAME(DEPTH_SRC) g = 0;
	XF_PTNAME(DEPTH_DST) A00 = D0 + D1 + D2;
	XF_PTNAME(DEPTH_DST) A01 = D3 + D4 + D5;
	XF_PTNAME(DEPTH_DST) A02 = D6 + D7 + D8;
	XF_PTNAME(DEPTH_DST) A03 = D9 + D10 + D11;
	XF_PTNAME(DEPTH_DST) A04 = D12 + D13 + D14;
	XF_PTNAME(DEPTH_DST) A05 = D15 + D16 + D17;
	XF_PTNAME(DEPTH_DST) A06 = D18 + D19 + D20;
	XF_PTNAME(DEPTH_DST) A07 = D21 + D22;
	XF_PTNAME(DEPTH_DST) A08 = D23 + D24;

	ap_int<22> A0 = A00 + A01 + A02;
	ap_int<22> A1 = A03 + A04 + A05;
	ap_int<22> A2 = A06 + A07 + A08;

	ap_int<25> A = A0+A1+A2;

	g = (A * XF_DIV_VAL_5x5) >> 15;

	return g;

		}


template<int NPC, int DEPTH_SRC, int DEPTH_DST>
void xFComputeMask5x5(
		XF_PTNAME(DEPTH_SRC) *Gradientvalues,
		XF_PTNAME(DEPTH_SRC) *src_buf1, XF_PTNAME(DEPTH_SRC) *src_buf2,
		XF_PTNAME(DEPTH_SRC) *src_buf3, XF_PTNAME(DEPTH_SRC) *src_buf4, XF_PTNAME(DEPTH_SRC) *src_buf5)
{
#pragma HLS INLINE

	Compute_Grad_Loop:
	for(int j = 0; j < XF_NPIXPERCYCLE(NPC); j++ )
	{
#pragma HLS LOOP_TRIPCOUNT min=8 max=8
#pragma HLS UNROLL
		Gradientvalues[j] = xFGradient5x5<DEPTH_SRC, DEPTH_DST>(
				src_buf1[j],   src_buf1[j+1], src_buf1[j+2], src_buf1[j+3],	src_buf1[j+4],
				src_buf2[j],   src_buf2[j+1], src_buf2[j+2], src_buf2[j+3], src_buf2[j+4],
				src_buf3[j],   src_buf3[j+1], src_buf3[j+2], src_buf3[j+3],	src_buf3[j+4],
				src_buf4[j],   src_buf4[j+1], src_buf4[j+2], src_buf4[j+3], src_buf4[j+4],
				src_buf5[j],   src_buf5[j+1], src_buf5[j+2], src_buf5[j+3],	src_buf5[j+4]);
	}
}


template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH_SRC, int WORDWIDTH_DST, int WORDWIDTH_AP, int TC>
void ProcessBox5x5(hls::stream< XF_SNAME(WORDWIDTH_SRC) > & _src_mat,
		hls::stream< XF_SNAME(WORDWIDTH_DST) > & _dst_mat,
		XF_SNAME(WORDWIDTH_SRC) buf[5][(COLS >> XF_BITSHIFT(NPC))], XF_PTNAME(DEPTH) src_buf1[XF_NPIXPERCYCLE(NPC)+4],
		XF_PTNAME(DEPTH) src_buf2[XF_NPIXPERCYCLE(NPC)+4], XF_PTNAME(DEPTH) src_buf3[XF_NPIXPERCYCLE(NPC)+4], XF_PTNAME(DEPTH) src_buf4[XF_NPIXPERCYCLE(NPC)+4], XF_PTNAME(DEPTH) src_buf5[XF_NPIXPERCYCLE(NPC)+4],
		XF_PTNAME(DEPTH) GradientValues[XF_NPIXPERCYCLE(NPC)], XF_SNAME(WORDWIDTH_DST) &inter_val, uint16_t img_width, uint16_t img_height, ap_uint<13> row_ind, uint16_t &shift,
		ap_uint<4> tp1, ap_uint<4> tp2, ap_uint<4> mid, ap_uint<4> bottom1, ap_uint<4> bottom2, ap_uint<13> row)
{
#pragma HLS INLINE
	ap_uint<8> buf_size = XF_NPIXPERCYCLE(NPC) + 4;
	ap_uint<8> step = XF_PIXELDEPTH(DEPTH);
	ap_uint<10> max_loop = XF_WORDDEPTH(WORDWIDTH_DST);
	uint16_t npc = XF_NPIXPERCYCLE(NPC);
	XF_SNAME(WORDWIDTH_SRC)  buf0, buf1, buf2, buf3, buf4;

	Col_Loop:
	for(ap_uint<13> col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		if(row < img_height)
			buf[row_ind][col] = _src_mat.read();
		else
			buf[bottom2][col] = 0;

		buf0 = buf[tp1][col];
		buf1 = buf[tp2][col];
		buf2 = buf[mid][col];
		buf3 = buf[bottom1][col];
		buf4 = buf[bottom2][col];


		if(NPC == XF_NPPC8)
		{
			xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH>(&src_buf1[4], buf0, 0);
			xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH>(&src_buf2[4], buf1, 0);
			xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH>(&src_buf3[4], buf2, 0);
			xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH>(&src_buf4[4], buf3, 0);
			xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH>(&src_buf5[4], buf4, 0);
		}
		else
		{
			src_buf1[4] = buf0;
			src_buf2[4] = buf1;
			src_buf3[4] = buf2;
			src_buf4[4] = buf3;
			src_buf5[4] = buf4;
		}

		xFComputeMask5x5<NPC, DEPTH, WORDWIDTH_AP>(GradientValues, src_buf1, src_buf2, src_buf3,
				src_buf4, src_buf5);

		for(ap_uint<4> i = 0; i < 4; i++)
		{
#pragma HLS unroll
			src_buf1[i] = src_buf1[buf_size-(4 - i)];
			src_buf2[i] = src_buf2[buf_size-(4 - i)];
			src_buf3[i] = src_buf3[buf_size-(4 - i)];
			src_buf4[i] = src_buf4[buf_size-(4 - i)];
			src_buf5[i] = src_buf5[buf_size-(4 - i)];
		}
		if(col == 0)
		{
			shift = 0;
			inter_val = 0;

			xfPackPixels<NPC, WORDWIDTH_DST, DEPTH>(&GradientValues[0], inter_val, 2, (npc-2), shift);
		}
		else
		{
			if((NPC == XF_NPPC8) || (NPC == XF_NPPC16))
			{
				xfPackPixels<NPC, WORDWIDTH_DST, DEPTH>(&GradientValues[0], inter_val, 0, 2, shift);

				_dst_mat.write(inter_val);

				shift= 0;
				inter_val = 0;
				xfPackPixels<NPC, WORDWIDTH_DST, DEPTH>(&GradientValues[0], inter_val, 2, (npc-2), shift);
			}
			else
			{
				if(col >= 2)
				{
					inter_val((max_loop-1), (max_loop-step)) = GradientValues[0];
					_dst_mat.write(inter_val);
				}
			}
		}
	} // Col_Loop


}
/**
 * xFBoxFilter5x5 : Compute a Box filter of size 5x5 over a window of the input image.
 * Inputs : _src_mat --> input image of type XF_8U, XF_16U or XF_16S
 * Output : _dst_mat --> output image of input type
 */
template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH_SRC, int WORDWIDTH_DST,int WORDWIDTH_AP, int TC,bool USE_URAM>
void xFBoxFilter5x5(hls::stream < XF_SNAME(WORDWIDTH_SRC) > &_src_mat,
		hls::stream < XF_SNAME(WORDWIDTH_DST) > &_dst_mat, uint16_t img_height, uint16_t img_width)
{
	ap_uint<13> row_ind, row, col;
	ap_uint<4> tp1,tp2, mid, bottom1,bottom2;
	ap_uint<8> buf_size = XF_NPIXPERCYCLE(NPC) + 4;
	ap_uint<8> step = XF_PIXELDEPTH(DEPTH);
	ap_uint<10> max_loop = XF_WORDDEPTH(WORDWIDTH_DST);
	uint16_t shift = 0;

	ap_uint<8> i;
	XF_PTNAME(DEPTH) GradientValues[XF_NPIXPERCYCLE(NPC)];
#pragma HLS ARRAY_PARTITION variable=GradientValues complete dim=1

	// Temporary buffers to hold image data from five rows
	XF_PTNAME(DEPTH) src_buf1[XF_NPIXPERCYCLE(NPC)+4], src_buf2[XF_NPIXPERCYCLE(NPC)+4],
			src_buf3[XF_NPIXPERCYCLE(NPC)+4], src_buf4[XF_NPIXPERCYCLE(NPC)+4],
			src_buf5[XF_NPIXPERCYCLE(NPC)+4];
#pragma HLS ARRAY_PARTITION variable=src_buf1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf3 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf4 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf5 complete dim=1

	XF_SNAME(WORDWIDTH_SRC) tmp_in;
	XF_SNAME(WORDWIDTH_DST) inter_val = 0;
	// Temporary buffer to hold image data from five rows
	XF_SNAME(WORDWIDTH_SRC)  buf[5][(COLS >> XF_BITSHIFT(NPC))];
if(USE_URAM)
{
#pragma HLS RESOURCE variable=buf core=RAM_S2P_URAM
#pragma HLS array reshape variable=buf dim=1 factor=5 cyclic	
}
else{
#pragma HLS RESOURCE variable=buf core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1
}
	row_ind = 2;

	Clear_Row_Loop:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		buf[0][col] = 0;
		buf[1][col] = 0;
		buf[row_ind][col] = _src_mat.read();
	}
	row_ind++;

	Read_Row2_Loop:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		buf[row_ind][col] = _src_mat.read();
	}
	row_ind++;

	Row_Loop:
	for(row = 2; row < img_height+2; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS

		// modify the buffer indices to re use
		if(row_ind == 4)
		{
			tp1 = 0; tp2 = 1; mid = 2; bottom1 = 3; bottom2 = 4;
		}
		else if(row_ind == 0)
		{
			tp1 = 1; tp2 = 2; mid = 3; bottom1 = 4; bottom2 = 0;
		}
		else if(row_ind == 1)
		{
			tp1 = 2; tp2 = 3; mid = 4; bottom1 = 0; bottom2 = 1;
		}
		else if(row_ind == 2)
		{
			tp1 = 3; tp2 = 4; mid = 0; bottom1 = 1; bottom2 = 2;
		}
		else if(row_ind == 3)
		{
			tp1 = 4; tp2 = 0; mid = 1; bottom1 = 2; bottom2 = 3;
		}

		src_buf1[0] = src_buf1[1] = src_buf1[2] = src_buf1[3] = 0;
		src_buf2[0] = src_buf2[1] = src_buf2[2] = src_buf2[3] = 0;
		src_buf3[0] = src_buf3[1] = src_buf3[2] = src_buf3[3] = 0;
		src_buf4[0] = src_buf4[1] = src_buf4[2] = src_buf4[3] = 0;
		src_buf5[0] = src_buf5[1] = src_buf5[2] = src_buf5[3] = 0;

		inter_val = 0;
		ProcessBox5x5<ROWS, COLS, DEPTH, NPC, WORDWIDTH_SRC, WORDWIDTH_DST, WORDWIDTH_AP, TC>( _src_mat, _dst_mat, buf, src_buf1,src_buf2, src_buf3, src_buf4, src_buf5,GradientValues,
				inter_val, img_width, img_height, row_ind, shift, tp1, tp2, mid, bottom1, bottom2, row);


		if((NPC == XF_NPPC8) || (NPC == XF_NPPC16))
		{

			for(i = 0; i < 6; i++)
			{
#pragma HLS unroll
				src_buf1[buf_size+i-(XF_NPIXPERCYCLE(NPC))+2] = 0;
				src_buf2[buf_size+i-(XF_NPIXPERCYCLE(NPC))+2] = 0;
				src_buf3[buf_size+i-(XF_NPIXPERCYCLE(NPC))+2] = 0;
				src_buf4[buf_size+i-(XF_NPIXPERCYCLE(NPC))+2] = 0;
				src_buf5[buf_size+i-(XF_NPIXPERCYCLE(NPC))+2] = 0;
			}

			GradientValues[0] = xFGradient5x5<DEPTH, WORDWIDTH_AP>(
					src_buf1[0], src_buf1[1], src_buf1[2], src_buf1[3], 0,
					src_buf2[0], src_buf2[1], src_buf2[2], src_buf2[3], 0,
					src_buf3[0], src_buf3[1], src_buf3[2], src_buf3[3], 0,
					src_buf4[0], src_buf4[1], src_buf4[2], src_buf4[3], 0,
					src_buf5[0], src_buf5[1], src_buf5[2], src_buf5[3], 0);

			GradientValues[1] = xFGradient5x5<DEPTH, WORDWIDTH_AP>(
					src_buf1[1], src_buf1[2], src_buf1[3], 0, 0,
					src_buf2[1], src_buf2[2], src_buf2[3], 0, 0,
					src_buf3[1], src_buf3[2], src_buf3[3], 0, 0,
					src_buf4[1], src_buf4[2], src_buf4[3], 0, 0,
					src_buf5[1], src_buf5[2], src_buf5[3], 0, 0);

			xfPackPixels<NPC, WORDWIDTH_DST, DEPTH>(&GradientValues[0], inter_val, 0, 2, shift);
			_dst_mat.write(inter_val);
		}
		else
		{
#pragma HLS ALLOCATION instances=xFGradient5x5 limit=1 function
			GradientValues[0] = xFGradient5x5<DEPTH, WORDWIDTH_AP>(
					src_buf1[buf_size-5], src_buf1[buf_size-4], src_buf1[buf_size-3], src_buf1[buf_size-2], 0,
					src_buf2[buf_size-5], src_buf2[buf_size-4],	src_buf2[buf_size-3], src_buf2[buf_size-2], 0,
					src_buf3[buf_size-5], src_buf3[buf_size-4],	src_buf3[buf_size-3], src_buf3[buf_size-2], 0,
					src_buf4[buf_size-5], src_buf4[buf_size-4],	src_buf4[buf_size-3], src_buf4[buf_size-2], 0,
					src_buf5[buf_size-5], src_buf5[buf_size-4],	src_buf5[buf_size-3], src_buf5[buf_size-2], 0);

			inter_val((max_loop-1), (max_loop-step)) = GradientValues[0];
			_dst_mat.write(inter_val);

			GradientValues[0] = xFGradient5x5<DEPTH, WORDWIDTH_AP>(
					src_buf1[buf_size-4], src_buf1[buf_size-3], src_buf1[buf_size-2], 0, 0,
					src_buf2[buf_size-4], src_buf2[buf_size-3],	src_buf2[buf_size-2], 0, 0,
					src_buf3[buf_size-4], src_buf3[buf_size-3],	src_buf3[buf_size-2], 0, 0,
					src_buf4[buf_size-4], src_buf4[buf_size-3],	src_buf4[buf_size-2], 0, 0,
					src_buf5[buf_size-4], src_buf5[buf_size-3],	src_buf5[buf_size-2], 0, 0);

			inter_val((max_loop-1), (max_loop-step)) = GradientValues[0];
			_dst_mat.write(inter_val);

		}

		row_ind++;

		if(row_ind == 5)
		{
			row_ind = 0;
		}
	} // Row_Loop

}  // end of xFBoxFilter5x5


/**
 * xFApplyMask7x7: apply a 7x7 mask
 * -----------------------------------
 * 		  |1	1 	1	1 	1	1 	1|
 *		  |1	1	1	1 	1	1 	1|
 * 		  |1	1	1	1 	1	1 	1|
 * 	1/49* |1	1	1	1 	1	1 	1|
 * 		  |1	1	1	1 	1	1 	1|
 * 		  |1	1	1	1 	1	1 	1|
 * 		  |1	1	1	1 	1	1 	1|
 * -----------------------------------
 */
template<int DEPTH_SRC, int DEPTH_DST>
XF_PTNAME(DEPTH_SRC) xFGradient7x7(XF_PTNAME(DEPTH_SRC) *src_buf1, XF_PTNAME(DEPTH_SRC) *src_buf2,
		XF_PTNAME(DEPTH_SRC) *src_buf3, XF_PTNAME(DEPTH_SRC) *src_buf4,
		XF_PTNAME(DEPTH_SRC) *src_buf5, XF_PTNAME(DEPTH_SRC) *src_buf6, XF_PTNAME(DEPTH_SRC) *src_buf7)
		{
#pragma HLS INLINE off
#pragma HLS PIPELINE II=1

	XF_PTNAME(DEPTH_SRC) g_y = 0;

	ap_int<20> A00 = src_buf1[0] + src_buf1[1] + src_buf1[2];
	ap_int<20> A01 = src_buf1[3] + src_buf1[4];
	ap_int<20> A02 = src_buf1[5] + src_buf1[6];

	ap_int<20> A03 = src_buf2[0] + src_buf2[1] + src_buf2[2];
	ap_int<20> A04 = src_buf2[3] + src_buf2[4];
	ap_int<20> A05 = src_buf2[5] + src_buf2[6];

	ap_int<20> A06 = src_buf3[0] + src_buf3[1] + src_buf3[2];
	ap_int<20> A07 = src_buf3[3] + src_buf3[4];
	ap_int<20> A08 = src_buf3[5] + src_buf3[6];

	ap_int<20> A09 = src_buf4[0] + src_buf4[1] + src_buf4[2];
	ap_int<20> A10 = src_buf4[3] + src_buf4[4];
	ap_int<20> A11 = src_buf4[5] + src_buf4[6];

	ap_int<20> A12 = src_buf5[0] + src_buf5[1] + src_buf5[2];
	ap_int<20> A13 = src_buf5[3] + src_buf5[4];
	ap_int<20> A14 = src_buf5[5] + src_buf5[6];

	ap_int<20> A15 = src_buf6[0] + src_buf6[1] + src_buf6[2];
	ap_int<20> A16 = src_buf6[3] + src_buf6[4];
	ap_int<20> A17 = src_buf6[5] + src_buf6[6];

	ap_int<20> A18 = src_buf7[0] + src_buf7[1] + src_buf7[2];
	ap_int<20> A19 = src_buf7[3] + src_buf7[4];
	ap_int<20> A20 = src_buf7[5] + src_buf7[6];

	ap_int<24> A0 = A00 + A01 + A02;
	ap_int<24> A1 = A03 + A04 + A05;
	ap_int<24> A2 = A06 + A07 + A08;
	ap_int<24> A3 = A09 + A10 + A11;
	ap_int<24> A4 = A12 + A13 + A14;
	ap_int<24> A5 = A15 + A16 + A17;
	ap_int<24> A6 = A18 + A19 + A20;

	ap_int<26> S00 = A0+A1+A2;
	ap_int<26> S01 = A3+A4;
	ap_int<26> S02 = A5+A6;

	ap_int<26> S0 = S00+S01;
	ap_int<26> S = S0+S02;

	ap_int<32> res = ((S * XF_DIV_VAL_7x7 ) >> 15);

	g_y = (XF_PTNAME(DEPTH_SRC))res;
	return g_y;
		}


template<int NPC, int DEPTH_SRC, int DEPTH_DST>
void xFComputeMask7x7(
		XF_PTNAME(DEPTH_SRC)* Gradientvalues,
		XF_PTNAME(DEPTH_SRC) *src_buf1, XF_PTNAME(DEPTH_SRC) *src_buf2,
		XF_PTNAME(DEPTH_SRC) *src_buf3, XF_PTNAME(DEPTH_SRC) *src_buf4,
		XF_PTNAME(DEPTH_SRC) *src_buf5, XF_PTNAME(DEPTH_SRC) *src_buf6,
		XF_PTNAME(DEPTH_SRC) *src_buf7)
{
#pragma HLS INLINE
	for(ap_uint<9> j = 0; j < XF_NPIXPERCYCLE(NPC); j++)
	{
#pragma HLS LOOP_TRIPCOUNT min=8 max=8
#pragma HLS UNROLL
		Gradientvalues[j] = xFGradient7x7<DEPTH_SRC, DEPTH_DST>(&src_buf1[j], &src_buf2[j],	&src_buf3[j], &src_buf4[j],
				&src_buf5[j], &src_buf6[j], &src_buf7[j]);
	}
}

template<int NPC, int DEPTH_SRC>
void xFCopyDataBox(XF_PTNAME(DEPTH_SRC) src_buf1[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH_SRC) src_buf2[XF_NPIXPERCYCLE(NPC)+6],
		XF_PTNAME(DEPTH_SRC) src_buf3[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH_SRC) src_buf4[XF_NPIXPERCYCLE(NPC)+6],
		XF_PTNAME(DEPTH_SRC) src_buf5[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH_SRC) src_buf6[XF_NPIXPERCYCLE(NPC)+6],
		XF_PTNAME(DEPTH_SRC) src_buf7[XF_NPIXPERCYCLE(NPC)+6]){
#pragma HLS INLINE
	ap_uint<5> buf_size = (XF_NPIXPERCYCLE(NPC)+6);
	for(ap_uint<4> i = 0; i < 6; i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=6 max=6
#pragma HLS unroll
		src_buf1[i] = src_buf1[buf_size-(6 - i)];
		src_buf2[i] = src_buf2[buf_size-(6 - i)];
		src_buf3[i] = src_buf3[buf_size-(6 - i)];
		src_buf4[i] = src_buf4[buf_size-(6 - i)];
		src_buf5[i] = src_buf5[buf_size-(6 - i)];
		src_buf6[i] = src_buf6[buf_size-(6 - i)];
		src_buf7[i] = src_buf7[buf_size-(6 - i)];
	}
}

template<int NPC, int WORDWIDTH_SRC, int DEPTH_SRC>
void xFExtractDataBox(XF_PTNAME(DEPTH_SRC) src_buf1[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH_SRC) src_buf2[XF_NPIXPERCYCLE(NPC)+6],
		XF_PTNAME(DEPTH_SRC) src_buf3[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH_SRC) src_buf4[XF_NPIXPERCYCLE(NPC)+6],
		XF_PTNAME(DEPTH_SRC) src_buf5[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH_SRC) src_buf6[XF_NPIXPERCYCLE(NPC)+6],
		XF_PTNAME(DEPTH_SRC) src_buf7[XF_NPIXPERCYCLE(NPC)+6], XF_SNAME(WORDWIDTH_SRC) buf0, XF_SNAME(WORDWIDTH_SRC) buf1, XF_SNAME(WORDWIDTH_SRC) buf2,
		XF_SNAME(WORDWIDTH_SRC) buf3, XF_SNAME(WORDWIDTH_SRC) buf4, XF_SNAME(WORDWIDTH_SRC) buf5, XF_SNAME(WORDWIDTH_SRC) buf6)
{
#pragma HLS INLINE
	xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH_SRC>(&src_buf1[6], buf0, 0);
	xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH_SRC>(&src_buf2[6], buf1, 0);
	xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH_SRC>(&src_buf3[6], buf2, 0);
	xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH_SRC>(&src_buf4[6], buf3, 0);
	xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH_SRC>(&src_buf5[6], buf4, 0);
	xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH_SRC>(&src_buf6[6], buf5, 0);
	xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH_SRC>(&src_buf7[6], buf6, 0);
}

template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH_SRC, int WORDWIDTH_DST, int WORDWIDTH_AP, int TC>
void ProcessBox7x7(hls::stream< XF_SNAME(WORDWIDTH_SRC) > & _src_mat,
		hls::stream< XF_SNAME(WORDWIDTH_DST) > & _dst_mat,
		XF_SNAME(WORDWIDTH_SRC) buf[5][(COLS >> XF_BITSHIFT(NPC))], XF_PTNAME(DEPTH) src_buf1[XF_NPIXPERCYCLE(NPC)+6],
		XF_PTNAME(DEPTH) src_buf2[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH) src_buf3[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH) src_buf4[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH) src_buf5[XF_NPIXPERCYCLE(NPC)+6],
		XF_PTNAME(DEPTH) src_buf6[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH) src_buf7[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH) GradientValues[XF_NPIXPERCYCLE(NPC)], XF_SNAME(WORDWIDTH_DST) &inter_val, uint16_t img_width, uint16_t img_height, ap_uint<13> row_ind, uint16_t &shiftx,
		ap_uint<5> tp1, ap_uint<5> tp2, ap_uint<5> tp3, ap_uint<5> mid, ap_uint<5> bottom1, ap_uint<5> bottom2, ap_uint<5> bottom3, ap_uint<13> row)
{
#pragma HLS INLINE

	XF_SNAME(WORDWIDTH_SRC) buf0, buf1, buf2, buf3, buf4, buf5, buf6;
	ap_uint<8> buf_size = (XF_NPIXPERCYCLE(NPC)+6);
	ap_uint<10> max_loop = XF_WORDDEPTH(WORDWIDTH_DST);
	ap_uint<9> npc = XF_NPIXPERCYCLE(NPC);
	ap_uint<8> step = XF_PIXELDEPTH(DEPTH);


	Col_Loop:
	for(ap_uint<13> col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		if(row < img_height)
			buf[row_ind][col] = _src_mat.read();
		else
			buf[bottom3][col] = 0;
		buf0 = buf[tp1][col];
		buf1 = buf[tp2][col];
		buf2 = buf[tp3][col];
		buf3 = buf[mid][col];
		buf4 = buf[bottom1][col];
		buf5 = buf[bottom2][col];
		buf6 = buf[bottom3][col];

		if(NPC == XF_NPPC8)
		{
			xFExtractDataBox<NPC, WORDWIDTH_SRC, DEPTH>(src_buf1, src_buf2, src_buf3, src_buf4,
					src_buf5, src_buf6, src_buf7, buf0, buf1, buf2, buf3, buf4, buf5, buf6);
		}
		else
		{
			src_buf1[6] = buf0;
			src_buf2[6] = buf1;
			src_buf3[6] = buf2;
			src_buf4[6] = buf3;
			src_buf5[6] = buf4;
			src_buf6[6] = buf5;
			src_buf7[6] = buf6;
		}

		xFComputeMask7x7<NPC, DEPTH, WORDWIDTH_AP>(GradientValues, src_buf1, src_buf2, src_buf3, src_buf4,
				src_buf5, src_buf6, src_buf7);

		xFCopyDataBox<NPC, DEPTH>(src_buf1, src_buf2, src_buf3, src_buf4,
				src_buf5, src_buf6, src_buf7);

		if(col == 0)
		{
			shiftx = 0;
			inter_val = 0;

			xfPackPixels<NPC, WORDWIDTH_DST, DEPTH>(&GradientValues[0], inter_val, 3, (npc-3), shiftx);
		}
		else
		{
			if((NPC == XF_NPPC8) || (NPC == XF_NPPC16))
			{
				xfPackPixels<NPC, WORDWIDTH_DST, DEPTH>(&GradientValues[0], inter_val, 0, 3, shiftx);

				_dst_mat.write(inter_val);
				shiftx = 0;
				inter_val = 0;

				xfPackPixels<NPC, WORDWIDTH_DST, DEPTH>(&GradientValues[0], inter_val, 3, (npc-3), shiftx);
			}
			else
			{
				if(col >=3 )
				{
					inter_val((max_loop-1), (max_loop-step)) = GradientValues[0];
					_dst_mat.write(inter_val);
				}
			}
		}
	}// Col_Loop
}

template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH_SRC, int WORDWIDTH_DST, int WORDWIDTH_AP, int TC>
void RightBorderBox7x7(hls::stream< XF_SNAME(WORDWIDTH_DST) > & _dst_mat,
		XF_PTNAME(DEPTH) src_buf1[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH) src_buf2[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH) src_buf3[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH) src_buf4[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH) src_buf5[XF_NPIXPERCYCLE(NPC)+6],
		XF_PTNAME(DEPTH) src_buf6[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH) src_buf7[XF_NPIXPERCYCLE(NPC)+6], XF_PTNAME(DEPTH) GradientValues[XF_NPIXPERCYCLE(NPC)], XF_SNAME(WORDWIDTH_DST) &inter_val, uint16_t img_width, uint16_t img_height, ap_uint<13> row_ind, uint16_t &shiftx,
		ap_uint<13> row)
{
#pragma HLS INLINE
	ap_uint<4> i;
	ap_uint<8> buf_size = (XF_NPIXPERCYCLE(NPC)+6);
	ap_uint<10> max_loop = XF_WORDDEPTH(WORDWIDTH_DST);
	ap_uint<8> step = XF_PIXELDEPTH(DEPTH);

	if(row >= 3)
	{
		if((NPC == XF_NPPC8) || (NPC == XF_NPPC16))
		{
			for(i = 0; i < 8; i++)
			{
#pragma HLS LOOP_TRIPCOUNT min=8 max=8
#pragma HLS unroll
				src_buf1[buf_size+i-(XF_NPIXPERCYCLE(NPC))] = 0;
				src_buf2[buf_size+i-(XF_NPIXPERCYCLE(NPC))] = 0;
				src_buf3[buf_size+i-(XF_NPIXPERCYCLE(NPC))] = 0;
				src_buf4[buf_size+i-(XF_NPIXPERCYCLE(NPC))] = 0;
				src_buf5[buf_size+i-(XF_NPIXPERCYCLE(NPC))] = 0;
				src_buf6[buf_size+i-(XF_NPIXPERCYCLE(NPC))] = 0;
				src_buf7[buf_size+i-(XF_NPIXPERCYCLE(NPC))] = 0;
			}

			for(i = 0; i < 3; i++)
			{
#pragma HLS LOOP_TRIPCOUNT min=3 max=3
#pragma HLS unroll
				GradientValues[i] = xFGradient7x7<DEPTH, WORDWIDTH_AP>(
						&src_buf1[i], &src_buf2[i], &src_buf3[i],
						&src_buf4[i], &src_buf5[i], &src_buf6[i], &src_buf7[i]);
			}
			xfPackPixels<NPC, WORDWIDTH_DST, DEPTH>(&GradientValues[0], inter_val, 0, 3, shiftx);
			_dst_mat.write(inter_val);

			shiftx = 0;
			inter_val = 0;
		}
		else
		{
			src_buf1[6] = 0;
			src_buf2[6] = 0;
			src_buf3[6] = 0;
			src_buf4[6] = 0;
			src_buf5[6] = 0;
			src_buf6[6] = 0;
			src_buf7[6] = 0;

			for(i = 0; i < 3; i++)
			{
#pragma HLS LOOP_TRIPCOUNT min=3 max=3
#pragma HLS ALLOCATION instances=xFGradient7x7 limit=1 function

				GradientValues[0] = xFGradient7x7<DEPTH, WORDWIDTH_AP>(
						&src_buf1[0], &src_buf2[0], &src_buf3[0],
						&src_buf4[0], &src_buf5[0], &src_buf6[0], &src_buf7[0]);

				xFCopyDataBox<NPC, DEPTH>(src_buf1, src_buf2, src_buf3, src_buf4,
						src_buf5, src_buf6, src_buf7);

				inter_val((max_loop-1), (max_loop-step)) = GradientValues[0];
				_dst_mat.write(inter_val);
			}
		}
	}
}
/**
 * xFBoxFilter : Compute a Box filter of size 7x7 over a window of the input image.
 * Inputs : _src_mat --> input image of type XF_8U, XF_16U or XF_16S
 * Output : _dst_mat --> output image of input type
 */
template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH_SRC,
int WORDWIDTH_DST,int WORDWIDTH_AP, int TC,bool USE_URAM>
void xFBoxFilter7x7(hls::stream< XF_SNAME(WORDWIDTH_SRC) > &_src_mat,
		hls::stream< XF_SNAME(WORDWIDTH_DST) > &_dst_mat, uint16_t img_height, uint16_t img_width)
{
	ap_uint<13> row_ind, row, col;
	ap_uint<5> tp1,tp2, tp3, mid, bottom1, bottom2, bottom3, i;
	// Gradient output values stored in these buffer
	XF_PTNAME(DEPTH) GradientValues[XF_NPIXPERCYCLE(NPC)];
#pragma HLS ARRAY_PARTITION variable=GradientValues complete dim=1

	// Temporary buffers to hold image data from three rows.
	XF_PTNAME(DEPTH) src_buf1[XF_NPIXPERCYCLE(NPC) + 6], src_buf2[XF_NPIXPERCYCLE(NPC) + 6],
			src_buf3[XF_NPIXPERCYCLE(NPC) + 6], src_buf4[XF_NPIXPERCYCLE(NPC) + 6],
			src_buf5[XF_NPIXPERCYCLE(NPC) + 6], src_buf6[XF_NPIXPERCYCLE(NPC) + 6], src_buf7[XF_NPIXPERCYCLE(NPC) + 6];
#pragma HLS ARRAY_PARTITION variable=src_buf1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf3 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf4 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf5 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf6 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf7 complete dim=1

	XF_SNAME(WORDWIDTH_SRC) tmp_in;
	XF_SNAME(WORDWIDTH_DST) inter_val = 0;
	uint16_t shiftx = 0;

	XF_SNAME(WORDWIDTH_SRC) buf[7][(COLS >> XF_BITSHIFT(NPC))];
if(USE_URAM)
{
#pragma HLS RESOURCE variable=buf core=RAM_S2P_URAM
#pragma HLS array reshape variable=buf dim=1 factor=7 cyclic	
}
else{
#pragma HLS RESOURCE variable=buf core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1
}
	row_ind = 3;

	Clear_Row_Loop:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		buf[0][col] = 0;
		buf[1][col] = 0;
		buf[2][col] = 0;
		buf[row_ind][col] = _src_mat.read();
	}
	row_ind++;

	Read_Row1_Loop:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline

		buf[row_ind][col] = _src_mat.read();
	}
	row_ind++;

	Read_Row2_Loop:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline

		buf[row_ind][col] = _src_mat.read();
	}
	row_ind++;

	Row_Loop:
	for(row = 3; row < img_height+3; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS
		// modify the buffer indices to re use
		if(row_ind == 0)
		{
			tp1 = 1; tp2 = 2; tp3 = 3; mid = 4; bottom1 = 5; bottom2 = 6; bottom3 = 0;
		}
		else if(row_ind == 1)
		{
			tp1 = 2; tp2 = 3; tp3 = 4; mid = 5; bottom1 = 6; bottom2 = 0; bottom3 = 1;
		}
		else if(row_ind == 2)
		{
			tp1 = 3; tp2 = 4; tp3 = 5; mid = 6; bottom1 = 0; bottom2 = 1; bottom3 = 2;
		}
		else if(row_ind == 3)
		{
			tp1 = 4; tp2 = 5; tp3 = 6; mid = 0; bottom1 = 1; bottom2 = 2; bottom3 = 3;
		}
		else if(row_ind == 4)
		{
			tp1 = 5; tp2 = 6; tp3 = 0; mid = 1; bottom1 = 2; bottom2 = 3; bottom3 = 4;
		}
		else if(row_ind == 5)
		{
			tp1 = 6; tp2 = 0; tp3 = 1; mid = 2; bottom1 = 3; bottom2 = 4; bottom3 = 5;
		}
		else if(row_ind == 6)
		{
			tp1 = 0; tp2 = 1; tp3 = 2; mid = 3; bottom1 = 4; bottom2 = 5; bottom3 = 6;
		}

		for(i = 0; i < 6; i++)
		{
#pragma HLS unroll
			src_buf1[i] = 0;
			src_buf2[i] = 0;
			src_buf3[i] = 0;
			src_buf4[i] = 0;
			src_buf5[i] = 0;
			src_buf6[i] = 0;
			src_buf7[i] = 0;
		}
		inter_val = 0;

		ProcessBox7x7<ROWS, COLS, DEPTH, NPC, WORDWIDTH_SRC, WORDWIDTH_DST, WORDWIDTH_AP, TC>(_src_mat, _dst_mat, buf, src_buf1, src_buf2, src_buf3, src_buf4, src_buf5, src_buf6, src_buf7,
				GradientValues,	inter_val, img_width, img_height, row_ind, shiftx, tp1, tp2, tp3, mid, bottom1, bottom2, bottom3, row);

		RightBorderBox7x7<ROWS, COLS, DEPTH, NPC, WORDWIDTH_SRC, WORDWIDTH_DST, WORDWIDTH_AP, TC>(_dst_mat, src_buf1, src_buf2, src_buf3, src_buf4, src_buf5, src_buf6, src_buf7,
				GradientValues,	inter_val, img_width, img_height, row_ind, shiftx, row);
		row_ind++;
		if(row_ind == 7)
		{
			row_ind = 0;
		}
	}//Row_Loop ends here
} // end of function xFBoxFilter7x7


/**
 * xFBoxFilter : This function calls the filter operations depending upon the
 * filter type. This acts as a wrapper function.
 */
template<int ROWS, int COLS,int DEPTH, int NPC, int WORDWIDTH_SRC, int WORDWIDTH_DST,bool USE_URAM>
void xFBoxFilterKernel(hls::stream< XF_SNAME(WORDWIDTH_SRC) > &_src,
		hls::stream< XF_SNAME(WORDWIDTH_DST) > & _dst,uint8_t _filter_type, uint8_t _border_type,uint16_t img_height,
		uint16_t img_width)
{

	assert(((_filter_type == XF_FILTER_3X3) || (_filter_type == XF_FILTER_5X5) || (_filter_type == XF_FILTER_7X7)) && ("Filter width should be 3 or 5 or 7."));
	assert(_border_type == XF_BORDER_CONSTANT && "Only XF_BORDER_CONSTANT is supported");

	assert(((img_height <= ROWS ) && (img_width <= COLS)) && "ROWS and COLS should be greater than input image");

	img_width = img_width >> XF_BITSHIFT(NPC);

	if(_filter_type == XF_FILTER_3X3)
	{
		xFBoxFilter3x3<ROWS,COLS,DEPTH,NPC,WORDWIDTH_SRC,
		WORDWIDTH_DST,(COLS>>XF_BITSHIFT(NPC)),USE_URAM>(_src,_dst, img_height, img_width);
	}
	else if(_filter_type == XF_FILTER_5X5)
	{
		if(NPC == XF_NPPC16)
		{
			xFBoxFilter5x5<ROWS,COLS,DEPTH,NPC,WORDWIDTH_SRC,
			WORDWIDTH_DST,XF_304SW,(COLS>>XF_BITSHIFT(NPC)),USE_URAM>(_src,_dst, img_height, img_width);
		}
		else if(NPC == XF_NPPC8)
		{
			xFBoxFilter5x5<ROWS,COLS,DEPTH,NPC,WORDWIDTH_SRC,
			WORDWIDTH_DST,XF_152SW,(COLS>>XF_BITSHIFT(NPC)),USE_URAM>(_src,_dst, img_height, img_width);
		}
		else
		{
			xFBoxFilter5x5<ROWS,COLS,DEPTH,NPC,WORDWIDTH_SRC,
			WORDWIDTH_DST,XF_19SW,(COLS>>XF_BITSHIFT(NPC)),USE_URAM>(_src,_dst, img_height, img_width);
		}
	}

	else if(_filter_type == XF_FILTER_7X7)
	{
		if(NPC == XF_NPPC16)
		{
			xFBoxFilter7x7<ROWS,COLS,DEPTH,NPC,WORDWIDTH_SRC,
			WORDWIDTH_DST,XF_304SW,(COLS>>XF_BITSHIFT(NPC)),USE_URAM>(_src,_dst, img_height, img_width);
		}
		else if(NPC == XF_NPPC8)
		{
			xFBoxFilter7x7<ROWS,COLS,DEPTH,NPC,WORDWIDTH_SRC,
			WORDWIDTH_DST,XF_152SW,(COLS>>XF_BITSHIFT(NPC)),USE_URAM>(_src,_dst, img_height, img_width);
		}
		else
		{
			xFBoxFilter7x7<ROWS,COLS,DEPTH,NPC,WORDWIDTH_SRC,
			WORDWIDTH_DST,XF_19SW,(COLS>>XF_BITSHIFT(NPC)),USE_URAM>(_src,_dst, img_height, img_width);
		}
	}

} // end of wrapper function

#pragma SDS data mem_attribute("_src_mat.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("_dst_mat.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("_src_mat.data":SEQUENTIAL, "_dst_mat.data":SEQUENTIAL)
#pragma SDS data copy("_src_mat.data"[0:"_src_mat.size"], "_dst_mat.data"[0:"_dst_mat.size"])

template<int BORDER_TYPE,int FILTER_TYPE, int SRC_T, int ROWS, int COLS,int NPC=1,bool USE_URAM=false>
void boxFilter(xf::Mat<SRC_T, ROWS, COLS, NPC> & _src_mat,xf::Mat<SRC_T, ROWS, COLS, NPC> & _dst_mat)
{

	hls::stream< XF_TNAME(SRC_T,NPC)> _src;
	hls::stream< XF_TNAME(SRC_T,NPC)> _dst;

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

	for(int i=0; i<_src_mat.rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
		for(int j=0; j<(_src_mat.cols)>>(XF_BITSHIFT(NPC));j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS/NPC
#pragma HLS LOOP_FLATTEN off
#pragma HLS PIPELINE
			_src.write( *(_src_mat.data + i*(_src_mat.cols>>(XF_BITSHIFT(NPC))) +j) );
		}
	}


	xFBoxFilterKernel<ROWS,COLS,XF_DEPTH(SRC_T,NPC),NPC,XF_WORDWIDTH(SRC_T,NPC),XF_WORDWIDTH(SRC_T,NPC),USE_URAM>(_src,_dst,FILTER_TYPE,BORDER_TYPE,_src_mat.rows,_src_mat.cols);


	for(int i=0; i<_dst_mat.rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
		for(int j=0; j<(_dst_mat.cols)>>(XF_BITSHIFT(NPC));j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS/NPC
#pragma HLS PIPELINE
#pragma HLS LOOP_FLATTEN off
			*(_dst_mat.data + i*(_dst_mat.cols>>(XF_BITSHIFT(NPC))) +j) = _dst.read();
		}
	}
}
}
#endif
