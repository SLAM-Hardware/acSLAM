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

#ifndef _XF_MAX_SUPPRESSION_HPP_
#define _XF_MAX_SUPPRESSION_HPP_

#ifndef __cplusplus
#error C++ is needed to include this header
#endif

/**
 * FindMax: compare center pixels with the surrounding pixels
 *
 */
template<typename SRC_T>
bool xFFindMaxRad1(SRC_T t0, SRC_T t1, SRC_T t2,
		SRC_T m0, SRC_T m1, SRC_T m2,
		SRC_T b0, SRC_T b1, SRC_T b2)
{
#pragma HLS INLINE off
	bool Max = false;
	if(m1 > t1 &&  m1> m0 && m1 > m2 && m1 > b1)
		Max = true;

	return Max;
}

/**
 * Max suppression
 */
template<int NPC , int IN_DEPTH, typename DST_T>
void xFSuppressionRad1(DST_T *Maxarray, XF_PTNAME(IN_DEPTH)* l00_buf,
		XF_PTNAME(IN_DEPTH)* l10_buf, XF_PTNAME(IN_DEPTH)* l20_buf)
{
#pragma HLS INLINE off
	Suppression_Loop:
	for(ap_uint<8> i = 0; i < (1 << XF_BITSHIFT(NPC)); i++)
	{
#pragma HLS UNROLL
		bool Max = xFFindMaxRad1(l00_buf[i], l00_buf[i+1], l00_buf[i+2],
				l10_buf[i], l10_buf[i+1], l10_buf[i+2],
				l20_buf[i], l20_buf[i+1], l20_buf[i+2]);

		Maxarray[i] = Max ? 255 : 0;
	}
}

template<int ROWS, int COLS, int IN_DEPTH, int OUT_DEPTH, int NPC, int IN_WW, int OUT_WW, int TC>
void ProcessMax1(hls::stream< XF_SNAME(IN_WW) > &_src_mat,
				 hls::stream< XF_SNAME(OUT_WW)> &_dst_mat,XF_SNAME(IN_WW) buf[3][(COLS >> XF_BITSHIFT(NPC))], XF_PTNAME(IN_DEPTH) l00_buf[XF_NPIXPERCYCLE(NPC)+2],
				 XF_PTNAME(IN_DEPTH) l10_buf[XF_NPIXPERCYCLE(NPC)+2], XF_PTNAME(IN_DEPTH) l20_buf[XF_NPIXPERCYCLE(NPC)+2],
				 XF_PTNAME(OUT_DEPTH) Array[XF_NPIXPERCYCLE(NPC)], XF_SNAME(OUT_WW) &P0, uint16_t img_width, ap_uint<13> row_ind, uint16_t &shift, ap_uint<2> tp, ap_uint<2> mid, ap_uint<2> bottom, bool flag)
{
#pragma HLS INLINE off
	ap_uint<5> nms_bufsize = ((1<<XF_BITSHIFT(NPC))+2);
	XF_SNAME(IN_WW) buf0, buf1, buf2;
	uint16_t npc = XF_NPIXPERCYCLE(NPC);

	Col_Loop:
	for(ap_uint<13> col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		if(flag)
			buf[row_ind][col] = _src_mat.read();
		buf0 = buf[tp][col];
		buf1 = buf[mid][col];
		buf2 = buf[bottom][col];

		xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l00_buf, buf0, 2);
		xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l10_buf, buf1, 2);
		xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l20_buf, buf2, 2);

		xFSuppressionRad1<NPC, IN_DEPTH>(Array, l00_buf, l10_buf, l20_buf);

		if(col == 0)
		{
			shift = 0;
			P0 = 0;
			xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&Array[0], P0, 1, (npc-1), shift);
		}
		else
		{
			xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&Array[0], P0, 0, 1, shift);
			_dst_mat.write(P0);

			shift = 0;
			P0 = 0;
			xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&Array[0], P0, 1, (npc-1), shift);
		}
		l00_buf[0] = l00_buf[nms_bufsize-2];
		l00_buf[1] = l00_buf[nms_bufsize-1];
		l10_buf[0] = l10_buf[nms_bufsize-2];
		l10_buf[1] = l10_buf[nms_bufsize-1];
		l20_buf[0] = l20_buf[nms_bufsize-2];
		l20_buf[1] = l20_buf[nms_bufsize-1];

	} // Col_Loop
}

/**
 *
 * Non Maximum suppression
 * _src_mat : input image
 * _dst_mat : output image
 */
