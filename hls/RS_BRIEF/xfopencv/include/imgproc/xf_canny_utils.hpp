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

#ifndef _XF_CANNY_UTILS_HPP_
#define _XF_CANNY_UTILS_HPP_

#ifndef __cplusplus
#error C++ is needed to include this header
#endif

#define EDGELINK		1


template<int DEPTH_IN,int DEPTH_PHASE,int DEPTH_OUT>
XF_PTNAME(DEPTH_OUT) xFFindmax3x3(
		XF_PTNAME(DEPTH_IN) _i00, XF_PTNAME(DEPTH_IN) _i01,
		XF_PTNAME(DEPTH_IN) _i02, XF_PTNAME(DEPTH_IN) _i10, XF_PTNAME(DEPTH_IN) _i11,
		XF_PTNAME(DEPTH_IN) _i12, XF_PTNAME(DEPTH_IN) _i20, XF_PTNAME(DEPTH_IN) _i21,
		XF_PTNAME(DEPTH_IN) _i22, XF_PTNAME(DEPTH_PHASE) angle,
		uchar_t _low_threshold, uchar_t _high_threshold
)
{

	uchar_t NMS = 0;
	int cmp1, cmp2;
	if (_i11 > _low_threshold)
	{
		if(angle == 0){
			cmp1 = _i10; cmp2 = _i12;
			if ((_i11 > cmp1) && (_i11 >= cmp2)){
				if (_i11 > _high_threshold)
				{
					if (EDGELINK)
						NMS = 3;
					else
						NMS = 0xff;
				}
				else
				{
					if (EDGELINK)
						NMS = 1;
					else
						NMS = 0x80;

				}
			}
			else
			{
				NMS = 0;
			}
		}
		if(angle == 45){
			cmp1 = _i02; cmp2 = _i20;
			if ((_i11 > cmp1) && (_i11 > cmp2)){
				if (_i11 > _high_threshold)
				{
					if (EDGELINK)
						NMS = 3;
					else
						NMS = 0xff;

				}
				else
				{
					if (EDGELINK)
						NMS = 1;
					else
						NMS = 0x80;

				}
			}
			else
			{
				NMS = 0;
			}

		}
		if(angle == 90)
		{
			cmp1 = _i01; cmp2 = _i21;
			if ((_i11 > cmp1) && (_i11 >= cmp2)){
				if (_i11 > _high_threshold)
				{
					if (EDGELINK)
						NMS = 3;
					else
						NMS = 0xff;

				}
				else
				{
					if (EDGELINK)
						NMS = 1;
					else
						NMS = 0x80;

				}
			}
			else
			{
				NMS = 0;
			}

		}
		if(angle == 135){
			cmp1 = _i00; cmp2 = _i22;
			if ((_i11 > cmp1) && (_i11 > cmp2)){
				if (_i11 > _high_threshold)
				{
					if (EDGELINK)
						NMS = 3;
					else
						NMS = 0xff;

				}
				else
				{
					if (EDGELINK)
						NMS = 1;
					else
						NMS = 0x80;
				}
			}
			else
			{
				NMS = 0;
			}

		}
	}
	else
	{
		NMS = 0;
	}
	return NMS;
}


/**
 * xFComputeMaskValue3x3 function:
 * If PO is enabled then 16 mask_value will be computed, by unrolling the filter_loop.
 * If RO is enabled then 8 mask_value  will be computed, by unrolling the filter_loop.
 */
