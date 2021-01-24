#include</home/cag/work/vivado/single_adder/adder.h>

void adder(hls::stream<ap_axiu<8,1,1,1> > &in, hls::stream<ap_axiu<16,1,1,1> > &out){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=out
#pragma HLS INTERFACE axis register both port=in

	ap_uint<8> dataIn[10];
	ap_axiu<16,1,1,1> dataOut;
	for(int i=0; i< 10; i++){
#pragma HLS pipeline
		dataIn[i] = (in.read()).data;
		dataOut.data = dataIn[i] * 5;
		dataOut.keep = 0x3;
		if(i==9) dataOut.last = 1;
		else dataOut.last=0;
		out.write(dataOut);
	}

//	int cnt = 0;
//	for(int i=0; i <10; i++){
//		dataOut.data = dataIn[i] * 5;
//		dataOut.keep = 0x3;
//		if(i==9) dataOut.last = 1;
//		out.write(dataOut);
//	}
}
