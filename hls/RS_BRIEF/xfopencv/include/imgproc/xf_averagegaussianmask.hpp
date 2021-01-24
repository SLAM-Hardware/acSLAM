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
 HOWEVER CXFSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ***************************************************************************/

#ifndef _XF_AVERAGEGAUSSIANMASK_HPP_
#define _XF_AVERAGEGAUSSIANMASK_HPP_

/*****************************************************************
 * 		                 GaussianFilter3x3
 *****************************************************************
 * -------------
 * |1	2 	1|
 * |2	4	2|
 * |1	2	1|
 * -------------
 *****************************************************************/
template<int DEPTH_SRC>
XF_PTNAME(DEPTH_SRC) xFGaussianFixed3x3(XF_PTNAME(DEPTH_SRC) t0, XF_PTNAME(DEPTH_SRC) t1, XF_PTNAME(DEPTH_SRC) t2,
		XF_PTNAME(DEPTH_SRC) m0, XF_PTNAME(DEPTH_SRC) m1, XF_PTNAME(DEPTH_SRC) m2,
		XF_PTNAME(DEPTH_SRC) b0, XF_PTNAME(DEPTH_SRC) b1, XF_PTNAME(DEPTH_SRC) b2)
		{
#pragma HLS INLINE

	XF_PTNAME(DEPTH_SRC) g_x = 0;
	uint16_t A00 = (uint16_t)t0+t2;
	uint16_t A01 = (uint16_t)b0+b2;
	uint16_t A02 = (uint16_t)t1+m0;
	uint16_t A03 = (uint16_t)m2+b1;
	uint16_t A0 = (uint16_t)A00+A01;
	uint16_t A1 = ((uint16_t)(A02+A03))<<1;
	uint16_t A2 = (((uint16_t)m1)<<2);
	g_x = ((A0+A1+A2) >> 4);
	return g_x;
		}

/**
 * xFAverageGaussian3x3 : Applies the mask and Computes the gradient values
 *
 */
template<int NPC,int DEPTH_SRC>
void xFAverageGaussian3x3(XF_PTNAME(DEPTH_SRC) *Maskvalues,
		XF_PTNAME(DEPTH_SRC) *src_buf1,
		XF_PTNAME(DEPTH_SRC) *src_buf2,
		XF_PTNAME(DEPTH_SRC) *src_buf3)
{
#pragma HLS INLINE

	Compute_Grad_Loop:
	for(int j = 0; j < (1 << XF_BITSHIFT(NPC)); j++)
	{
#pragma HLS UNROLL
		Maskvalues[j] = xFGaussianFixed3x3<DEPTH_SRC>(
				src_buf1[j], src_buf1[j+1], src_buf1[j+2],
				src_buf2[j], src_buf2[j+1], src_buf2[j+2],
				src_buf3[j], src_buf3[j+1],	src_buf3[j+2]);
	}
}

template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH, int TC>
void ProcessAverageGaussian3x3(hls::stream< XF_SNAME(WORDWIDTH) > & _src_mat,
		hls::stream< XF_SNAME(WORDWIDTH) > & _out_mat,
		XF_SNAME(WORDWIDTH) buf[3][(COLS >> XF_BITSHIFT(NPC))], XF_PTNAME(DEPTH) src_buf1[XF_NPIXPERCYCLE(NPC)+2],
		XF_PTNAME(DEPTH) src_buf2[XF_NPIXPERCYCLE(NPC)+2], XF_PTNAME(DEPTH) src_buf3[XF_NPIXPERCYCLE(NPC)+2],
		XF_PTNAME(DEPTH) OutputValues[XF_NPIXPERCYCLE(NPC)],
		XF_SNAME(WORDWIDTH) &P0, uint16_t img_width,  uint16_t img_height, uint16_t &shift_x,  ap_uint<2> tp, ap_uint<2> mid, ap_uint<2> bottom, ap_uint<13> row)
{
#pragma HLS INLINE

	XF_SNAME(WORDWIDTH) buf0, buf1, buf2;
	ap_uint<5> npc = XF_NPIXPERCYCLE(NPC);
	ap_uint<5> buf_size = XF_NPIXPERCYCLE(NPC) + 2;

	Col_Loop:
	for(ap_uint<13> col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		if(row < img_height)
			buf[bottom][col] = _src_mat.read(); // Read data
		else
			buf[bottom][col] = 0;

		buf0 = buf[tp][col];
		buf1 = buf[mid][col];
		buf2 = buf[bottom][col];

		xfExtractPixels<NPC, WORDWIDTH, DEPTH>(&src_buf1[2], buf0, 0);
		xfExtractPixels<NPC, WORDWIDTH, DEPTH>(&src_buf2[2], buf1, 0);
		xfExtractPixels<NPC, WORDWIDTH, DEPTH>(&src_buf3[2], buf2, 0);

		xFAverageGaussian3x3<NPC, DEPTH>(OutputValues,
				src_buf1, src_buf2, src_buf3);

		if(col == 0)
		{
			shift_x = 0;
			P0 = 0;

			xfPackPixels<NPC, WORDWIDTH, DEPTH>(&OutputValues[0], P0, 1, (npc-1), shift_x);

		}
		else
		{
			xfPackPixels<NPC, WORDWIDTH, DEPTH>(&OutputValues[0], P0, 0, 1, shift_x);


			_out_mat.write(P0);

			shift_x = 0;
			P0 = 0;

			xfPackPixels<NPC, WORDWIDTH, DEPTH>(&OutputValues[0], P0, 1, (npc-1), shift_x);

		}

		src_buf1[0] = src_buf1[buf_size-2];
		src_buf1[1] = src_buf1[buf_size-1];
		src_buf2[0] = src_buf2[buf_size-2];
		src_buf2[1] = src_buf2[buf_size-1];
		src_buf3[0] = src_buf3[buf_size-2];
		src_buf3[1] = src_buf3[buf_size-1];
	} // Col_Loop
}


