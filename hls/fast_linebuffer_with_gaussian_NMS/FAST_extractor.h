#ifndef __FAST_EXTRACTOR__
#define __FAST_EXTRACTOR__
	#include "hls_stream.h"
	//#include "./include/common/xf_utility.h"
	#include "ap_axi_sdata.h"
	#include "ap_int.h"
//	#include "math.h"
	#include "hls_math.h"
	void FAST_extractor(hls::stream<ap_axiu<8,1,1,1> > &srcStream, hls::stream<ap_axiu<32,1,1,1>  > &outStream);

	#define NO  	1 			// Normal
	#define RO  	0 			// Resource Optimized

	#define WIDTH 	1241
	#define HEIGHT	376

	#define COL_IMAGE_BIT 11
	#define ROW_IMAGE_BIT 9

	#define WIN_SZ 9
	#define WIN_SZ_IDX_BIT 4
	#define NPC 1
	#define PIXEL_BIT 8
#endif
