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

#ifndef _XF_SCHARR_HPP_
#define _XF_SCHARR_HPP_

#ifndef __cplusplus
#error C++ is needed to include this header
#endif

typedef unsigned short  uint16_t;

#include "hls_stream.h"
#include "common/xf_common.h"
#include "common/xf_utility.h"


namespace xf{

/********************************************************************
 * xFGradientX : X-Gradient Computation
 *
 * -------------
 * |-3	0   3|
 * |-10	0  10|
 * |-3	0	3|
 * -------------
 ********************************************************************/
template<int DEPTH_SRC, int DEPTH_DST>
XF_PTNAME(DEPTH_DST) xFGradientX(XF_PTNAME(DEPTH_SRC) vt0, XF_PTNAME(DEPTH_SRC) vt1,
		XF_PTNAME(DEPTH_SRC) vt2, XF_PTNAME(DEPTH_SRC) vm0,
		XF_PTNAME(DEPTH_SRC) vm1, XF_PTNAME(DEPTH_SRC) vm2,
		XF_PTNAME(DEPTH_SRC) vb0, XF_PTNAME(DEPTH_SRC) vb1,
		XF_PTNAME(DEPTH_SRC) vb2)
		{
#pragma HLS INLINE off
/*	XF_PTNAME(DEPTH_DST) temp_g;
	XF_PTNAME(DEPTH_DST) M00 = (XF_PTNAME(DEPTH_DST))vm2 << 3;
	M00 = M00 + vm2 + vm2;
	XF_PTNAME(DEPTH_DST) M01 = (XF_PTNAME(DEPTH_DST))vm0 << 3;
	M01 = M01 + vm0 + vm0;
	XF_PTNAME(DEPTH_DST) A00 = (XF_PTNAME(DEPTH_DST))vt2 << 1;
	A00 = A00 + vt2;
	XF_PTNAME(DEPTH_DST) A01 = (XF_PTNAME(DEPTH_DST))vb2 << 1;
	A01 = A01 + vb2;
	A00 = A00 + A01;
	XF_PTNAME(DEPTH_DST) S00 = (XF_PTNAME(DEPTH_DST))vt0 << 1;
	S00 = S00 + vt0;
	XF_PTNAME(DEPTH_DST) S01 = (XF_PTNAME(DEPTH_DST))vb0 << 1;
	S01 = S01 + vb0;
	S00 = S00 + S01;
	temp_g = M00 - M01;
	temp_g = temp_g + A00;
	temp_g = temp_g - S00;*/

	XF_PTNAME(DEPTH_DST) g_x;
//	ap_uint<8> g_x;
	short int temp_g;
	short int M00 = (short int)vm2 << 3;
	M00 = M00 + vm2 + vm2;
	short int M01 = (short int)vm0 << 3;
	M01 = M01 + vm0 + vm0;
	short int A00 = (short int)vt2 << 1;
	A00 = A00 + vt2;
	short int A01 = (short int)vb2 << 1;
	A01 = A01 + vb2;
	A00 = A00 + A01;
	short int S00 = (short int)vt0 << 1;
	S00 = S00 + vt0;
	short int S01 = (short int)vb0 << 1;
	S01 = S01 + vb0;
	S00 = S00 + S01;
	temp_g = M00 - M01;
	temp_g = temp_g + A00;
	temp_g = temp_g - S00;
	g_x = temp_g;

	if( (DEPTH_DST == XF_8UP) || (DEPTH_DST == XF_24UP ))
	{

		if(temp_g < 0)
			g_x = 0;
		if(temp_g > 255)
			g_x = 255;
	}
	return g_x;
}


/**********************************************************************
 *  xFGradientY : Y-Gradient Computation
 *
 * -------------
 * |-3	-10 -3|
 * | 0	 0	 0|
 * | 3	 10	 3|
 * -------------
 **********************************************************************/
template<int DEPTH_SRC, int DEPTH_DST>
XF_PTNAME(DEPTH_DST) xFGradientY(XF_PTNAME(DEPTH_SRC) vt0, XF_PTNAME(DEPTH_SRC) vt1,
		XF_PTNAME(DEPTH_SRC) vt2, XF_PTNAME(DEPTH_SRC) vm0,
		XF_PTNAME(DEPTH_SRC) vm1, XF_PTNAME(DEPTH_SRC) vm2,
		XF_PTNAME(DEPTH_SRC) vb0, XF_PTNAME(DEPTH_SRC) vb1,
		XF_PTNAME(DEPTH_SRC) vb2)
		{
#pragma HLS INLINE off
/*	XF_PTNAME(DEPTH_DST) temp_g;
	XF_PTNAME(DEPTH_DST) M00 = (XF_PTNAME(DEPTH_DST))vb1 << 3;
	M00 = M00 + vb1 + vb1;
	XF_PTNAME(DEPTH_DST) M01 = (XF_PTNAME(DEPTH_DST))vt1 << 3;
	M01 = M01 + vt1 + vt1;
	XF_PTNAME(DEPTH_DST) A00 = (XF_PTNAME(DEPTH_DST))vb0 << 1;
	A00 = A00 + vb0;
	XF_PTNAME(DEPTH_DST) A01 = (XF_PTNAME(DEPTH_DST))vb2 << 1;
	A01 = A01 + vb2;
	A00 = A00 + A01;
	XF_PTNAME(DEPTH_DST) S00 = (XF_PTNAME(DEPTH_DST))vt0 << 1;
	S00 = S00 + vt0;
	XF_PTNAME(DEPTH_DST) S01 = (XF_PTNAME(DEPTH_DST))vt2 << 1;
	S01 = S01 + vt2;
	S00 = S00 + S01;
	temp_g = M00 - M01;
	temp_g = temp_g + A00;
	temp_g = temp_g - S00;*/

	//XF_PTNAME(DEPTH_DST) g_y;
	XF_PTNAME(DEPTH_DST) g_y;
	//ap_uint<8> g_y;
	short int temp_g;
	short int M00 = (short int)vb1 << 3;
	M00 = M00 + vb1 + vb1;
	short int M01 = (short int)vt1 << 3;
	M01 = M01 + vt1 + vt1;
	short int A00 = (short int)vb0 << 1;
	A00 = A00 + vb0;
	short int A01 = (short int)vb2 << 1;
	A01 = A01 + vb2;
	A00 = A00 + A01;
	short int S00 = (short int)vt0 << 1;
	S00 = S00 + vt0;
	short int S01 = (short int)vt2 << 1;
	S01 = S01 + vt2;
	S00 = S00 + S01;
	temp_g = M00 - M01;
	temp_g = temp_g + A00;
	temp_g = temp_g - S00;

	g_y = temp_g;

	if( (DEPTH_DST == XF_8UP) || (DEPTH_DST == XF_24UP ))
	{
		if(temp_g < 0)
			g_y = 0;
		if(temp_g > 255)
			g_y = 255;
	}

	return g_y;
		}
/**
 * xFScharr3x3 : Applies the mask and Computes the gradient values
 *
 */
template<int NPC,int PLANES,int DEPTH_SRC,int DEPTH_DST>
void xFScharr3x3(XF_PTNAME(DEPTH_DST) GradientvaluesX[XF_NPIXPERCYCLE(NPC)], XF_PTNAME(DEPTH_DST) GradientvaluesY[XF_NPIXPERCYCLE(NPC)],
		XF_PTNAME(DEPTH_SRC) src_buf1[XF_NPIXPERCYCLE(NPC)+2], XF_PTNAME(DEPTH_SRC) src_buf2[XF_NPIXPERCYCLE(NPC)+2], XF_PTNAME(DEPTH_SRC) src_buf3[XF_NPIXPERCYCLE(NPC)+2])
{
#pragma HLS INLINE off
	int STEP;
	if( (DEPTH_DST == XF_48SP) || (DEPTH_DST == XF_16SP) )
	{
		  STEP=16;
	}
	else
	{
		  STEP=8;
	}

	Compute_Grad_Loop:
	for(ap_uint<5> j = 0; j < XF_NPIXPERCYCLE(NPC); j++)
	{
#pragma HLS PIPELINE
		int p=0;
		for(ap_uint<5> c=0,k = 0; c < PLANES; c++,k+=8)
		{


#pragma HLS UNROLL

		GradientvaluesX[j].range(p+(STEP-1),p) = xFGradientX<DEPTH_SRC, DEPTH_DST>(
				src_buf1[j].range(k+7,k), src_buf1[j+1].range(k+7,k), src_buf1[j+2].range(k+7,k),
				src_buf2[j].range(k+7,k), src_buf2[j+1].range(k+7,k), src_buf2[j+2].range(k+7,k),
				src_buf3[j].range(k+7,k), src_buf3[j+1].range(k+7,k), src_buf3[j+2].range(k+7,k));

		GradientvaluesY[j].range(p+(STEP-1),p) = xFGradientY<DEPTH_SRC, DEPTH_DST>(
				src_buf1[j].range(k+7,k), src_buf1[j+1].range(k+7,k), src_buf1[j+2].range(k+7,k),
				src_buf2[j].range(k+7,k), src_buf2[j+1].range(k+7,k), src_buf2[j+2].range(k+7,k),
				src_buf3[j].range(k+7,k), src_buf3[j+1].range(k+7,k),	src_buf3[j+2].range(k+7,k));

			p+=STEP;
		}
	}
}


/**************************************************************************************
 * ProcessScharr3x3 : Computes gradients for the column input data
 **************************************************************************************/
template<int SRC_T, int DST_T, int ROWS, int COLS, int PLANES,int DEPTH_SRC, int DEPTH_DST, int NPC, int WORDWIDTH_SRC, int WORDWIDTH_DST, int TC>
void ProcessScharr3x3(xf::Mat<SRC_T, ROWS, COLS, NPC> & _src_mat,xf::Mat<DST_T, ROWS, COLS, NPC> & _gradx_mat,
		xf::Mat<DST_T, ROWS, COLS, NPC> & _grady_mat,
		XF_SNAME(WORDWIDTH_SRC) buf[3][(COLS >> XF_BITSHIFT(NPC))], XF_PTNAME(DEPTH_SRC) src_buf1[XF_NPIXPERCYCLE(NPC)+2],
		XF_PTNAME(DEPTH_SRC) src_buf2[XF_NPIXPERCYCLE(NPC)+2], XF_PTNAME(DEPTH_SRC) src_buf3[XF_NPIXPERCYCLE(NPC)+2],
		XF_PTNAME(DEPTH_DST) GradientValuesX[XF_NPIXPERCYCLE(NPC)], XF_PTNAME(DEPTH_DST) GradientValuesY[XF_NPIXPERCYCLE(NPC)],
		XF_SNAME(WORDWIDTH_DST) &P0, XF_SNAME(WORDWIDTH_DST) &P1, uint16_t img_width, uint16_t img_height, ap_uint<13> row_ind, uint16_t &shift_x, uint16_t &shift_y,
		ap_uint<2> tp, ap_uint<2> mid, ap_uint<2> bottom, ap_uint<13> row, int &read_index, int &write_index)
{
#pragma HLS INLINE

	XF_SNAME(WORDWIDTH_SRC) buf0, buf1, buf2;
	uint16_t npc = XF_NPIXPERCYCLE(NPC);
	ap_uint<5> buf_size = XF_NPIXPERCYCLE(NPC) + 2;

	Col_Loop:
	for(ap_uint<13> col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		if(row < img_height)
			buf[row_ind][col] = _src_mat.data[read_index++]; // Read data
		else
			buf[bottom][col] = 0;
		buf0 = buf[tp][col];
		buf1 = buf[mid][col];
		buf2 = buf[bottom][col];

		if(NPC == XF_NPPC8)
		{
			xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH_SRC>(&src_buf1[2], buf0, 0);
			xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH_SRC>(&src_buf2[2], buf1, 0);
			xfExtractPixels<NPC, WORDWIDTH_SRC, DEPTH_SRC>(&src_buf3[2], buf2, 0);
		}
		else
		{
			src_buf1[2] = buf0;
			src_buf2[2] = buf1;
			src_buf3[2] = buf2;
		}


		xFScharr3x3<NPC,PLANES, DEPTH_SRC, DEPTH_DST>(GradientValuesX, GradientValuesY,
				src_buf1, src_buf2, src_buf3);

		if(col == 0)
		{
			shift_x = 0; shift_y = 0;
			P0 = 0; P1 = 0;


			xfPackPixels<NPC, WORDWIDTH_DST, DEPTH_DST>(&GradientValuesX[0], P0, 1, (npc-1), shift_x);
			xfPackPixels<NPC, WORDWIDTH_DST, DEPTH_DST>(&GradientValuesY[0], P1, 1, (npc-1), shift_y);

		}
		else
		{


			xfPackPixels<NPC, WORDWIDTH_DST, DEPTH_DST>(&GradientValuesX[0], P0, 0, 1, shift_x);
			xfPackPixels<NPC, WORDWIDTH_DST, DEPTH_DST>(&GradientValuesY[0], P1, 0, 1, shift_y);


			_gradx_mat.data[write_index] = P0;
			_grady_mat.data[write_index++] = P1;

			shift_x = 0; shift_y = 0;
			P0 = 0; P1 = 0;


			xfPackPixels<NPC, WORDWIDTH_DST, DEPTH_DST>(&GradientValuesX[0], P0, 1, (npc-1), shift_x);
			xfPackPixels<NPC, WORDWIDTH_DST, DEPTH_DST>(&GradientValuesY[0], P1, 1, (npc-1), shift_y);

		}

		src_buf1[0] = src_buf1[buf_size-2];
		src_buf1[1] = src_buf1[buf_size-1];
		src_buf2[0] = src_buf2[buf_size-2];
		src_buf2[1] = src_buf2[buf_size-1];
		src_buf3[0] = src_buf3[buf_size-2];
		src_buf3[1] = src_buf3[buf_size-1];
	} // Col_Loop
}

template<int SRC_T, int DST_T, int ROWS, int COLS,int PLANES,int DEPTH_SRC, int DEPTH_DST, int NPC, int WORDWIDTH_SRC, int WORDWIDTH_DST, int TC>
void xFScharrFilterKernel(xf::Mat<SRC_T, ROWS, COLS, NPC> & _src_mat,xf::Mat<DST_T, ROWS, COLS, NPC> & _gradx_mat,
		xf::Mat<DST_T, ROWS, COLS, NPC> & _grady_mat, uint16_t img_height, uint16_t img_width)
{
	ap_uint<13> row_ind;
	ap_uint<2> tp, mid, bottom;
	ap_uint<8> buf_size = XF_NPIXPERCYCLE(NPC) + 2;
	uint16_t shift_x = 0, shift_y = 0;
	ap_uint<13> row, col;
	int read_index = 0, write_index = 0;

	XF_PTNAME(DEPTH_DST) GradientValuesX[XF_NPIXPERCYCLE(NPC)*PLANES];										// X-Gradient result buffer
	XF_PTNAME(DEPTH_DST) GradientValuesY[XF_NPIXPERCYCLE(NPC)*PLANES];										// Y-Gradient result buffer
#pragma HLS ARRAY_PARTITION variable=GradientValuesX complete dim=1
#pragma HLS ARRAY_PARTITION variable=GradientValuesY complete dim=1

	XF_PTNAME(DEPTH_SRC) src_buf1[XF_NPIXPERCYCLE(NPC)+2], src_buf2[XF_NPIXPERCYCLE(NPC)+2],		// Temporary buffers to hold input data for processing
			src_buf3[XF_NPIXPERCYCLE(NPC)+2];
#pragma HLS ARRAY_PARTITION variable=src_buf1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf3 complete dim=1

	XF_SNAME(WORDWIDTH_DST) P0, P1;																	// Output data is packed
	// Line buffer to hold image data
	XF_SNAME(WORDWIDTH_SRC) buf[3][(COLS >> XF_BITSHIFT(NPC))];													// Line buffer
#pragma HLS RESOURCE variable=buf core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1
	row_ind = 1;

	Clear_Row_Loop:
	for(col = 0; col < img_width; col++)															// Top row border care
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		buf[0][col] = 0;
		buf[row_ind][col] = _src_mat.data[read_index++]; 														// Read data
	}
	row_ind++;