template<int NPC, int DEPTH_IN,int DEPTH_PHASE, int DEPTH_OUT >
void xFNMS3x3(XF_PTNAME(DEPTH_OUT)* _mask_value,
		XF_PTNAME(DEPTH_IN)* _l00_buf, XF_PTNAME(DEPTH_IN)* _l10_buf,
		XF_PTNAME(DEPTH_IN)* _l20_buf, XF_PTNAME(DEPTH_PHASE)* _angle_buf,
		uchar_t _low_threshold, uchar_t _high_threshold)
{
#pragma HLS INLINE
	ap_uint<5> filter_loop = XF_NPIXPERCYCLE(NPC);

	ComputeMaxLoop:
	for(ap_uint<6> j = 0; j < filter_loop; j++ )
	{
#pragma HLS unroll
		_mask_value[j] = xFFindmax3x3<DEPTH_IN,DEPTH_PHASE,DEPTH_OUT>( _l00_buf[j], _l00_buf[j+1],
				_l00_buf[j+2], _l10_buf[j], _l10_buf[j+1], _l10_buf[j+2],
				_l20_buf[j], _l20_buf[j+1], _l20_buf[j+2], _angle_buf[j+1], _low_threshold, _high_threshold);
	} // end of computeMaskValueLoop
}

template<int ROWS, int COLS,int DEPTH_IN,int DEPTH_PHASE,int DEPTH_OUT,int NPC,int WORDWIDTH_SRC,int WORDWIDTH_PHASE,
int WORDWIDTH_DST, int COLS_COUNT>
void ProcessNms3x3canny(hls::stream< XF_SNAME(WORDWIDTH_SRC) > & _magnitude_mat,
		hls::stream< XF_SNAME(WORDWIDTH_PHASE) > & _phase_mat,
		hls::stream< XF_SNAME(WORDWIDTH_DST) > & _dst_mat,
		XF_SNAME(WORDWIDTH_SRC) buf[3][(COLS >> XF_BITSHIFT(NPC))],
		XF_SNAME(WORDWIDTH_PHASE) angle[2][COLS>>XF_BITSHIFT(NPC)],
		XF_PTNAME(DEPTH_IN) l00_buf[XF_NPIXPERCYCLE(NPC)+2],
		XF_PTNAME(DEPTH_IN) l10_buf[XF_NPIXPERCYCLE(NPC)+2],
		XF_PTNAME(DEPTH_IN) l20_buf[XF_NPIXPERCYCLE(NPC)+2],
		XF_PTNAME(DEPTH_PHASE) angle_buf[XF_NPIXPERCYCLE(NPC)+2],
		XF_PTNAME(DEPTH_OUT) nmsvalue[XF_NPIXPERCYCLE(NPC)],
		XF_SNAME(WORDWIDTH_DST) &P0,
		uint16_t imgwidth,
		uint16_t imgheight,
		ap_uint<2> top, ap_uint<2> mid,
		ap_uint<2> bottom,ap_uint<13> row,
		ap_uint<13> row_ind,ap_uint<13> read_ind,
		ap_uint<13> write_ind,
		uchar_t low_threshold,
		uchar_t high_threshold)
{
#pragma HLS INLINE
	uint16_t step_out = XF_PIXELDEPTH(DEPTH_OUT),
	max_loop_out = XF_WORDDEPTH(WORDWIDTH_DST);
	uint16_t buf_size = XF_NPIXPERCYCLE(NPC) + 2;
	XF_SNAME(WORDWIDTH_SRC) buf0, buf1, buf2;
	colLoop1:
	for(ap_uint<13> col = 0; col < (imgwidth); col++)  // Width of the image
	{
#pragma HLS LOOP_TRIPCOUNT min=COLS_COUNT max=COLS_COUNT
#pragma HLS pipeline
		if(row < imgheight)
		{
			buf[row_ind][col] = _magnitude_mat.read();
			angle[read_ind][col] = _phase_mat.read();
		}
		else {
			buf[row_ind][col] = 0;
			angle[read_ind][col] = 0;
		}

		//ang = angle[angle_ind][col];
		buf0 = buf[top][col];
		buf1 = buf[mid][col];
		buf2 = buf[bottom][col];

		/*		     Extract the data from the packed pixels       */
		xfExtractPixels<NPC,WORDWIDTH_SRC,DEPTH_IN>(&l00_buf[2],buf0,0);
		xfExtractPixels<NPC,WORDWIDTH_SRC,DEPTH_IN>(&l10_buf[2],buf1,0);
		xfExtractPixels<NPC,WORDWIDTH_SRC,DEPTH_IN>(&l20_buf[2],buf2,0);

		xfExtractPixels<NPC,WORDWIDTH_PHASE,DEPTH_PHASE>(&angle_buf[2],angle[write_ind][col],0);

		xFNMS3x3<NPC,DEPTH_IN,DEPTH_PHASE,DEPTH_OUT>(nmsvalue, l00_buf,
				l10_buf, l20_buf,angle_buf,low_threshold,high_threshold);

		if(col == 0)
		{
			ap_uint<6> j = 1;

			P0loop1:
			for(ap_uint<6> i = 0; i < (max_loop_out-step_out); i += step_out)
			{
#pragma HLS LOOP_TRIPCOUNT min=NPC max=NPC
#pragma HLS unroll
				P0.range(i + (step_out-1), i) = nmsvalue[j++];
			}
		}
		else
		{
			P0.range((max_loop_out-1),( max_loop_out-step_out)) = nmsvalue[0];
			_dst_mat.write(P0);
			ap_uint<6> j = 1;

			P0loop2:
			for(ap_uint<6> i = 0; i < (max_loop_out-step_out); i += step_out)
			{
#pragma HLS LOOP_TRIPCOUNT min=NPC max=NPC
#pragma HLS unroll
				P0.range(i + (step_out-1), i) = nmsvalue[j++];
			}
		}
		l00_buf[0] = l00_buf[buf_size-2];
		l00_buf[1] = l00_buf[buf_size-1];

		l10_buf[0] = l10_buf[buf_size-2];
		l10_buf[1] = l10_buf[buf_size-1];

		l20_buf[0] = l20_buf[buf_size-2];
		l20_buf[1] = l20_buf[buf_size-1];

		angle_buf[0] = angle_buf[buf_size-2];
		angle_buf[1] = angle_buf[buf_size-1];

	}  // end of colLoop1
}
/**
 * xFSuppression : Compute a Box filter of size 3x3 over a window of the input image.
 * Inputs : _src_mat --> input image of type XF_8U, XF_16U or XF_16S
 * Output : _dst_mat --> output image of input type
 */


