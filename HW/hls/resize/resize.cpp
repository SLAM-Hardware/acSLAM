#include "resize.h"

using namespace std;

ap_ufixed<16, 2> scale;
ap_ufixed<16, 2> inv_scale;
ap_uint<32> width;
ap_uint<32> height;
ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> new_width;
ap_ufixed<HEIGHT_BIT + 8, HEIGHT_BIT> new_height;
ap_uint<WIDTH_BIT> unit_num;
const ap_ufixed<16, 2> scale_1 = 1;

void process(hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream, hls::stream<ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outStream);

void process_cfg(hls::stream<ap_axiu<32, 1, 1, 1> > &cfgStream, hls::stream<ap_axiu<32, 1, 1, 1> > &cfgoutStream);

void process_scale_1(hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream, hls::stream<ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outStream);

void process_input(hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream, hls::stream<ap_uint<PROCESS_BIT> > &pixelData);

void process_buf(hls::stream<ap_uint<PROCESS_BIT> > &pixelData, hls::stream<ap_uint<PROCESS_BIT> > &outData);

void process_select(hls::stream<ap_uint<PROCESS_BIT> > &outData, hls::stream<ap_uint<8 + PROCESS_BIT> > &selectData);

void process_output(hls::stream<ap_uint<8 + PROCESS_BIT> > &selectData, hls::stream<ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outStream);

void resize(hls::stream<ap_axiu<32, 1, 1, 1> > &cfgStream, hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream, hls::stream<ap_axiu<32, 1, 1, 1> > &cfgoutStream, hls::stream<ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outStream)
{
#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS INTERFACE axis register both port = cfgStream
#pragma HLS INTERFACE axis register both port = srcStream
#pragma HLS INTERFACE axis register both port = cfgoutStream
#pragma HLS INTERFACE axis register both port = outStream
	
	process_cfg(cfgStream, cfgoutStream);
	if (scale == 1)
		process_scale_1(srcStream, outStream);
	else
		process(srcStream, outStream);
}

void process_cfg(hls::stream<ap_axiu<32, 1, 1, 1> > &cfgStream, hls::stream<ap_axiu<32, 1, 1, 1> > &cfgoutStream)
{
#pragma HLS INLINE off
	width = cfgStream.read().data;
	height = cfgStream.read().data;
	scale.range(15, 0) = cfgStream.read().data.range(15, 0);
	inv_scale.range(15, 0) = cfgStream.read().data.range(15, 0);
	ap_axiu<32, 1, 1, 1> cfgout;
	new_width = width / scale;
	new_width = my_floor<ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT>(new_width);
	new_height = height / scale;
	new_height = my_floor<ap_ufixed<HEIGHT_BIT + 8, HEIGHT_BIT>, HEIGHT_BIT + 8, HEIGHT_BIT>(new_height);
	ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> unit_num_ufixed = width;
	unit_num_ufixed = my_ceil<ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT>(unit_num_ufixed / PROCESS_NUM);
	unit_num = unit_num_ufixed.range(WIDTH_BIT + 7, 8);
	cfgout.data = new_width.range(WIDTH_BIT + 7, 8);
	cfgout.keep = 0xF;
	cfgout.last = 0;
	cfgoutStream.write(cfgout);
	cfgout.data = new_height.range(HEIGHT_BIT + 7, 8);
	cfgout.keep = 0xF;
	cfgout.last = 1;
	cfgoutStream.write(cfgout);
}