template<int ROWS, int COLS, int IN_DEPTH, int OUT_DEPTH, int NPC, int IN_WW, int OUT_WW, int TC>
void xFMaxSuppressionRad1(hls::stream< XF_SNAME(IN_WW) > &_src_mat,
						  hls::stream< XF_SNAME(OUT_WW)> &_dst_mat,
						  uint16_t img_height, uint16_t img_width)
{

	ap_uint<13> row_ind, row, col;
	ap_uint<2> tp, mid, bottom;
	uint16_t shift = 0;
	XF_PTNAME(OUT_DEPTH) Array[(1 << XF_BITSHIFT(NPC))];
#pragma HLS ARRAY_PARTITION variable=Array complete dim=1

	ap_uint<5> nms_bufsize = ((1<<XF_BITSHIFT(NPC))+2);

	// Temporary buffers to hold image data from three rows.
	XF_PTNAME(IN_DEPTH) l00_buf[(1<<XF_BITSHIFT(NPC))+2], l10_buf[(1<<XF_BITSHIFT(NPC))+2], l20_buf[(1<<XF_BITSHIFT(NPC))+2];
#pragma HLS ARRAY_PARTITION variable=l00_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l10_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l20_buf complete dim=1

	XF_SNAME(OUT_WW) P0;

	// Line buffer to hold the image data
	XF_SNAME(IN_WW) buf[3][(COLS >> XF_BITSHIFT(NPC))];
#pragma HLS RESOURCE variable=buf core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1
	row_ind = 1;
	Clear_first_Row:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		buf[0][col] = 0;
		buf[row_ind][col] = _src_mat.read();
	}

	row_ind++;
	Row_Loop:
	for(row = 1; row < img_height; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS
#pragma HLS
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

		l00_buf[0] = l00_buf[1] = 0;
		l10_buf[0] = l10_buf[1] = 0;
		l20_buf[0] = l20_buf[1] = 0;
		P0 = 0;
		ProcessMax1<ROWS, COLS, IN_DEPTH, OUT_DEPTH, NPC, IN_WW, OUT_WW, TC>(_src_mat, _dst_mat, buf, l00_buf, l10_buf, l20_buf, Array, P0, img_width, row_ind, shift, tp, mid, bottom,true);

		if(row)
		{
			XF_PTNAME(IN_DEPTH) val = (XF_PTNAME(IN_DEPTH))0;
			if((NPC == XF_NPPC8) || (NPC == XF_NPPC16))
			{

				bool Max = xFFindMaxRad1(
						l00_buf[nms_bufsize-2], l00_buf[nms_bufsize-1], val,
						l10_buf[nms_bufsize-2], l10_buf[nms_bufsize-1], val,
						l20_buf[nms_bufsize-2], l20_buf[nms_bufsize-1], val);

				Array[0] = Max ? 255 : 0;
			}
			else
			{
				bool Max = xFFindMaxRad1(
						l00_buf[nms_bufsize-3], l00_buf[nms_bufsize-2], val,
						l10_buf[nms_bufsize-3], l10_buf[nms_bufsize-2], val,
						l20_buf[nms_bufsize-3], l20_buf[nms_bufsize-2], val);

				Array[0] = Max ? 255 : 0;
			}
			xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&Array[0], P0, 0, 1, shift);
			//P0.range(((8 << XF_BITSHIFT(NPC))-1), ((8 << XF_BITSHIFT(NPC))-8)) = Array[0];				// Get bits from certain range of positions.
			_dst_mat.write(P0);
			shift = 0;
			P0 = 0;
		}
		row_ind++;
		if(row_ind == 3)
		{
			row_ind = 0;
		}
	} // Row_Loop

	if(row_ind == 3)
	{
		row_ind = 0;
	}
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

	l00_buf[0] = l00_buf[1] = 0;
	l10_buf[0] = l10_buf[1] = 0;
	l20_buf[0] = l20_buf[1] = 0;

	Clear_Row_Loop:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		buf[bottom][col] = 0;
	}
	ProcessMax1<ROWS, COLS, IN_DEPTH, OUT_DEPTH, NPC, IN_WW, OUT_WW, TC>(_src_mat, _dst_mat, buf, l00_buf, l10_buf, l20_buf, Array, P0, img_width, row_ind, shift, tp, mid, bottom,false);

	XF_PTNAME(IN_DEPTH) val = 0;
	if((NPC == XF_NPPC8) || (NPC == XF_NPPC16))
	{
		bool Max = xFFindMaxRad1(
				l00_buf[nms_bufsize-2], l00_buf[nms_bufsize-1], val,
				l10_buf[nms_bufsize-2], l10_buf[nms_bufsize-1], val,
				l20_buf[nms_bufsize-2], l20_buf[nms_bufsize-1], val);

		Array[0] = Max ? 255 : 0;
	}
	else
	{
		bool Max = xFFindMaxRad1(
				l00_buf[nms_bufsize-3], l00_buf[nms_bufsize-2], val,
				l10_buf[nms_bufsize-3], l10_buf[nms_bufsize-2], val,
				l20_buf[nms_bufsize-3], l20_buf[nms_bufsize-2], val);

		Array[0] = Max ? 255 : 0;
	}
	xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&Array[0], P0, 0, 1, shift);
	_dst_mat.write(P0);
	shift = 0;
	P0 = 0;
}
// xFMaxSuppressionRad1


/**
 * FindMax: compare center pixels with the surrounding pixels
 *
 */
template<typename SRC_T>
bool xFFindMaxRad2(SRC_T l22, SRC_T l02, SRC_T l11,
		SRC_T l12, SRC_T l13, SRC_T l20,
		SRC_T l21, SRC_T l23, SRC_T l24,
		SRC_T l31, SRC_T l32, SRC_T l33,
		SRC_T l42)
{
#pragma HLS INLINE off
	bool Max = false;
	if ((l22 > l02) && (l22 > l11) && (l22 > l12) && (l22 > l13) &&
			(l22 > l20) && (l22 > l21) && (l22 > l23) && (l22 > l24) &&
			(l22 > l31) && (l22 > l32) && (l22 > l33) && (l22 > l42))

		Max = true;
	return Max;
}