	Row_Loop:																						// Process complete image
	for(row = 1; row < img_height+1; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS
		if(row_ind == 2)																			// Indexes to hold maintain the row index
		{
			tp = 0; mid = 1; bottom = 2;
		}
		else if(row_ind == 0)
		{
			tp = 1; mid = 2; bottom = 0;
		}
		else if(row_ind == 1)
		{
			tp = 2; mid = 0; bottom = 1;
		}

		src_buf1[0] = src_buf1[1] = 0;
		src_buf2[0] = src_buf2[1] = 0;
		src_buf3[0] = src_buf3[1] = 0;

		/***********		Process complete row			**********/
		P0 = P1 = 0;
		ProcessScharr3x3<SRC_T, DST_T, ROWS, COLS, PLANES,DEPTH_SRC, DEPTH_DST, NPC, WORDWIDTH_SRC, WORDWIDTH_DST, TC>
		(_src_mat, _gradx_mat, _grady_mat, buf, src_buf1, src_buf2, src_buf3, GradientValuesX, GradientValuesY, P0, P1, img_width, img_height, row_ind, shift_x, shift_y, tp, mid, bottom, row, read_index, write_index);

		/*			Last column border care	for 8-pixel Case			*/
		if((NPC == XF_NPPC8))
		{
			//	Compute gradient at last column
			GradientValuesX[0] = xFGradientX<DEPTH_SRC, DEPTH_DST>(
					src_buf1[buf_size-2], src_buf1[buf_size-1], 0,
					src_buf2[buf_size-2], src_buf2[buf_size-1], 0,
					src_buf3[buf_size-2], src_buf3[buf_size-1], 0);

			GradientValuesY[0] = xFGradientY<DEPTH_SRC, DEPTH_DST>(
					src_buf1[buf_size-2], src_buf1[buf_size-1], 0,
					src_buf2[buf_size-2], src_buf2[buf_size-1], 0,
					src_buf3[buf_size-2], src_buf3[buf_size-1], 0);
		}
		else							/*			Last column border care	for NO Case			*/
		{
			int STEP,q=0;
			if((DEPTH_DST == XF_48SP) || (DEPTH_DST == XF_16SP)	)
			{
				  STEP=16;
			}
			else
			{
				  STEP=8;
			}

			for(ap_uint<5> i=0,k=0;i<PLANES;i++,k+=8)
			{
			#pragma HLS UNROLL
			GradientValuesX[0].range(q+(STEP-1),q) = xFGradientX<DEPTH_SRC, DEPTH_DST>(
					src_buf1[buf_size-3].range(k+7,k), src_buf1[buf_size-2].range(k+7,k), 0,
					src_buf2[buf_size-3].range(k+7,k), src_buf2[buf_size-2].range(k+7,k), 0,
					src_buf3[buf_size-3].range(k+7,k), src_buf3[buf_size-2].range(k+7,k), 0);

			GradientValuesY[0].range(q+(STEP-1),q) = xFGradientY<DEPTH_SRC, DEPTH_DST>(
					src_buf1[buf_size-3].range(k+7,k), src_buf1[buf_size-2].range(k+7,k), 0,
					src_buf2[buf_size-3].range(k+7,k), src_buf2[buf_size-2].range(k+7,k), 0,
					src_buf3[buf_size-3].range(k+7,k), src_buf3[buf_size-2].range(k+7,k), 0);
			q+=STEP;
			}
		}

		xfPackPixels<NPC, WORDWIDTH_DST, DEPTH_DST>(&GradientValuesX[0], P0, 0, 1, shift_x);
		xfPackPixels<NPC, WORDWIDTH_DST, DEPTH_DST>(&GradientValuesY[0], P1, 0, 1, shift_y);

		_gradx_mat.data[write_index] = P0;
		_grady_mat.data[write_index++] = P1;

		shift_x = 0; shift_y = 0;
		P0 = 0; P1 = 0;

		row_ind++;
		if(row_ind == 3)
		{
			row_ind = 0;
		}
	} // Row_Loop
}



