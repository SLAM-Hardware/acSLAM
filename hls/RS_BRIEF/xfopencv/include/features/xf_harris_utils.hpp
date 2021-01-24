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

#ifndef _XF_HARRIS_UTILS_H_
#define _XF_HARRIS_UTILS_H_

#ifndef __cplusplus
#error C++ is needed to use this file!
#endif

/**
 *  xFDuplicate
 */
template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH, int TC>
void xFDuplicate(hls::stream< XF_SNAME(WORDWIDTH) > &_src_mat,
				 hls::stream< XF_SNAME(WORDWIDTH) > &_dst1_mat,
				 hls::stream< XF_SNAME(WORDWIDTH) > &_dst2_mat, uint16_t img_height, uint16_t img_width)
{
	img_width = img_width >> XF_BITSHIFT(NPC);

	ap_uint<13> row, col;
	Row_Loop:
	for(row = 0 ; row < img_height; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS
#pragma HLS LOOP_FLATTEN off
		Col_Loop:
		for(col = 0; col < img_width; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
			XF_SNAME(WORDWIDTH) tmp_src;
			tmp_src = _src_mat.read();
			_dst1_mat.write(tmp_src);
			_dst2_mat.write(tmp_src);
		}
	}
}
// xFDuplicate

/**
 * xFSquare : Compute square of the input image
 */
template<int ROWS, int COLS, int IN_DEPTH, int OUT_DEPTH,
		 int NPC, int IN_WW, int OUT_WW, int TC, typename SCALE_T>
void xFSquare(hls::stream < XF_SNAME(IN_WW) > &_src_mat,
			  hls::stream < XF_SNAME(OUT_WW) > &_dst_mat,
			  SCALE_T scale, uint8_t filter_width, uint16_t img_height, uint16_t img_width)
{
	img_width = img_width >> XF_BITSHIFT(NPC);

	ap_uint<13> row, col;
	XF_SNAME(IN_WW) tmp_src;
	XF_SNAME(OUT_WW) tmp_dst;
	uint16_t shift = 0;
	uint16_t npc = XF_NPIXPERCYCLE(NPC);

	XF_PTNAME(IN_DEPTH) src_buf[(1 << XF_BITSHIFT(NPC))];
	XF_PTNAME(OUT_DEPTH) dst_buf[(1 << XF_BITSHIFT(NPC))];
#pragma HLS ARRAY_PARTITION variable=src_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst_buf complete dim=1
	Row_Loop:
	for(row = 0 ; row < img_height; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS
#pragma HLS LOOP_FLATTEN off
		Col_Loop:
		for(col = 0; col < img_width; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
			tmp_src = _src_mat.read();
			xfExtractPixels<NPC, IN_WW, IN_DEPTH>(src_buf, tmp_src, 0);

			Square_Loop:
			for(ap_uint<9> k = 0; k < (1 << XF_BITSHIFT(NPC)); k++)
			{
#pragma HLS UNROLL
				XF_PTNAME(IN_DEPTH) val;

				if(filter_width == XF_FILTER_7X7){
					int16_t  val2;
					uint16_t val1;
					val2 = src_buf[k] >> 9;
					if(val2 < 0)
						val1 = -(val2);
					else
						val1 = val2;

					dst_buf[k] = ((val1 * val1) >> scale);

				}else {
					if(src_buf[k] < 0)
						val = -(src_buf[k]);
					else
						val = src_buf[k];

					dst_buf[k] = (val * val) >> scale;
				}
			}

			tmp_dst = 0;
			xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&dst_buf[0], tmp_dst, 0, npc, shift);
			shift = 0;
			_dst_mat.write(tmp_dst);									// Write the data in to output stream
		} // Col_Loop
	} // Row_Loop
}
// xFSquare

/**
 *  xFMultiply : Compute dst = src1 * src2
 */
template<int ROWS, int COLS, int IN_DEPTH, int OUT_DEPTH, int NPC,
         int IN_WW, int OUT_WW, int TC,  typename SCALE_T>
