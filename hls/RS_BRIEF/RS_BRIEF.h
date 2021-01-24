#ifndef _RS_BRIEF_H_
#define _RS_BRIEF_H_

#include "hls_stream.h"
#include "/home/cag/wangcheng/slam-accelerator/hls/RS_BRIEF/xfopencv/include/common/xf_common.h"
#include "/home/cag/wangcheng/slam-accelerator/hls/RS_BRIEF/xfopencv/include/common/xf_utility.h"
#include "ap_int.h"
#include "hls_math.h"
#include "ap_fixed.h"
#include "ap_axi_sdata.h"

#define __MIN(a,b) ((a < b) ? a : b)
#define __MAX(a,b) ((a > b) ? a : b)
//#define HW_COSIM
#define PSize       16
#define NUM         25

#define WIDTH 	1241
#define HEIGHT	376

void RS_BRIEF(hls::stream<ap_axiu<32,1,1,1> > &srcStream,hls::stream<ap_axiu<512,1,1,1> > & descriptor_out);
#endif