void process_scale_1(hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream, hls::stream<ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outStream)
{
	ap_ufixed<PIXEL_NUM_BIT+8, PIXEL_NUM_BIT> read_tc = height*width;
	read_tc = my_ceil<ap_ufixed<PIXEL_NUM_BIT+8, PIXEL_NUM_BIT>, PIXEL_NUM_BIT+8, PIXEL_NUM_BIT>(read_tc / INPUT_PIXEL_NUM);
	ap_uint<PIXEL_NUM_BIT> read_tc_uint = read_tc;
	ap_uint<PIXEL_NUM_BIT> read_ind = 0;
	ap_uint<8> pixel_num = 0;
	ap_uint<8> rmn_num = 0;
	ap_uint<INPUT_BIT> data = 0;
	ap_uint<OUTPUT_BIT> write_tmp = 0;
	ap_uint<PIXEL_NUM_BIT> p_cnt = 0;
	ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> out;
	while (read_ind < read_tc_uint || rmn_num >= OUTPUT_PIXEL_NUM)
	{
#pragma HLS DEPENDENCE variable=rmn_num inter RAW false
#pragma HLS DEPENDENCE variable=read_ind intra RAW false
#pragma HLS PIPELINE
		if (rmn_num >= OUTPUT_PIXEL_NUM)
		// if the remaining number of pixels in this module is greater than OUTPUT_PIXEL_NUM
		{
			rmn_num = rmn_num - OUTPUT_PIXEL_NUM;
			out.data = write_tmp;
			for (ap_uint<8> i = 0; i < OUTPUT_BIT >> 3; i++)
#pragma HLS UNROLL
				out.keep.range(i, i) = 1;
			if (p_cnt == width * height && rmn_num == 0)
				out.last = 1;
			else
				out.last = 0;
			outStream.write(out);
			write_tmp = 0;
			if (rmn_num > 0)
				write_tmp = data.range(pixel_num * PIXEL_BIT - 1, (pixel_num - rmn_num) * PIXEL_BIT);
		}
		else
		{
			data = srcStream.read().data;
			if (read_ind == read_tc_uint - 1)
				pixel_num = height * width - INPUT_PIXEL_NUM * (read_tc_uint - 1);
			else
				pixel_num = INPUT_PIXEL_NUM;
			p_cnt = p_cnt + pixel_num;
			read_ind = read_ind + 1;
			if (rmn_num + pixel_num >= OUTPUT_PIXEL_NUM)
			{
				write_tmp.range(OUTPUT_BIT - 1, rmn_num * PIXEL_BIT) = data.range(OUTPUT_BIT - 1 - rmn_num * PIXEL_BIT, 0);
				rmn_num = rmn_num + pixel_num - OUTPUT_PIXEL_NUM;
				out.data = write_tmp;
				for (ap_uint<8> i = 0; i < OUTPUT_BIT >> 3; i++)
#pragma HLS UNROLL
					out.keep.range(i, i) = 1;
				if (p_cnt == width * height && rmn_num == 0)
					out.last = 1;
				else
					out.last = 0;
				outStream.write(out);
				write_tmp = 0;
				if (rmn_num > 0)
				{
					write_tmp = data.range(pixel_num * PIXEL_BIT - 1, (pixel_num - rmn_num) * PIXEL_BIT);
				}
			}
			else
			{
				if (pixel_num > 0)
				{
					write_tmp.range((rmn_num + pixel_num) * PIXEL_BIT - 1, rmn_num * PIXEL_BIT) = data.range(pixel_num * PIXEL_BIT - 1, 0);
				}
				rmn_num = rmn_num + pixel_num;
			}
		}
	}
	if (rmn_num > 0)
	{
		out.data = write_tmp;
		for (ap_uint<8> i = 0; i < OUTPUT_BIT >> 3; i++)
#pragma HLS UNROLL
			out.keep.range(i, i) = 1;
		out.last = 1;
		outStream.write(out);
	}
}

void process(hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream, hls::stream<ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outStream)
{
#pragma HLS DATAFLOW
	hls::stream<ap_uint<PROCESS_BIT> > pixelData;
#pragma HLS STREAM variable = pixelData depth = 2 dim = 1
	hls::stream<ap_uint<PROCESS_BIT> > outData;
#pragma HLS STREAM variable = outData depth = 2 dim = 1
	hls::stream<ap_uint<8 + PROCESS_BIT> > selectData;
#pragma HLS STREAM variable = selectData depth = 2 dim = 1
	process_input(srcStream, pixelData);
	process_buf(pixelData, outData);
	process_select(outData, selectData);
	process_output(selectData, outStream);
#ifdef DEBUG
	for (int i = 0; i < height-1; i++)
	{
		for (int j = 0; j < 1; j++)
		{
			ap_uint<INPUT_BIT> data = outData.read();
			for (int read_ind = 0; read_ind<PROCESS_NUM; read_ind++)
				cout << data.range((read_ind+1)*PIXEL_BIT-1,read_ind*PIXEL_BIT) << " ";
		}
		cout << endl;
	}
#endif
}

