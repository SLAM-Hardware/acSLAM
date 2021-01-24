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

#ifndef _XF_REMAP_HPP_
#define _XF_REMAP_HPP_

#ifndef __cplusplus
#error C++ is needed to include this header.
#endif

#include "hls_stream.h"
#include "common/xf_common.h"
#include "common/xf_utility.h"
#include <algorithm>

#define XF_RESIZE_INTER_TAB_SIZE 32
#define XF_RESIZE_INTER_BITS     5

namespace xf{

template <int SRC_T, int DST_T, int MAP_T, int WIN_ROW, int ROWS, int COLS, int NPC,bool USE_URAM>
void xFRemapNNI(
		xf::Mat<SRC_T, ROWS, COLS, NPC>  &src,
		xf::Mat<DST_T, ROWS, COLS, NPC>  &dst,
		xf::Mat<MAP_T, ROWS, COLS, NPC>  &mapx,
		xf::Mat<MAP_T, ROWS, COLS, NPC>  &mapy,
		uint16_t rows, uint16_t cols
)
{
	XF_TNAME(DST_T,NPC) buf[WIN_ROW][COLS];
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1

	XF_TNAME(SRC_T,NPC) s;
	int read_pointer_src = 0, read_pointer_map = 0, write_pointer = 0;

	ap_uint<64> bufUram[WIN_ROW][(COLS+7)/8];
#pragma HLS resource variable=bufUram core=RAM_T2P_URAM latency=2
	XF_TNAME(SRC_T,NPC) sx8[8];
#pragma HLS ARRAY_PARTITION variable=sx8 complete dim=1

	XF_TNAME(DST_T,NPC) d;
	XF_TNAME(MAP_T,NPC) mx_fl;
	XF_TNAME(MAP_T,NPC) my_fl;

	assert(rows <= ROWS);
	assert(cols <= COLS);
	int ishift=WIN_ROW/2;
	int r[WIN_ROW] = {};
	int row_tripcount = ROWS+WIN_ROW;

	loop_height: for( int i=0; i< rows+ishift; i++)
	{
#pragma HLS LOOP_FLATTEN OFF
#pragma HLS LOOP_TRIPCOUNT min=1 max=row_tripcount

		loop_width: for( int j=0; j< cols; j++)
		{
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=buf     inter false
#pragma HLS dependence variable=bufUram inter false
#pragma HLS dependence variable=r       inter false
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS

			if(i<rows&& j<cols)
			{
				s = src.data[read_pointer_src++];

				if (USE_URAM) {
					sx8[j%8] = s;
					for (int k=0; k<8; k++) bufUram[i % WIN_ROW][j/8](k*8+7,k*8) = sx8[k];
				}
			}

			if (!USE_URAM)
				buf[i % WIN_ROW][j] = s;
			r[i % WIN_ROW] = i;

			if(i>=ishift)
			{
				mx_fl = mapx.data[read_pointer_map];
				my_fl = mapy.data[read_pointer_map++];
				int x = (int)(mx_fl+0.5f);
				int y = (int)(my_fl+0.5f);

				bool in_range = (y>=0 && my_fl<=(rows-1) && r[y%WIN_ROW] == y && x>=0 && mx_fl<=(cols-1));
				if(in_range)
					if (USE_URAM) {
						XF_TNAME(DST_T,NPC) dx9[8];
#pragma HLS ARRAY_PARTITION variable=dx9 complete dim=1

		ap_uint<72> tempvalue = bufUram[y%WIN_ROW][x/8];
for (int k=0; k<8; k++)
{
	dx9[k] = tempvalue.range(k*8+7,k*8);
}
d = dx9[x%8];
					} else
						d = buf[y%WIN_ROW][x];
				else
					d = 0;

				dst.data[write_pointer++] = d;
			}
		}
	}
}


#define TWO_POW_16 65536
template <int SRC_T, int DST_T, int MAP_T, int WIN_ROW, int ROWS, int COLS, int NPC,bool USE_URAM>
void xFRemapLI(
		xf::Mat<SRC_T, ROWS, COLS, NPC>   &src,
		xf::Mat<DST_T, ROWS, COLS, NPC>   &dst,
		xf::Mat<MAP_T, ROWS, COLS, NPC>   &mapx,
		xf::Mat<MAP_T, ROWS, COLS, NPC>  &mapy,
		uint16_t rows, uint16_t cols
)
{
	// Add one to always get zero for boundary interpolation. Maybe need initialization here?
	 XF_TNAME(DST_T,NPC) buf[WIN_ROW/2+1][2][COLS/2+1][2]; //AK,ZoTech: static added for initialization, otherwise X are generated in co-sim.
#pragma HLS array_partition complete variable=buf dim=2
#pragma HLS array_partition complete variable=buf dim=4
	XF_TNAME(SRC_T,NPC) s;


	//URAM storage garnularity is 3x3-pel block in 2x2-pel picture grid, it fits to one URAM word
	ap_uint<72> bufUram[(WIN_ROW+1)/2][(COLS+1)/2];
#pragma HLS resource variable=bufUram core=RAM_T2P_URAM latency=2

	ap_uint<24> lineBuf[(COLS+1)/2];
#pragma HLS resource variable=lineBuf core=RAM_S2P_BRAM latency=1

	XF_TNAME(MAP_T,NPC) mx;
	XF_TNAME(MAP_T,NPC) my;

	int read_pointer_src = 0, read_pointer_map = 0, write_pointer = 0;

	assert(rows <= ROWS);
	assert(cols <= COLS);
	int ishift=WIN_ROW/2;
	int r1[WIN_ROW] = {};
	int r2[WIN_ROW] = {};
	int row_tripcount = ROWS+WIN_ROW;

	bool store_col = 1;
	bool store_row = 1;

	ap_uint<16> temppix     = 0;
	ap_uint<24> pixval      = 0;
	ap_uint<48> pixval_2    = 0;
	ap_uint<24> prev_pixval = 0;

	loop_height: for( int i=0; i< rows+ishift; i++)
	{
#pragma HLS LOOP_FLATTEN OFF
#pragma HLS LOOP_TRIPCOUNT min=1 max=row_tripcount

		// Initialize for every row
		store_col = 1;

		loop_width: for( int j=0; j<cols+1; j++)
		{
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=buf     inter false
#pragma HLS dependence variable=bufUram inter false
#pragma HLS dependence variable=bufUram intra false
#pragma HLS dependence variable=r1      inter false
#pragma HLS dependence variable=r2      inter false
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS+2

			if (i<rows && j<cols) {
				s = src.data[read_pointer_src++];
			} else {
				s = 0;
			}

			if (USE_URAM && i<rows) {

				if (store_col && (j!=0)) {
					pixval.range(15, 0) = temppix;
					pixval.range(23,16) = s;

					if (store_row) {
						// Store every 3rd row in a buffer
						lineBuf[(j/2)-1] = pixval;
					} else {
						// Read the stored row and fill in
						prev_pixval = lineBuf[(j/2)-1];
					}

					if (i!=0) {
						if (store_row) {
							bufUram[((i-1)/2)%(WIN_ROW/2)][(j/2)-1].range(71, 48) = pixval;
						} else {
							pixval_2.range(23,  0) = prev_pixval;
							pixval_2.range(47, 24) = pixval;
							bufUram[((i-1)/2)%(WIN_ROW/2)][(j/2)-1].range(47, 0) = pixval_2;
						}
					}
				}

				if (store_col) {
					temppix.range( 7, 0) = s;
				} else {
					temppix.range(15, 8) = s;
				}

				store_col= !(store_col);
			}

			if (!USE_URAM) {
				if((i % WIN_ROW) % 2) {
					buf[(i % WIN_ROW)/2][(i % WIN_ROW) % 2][j/2][j%2] = s;
				} else {
					buf[(i % WIN_ROW)/2][(i % WIN_ROW) % 2][j/2][j%2] = s;
				}
			}

			r1[i % WIN_ROW] = i;
			r2[i % WIN_ROW] = i;

			if(i>=ishift && j<cols)
			{
				mx = mapx.data[read_pointer_map];
				my = mapy.data[read_pointer_map++];
				float x_fl = mx;
				float y_fl = my;

				int x_fix = (int) ((float)x_fl * (float)XF_RESIZE_INTER_TAB_SIZE);  // mapx data in A16.XF_RESIZE_INTER_TAB_SIZE format
				int y_fix = (int) ((float)y_fl * (float)XF_RESIZE_INTER_TAB_SIZE);  // mapy data in A16.XF_RESIZE_INTER_TAB_SIZE format

				int x = x_fix >> XF_RESIZE_INTER_BITS;
				int y = y_fix >> XF_RESIZE_INTER_BITS;
				int x_frac = x_fix  & (XF_RESIZE_INTER_TAB_SIZE-1);
				int y_frac = y_fix  & (XF_RESIZE_INTER_TAB_SIZE-1);
				int ynext = y+1;

				ap_ufixed<XF_RESIZE_INTER_BITS, 0> iu, iv;
				iu(XF_RESIZE_INTER_BITS-1, 0) = x_frac;
				iv(XF_RESIZE_INTER_BITS-1, 0) = y_frac;

				// Note that the range here is larger than expected by 1 horizontal and 1 vertical pixel, to allow
				// Interpolating at the edge of the image
				bool in_range = (y>=0 && y_fl<=(rows-1) && r1[y%WIN_ROW]==y && r2[ynext%WIN_ROW]==ynext && x>=0 && x_fl<=(cols-1));

				int xa0, xa1, ya0, ya1;
				// The buffer is essentially cyclic partitioned, but we have
				// to do this manually because HLS can't figure it out.
				// The code below is wierd, but it is this code expanded.
				//  if ((y % WIN_ROW) % 2) {
				//                     // Case 1, where y hits in bank 1 and ynext in bank 0
				//                     ya0 = (ynext%WIN_ROW)/2;
				//                     ya1 = (y%WIN_ROW)/2;
				//                 } else {
				//                     // The simpler case, where y hits in bank 0 and ynext hits in bank 1
				//                     ya0 = (y%WIN_ROW)/2;
				//                     ya1 = (ynext%WIN_ROW)/2;
				//                 }
				// Both cases reduce to this, if WIN_ROW is a multiple of two.
				assert(((WIN_ROW & 1) == 0) && "WIN_ROW must be a multiple of two");
				xa0 = x/2 + x%2;
				xa1 = x/2;
				ya0 = (y/2 + y%2)%(WIN_ROW/2);
				ya1 = (y/2)%(WIN_ROW/2);

				XF_TNAME(DST_T,NPC) d00, d01, d10, d11;

				if (USE_URAM) {
					XF_TNAME(DST_T,NPC) d3x3[9];
#pragma HLS ARRAY_PARTITION variable=d3x3 complete

					ap_uint<72> tempbuf;
					tempbuf=bufUram[ya1][xa1];

					for (int k=0; k<9; k++)
					{
						d3x3[k] = tempbuf.range(k*8+7,k*8);
					}

					d00 = d3x3[(y%2  )*3 + x%2  ];
					d01 = d3x3[(y%2  )*3 + x%2+1];
					d10 = d3x3[(y%2+1)*3 + x%2  ];
					d11 = d3x3[(y%2+1)*3 + x%2+1];
				} else {
					d00=buf[ya0][0][xa0][0];
					d01=buf[ya0][0][xa1][1];
					d10=buf[ya1][1][xa0][0];
					d11=buf[ya1][1][xa1][1];

					if(x%2) {
						//std::swap(d00,d01);
						int t=d00;
						d00=d01;
						d01=t;

						int t2=d10;
						d10=d11;
						d11=d10;
						//std::swap(d10,d11);
					}
					if(y%2) {

						int t=d00;
						d00=d10;
						d10=t;

						int t2=d01;
						d01=d11;
						d11=d01;
						//std::swap(d00,d10);
						// std::swap(d01,d11);
					}
					//if(x == (cols-1))
					//{
					//	d01=0;d11=0;
					//}
				}
				ap_ufixed<2*XF_RESIZE_INTER_BITS + 1, 1> k01 = (1-iv)*(  iu); // iu-iu*iv
				ap_ufixed<2*XF_RESIZE_INTER_BITS + 1, 1> k10 = (  iv)*(1-iu); // iv-iu*iv
				ap_ufixed<2*XF_RESIZE_INTER_BITS + 1, 1> k11 = (  iv)*(  iu); // iu*iv
				ap_ufixed<2*XF_RESIZE_INTER_BITS + 1, 1> k00 = 1-iv-k01; //(1-iv)*(1-iu) = 1-iu-iv+iu*iv = 1-iv-k01
				assert( k00 + k01 + k10 + k11 == 1);

				XF_TNAME(DST_T,NPC) d;

				if(in_range)
					d = d00 * k00 + d01 * k01 + d10 * k10 + d11 * k11;
				else
					d = 0;

				dst.data[write_pointer++] = d;
			}
		}

		store_row = !(store_row);
	}
}



//#pragma SDS data data_mover("_src_mat.data":AXIDMA_SIMPLE,"_remapped_mat.data":AXIDMA_SIMPLE,"_mapx_mat.data":AXIDMA_SIMPLE,"_mapy_mat.data":AXIDMA_SIMPLE)
//#pragma SDS data mem_attribute("_src_mat.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS,"_remapped_mat.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS,"_mapx_mat.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS,"_mapy_mat.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data access_pattern("_src_mat.data":SEQUENTIAL,"_remapped_mat.data":SEQUENTIAL,"_mapx_mat.data":SEQUENTIAL,"_mapy_mat.data":SEQUENTIAL)
#pragma SDS data copy("_src_mat.data"[0:"_src_mat.rows*_src_mat.cols"], "_remapped_mat.data"[0:"_remapped_mat.size"],"_mapx_mat.data"[0:"_mapx_mat.size"],"_mapy_mat.data"[0:"_mapy_mat.size"])
template<int WIN_ROWS, int INTERPOLATION_TYPE, int SRC_T, int MAP_T, int DST_T, int ROWS, int COLS, int NPC , bool USE_URAM=false>
void remap (xf::Mat<SRC_T, ROWS, COLS, NPC> &_src_mat, xf::Mat<DST_T, ROWS, COLS, NPC> &_remapped_mat, xf::Mat<MAP_T, ROWS, COLS, NPC> &_mapx_mat,
		xf::Mat<MAP_T, ROWS, COLS, NPC> &_mapy_mat)
{
#pragma HLS inline off
#pragma HLS dataflow

	assert ((MAP_T == XF_32FC1) && "The MAP_T must be XF_32FC1");
	//    assert ((SRC_T == XF_8UC1) && "The SRC_T must be XF_8UC1");
	//    assert ((DST_T == XF_8UC1) && "The DST_T must be XF_8UC1");
	assert ((NPC == XF_NPPC1) && "The NPC must be XF_NPPC1");

	int depth_est = WIN_ROWS*_src_mat.cols;

	uint16_t rows = _src_mat.rows;
	uint16_t cols = _src_mat.cols;


	if(INTERPOLATION_TYPE == XF_INTERPOLATION_NN) {
		xFRemapNNI<SRC_T, DST_T, MAP_T, WIN_ROWS, ROWS, COLS, NPC,USE_URAM>(_src_mat, _remapped_mat, _mapx_mat, _mapy_mat,rows,cols);
	} else if(INTERPOLATION_TYPE == XF_INTERPOLATION_BILINEAR) {
		xFRemapLI<SRC_T, DST_T, MAP_T, WIN_ROWS,ROWS,COLS, NPC,USE_URAM>(_src_mat, _remapped_mat, _mapx_mat, _mapy_mat,rows,cols);
	}
	else {
		assert (((INTERPOLATION_TYPE == XF_INTERPOLATION_NN)||(INTERPOLATION_TYPE == XF_INTERPOLATION_BILINEAR)) && "The INTERPOLATION_TYPE must be either XF_INTERPOLATION_NN or XF_INTERPOLATION_BILINEAR");
	}
}
}

#endif//_XF_REMAP_HPP_