//#pragma SDS data mem_attribute("_src_mat.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
//#pragma SDS data mem_attribute("_dst_matx.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
//#pragma SDS data mem_attribute("_dst_maty.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("_src_mat.data":SEQUENTIAL, "_dst_matx.data":SEQUENTIAL,"_dst_maty.data":SEQUENTIAL)
//#pragma SDS data data_mover("_src_mat.data":AXIDMA_SIMPLE)
//#pragma SDS data data_mover("_dst_matx.data":AXIDMA_SIMPLE)
//#pragma SDS data data_mover("_dst_maty.data":AXIDMA_SIMPLE)
//#pragma SDS data sys_port("_src_mat.data":HP)
//#pragma SDS data sys_port("_dst_matx.data":HP)
//#pragma SDS data sys_port("_dst_maty.data":HP)
#pragma SDS data copy("_src_mat.data"[0:"_src_mat.size"], "_dst_matx.data"[0:"_dst_matx.size"],"_dst_maty.data"[0:"_dst_maty.size"])

template<int BORDER_TYPE, int SRC_T,int DST_T, int ROWS, int COLS,int NPC=1>
void Scharr(xf::Mat<SRC_T, ROWS, COLS, NPC> & _src_mat,xf::Mat<DST_T, ROWS, COLS, NPC> & _dst_matx,xf::Mat<DST_T, ROWS, COLS, NPC> & _dst_maty)
{
	
#pragma HLS INLINE OFF

	assert(((NPC == XF_NPPC1) || (NPC == XF_NPPC8))
			&& "NPC must be XF_NPPC1 or XF_NPPC8");

	assert((BORDER_TYPE == XF_BORDER_CONSTANT) && "Border type must be XF_BORDER_CONSTANT ");

	uint16_t img_height = _src_mat.rows;
	uint16_t img_width = (_src_mat.cols >> XF_BITSHIFT(NPC));

	assert(((img_height <= ROWS ) && (_src_mat.cols <= COLS)) && "ROWS and COLS should be greater than input image");

	xFScharrFilterKernel<SRC_T, DST_T, ROWS,COLS,XF_CHANNELS(SRC_T,NPC),XF_DEPTH(SRC_T,NPC),XF_DEPTH(DST_T,NPC),NPC,XF_WORDWIDTH(SRC_T,NPC),XF_WORDWIDTH(DST_T,NPC), (COLS>>XF_BITSHIFT(NPC))>
	(_src_mat,_dst_matx,_dst_maty,img_height,img_width);

}
}
#endif  // _XF_SCHARR_HPP_
