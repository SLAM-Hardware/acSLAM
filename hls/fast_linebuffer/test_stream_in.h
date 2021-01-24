#include "hls_stream.h"
#include "./include/common/xf_utility.h"
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "math.h"

void test_stream_in(hls::stream<ap_axiu<8,1,1,1> > &srcStream, hls::stream<ap_axiu<16,1,1,1>  > &outStream);

#define NO  	1 			// Normal
#define RO  	0 			// Resource Optimized

#define WIDTH 	640
#define HEIGHT	480

#define COL_IMAGE_BIT 10
#define ROW_IMAGE_BIT 9

#define WIN_SZ 7
#define WIN_SZ_IDX_BIT 4
#define NPC 1
#define PIXEL_BIT 8