void process_input(hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream, hls::stream<ap_uint<PROCESS_BIT> > &pixelData)
{
#pragma HLS INLINE off
	ap_uint<PIXEL_NUM_BIT> cnt = 0;
	ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> dataIn;

	ap_uint<INPUT_BIT> data = 0;
	ap_uint<INPUT_BIT> prv_data = 0;
	ap_uint<11> rmn_num = 0;
	ap_uint<11> write_num = 0;
	ap_uint<PROCESS_BIT> write_tmp = 0;
	for (ap_uint<HEIGHT_BIT> i = 0; i < height; i++)
	{
		for (ap_uint<WIDTH_BIT> j = 0; j < unit_num; j++)
		{
#pragma HLS PIPELINE
			if (j == unit_num - 1)
				write_num = (width - PROCESS_NUM * (unit_num - 1)) * PIXEL_BIT;
			else
				write_num = PROCESS_NUM * PIXEL_BIT;

			if (rmn_num >= write_num)
			{
				write_tmp = 0;
				write_tmp.range(write_num - 1, 0) = data.range(INPUT_BIT - rmn_num + write_num - 1, INPUT_BIT - rmn_num);
				rmn_num = rmn_num - write_num;
			}
			else
			{
				dataIn = srcStream.read();
				prv_data = data;
				data = dataIn.data;
				if (rmn_num > 0)
				{
					write_tmp = 0;
					write_tmp.range(rmn_num - 1, 0) = prv_data.range(INPUT_BIT - 1, INPUT_BIT - rmn_num);
					write_tmp.range(write_num - 1, rmn_num) = data.range(write_num - rmn_num - 1, 0);
					rmn_num = INPUT_BIT - (write_num - rmn_num);
				}
				else
				{
					write_tmp = 0;
					write_tmp.range(write_num - 1, 0) = data.range(write_num - 1, 0);
					rmn_num = INPUT_BIT - write_num;
				}
			}
			pixelData.write(write_tmp);
		}
	}
}