/**
 * Max suppression
 */
template<int NPC, int IN_DEPTH, typename DST_T>
void xFSuppressionRad2(DST_T *Maxarray, XF_PTNAME(IN_DEPTH)* l00_buf, XF_PTNAME(IN_DEPTH)* l10_buf,
		XF_PTNAME(IN_DEPTH)* l20_buf, XF_PTNAME(IN_DEPTH)* l30_buf, XF_PTNAME(IN_DEPTH)* l40_buf)
{
#pragma HLS INLINE off
	Suppression_Loop:
	for(ap_uint<5> i = 0; i < (1 << XF_BITSHIFT(NPC)); i++)
	{
#pragma HLS UNROLL
		bool Max = xFFindMaxRad2(l20_buf[i+2], l00_buf[i+2], l10_buf[i+1],
								 l10_buf[i+2], l10_buf[i+3], l20_buf[i],
								 l20_buf[i+1], l20_buf[i+3], l20_buf[i+4],
								 l30_buf[i+1], l30_buf[i+2], l30_buf[i+3], l40_buf[i+2]);

		Maxarray[i] = Max ? 255 : 0;
	}
}
// xFSuppressionRad2


template<int ROWS, int COLS, int IN_DEPTH, int OUT_DEPTH, int NPC, int IN_WW, int OUT_WW, int TC>
void ProcessRad2(hls::stream< XF_SNAME(IN_WW) > & _src_mat,
				 hls::stream< XF_SNAME(OUT_WW) > & _dst_mat,
				 XF_SNAME(IN_WW) buf[5][(COLS >> XF_BITSHIFT(NPC))], XF_PTNAME(IN_DEPTH) l00_buf[XF_NPIXPERCYCLE(NPC)+4],
				 XF_PTNAME(IN_DEPTH) l10_buf[XF_NPIXPERCYCLE(NPC)+4], XF_PTNAME(IN_DEPTH) l20_buf[XF_NPIXPERCYCLE(NPC)+4], XF_PTNAME(IN_DEPTH) l30_buf[XF_NPIXPERCYCLE(NPC)+4], XF_PTNAME(IN_DEPTH) l40_buf[XF_NPIXPERCYCLE(NPC)+4],
				 XF_PTNAME(OUT_DEPTH) Array[XF_NPIXPERCYCLE(NPC)], XF_SNAME(OUT_WW) &inter_valx, uint16_t img_width, ap_uint<13> row_ind, uint16_t &shift,
				 ap_uint<4> tp1, ap_uint<4> tp2, ap_uint<4> mid, ap_uint<4> bottom1, ap_uint<4> bottom2, bool flag)
{
#pragma HLS INLINE off
	ap_uint<8> nms_bufsize = (1 << XF_BITSHIFT(NPC))+4;
	uint16_t npc = XF_NPIXPERCYCLE(NPC);
	XF_SNAME(IN_WW) buf0, buf1, buf2, buf3, buf4;

	Col_Loop:
	for(ap_uint<13> col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		if(flag)
			buf[row_ind][col] = _src_mat.read();
		buf0 = buf[tp1][col];
		buf1 = buf[tp2][col];
		buf2 = buf[mid][col];
		buf3 = buf[bottom1][col];
		buf4 = buf[bottom2][col];

		xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l00_buf, buf0, 4);
		xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l10_buf, buf1, 4);
		xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l20_buf, buf2, 4);
		xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l30_buf, buf3, 4);
		xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l40_buf, buf4, 4);

		xFSuppressionRad2<NPC, IN_DEPTH>(Array, l00_buf, l10_buf,
				l20_buf, l30_buf, l40_buf);

		for(ap_uint<4> i = 0; i < 4; i++)
		{
#pragma HLS unroll
			l00_buf[i] = l00_buf[nms_bufsize-(4 - i)];
			l10_buf[i] = l10_buf[nms_bufsize-(4 - i)];
			l20_buf[i] = l20_buf[nms_bufsize-(4 - i)];
			l30_buf[i] = l30_buf[nms_bufsize-(4 - i)];
			l40_buf[i] = l40_buf[nms_bufsize-(4 - i)];
		}
		if(col == 0)
		{
			shift = 0;
			inter_valx = 0;

			xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&Array[0], inter_valx, 2, (npc-2), shift);
		}
		else
		{
			if((NPC == XF_NPPC8) || (NPC == XF_NPPC16))
			{
				xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&Array[0], inter_valx, 0, 2, shift);
				_dst_mat.write(inter_valx);
				shift = 0;
				inter_valx = 0;
				xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&Array[0], inter_valx, 2, (npc-2), shift);
			}
			else
			{
				if(col >=2 )
				{
					inter_valx(7, 0) = Array[0];
					_dst_mat.write(inter_valx);
				}
			}
		}
	} // Col_Loop

}

/**
 * xFMaxSuppressionRad2 : performs the non maximum suppression for Radius size 2
 * _src_mat	: Input image
 * _dst_mat	: Output image
 */
