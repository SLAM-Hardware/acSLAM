#ifndef RESIZE_H
#define RESIZE_H

//#define DEBUG

#include <iostream>
#include "hls_stream.h"
#include "ap_int.h"
#include "hls_math.h"
#include "ap_fixed.h"
#include "ap_axi_sdata.h"

#define SCALE 3.5
#define PIXEL_BIT 8
#define INPUT_BIT INPUT_PIXEL_NUM * PIXEL_BIT
#define INPUT_PIXEL_NUM 16
#define OUTPUT_BIT OUTPUT_PIXEL_NUM * PIXEL_BIT
#define OUTPUT_PIXEL_NUM 4
#define WIDTH 1241
#define HEIGHT 376
#define WIN_SZ 2
#define WIDTH_BIT 11
#define HEIGHT_BIT 9
#define WIN_SZ_BIT 2
#define PIXEL_NUM_BIT WIDTH_BIT + HEIGHT_BIT
#define MAX_PIXEL_VAL 255
#define PROCESS_NUM INPUT_PIXEL_NUM
#define PROCESS_BIT PROCESS_NUM * PIXEL_BIT
#define MERGE_NUM 4
#define WIDTH_AFTER_MERGE 311 // ceil(WIDTH / MERGE_NUM)
#define LOG_2_MERGE_NUM 3
#define INPUT_STREAM_BIT INPUT_BIT
#define OUTPUT_STREAM_BIT OUTPUT_BIT
//const ap_ufixed<64, 2> inv_scale_64 = 1 / 1.2;

void resize(hls::stream<ap_axiu<32, 1, 1, 1> > &cfgStream, hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream, hls::stream<ap_axiu<32, 1, 1, 1> > &cfgoutStream, hls::stream<ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outStream);

template <class T, int W, int I>
T my_round(T x)
{
	T tmp = x;
	if (x.range(W - I - 1, W - I - 1) == 1)
		tmp.range(W - 1, W - I) = tmp.range(W - 1, W - I) + 1;
	tmp.range(W - I - 1, 0) = 0;
	return tmp;
}

template <class T, int W, int I>
T my_ceil(T x)
{
	T tmp = x;
	if (x.range(W - I - 1, 0) != 0)
		tmp.range(W - 1, W - I) = tmp.range(W - 1, W - I) + 1;
	tmp.range(W - I - 1, 0) = 0;
	return tmp;
}

template <class T, int W, int I>
T my_floor(T x)
{
	T tmp = x;
	tmp.range(W - I - 1, 0) = 0;
	return tmp;
}

#endif
