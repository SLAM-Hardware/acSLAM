/**
* This file is part of ac^2SLAM.
*
* Copyright (C) 2021 Cheng Wang <wangcheng at stu dot xjtu dot edu dot cn> (Xi'an Jiaotong University)
* For more information see <https://github.com/SLAM-Hardware/acSLAM>
*
* ac^2SLAM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ac^2SLAM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ac^2SLAM. If not, see <http://www.gnu.org/licenses/>.
*/

/***************************************************************************
Change the Parallelism
1. Change INPUT_PIXEL_NUM to 4n (n=1,2,3...).
2. Change READ_NUM to ceil((HALF_WIN_SZ + 1)/ INPUT_PIXEL_NUM).
3. Change ARRAY_PARTITION and UNROLL factor in RS_BRIEF.cpp. 
   The factor is equal to INPUT_PIXEL_NUM/4.
 ***************************************************************************/

#ifndef RS_BRIEF_H_
#define RS_BRIEF_H_

// #define DEBUG

// #define BOARDER_101

#include <iostream>
#include "hls_stream.h"
#include "ap_int.h"
#include "hls_math.h"
#include "ap_fixed.h"
#include "ap_axi_sdata.h"

#define PIXEL_BIT 8
#define INPUT_BIT INPUT_PIXEL_NUM * PIXEL_BIT
#define INPUT_PIXEL_NUM 4
#define WIDTH 1241
#define HEIGHT 376
#define WIN_SZ 29
#define HALF_WIN_SZ (WIN_SZ >> 1)
#define WIDTH_BIT 11
#define HEIGHT_BIT 9
#define WIN_SZ_BIT 5
#define PIXEL_NUM_BIT WIDTH_BIT + HEIGHT_BIT
#define MAX_PIXEL_VAL 255
#define PROCESS_NUM INPUT_PIXEL_NUM // equal to INPUT_PIXEL_NUM
#define PROCESS_BIT PROCESS_NUM * PIXEL_BIT
#define MERGE_NUM 4
#define WIDTH_AFTER_MERGE 319 // ceil((WIDTH + WIN_SZ - 1) / MERGE_NUM)
#define LOG_2_MERGE_NUM 3
#define PADDING_WIDTH_AFTER_MERGE 4
#define FIRST_PIXEL_INDEX (PADDING_WIDTH_AFTER_MERGE - 1) //or PADDING_WIDTH_AFTER_MERGE when HALF_WIN_SZ%MERGE_NUM==0
#define READ_NUM 4//ceil((HALF_WIN_SZ + 1)/ INPUT_PIXEL_NUM)
#define REMAIN_NUM (HALF_WIN_SZ - (READ_NUM - 1) * INPUT_PIXEL_NUM)
#define INPUT_STREAM_BIT INPUT_BIT

void RS_BRIEF(hls::stream<ap_axiu<32, 1, 1, 1> > &cfgStream, hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcPixelStream, hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcFASTStream, hls::stream<ap_axiu<512, 1, 1, 1> > &outStream);


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