/**
 * xFAverageGaussianMask3x3 : Computes Gaussian of the input image for filtersize 3x3
 * _src_mat		: Input image
 * _dst_mat	    : Result
 */
template<int ROWS, int COLS, int DEPTH_SRC, int NPC, int WORDWIDTH_SRC,int TC>
void xFAverageGaussianMask3x3(hls::stream<XF_SNAME(WORDWIDTH_SRC) >& _src_mat,
		hls::stream< XF_SNAME(WORDWIDTH_SRC) >& _out_mat,
		uint16_t img_height,uint16_t img_width)
{
	img_width = img_width>>XF_BITSHIFT(NPC);
	ap_uint<13> row_ind;
	ap_uint<2> tp, mid, bottom;
	ap_uint<5> buf_size = XF_NPIXPERCYCLE(NPC) + 2;
	uint16_t shift_x = 0;
	ap_uint<13> row, col;

	XF_PTNAME(DEPTH_SRC) OutputValues[XF_NPIXPERCYCLE(NPC)];

#pragma HLS ARRAY_PARTITION variable=OutputValues complete dim=1


	XF_PTNAME(DEPTH_SRC) src_buf1[XF_NPIXPERCYCLE(NPC)+2], src_buf2[XF_NPIXPERCYCLE(NPC)+2],src_buf3[XF_NPIXPERCYCLE(NPC)+2];
#pragma HLS ARRAY_PARTITION variable=src_buf1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf3 complete dim=1

	XF_SNAME(WORDWIDTH_SRC) P0;

	XF_SNAME(WORDWIDTH_SRC) buf[3][(COLS >> XF_BITSHIFT(NPC))];
#pragma HLS RESOURCE variable=buf core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1
	row_ind = 1;

	Clear_Row_Loop:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		buf[0][col] = 0;
		buf[row_ind][col] = _src_mat.read();
	}
	row_ind++;

	Row_Loop:
	for(row = 1; row < img_height+1; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS
		if(row_ind == 2)
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


		P0 = 0;
		ProcessAverageGaussian3x3<ROWS, COLS, DEPTH_SRC, NPC, WORDWIDTH_SRC, TC>
		(_src_mat, _out_mat, buf, src_buf1, src_buf2, src_buf3,OutputValues, P0, img_width, img_height, shift_x, tp, mid, bottom, row);


		if ((NPC == XF_NPPC8) || (NPC == XF_NPPC16))
		{

			OutputValues[0] = xFGaussianFixed3x3<DEPTH_SRC>(
					src_buf1[buf_size-2],src_buf1[buf_size-1], 0,
					src_buf2[buf_size-2],src_buf2[buf_size-1], 0,
					src_buf3[buf_size-2],src_buf3[buf_size-1], 0);


		}
		else
		{
			OutputValues[0] = xFGaussianFixed3x3<DEPTH_SRC>(
					src_buf1[buf_size-3], src_buf1[buf_size-2], 0,
					src_buf2[buf_size-3], src_buf2[buf_size-2], 0,
					src_buf3[buf_size-3], src_buf3[buf_size-2], 0);


		}

		xfPackPixels<NPC, WORDWIDTH_SRC, DEPTH_SRC>(&OutputValues[0], P0, 0, 1, shift_x);


		_out_mat.write(P0);


		shift_x = 0;
		P0 = 0;

		row_ind++;
		if(row_ind == 3)
		{
			row_ind = 0;
		}
	} // Row_Loop
}
// xFAverageGaussianMask3x3

#endif