void process_buf(hls::stream<ap_uint<PROCESS_BIT> > &pixelData, hls::stream<ap_uint<PROCESS_BIT> > &outData)
{
#pragma HLS INLINE off

	ap_uint<PIXEL_BIT * MERGE_NUM> image_buf[WIN_SZ][WIDTH_AFTER_MERGE];
#pragma HLS ARRAY_PARTITION variable = image_buf cyclic factor = 4 dim = 2
//#pragma HLS ARRAY_PARTITION variable = image_buf complete dim = 1
#pragma HLS RESOURCE variable = image_buf core = RAM_2P_BRAM

	ap_uint<PIXEL_BIT> win_buf[WIN_SZ][WIN_SZ + PROCESS_NUM - 1];
#pragma HLS ARRAY_PARTITION variable = win_buf complete dim = 0

	ap_uint<PIXEL_BIT> read_buf[PROCESS_NUM];
#pragma HLS ARRAY_PARTITION variable = read_buf complete dim = 0

	ap_uint<PIXEL_BIT> new_p_buf[PROCESS_NUM];
#pragma HLS ARRAY_PARTITION variable = new_p_buf complete dim = 0

	ap_uint<WIN_SZ_BIT> win_ind[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable = win_ind complete dim = 1

	ap_uint<11> write_num = 0;
	ap_uint<8> write_pixel_num = 0;

	//initialization
	for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ; i++)
#pragma HLS UNROLL
		win_ind[i] = i;

//	for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < WIDTH_AFTER_MERGE; col_ind++)
//#pragma HLS UNROLL factor = 4
//#pragma HLS PIPELINE
//		image_buf[WIN_SZ - 1][col_ind] = 0;
//
//	for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ; i++)
//#pragma HLS UNROLL
//		for (ap_uint<WIDTH_BIT> j = 0; j < WIN_SZ + PROCESS_NUM - 1; j++)
//#pragma HLS UNROLL
//			win_buf[i][j] = 0;

	for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < WIN_SZ - 1; row_ind++)
		for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < unit_num; read_ind++)
		{
#pragma HLS PIPELINE
			ap_uint<PROCESS_BIT> data = pixelData.read();
			for (ap_uint<8> i = 0; i < PROCESS_NUM; i++)
#pragma HLS UNROLL
				read_buf[i] = data.range((i+1)*PIXEL_BIT-1, i*PIXEL_BIT);

			if (read_ind == unit_num - 1)
			{
				write_num = (width - PROCESS_NUM * (unit_num - 1)) * PIXEL_BIT;
				write_pixel_num = width - PROCESS_NUM * (unit_num - 1);
			}
			else
			{
				write_num = PROCESS_NUM * PIXEL_BIT;
				write_pixel_num = PROCESS_NUM;
			}

			for (ap_uint<8> col_ind = 0; col_ind < PROCESS_NUM / MERGE_NUM; col_ind++)
			{
#pragma HLS UNROLL
				ap_uint<PIXEL_BIT * MERGE_NUM> merge_tmp = 0;
				for (ap_uint<LOG_2_MERGE_NUM> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++)
				{
#pragma HLS UNROLL
					if (pixel_ind + col_ind * MERGE_NUM < write_pixel_num)
						merge_tmp.range((pixel_ind+1)*PIXEL_BIT-1, pixel_ind*PIXEL_BIT) = read_buf[pixel_ind + col_ind * MERGE_NUM];
				}
				image_buf[row_ind][col_ind + read_ind * PROCESS_NUM / MERGE_NUM] = merge_tmp;
			}
		}

Row_Loop:
	for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < height - (WIN_SZ - 1); row_ind++)
	{
		for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < unit_num; read_ind++)
		{
#pragma HLS PIPELINE
			ap_uint<PROCESS_BIT> data = pixelData.read();
			for (ap_uint<8> i = 0; i < PROCESS_NUM; i++)
#pragma HLS UNROLL
				read_buf[i] = data.range((i+1)*PIXEL_BIT-1, i*PIXEL_BIT);

			if (read_ind == unit_num - 1)
			{
				write_num = (width - PROCESS_NUM * (unit_num - 1)) * PIXEL_BIT;
				write_pixel_num = width - PROCESS_NUM * (unit_num - 1);
			}
			else
			{
				write_num = PROCESS_NUM * PIXEL_BIT;
				write_pixel_num = PROCESS_NUM;
			}

			for (ap_uint<8> col_ind = 0; col_ind < PROCESS_NUM / MERGE_NUM; col_ind++)
			{
#pragma HLS UNROLL
				ap_uint<PIXEL_BIT * MERGE_NUM> merge_tmp = 0;
				for (ap_uint<LOG_2_MERGE_NUM> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++)
				{
#pragma HLS UNROLL
					if (pixel_ind + col_ind * MERGE_NUM < write_pixel_num)
					{
						merge_tmp.range((pixel_ind+1)*PIXEL_BIT-1, pixel_ind*PIXEL_BIT) = read_buf[pixel_ind + col_ind * MERGE_NUM];
						win_buf[WIN_SZ - 1][col_ind * MERGE_NUM + pixel_ind + WIN_SZ - 1] = read_buf[col_ind * MERGE_NUM + pixel_ind];
					}
				}
				image_buf[win_ind[WIN_SZ - 1]][col_ind + read_ind * PROCESS_NUM / MERGE_NUM] = merge_tmp;
			}

#ifdef DEBUG
			for (int i = 0; i < WIN_SZ; i++)
			{
				for (int j = 0; j < ceil((double)width / MERGE_NUM); j++)
				{
					ap_uint<MERGE_NUM * PIXEL_BIT> cout_tmp =image_buf[i][j];
					for (int k = 0; k < MERGE_NUM; k++)
					{
						cout << cout_tmp.range(7, 0) << " ";
						cout_tmp = cout_tmp >> PIXEL_BIT;
					}
				}
				cout << endl;
			}
			cout << "-----------------image_buf------------------" << endl;
#endif

			for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ - 1; i++)
			{
#pragma HLS UNROLL
				for (ap_uint<8> col_ind = 0; col_ind < PROCESS_NUM / MERGE_NUM; col_ind++)
				{
#pragma HLS UNROLL
					ap_uint<PIXEL_BIT * MERGE_NUM> read_tmp = image_buf[win_ind[i]][col_ind + read_ind * PROCESS_NUM / MERGE_NUM];
					for (ap_uint<LOG_2_MERGE_NUM> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++)
					{
#pragma HLS UNROLL
						win_buf[i][col_ind * MERGE_NUM + pixel_ind + WIN_SZ - 1] = read_tmp.range((pixel_ind+1)*PIXEL_BIT-1, pixel_ind*PIXEL_BIT);
					}
				}
			}