void xFMultiply(hls::stream< XF_SNAME(IN_WW) > &_src1_mat,
				hls::stream< XF_SNAME(IN_WW) > &_src2_mat,
				hls::stream< XF_SNAME(OUT_WW) > &_dst_mat,
		        SCALE_T scale, uint8_t filter_width, uint16_t img_height, uint16_t img_width)
{
	img_width = img_width >> XF_BITSHIFT(NPC);

	ap_uint<13> row, col;
	XF_SNAME(IN_WW) tmp_src1, tmp_src2;
	XF_SNAME(OUT_WW) tmp_dst;
	XF_PTNAME(IN_DEPTH) src_buf1[(1 << XF_BITSHIFT(NPC))], src_buf2[(1 << XF_BITSHIFT(NPC))];
	XF_PTNAME(OUT_DEPTH) dst_buf[(1 << XF_BITSHIFT(NPC))];
#pragma HLS ARRAY_PARTITION variable=src_buf1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst_buf complete dim=1

	uint16_t npc = XF_NPIXPERCYCLE(NPC);
	uint16_t shift = 0;

	Row_Loop:
	for(row = 0 ; row < img_height; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS
#pragma HLS LOOP_FLATTEN off
		Col_Loop:
		for(col = 0; col < img_width; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
			tmp_src1 = _src1_mat.read();	// Read data from the source1
			tmp_src2 = _src2_mat.read();	// Read data from the source2

			/* Extract data from source */
			xfExtractPixels<NPC, IN_WW, IN_DEPTH>(src_buf1, tmp_src1, 0);
			xfExtractPixels<NPC, IN_WW, IN_DEPTH>(src_buf2, tmp_src2, 0);

			for(ap_uint<9> k = 0; k < (1 << XF_BITSHIFT(NPC)); k++)
			{
#pragma HLS UNROLL
				XF_PTNAME(IN_DEPTH) val1 = src_buf1[k];
				XF_PTNAME(IN_DEPTH) val2 = src_buf2[k];
//TODO:: I included only for filter 3x3
				if(filter_width == XF_FILTER_7X7)
				{
					int16_t val11 = val1 >> 9;
					int16_t val22 = val2 >> 9;
					dst_buf[k] = (val11 * val22) >> scale;

				}else {
					dst_buf[k] = (val1 * val2) >> scale;
				}
			}

			tmp_dst = 0;
			xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&dst_buf[0], tmp_dst, 0, npc, shift);
			shift = 0;

			_dst_mat.write(tmp_dst);   // Write data into the output stream
		} // Col_Loop
	} // Row_Loop
}
// xFMultiply

/**
 * Thresholding function
 * Arguments: Input Stream, Output Stream, Threshold
 */
template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH, int TC>
void xFThreshold(hls::stream< XF_SNAME(WORDWIDTH) > &_src_mat,
				 hls::stream< XF_SNAME(WORDWIDTH) > &_dst_mat,
				 uint16_t threshold, uint16_t img_height, uint16_t img_width)
{
	img_width = img_width >> XF_BITSHIFT(NPC);

	XF_SNAME(WORDWIDTH) tmp_src;
	int buf1;
	XF_PTNAME(DEPTH) thresh = threshold;
	int res[(1 << XF_BITSHIFT(NPC))];
	ap_uint<9> i, j;
	ap_uint<8> STEP = XF_PIXELDEPTH(XF_32UP);
	Row_Loop:
	for(uint16_t row = 0 ; row < img_height; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS
#pragma HLS LOOP_FLATTEN off
		Col_Loop:
		for(uint16_t col = 0; col < img_width; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
			tmp_src = _src_mat.read(); // Read data from the input stream

			Threshold_Loop:
			for(i = 0, j = 0; i < (32 << XF_BITSHIFT(NPC)); i+=32)
			{
#pragma HLS unroll
				buf1 = tmp_src.range(i+31, i);

				/*	Pack the data into result  */
				buf1 = (buf1 > thresh) ? buf1 : 0;
				res[j++] = (uint32_t)buf1;
			}

			uint16_t npc = XF_NPIXPERCYCLE(NPC);

			uint16_t shift = 0;
			tmp_src = 0;

			for(i = 0; i < npc; i++)
			{
#pragma HLS unroll
				uint32_t tmp =  res[i];
				tmp_src = tmp_src | (((XF_SNAME(WORDWIDTH)) tmp) << (shift*STEP));
				shift++;
			}
			shift = 0;

			_dst_mat.write(tmp_src);	// Write data into output pixel
		} // Col_Loop
	} // Row_Loop
}
// xFThreshold