template<int ROWS, int COLS, int IN_DEPTH, int OUT_DEPTH, int NPC, int IN_WW, int OUT_WW, int TC>
void xFMaxSuppressionRad2(hls::stream< XF_SNAME(IN_WW) > &_src_mat,
						  hls::stream< XF_SNAME(OUT_WW) > &_dst_mat, uint16_t img_height, uint16_t img_width)
{
	ap_uint<13> row_ind, row, col;
	ap_uint<8> tp1, tp2, mid, bottom1, bottom2;
	ap_uint<8> nms_bufsize = (1 << XF_BITSHIFT(NPC))+4;

	XF_PTNAME(OUT_DEPTH) Array[(1 << XF_BITSHIFT(NPC))];
#pragma HLS ARRAY_PARTITION variable=Array complete dim=1

	// Temporary buffers to hold image data from three rows.
	XF_PTNAME(IN_DEPTH) l00_buf[(1 << XF_BITSHIFT(NPC))+4], l10_buf[(1 << XF_BITSHIFT(NPC))+4], l20_buf[(1 << XF_BITSHIFT(NPC))+4];
	XF_PTNAME(IN_DEPTH) l30_buf[(1 << XF_BITSHIFT(NPC))+4], l40_buf[(1 << XF_BITSHIFT(NPC))+4];
#pragma HLS ARRAY_PARTITION variable=l00_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l10_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l20_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l30_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l40_buf complete dim=1

	ap_uint<8> i = 0;
	uint16_t shift = 0;
	XF_SNAME(IN_WW)  tmp_in;
	XF_SNAME(OUT_WW) inter_valx = 0;
	uint16_t npc = XF_NPIXPERCYCLE(NPC);
	XF_SNAME(IN_WW)  buf[5][(COLS >> XF_BITSHIFT(NPC))];
#pragma HLS RESOURCE variable=buf core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1

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

	Read_Row1_Loop:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		buf[row_ind][col] = _src_mat.read();
	}
	row_ind++;

	Row_Loop:
	for(row = 2; row < img_height; row++)
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

		l00_buf[0] = l00_buf[1] = l00_buf[2] = l00_buf[3] = 0;
		l10_buf[0] = l10_buf[1] = l10_buf[2] = l10_buf[3] = 0;
		l20_buf[0] = l20_buf[1] = l20_buf[2] = l20_buf[3] = 0;
		l30_buf[0] = l30_buf[1] = l30_buf[2] = l30_buf[3] = 0;
		l40_buf[0] = l40_buf[1] = l40_buf[2] = l40_buf[3] = 0;
		inter_valx = 0;

		ProcessRad2<ROWS, COLS, IN_DEPTH, OUT_DEPTH, NPC, IN_WW, OUT_WW, TC>( _src_mat, _dst_mat, buf, l00_buf, l10_buf, l20_buf, l30_buf, l40_buf, Array,
						inter_valx, img_width, row_ind, shift, tp1, tp2, mid, bottom1, bottom2, true);

		if(row >= 2)
		{
			if((NPC == XF_NPPC8) || (NPC == XF_NPPC16)){
				for(i = 4; i < nms_bufsize; i++)
				{
#pragma HLS unroll
					l00_buf[i] = 0;
					l10_buf[i] = 0;
					l20_buf[i] = 0;
					l30_buf[i] = 0;
					l40_buf[i] = 0;
				}

				Array[0] = xFFindMaxRad2(l20_buf[2], l00_buf[2], l10_buf[1],
						l10_buf[2], l10_buf[3], l20_buf[0],
						l20_buf[1], l20_buf[3], l20_buf[4],
						l30_buf[1], l30_buf[2], l30_buf[3],
						l40_buf[2]);

				Array[1] = xFFindMaxRad2(l20_buf[3], l00_buf[3], l10_buf[2],
						l10_buf[2], l10_buf[4], l20_buf[1],
						l20_buf[1], l20_buf[4], l20_buf[5],
						l30_buf[2], l30_buf[3], l30_buf[4],
						l40_buf[3]);

				xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&Array[0], inter_valx, 0, 2, shift);
				_dst_mat.write(inter_valx);
				shift = 0;
				inter_valx = 0;
			}else if(NPC == XF_NPPC1){
				l00_buf[4] = 0;
				l10_buf[4] = 0;
				l20_buf[4] = 0;
				l30_buf[4] = 0;
				l40_buf[4] = 0;

				Array[0] = xFFindMaxRad2(l20_buf[2], l00_buf[2], l10_buf[1],
										 l10_buf[2], l10_buf[3], l20_buf[0],
										 l20_buf[1], l20_buf[3], l20_buf[4],
										 l30_buf[1], l30_buf[2], l30_buf[3], l40_buf[2]);
				ap_uint<16> step = XF_PIXELDEPTH(OUT_DEPTH);
				inter_valx(((step << XF_BITSHIFT(NPC))-1), ((step << XF_BITSHIFT(NPC))-step)) = Array[0];
				_dst_mat.write(inter_valx);

				lbufLoop3:
				for(i = 0; i < 4; i++)
				{
#pragma HLS unroll
					l00_buf[i] = l00_buf[nms_bufsize-(4 - i)];
					l10_buf[i] = l10_buf[nms_bufsize-(4 - i)];
					l20_buf[i] = l20_buf[nms_bufsize-(4 - i)];
					l30_buf[i] = l30_buf[nms_bufsize-(4 - i)];
					l40_buf[i] = l40_buf[nms_bufsize-(4 - i)];
				}

				l00_buf[3] = 0;
				l10_buf[3] = 0;
				l20_buf[3] = 0;
				l30_buf[3] = 0;
				l40_buf[3] = 0;

				Array[0] = xFFindMaxRad2(l20_buf[2], l00_buf[2], l10_buf[1],
						l10_buf[2], l10_buf[3], l20_buf[0],
						l20_buf[1], l20_buf[3], l20_buf[4],
						l30_buf[1], l30_buf[2], l30_buf[3],
						l40_buf[2]);

				inter_valx(((step << XF_BITSHIFT(NPC))-1), ((step << XF_BITSHIFT(NPC))-step)) = Array[0];
				_dst_mat.write(inter_valx);
			}
		}
		row_ind++;
		if(row_ind == 5)
		{
			row_ind = 0;
		}
	} // Row_Loop ends here

	Border_Row_Loop:
	for(row = 0; row < 2; row++)
	{
		if(row_ind == 5)
		{
			row_ind = 0;
		}
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

		Clear_Row_Loop1:
		for(col = 0; col < img_width; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
			buf[bottom2][col] = 0;
		}

		l00_buf[0] = l00_buf[1] = l00_buf[2] = l00_buf[3] = 0;
		l20_buf[0] = l20_buf[1] = l20_buf[2] = l20_buf[3] = 0;
		l30_buf[0] = l30_buf[1] = l30_buf[2] = l30_buf[3] = 0;
		l40_buf[0] = l40_buf[1] = l40_buf[2] = l40_buf[3] = 0;

		ProcessRad2<ROWS, COLS, IN_DEPTH, OUT_DEPTH, NPC, IN_WW, OUT_WW, TC>( _src_mat, _dst_mat, buf, l00_buf, l10_buf, l20_buf, l30_buf, l40_buf, Array,
								inter_valx, img_width, row_ind, shift, tp1, tp2, mid, bottom1, bottom2, false);

		if(NPC == XF_NPPC8 || NPC == XF_NPPC16){
			Array[0] = xFFindMaxRad2(l20_buf[2], l00_buf[2], l10_buf[1], l10_buf[2],
					l10_buf[3], l20_buf[0], l20_buf[1], l20_buf[3],
					l20_buf[4], l30_buf[1], l30_buf[2], l30_buf[3],
					l40_buf[2]);

			Array[1] = xFFindMaxRad2(l20_buf[3], l00_buf[3], l10_buf[2], l10_buf[2],
					l10_buf[4], l20_buf[1], l20_buf[1], l20_buf[4],
					l20_buf[5], l30_buf[2], l30_buf[3], l30_buf[4],
					l40_buf[3]);

			xfPackPixels<NPC, OUT_WW, OUT_DEPTH>(&Array[0], inter_valx, 0, 2, shift);
			_dst_mat.write(inter_valx);
			shift = 0;
			inter_valx = 0;
		}
		else if(NPC == XF_NPPC1){
			l00_buf[4] = 0;
			l10_buf[4] = 0;
			l20_buf[4] = 0;
			l30_buf[4] = 0;
			l40_buf[4] = 0;

			Array[0] = xFFindMaxRad2(l20_buf[2], l00_buf[2], l10_buf[1],
					l10_buf[2], l10_buf[3], l20_buf[0],
					l20_buf[1], l20_buf[3], l20_buf[4],
					l30_buf[1], l30_buf[2], l30_buf[3],
					l40_buf[2]);
			ap_uint<8> step = XF_PIXELDEPTH(OUT_DEPTH);
			inter_valx(((step << XF_BITSHIFT(NPC))-1), ((step << XF_BITSHIFT(NPC))-step)) = Array[0];
			_dst_mat.write(inter_valx);

			lbufLoop33:
			for(i = 0; i < 4; i++)
			{
#pragma HLS unroll
				l00_buf[i] = l00_buf[nms_bufsize-(4 - i)];
				l10_buf[i] = l10_buf[nms_bufsize-(4 - i)];
				l20_buf[i] = l20_buf[nms_bufsize-(4 - i)];
				l30_buf[i] = l30_buf[nms_bufsize-(4 - i)];
				l40_buf[i] = l40_buf[nms_bufsize-(4 - i)];
			}

			l00_buf[3] = 0;
			l10_buf[3] = 0;
			l20_buf[3] = 0;
			l30_buf[3] = 0;
			l40_buf[3] = 0;

			Array[0] = xFFindMaxRad2(l20_buf[2], l00_buf[2], l10_buf[1],
					l10_buf[2], l10_buf[3], l20_buf[0],
					l20_buf[1], l20_buf[3], l20_buf[4],
					l30_buf[1], l30_buf[2], l30_buf[3],
					l40_buf[2]);

			inter_valx(((step << XF_BITSHIFT(NPC))-1), ((step << XF_BITSHIFT(NPC))-step)) = Array[0];
			_dst_mat.write(inter_valx);
		}
	}
}