#ifdef DEBUG
			for (int i = 0; i < WIN_SZ; i++)
			{
				for (int j = 0; j < WIN_SZ + PROCESS_NUM - 1; j++)
					cout << win_buf[i][j] << " ";
				cout << endl;
			}
			cout << "-----------------win_buf------------------" << endl;
#endif

			//compute pixel
			for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < PROCESS_NUM; col_ind++)
			{
#pragma HLS UNROLL
				ap_uint<WIDTH_BIT> col_ind_tmp = col_ind + read_ind * PROCESS_NUM;

				ap_ufixed<HEIGHT_BIT + 8, HEIGHT_BIT> row_tmp = row_ind;
				ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> col_tmp = col_ind_tmp - 1;

				row_tmp = my_ceil<ap_ufixed<HEIGHT_BIT + 8, HEIGHT_BIT>, HEIGHT_BIT + 8, HEIGHT_BIT>(row_tmp * inv_scale) * scale;
				col_tmp = my_ceil<ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT>(col_tmp * inv_scale) * scale;
				
				ap_ufixed<PIXEL_BIT + 4, PIXEL_BIT + 2> new_p = win_buf[0][col_ind] * ((row_ind + 1) - row_tmp) * (col_ind_tmp - col_tmp) +
																win_buf[0][col_ind + 1] * ((row_ind + 1) - row_tmp) * (col_tmp - (col_ind_tmp - 1)) +
																win_buf[1][col_ind] * (row_tmp - row_ind) * (col_ind_tmp - col_tmp) +
																win_buf[1][col_ind + 1] * (row_tmp - row_ind) * (col_tmp - (col_ind_tmp - 1));

				if (new_p > MAX_PIXEL_VAL)
					new_p = MAX_PIXEL_VAL;
				ap_uint<PIXEL_BIT> new_p_out = my_round<ap_ufixed<PIXEL_BIT + 4, PIXEL_BIT + 2>, PIXEL_BIT + 4, PIXEL_BIT + 2>(new_p);
				new_p_buf[col_ind] = new_p_out;
			}

#ifdef DEBUG
			for (int i = 0; i < PROCESS_NUM; i++)
			{
				cout << new_p_buf[i] << " ";
			}
			cout << endl
				 << "-----------------new_p_buf------------------" << endl;
#endif

			//ouput
			ap_uint<INPUT_BIT> out_data = 0;
			for (ap_uint<10> i = 0; i < PROCESS_NUM; i++)
#pragma HLS UNROLL
				out_data.range((i + 1) * PIXEL_BIT - 1, i * PIXEL_BIT) = new_p_buf[i];
			outData.write(out_data);

			//move win_buf
			for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ; i++)
#pragma HLS UNROLL
				for (ap_uint<WIDTH_BIT> j = 0; j < WIN_SZ - 1; j++)
