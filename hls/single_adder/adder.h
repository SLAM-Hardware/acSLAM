#include<hls_stream.h>
#include<ap_int.h>
#include "ap_axi_sdata.h"

void adder(hls::stream<ap_axiu<8,1,1,1> > &in, hls::stream<ap_axiu<16,1,1,1> > &out);