/**
 * xFFindMaxRad3: compare center pixels with the surrounding pixels
 *
 */
template<typename SRC_T>
bool xFFindMaxRad3(SRC_T* l00_buf, SRC_T* l10_buf, SRC_T* l20_buf,
		SRC_T* l30_buf, SRC_T* l40_buf, SRC_T* l50_buf,
		SRC_T* l60_buf)
{
	bool MaxVal = false;
	int max = l30_buf[3];

	if(max > l00_buf[3] && max > l10_buf[1] && max > l10_buf[2] &&
			max > l10_buf[3] && max > l10_buf[4] && max > l10_buf[5] &&
			max > l20_buf[1] && max > l20_buf[2] && max > l20_buf[3] &&
			max > l20_buf[4] && max > l20_buf[5] && max > l30_buf[0] &&
			max > l30_buf[1] && max > l30_buf[2] && max > l30_buf[4] &&
			max > l30_buf[5] && max > l30_buf[6] && max > l40_buf[1] &&
			max > l40_buf[2] && max > l40_buf[3] && max > l40_buf[4] &&
			max > l40_buf[5] && max > l50_buf[1] && max > l50_buf[2] &&
			max > l50_buf[3] && max > l50_buf[4] && max > l50_buf[5] &&
			max > l60_buf[3])
	{
		MaxVal = true;
	}
	return MaxVal;
}
// xFFindMaxRad3