#pragma HLS UNROLL
					win_buf[i][j] = win_buf[i][j + PROCESS_NUM];
		}

		ap_uint<WIN_SZ_BIT> win_ind_tmp = win_ind[0];
		for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ - 1; i++)
#pragma HLS UNROLL
			win_ind[i] = win_ind[i + 1];
		win_ind[WIN_SZ - 1] = win_ind_tmp;
	}
}

void process_select(hls::stream<ap_uint<PROCESS_BIT> > &outData, hls::stream<ap_uint<8 + PROCESS_BIT> > &selectData)
{
#pragma HLS INLINE off

	ap_uint<PIXEL_BIT> image_buf[PROCESS_NUM];
#pragma HLS ARRAY_PARTITION variable = image_buf complete dim = 1

	ap_uint<PIXEL_BIT> input_buf[INPUT_PIXEL_NUM];
#pragma HLS ARRAY_PARTITION variable = input_buf complete dim = 1

	for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < PROCESS_NUM; col_ind++)
#pragma HLS UNROLL
		image_buf[col_ind] = 0;

	for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < height - 1; row_ind++)
	{
		ap_ufixed<HEIGHT_BIT + 8, HEIGHT_BIT> row_ind_up = row_ind * inv_scale;
		ap_ufixed<HEIGHT_BIT + 8, HEIGHT_BIT> row_ind_down = (row_ind + 1) * inv_scale;
		row_ind_up = my_ceil<ap_ufixed<HEIGHT_BIT + 8, HEIGHT_BIT>, HEIGHT_BIT + 8, HEIGHT_BIT>(row_ind_up);
		row_ind_down = my_ceil<ap_ufixed<HEIGHT_BIT + 8, HEIGHT_BIT>, HEIGHT_BIT + 8, HEIGHT_BIT>(row_ind_down);

		for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < unit_num; read_ind++)
		{
#pragma HLS PIPELINE
			ap_uint<PROCESS_BIT> data = outData.read();
			for (ap_uint<8> i = 0; i < INPUT_PIXEL_NUM; i++)
#pragma HLS UNROLL
				input_buf[i] = data.range((i + 1) * PIXEL_BIT - 1, i * PIXEL_BIT);
			if (row_ind_up != row_ind_down && row_ind_down <= new_height)
			{
				ap_uint<WIDTH_BIT> min_col_ind_tmp = read_ind * PROCESS_NUM;
				ap_uint<WIDTH_BIT> max_col_ind_tmp = PROCESS_NUM - 1 + read_ind * PROCESS_NUM;
				ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> min_col_ind = (min_col_ind_tmp - 1) * inv_scale;
				ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> max_col_ind = max_col_ind_tmp * inv_scale;
				min_col_ind = my_ceil<ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT>(min_col_ind);
				max_col_ind = my_ceil<ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT>(max_col_ind);
				if (max_col_ind > new_width)
					max_col_ind = new_width;
				for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < PROCESS_NUM; col_ind++)
				{
#pragma HLS UNROLL
					ap_uint<WIDTH_BIT> col_ind_left = col_ind + min_col_ind;
					ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> col_ind_tmp = col_ind_left * scale;
					ap_uint<WIDTH_BIT> col_ind_tmp_uint = col_ind_tmp;
					if (col_ind_left < max_col_ind){
						if (col_ind_tmp_uint + 1 >= read_ind * PROCESS_NUM)
							col_ind_tmp_uint = col_ind_tmp_uint + 1 - read_ind * PROCESS_NUM;
						else
							col_ind_tmp_uint = 0;
					}
					else
						col_ind_tmp_uint = 0;
					image_buf[col_ind] = input_buf[col_ind_tmp_uint]; //(data>>(col_ind_tmp_uint_short * PIXEL_BIT)).range(PIXEL_BIT-1, 0);//data.range((col_ind_tmp_uint + 1) * PIXEL_BIT - 1, col_ind_tmp_uint * PIXEL_BIT);
				}
#ifdef DEBUG
				cout << row_ind << endl;
				cout << row_ind_up << " " << row_ind_down << endl;
				for (int i = 0; i < PROCESS_NUM; i++)
					cout << image_buf[i] << " ";
				cout << endl
					 << "----------------------------------new_image_buf--------------------------------" << endl;
#endif

				ap_uint<8 + PROCESS_BIT> write_tmp = 0;
				for (ap_uint<8> i = 0; i < PROCESS_NUM; i++)
				{
#pragma HLS UNROLL
					write_tmp.range((i + 1) * PIXEL_BIT - 1, i * PIXEL_BIT) = image_buf[i];
				}
				write_tmp.range(7 + PROCESS_BIT, PROCESS_BIT) = max_col_ind - min_col_ind;
				selectData.write(write_tmp);
			}
		}
	}
}