/**
 *  Compute score = det - k * trace^2
 *
 *  _src1_mat --> gx^2
 *  _src1_mat --> gx * gy
 *  _src1_mat --> gy^2
 *  _dst_mat --> Result
 */
template<int ROWS, int COLS, int IN_DEPTH, int OUT_DEPTH, int NPC, int IN_WW, int OUT_WW, int TC>
void xFComputeScore(hls::stream < XF_SNAME(IN_WW) > &_src1_mat,
					hls::stream < XF_SNAME(IN_WW) > &_src2_mat,
					hls::stream < XF_SNAME(IN_WW) > &_src3_mat,
					hls::stream < XF_SNAME(OUT_WW) > &_dst_mat, uint16_t img_height,
					uint16_t img_width, uint16_t thresold, uint8_t _filter_type)
{
	img_width = img_width >> XF_BITSHIFT(NPC);
	XF_SNAME(IN_WW) tmp_src1, tmp_src2, tmp_src3;

	XF_PTNAME(OUT_DEPTH) dst_buf[(1 << XF_BITSHIFT(NPC))];
	XF_SNAME(OUT_WW) tmp_dst;
	ap_uint<8> in_step = XF_PIXELDEPTH(IN_DEPTH);
	uint16_t in_sumloop = (XF_PIXELDEPTH(IN_DEPTH) << XF_BITSHIFT(NPC));
	uint16_t npc = XF_NPIXPERCYCLE(NPC);

	ap_int<32> tmp_res[2];
	ap_int<32> det_res;
	ap_int<17> trace_res;
#pragma HLS RESOURCE variable=trace_res core=DSP48 latency=2
	ap_int<50> trace_res1;
#pragma HLS RESOURCE variable=trace_res1 core=DSP48 latency=2
	ap_int<32> trace_res2;
#pragma HLS RESOURCE variable=trace_res2 core=DSP48 latency=2
	ap_uint<13> row, col;
	ap_uint<10> i, j;
	uint16_t shift = 0;

	Row_Loop:
	for(row = 0 ; row < img_height; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS
#pragma HLS LOOP_FLATTEN off
		Col_Loop:
		for(col = 0; col < img_width; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
			tmp_src1 = _src1_mat.read();
			tmp_src2 = _src2_mat.read();
			tmp_src3 = _src3_mat.read();

			Determinant_Loop:
			for(i = 0, j= 0; i < in_sumloop; i += in_step)
			{
#pragma HLS unroll
				XF_PTNAME(IN_DEPTH) val1, val2, val3;
				val1 = tmp_src1.range(i+(in_step-1), i);
				val2 = tmp_src2.range(i+(in_step-1), i);
				val3 = tmp_src3.range(i+(in_step-1), i);
//TODO:shift according to box filter
				if(_filter_type == XF_FILTER_7X7){
					val1 = val1 >> 0;
					val2 = val2 >> 0;
					val3 = val3 >> 0;
				}else{
					val1 = val1 >> 2;
					val2 = val2 >> 2;
					val3 = val3 >> 2;
				}

				/* Compute determinant */

				tmp_res[0] = ((ap_int<32>)val1 * (ap_int<32>)val2);
				tmp_res[1] = ((ap_int<32>)val3 * (ap_int<32>)val3);

				det_res = tmp_res[0] - tmp_res[1];

				/*	Compute trace */
				trace_res = val1 + val2;

				/*	Compute det - k*trace^2	 */
				trace_res1 = trace_res * trace_res;
				trace_res2 = (trace_res1 * thresold) >> 16;

				if(_filter_type == XF_FILTER_7X7){
					dst_buf[j++]  = (XF_PTNAME(OUT_DEPTH))((det_res - trace_res2) >> 8);
				}
				else{
					dst_buf[j++]  = (XF_PTNAME(OUT_DEPTH))(det_res - trace_res2);
				}
			}
			tmp_dst = 0;
			xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&dst_buf[0], tmp_dst, 0, npc, shift);
			shift = 0;
			_dst_mat.write(tmp_dst); // Write data into output pixel
		}
	}
}
// xFDeterminant

#endif // _XF_HARRIS_UTILS_H_