/**
 * Max suppression
 */
template<int NPC, typename SRC_T, typename DST_T>
void xFSuppressionRad3(SRC_T *Maxarray, DST_T* l00_buf, DST_T* l10_buf,
		DST_T* l20_buf, DST_T* l30_buf, DST_T* l40_buf,
		DST_T* l50_buf, DST_T* l60_buf)
{
	for(ap_uint<8> i = 0; i < (1 << XF_BITSHIFT(NPC)); i++)
	{
#pragma HLS UNROLL
		bool Max = xFFindMaxRad3(&l00_buf[i], &l10_buf[i], &l20_buf[i],
				&l30_buf[i], &l40_buf[i], &l50_buf[i],
				&l60_buf[i]);

		Maxarray[i] = Max ? 255 : 0;
	}
}
// xFSuppressionRad3

/**
 * xFMaxSuppressionRad3 : performs the non maximum suppression for Radius size 3
 * _src_mat	: Input image
 * _dst_mat	: Output image
 */
template<int ROWS, int COLS, int IN_DEPTH, int OUT_DEPTH, int NPC, int IN_WW, int OUT_WW, int TC>
void xFMaxSuppressionRad3(hls::stream< XF_SNAME(IN_WW) > &_src_mat,
						  hls::stream< XF_SNAME(OUT_WW) > &_dst_mat, uint16_t img_height, uint16_t img_width)
{
	ap_uint<13> row, col, row_ind;
	ap_uint<8> tp1,tp2, tp3, mid, bottom1, bottom2, bottom3;
	ap_uint<8> nms_bufsize = (1 << XF_BITSHIFT(NPC))+6;

	XF_PTNAME(OUT_DEPTH) Array[(1 << XF_BITSHIFT(NPC))];
#pragma HLS ARRAY_PARTITION variable=Array complete dim=1

	XF_SNAME(IN_WW) buf0, buf1, buf2, buf3, buf4, buf5, buf6;

	// Temporary buffers to hold image data from three rows.
	XF_PTNAME(IN_DEPTH) l00_buf[(1 << XF_BITSHIFT(NPC))+6], l10_buf[(1 << XF_BITSHIFT(NPC))+6], l20_buf[(1 << XF_BITSHIFT(NPC))+6],
			l30_buf[(1 << XF_BITSHIFT(NPC))+6], l40_buf[(1 << XF_BITSHIFT(NPC))+6];
	XF_PTNAME(IN_DEPTH) l50_buf[(1 << XF_BITSHIFT(NPC))+6], l60_buf[(1 << XF_BITSHIFT(NPC))+6];
#pragma HLS ARRAY_PARTITION variable=l00_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l10_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l20_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l30_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l40_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l50_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l60_buf complete dim=1

	ap_uint<10> i = 0, shift;
	ap_uint<8> k;
	XF_SNAME(IN_WW) tmp_in;
	XF_SNAME(OUT_WW) inter_valx = 0;
	XF_SNAME(IN_WW) buf[7][(COLS >> XF_BITSHIFT(NPC))];
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1

	row_ind = 3;

	Clear_Row_Loop:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		buf[0][col] = 0;
		buf[1][col] = 0;
		buf[2][col] = 0;
	}

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

	Read_Row3_Loop:
	for(col = 0; col < img_width; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
		buf[row_ind][col] = _src_mat.read();
	}
	row_ind++;

	Row_Loop:
	for(row = 3; row < img_height; row++)
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
			l00_buf[i] = 0;
			l10_buf[i] = 0;
			l20_buf[i] = 0;
			l30_buf[i] = 0;
			l40_buf[i] = 0;
			l50_buf[i] = 0;
			l60_buf[i] = 0;
		}
		inter_valx = 0;

		Col_Loop:
		for(col = 0; col < img_width; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline

			buf[row_ind][col] = _src_mat.read();

			buf0 = buf[tp1][col];
			buf1 = buf[tp2][col];
			buf2 = buf[tp3][col];
			buf3 = buf[mid][col];
			buf4 = buf[bottom1][col];
			buf5 = buf[bottom2][col];
			buf6 = buf[bottom3][col];

			xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l00_buf, buf0, 6);
			xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l10_buf, buf1, 6);
			xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l20_buf, buf2, 6);
			xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l30_buf, buf3, 6);
			xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l40_buf, buf4, 6);
			xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l50_buf, buf5, 6);
			xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l60_buf, buf6, 6);

			xFSuppressionRad3<NPC>(Array, l00_buf, l10_buf, l20_buf, l30_buf,
					l40_buf, l50_buf, l60_buf);

			for(i = 0; i < 6; i++)
			{
#pragma HLS unroll
				l00_buf[i] = l00_buf[nms_bufsize-(6 - i)];
				l10_buf[i] = l10_buf[nms_bufsize-(6 - i)];
				l20_buf[i] = l20_buf[nms_bufsize-(6 - i)];
				l30_buf[i] = l30_buf[nms_bufsize-(6 - i)];
				l40_buf[i] = l40_buf[nms_bufsize-(6 - i)];
				l50_buf[i] = l50_buf[nms_bufsize-(6 - i)];
				l60_buf[i] = l60_buf[nms_bufsize-(6 - i)];
			}
			if(col == 0)
			{
				for(k = 3; k < (1 << XF_BITSHIFT(NPC)); k++)
				{
#pragma HLS unroll
					shift = (k-3) << 3;
					inter_valx(shift+7, shift) = Array[k];
				}
			}
			else
			{
				for(k = 0; k < 3; k++)
				{
#pragma HLS unroll
					shift = ((1 << XF_BITSHIFT(NPC))-3 + k) << 3;
					inter_valx(shift+7, shift) = Array[k];
				}
				_dst_mat.write(inter_valx);
				for(k = 3; k < (1 << XF_BITSHIFT(NPC)); k++)
				{
#pragma HLS unroll
					shift = (k-3) << 3;
					inter_valx(shift+7, shift) = Array[k];
				}
			}

		}// Col_Loop

		if(row >= 3)
		{
			for(i = 0; i < 8; i++)
			{
#pragma HLS unroll
				l00_buf[i] = l00_buf[nms_bufsize-(6 + i)];
				l10_buf[i] = l10_buf[nms_bufsize-(6 + i)];
				l20_buf[i] = l20_buf[nms_bufsize-(6 + i)];
				l30_buf[i] = l30_buf[nms_bufsize-(6 + i)];
				l40_buf[i] = l40_buf[nms_bufsize-(6 + i)];
				l50_buf[i] = l50_buf[nms_bufsize-(6 + i)];
				l60_buf[i] = l60_buf[nms_bufsize-(6 + i)];
			}

			Array[0] = xFFindMaxRad3(&l00_buf[0], &l10_buf[0], &l20_buf[0],
					&l30_buf[0], &l40_buf[0], &l50_buf[0], &l60_buf[0]);

			Array[1] = xFFindMaxRad3(&l00_buf[1], &l10_buf[1], &l20_buf[1],
					&l30_buf[1], &l40_buf[1], &l50_buf[1], &l60_buf[1]);

			Array[2] = xFFindMaxRad3(&l00_buf[2], &l10_buf[2], &l20_buf[2],
					&l30_buf[2], &l40_buf[2], &l50_buf[2], &l60_buf[2]);

			for(k = 0; k < 3; k++)
			{
#pragma HLS unroll
				shift = ((1 << XF_BITSHIFT(NPC)) - 3 + k) << 3;
				inter_valx(shift+7,shift) = Array[k];
			}
			_dst_mat.write(inter_valx);
		}
		row_ind++;
		if(row_ind == 7)
		{
			row_ind = 0;
		}
	}// Row_Loop

	Border_Row_Loop:
	for(row = 0; row < 3; row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=3 max=3

		for(i = 0; i < 6; i++)
		{
#pragma HLS LOOP_TRIPCOUNT min=6 max=6
#pragma HLS unroll
			l00_buf[i] = 0;
			l10_buf[i] = 0;
			l20_buf[i] = 0;
			l30_buf[i] = 0;
			l40_buf[i] = 0;
			l50_buf[i] = 0;
			l60_buf[i] = 0;
		}
		for(col = 0; col < img_width; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
			if(row == 0)
			{
				buf0 = buf[tp2][col];
				buf1 = buf[tp3][col];
				buf2 = buf[mid][col];
				buf3 = buf[bottom1][col];
				buf4 = buf[bottom2][col];
				buf5 = buf[bottom3][col];
				buf6 = 0;

				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l00_buf, buf0, 6);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l10_buf, buf1, 6);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l20_buf, buf2, 6);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l30_buf, buf3, 6);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l40_buf, buf4, 6);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l50_buf, buf5, 6);

			}
			else if(row == 1)
			{

				buf0 = buf[tp3][col];
				buf1 = buf[mid][col];
				buf2 = buf[bottom1][col];
				buf3 = buf[bottom2][col];
				buf4 = buf[bottom3][col];
				buf5 = 0;
				buf6 = 0;

				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l00_buf, buf0, 4);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l10_buf, buf1, 4);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l20_buf, buf2, 4);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l30_buf, buf3, 6);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l40_buf, buf4, 6);

			}
			else if(row == 2)
			{
				buf0 = buf[mid][col];
				buf1 = buf[bottom1][col];
				buf2 = buf[bottom2][col];
				buf3 = buf[bottom3][col];
				buf4 = 0;
				buf5 = 0;
				buf6 = 0;

				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l00_buf, buf0, 4);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l10_buf, buf1, 4);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l20_buf, buf2, 4);
				xfExtractPixels<NPC, IN_WW, IN_DEPTH>(l30_buf, buf3, 6);
			}

			xFSuppressionRad3<NPC>(Array, l00_buf, l10_buf, l20_buf, l30_buf,
					l40_buf, l50_buf, l60_buf);

			for(i = 0; i < 6; i++)
			{
#pragma HLS unroll
				l00_buf[i] = l00_buf[nms_bufsize-(6 - i)];
				l10_buf[i] = l10_buf[nms_bufsize-(6 - i)];
				l20_buf[i] = l20_buf[nms_bufsize-(6 - i)];
				l30_buf[i] = l30_buf[nms_bufsize-(6 - i)];
				l40_buf[i] = l40_buf[nms_bufsize-(6 - i)];
				l50_buf[i] = l50_buf[nms_bufsize-(6 - i)];
				l60_buf[i] = l60_buf[nms_bufsize-(6 - i)];
			}
			if(col == 0)
			{
				for(k = 3; k < (1 << XF_BITSHIFT(NPC)); k++)
				{
#pragma HLS unroll
					shift = (k-3) << 3;
					inter_valx(shift+7,shift) = Array[k];
				}
			}
			else
			{
				for(k = 0; k < 3; k++)
				{
#pragma HLS unroll
					shift = ((1 << XF_BITSHIFT(NPC))-3 + k) << 3;
					inter_valx(shift+7, shift) = Array[k];
				}
				_dst_mat.write(inter_valx);
				for(k = 3; k < (1 << XF_BITSHIFT(NPC)); k++)
				{
#pragma HLS unroll
					shift = (k-3) << 3;
					inter_valx(shift+7, shift) = Array[k];
				}
			}
		} // Border_Col_Loop

		Array[0] = xFFindMaxRad3(&l00_buf[0], &l10_buf[0], &l20_buf[0],
				&l30_buf[0], &l40_buf[0], &l50_buf[0], &l60_buf[0]);

		Array[1] = xFFindMaxRad3(&l00_buf[1], &l10_buf[1], &l20_buf[1],
				&l30_buf[1], &l40_buf[1], &l50_buf[1], &l60_buf[1]);

		Array[2] = xFFindMaxRad3(&l00_buf[2], &l10_buf[2], &l20_buf[2],
				&l30_buf[2], &l40_buf[2], &l50_buf[2], &l60_buf[2]);

		for(k = 0; k < 3; k++)
		{
#pragma HLS unroll
			shift = ((1 << XF_BITSHIFT(NPC))-3+k) << 3;
			inter_valx(shift+7,shift) = Array[k];
		}
		_dst_mat.write(inter_valx);
	} // Border_Row_Loop
}