template<int ROWS, int COLS, int DEPTH_IN,int DEPTH_PHASE, int DEPTH_OUT, int NPC,
int WORDWIDTH_SRC, int WORDWIDTH_PHASE,int WORDWIDTH_DST, int COLS_COUNT>
void xFSuppression3x3(hls::stream<XF_SNAME(WORDWIDTH_SRC)>& _magnitude_mat,
		hls::stream<XF_SNAME(WORDWIDTH_PHASE)>& _phase_mat,
		hls::stream<XF_SNAME(WORDWIDTH_DST)>& _dst_mat, uchar_t low_threshold,
		uchar_t high_threshold, uint16_t imgheight, uint16_t imgwidth) {

	imgwidth = imgwidth >> XF_BITSHIFT(NPC);
	ap_uint<13> row_ind = 1;
	ap_uint<2> top, mid, bottom;
	ap_uint<13> read_ind = 1, write_ind = 0;
	uint16_t step_out = XF_PIXELDEPTH(DEPTH_OUT), max_loop_out = XF_WORDDEPTH(
			WORDWIDTH_DST);
	uint16_t buf_size = XF_NPIXPERCYCLE(NPC) + 2;

	XF_PTNAME (DEPTH_OUT)
	nmsvalue[XF_NPIXPERCYCLE(NPC)];
#pragma HLS ARRAY_PARTITION variable=nmsvalue complete dim=1

	XF_PTNAME (DEPTH_IN)
	l00_buf[XF_NPIXPERCYCLE(NPC) + 2], l10_buf[XF_NPIXPERCYCLE(NPC) + 2], l20_buf[XF_NPIXPERCYCLE(
			NPC) + 2];	// Temporary buffers to hold image data from three rows.
#pragma HLS ARRAY_PARTITION variable=l00_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l10_buf complete dim=1
#pragma HLS ARRAY_PARTITION variable=l20_buf complete dim=1

	XF_SNAME(WORDWIDTH_DST) P0;
	XF_SNAME(WORDWIDTH_PHASE)ang, angle[2][COLS >> XF_BITSHIFT(NPC)];

#pragma HLS ARRAY_PARTITION variable=angle complete dim=1

	XF_SNAME(WORDWIDTH_SRC) buf[3][COLS >> XF_BITSHIFT(NPC)];
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1
	XF_PTNAME (DEPTH_PHASE) angle_buf[XF_NPIXPERCYCLE(NPC) + 2];
#pragma HLS ARRAY_PARTITION variable=angle_buf complete dim=1

	bufColLoop: for (ap_uint<13> col = 0; col < (imgwidth); col++) {
#pragma HLS LOOP_TRIPCOUNT min=COLS_COUNT max=COLS_COUNT
#pragma HLS pipeline
		buf[0][col] = 0;
		buf[row_ind][col] = _magnitude_mat.read();
		angle[0][col] = _phase_mat.read();
	}
	row_ind = 2;

	l00_buf[0] = l00_buf[1] = 0;
	l10_buf[0] = l10_buf[1] = 0;
	l20_buf[0] = l20_buf[1] = 0;
	angle_buf[0] = angle_buf[1] = 0;

	rowLoop: for (ap_uint<13> row = 1; row < (imgheight + 1); row++) // Height of the image
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS

		if (row_ind == 2) {
			top = 0;
			mid = 1;
			bottom = 2;
		} else if (row_ind == 0) {
			top = 1;
			mid = 2;
			bottom = 0;
		} else if (row_ind == 1) {
			top = 2;
			mid = 0;
			bottom = 1;
		}

		l00_buf[0] = l00_buf[1] = 0;
		l10_buf[0] = l10_buf[1] = 0;
		l20_buf[0] = l20_buf[1] = 0;
		angle_buf[0] = angle_buf[1] = 0;
		P0 = 0;

		ProcessNms3x3canny<ROWS, COLS, DEPTH_IN,DEPTH_PHASE, DEPTH_OUT, NPC, WORDWIDTH_SRC,WORDWIDTH_PHASE,
		WORDWIDTH_DST, COLS_COUNT>(_magnitude_mat, _phase_mat, _dst_mat,
				buf, angle, l00_buf, l10_buf, l20_buf, angle_buf, nmsvalue, P0,
				imgwidth, imgheight, top, mid, bottom, row, row_ind, read_ind,
				write_ind, low_threshold, high_threshold);

		XF_PTNAME (DEPTH_IN) val = 0;

		if (NPC == XF_NPPC1) {
			nmsvalue[0] = xFFindmax3x3<DEPTH_IN,DEPTH_PHASE, DEPTH_OUT>(
					l00_buf[buf_size - 3],		// Applying Mask
					l00_buf[buf_size - 2], val, l10_buf[buf_size - 3],
					l10_buf[buf_size - 2], val, l20_buf[buf_size - 3],
					l20_buf[buf_size - 2], val, angle_buf[buf_size - 2],
					low_threshold, high_threshold);	//	Take care of border at last column
		} else {
			nmsvalue[0] = xFFindmax3x3<DEPTH_IN,DEPTH_PHASE, DEPTH_OUT>(
					l00_buf[buf_size - 2], l00_buf[buf_size - 1], val,
					l10_buf[buf_size - 2], l10_buf[buf_size - 1], val,
					l20_buf[buf_size - 2], l20_buf[buf_size - 1], val,
					angle_buf[buf_size - 1], low_threshold, high_threshold);
		}
		P0.range((max_loop_out - 1), (max_loop_out - step_out)) = nmsvalue[0];
		_dst_mat.write(P0);

		row_ind++;
		read_ind++;
		write_ind++;
		if (row_ind == 3) {
			row_ind = 0;
		}
		if (read_ind == 2) {
			read_ind = 0;
		}

		if (write_ind == 2) {
			write_ind = 0;
		}
	}  // end of rowLoop
	// write data
}  // end of function

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<int ROWS, int COLS, int DEPTH_SRC, int DEPTH_DST, int NPC,
int WORDWIDTH_SRC, int WORDWIDTH_DST, int COLS_TRIP>
void xFAngleKernel(hls::stream<XF_SNAME(WORDWIDTH_SRC)>& _src1,
		hls::stream<XF_SNAME(WORDWIDTH_SRC)>& _src2,
		hls::stream<XF_SNAME(WORDWIDTH_DST)>& _dst, uint16_t imgheight,
		uint16_t imgwidth) {

	XF_SNAME(WORDWIDTH_SRC) val_src1, val_src2;
	XF_SNAME(WORDWIDTH_DST) val_dst;

	int16_t p, q;
	int16_t result;
	rowLoop: for (ap_uint<13> i = 0; i < (imgheight); i++) {
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS
#pragma HLS LOOP_FLATTEN off

		colLoop: for (ap_uint<13> j = 0; j < (imgwidth); j++) {
#pragma HLS LOOP_TRIPCOUNT min=COLS_TRIP max=COLS_TRIP
#pragma HLS pipeline

			val_src1 = (XF_SNAME(WORDWIDTH_SRC))(_src1.read());
			val_src2 = (XF_SNAME(WORDWIDTH_SRC))(_src2.read());

			int proc_loop = XF_WORDDEPTH(WORDWIDTH_DST), step = XF_PIXELDEPTH(
					DEPTH_DST);

			procLoop: for (ap_uint<13> k = 0, pix = 0; k < proc_loop;
					k += step, pix += 16) {
#pragma HLS unroll

				p = val_src1.range(pix + (16 - 1), pix); // Get bits from certain range of positions.
				q = val_src2.range(pix + (16 - 1), pix); // Get bits from certain range of positions.

#define CANNY_SHIFT			15

				XF_PTNAME(DEPTH_DST) angle;

				int xa = __ABS(p);
				int ya = __ABS(q) << CANNY_SHIFT;
				int TG22 = (int) (0.4142135623730950488016887242097
						* (1 << CANNY_SHIFT) + 0.5);

				int tg22x;
#pragma HLS RESOURCE variable=tg22x core=MulnS
				tg22x = xa * TG22;

				if (ya < tg22x) {
					angle = 0;
				} else {
					int tg67x = tg22x + (xa << (CANNY_SHIFT + 1));
					if (ya > tg67x) {
						angle = 90;
					} else {
						int sign = (p ^ q) < 0 ? -1 : 1;
						if (sign == 1) {
							angle = 135;
						} else {
							angle = 45;
						}
					}
				}
				val_dst.range(k + (step - 1), k) = angle;
			}
			_dst.write(val_dst);		// writing into the output stream
		}
	}
}

/**
 * xFAngle: This function acts as a wrapper and calls the
 * Kernel function xFAngleKernel.
 */
template<int ROWS, int COLS, int DEPTH_SRC, int DEPTH_DST, int NPC,
int WORDWIDTH_SRC, int WORDWIDTH_DST>
void xFAngle(hls::stream<XF_SNAME(WORDWIDTH_SRC)>& _src1,
		hls::stream<XF_SNAME(WORDWIDTH_SRC)>& _src2,
		hls::stream<XF_SNAME(WORDWIDTH_DST)>& _dst, uint16_t imgheight,
		uint16_t imgwidth) {
#pragma HLS inline
	imgwidth = imgwidth >> XF_BITSHIFT(NPC);
	xFAngleKernel<ROWS, COLS, DEPTH_SRC, DEPTH_DST, NPC, WORDWIDTH_SRC,
	WORDWIDTH_DST, (COLS >> XF_BITSHIFT(NPC))>(_src1, _src2, _dst, imgheight,
			imgwidth);
}

#endif
