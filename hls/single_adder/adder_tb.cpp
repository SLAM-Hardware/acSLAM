#include</home/cag/work/vivado/single_adder/adder.h>
#include<iostream>

int main(int argv, char** argc){
	ap_uint<8> data[10]={0,1,2,3,4,5,6,7,8,9};
	ap_uint<16> data_out[10];
	ap_axiu<8,1,1,1> datatemp;

	hls::stream<ap_axiu<8,1,1,1> >  dataHWIn;
	hls::stream<ap_axiu<16,1,1,1> > dataHwOut;

	for (int i =0; i < 10; i++){
		data_out[i] = data[i] * 5;
	}

	for (int i=0; i < 10; i++){
		datatemp.data = data[i];
		dataHWIn.write(datatemp);
	}

	adder(dataHWIn, dataHwOut);

	for (int i =0; i < 10 ; i++){

		ap_axiu<16,1,1,1> dataOurRead;
		dataOurRead = dataHwOut.read();
		std::cout << std::setbase(10) << dataOurRead.data << " "  <<  dataOurRead.keep  << ' ' << dataOurRead.last << std::endl;
		if(data_out[i] == dataOurRead.data){}
		else{
			return 1;
		}
	}

	return 0;
}