void process_output(hls::stream<ap_uint<8 + PROCESS_BIT> > &selectData, hls::stream<ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outStream)
{
	ap_uint<PROCESS_BIT> data = 0;
	ap_uint<OUTPUT_BIT> write_tmp = 0;
	ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> out;
	ap_uint<8> pixel_num = 0;
	ap_uint<8> rmn_num = 0;
	ap_uint<PIXEL_NUM_BIT> p_cnt = 0;
	ap_uint<8 + PROCESS_BIT> s_data = 0;
	for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < new_height; row_ind++)
	{
		ap_uint<WIDTH_BIT> read_ind = 0;
		while (read_ind < unit_num || rmn_num >= OUTPUT_PIXEL_NUM)
		{
#pragma HLS PIPELINE

			if (rmn_num >= OUTPUT_PIXEL_NUM)
			{
				rmn_num = rmn_num - OUTPUT_PIXEL_NUM;
				out.data = write_tmp;
				for (ap_uint<8> i = 0; i < OUTPUT_BIT >> 3; i++)
#pragma HLS UNROLL
					out.keep.range(i, i) = 1;
				if (p_cnt == new_width * new_height && rmn_num == 0)
					out.last = 1;
				else
					out.last = 0;
				outStream.write(out);
				write_tmp = 0;
				if (rmn_num > 0)
					write_tmp = data.range(pixel_num * PIXEL_BIT - 1, (pixel_num - rmn_num) * PIXEL_BIT);
			}
			else
			{
				s_data = selectData.read();
				data = s_data.range(PROCESS_BIT - 1, 0);
				pixel_num = s_data.range(7 + PROCESS_BIT, PROCESS_BIT);
				p_cnt = p_cnt + pixel_num;
				read_ind = read_ind + 1;
				if (rmn_num + pixel_num >= OUTPUT_PIXEL_NUM)
				{
					write_tmp.range(OUTPUT_BIT - 1, rmn_num * PIXEL_BIT) = data.range(OUTPUT_BIT - 1 - rmn_num * PIXEL_BIT, 0);
					rmn_num = rmn_num + pixel_num - OUTPUT_PIXEL_NUM;
					out.data = write_tmp;
					for (ap_uint<8> i = 0; i < OUTPUT_BIT >> 3; i++)
#pragma HLS UNROLL
						out.keep.range(i, i) = 1;
					if (p_cnt == new_width * new_height && rmn_num == 0)
						out.last = 1;
					else
						out.last = 0;
					outStream.write(out);
					write_tmp = 0;
					if (rmn_num > 0)
						write_tmp = data.range(pixel_num * PIXEL_BIT - 1, (pixel_num - rmn_num) * PIXEL_BIT);
				}
				else
				{
					if (pixel_num > 0)
						write_tmp.range((rmn_num + pixel_num) * PIXEL_BIT - 1, rmn_num * PIXEL_BIT) = data.range(pixel_num * PIXEL_BIT - 1, 0);
					rmn_num = rmn_num + pixel_num;
				}
			}
		}
	}
	if (rmn_num > 0)
	{
		out.data = write_tmp;
		for (ap_uint<8> i = 0; i < OUTPUT_BIT >> 3; i++)
#pragma HLS UNROLL
			out.keep.range(i, i) = 1;
		out.last = 1;
		outStream.write(out);
	}
}