/*********************************************************************
 * xFMaxSuppression : Calls the Main Function depend on Requirements
 *********************************************************************/
template<int ROWS, int COLS, int IN_DEPTH, int OUT_DEPTH, int NPC, int IN_WW, int OUT_WW>
void xFMaxSuppression(hls::stream< XF_SNAME(IN_WW) > &_src_mat,
					  hls::stream< XF_SNAME(OUT_WW) > &_dst_mat,
					  uint8_t _nms_radius, uint16_t img_height, uint16_t img_width)
{
	img_width = img_width >> XF_BITSHIFT(NPC);

	assert(((_nms_radius == XF_NMS_RADIUS_1) || (_nms_radius == XF_NMS_RADIUS_2) ||
			(_nms_radius == XF_NMS_RADIUS_3)) && "radius size must be 1, 2 or 3");

	assert(((img_height <= ROWS ) && (img_width <= COLS)) && "ROWS and COLS should be greater than input image");
	
	if(_nms_radius == XF_NMS_RADIUS_1)
	{
		xFMaxSuppressionRad1<ROWS, COLS, IN_DEPTH, OUT_DEPTH, NPC,
		IN_WW, OUT_WW, (COLS >> XF_BITSHIFT(NPC))>(_src_mat, _dst_mat, img_height, img_width);
	}
	else if(_nms_radius == XF_NMS_RADIUS_2)
	{
		xFMaxSuppressionRad2<ROWS, COLS, IN_DEPTH, OUT_DEPTH, NPC,
		IN_WW, OUT_WW, (COLS >> XF_BITSHIFT(NPC))>(_src_mat, _dst_mat, img_height, img_width);
	}
	else
	{
		xFMaxSuppressionRad3<ROWS, COLS, IN_DEPTH, OUT_DEPTH, NPC,
		IN_WW, OUT_WW, (COLS >> XF_BITSHIFT(NPC))>(_src_mat, _dst_mat, img_height, img_width);
	}

}

#endif	// _XF_MAX_SUPPRESSION_HPP_
