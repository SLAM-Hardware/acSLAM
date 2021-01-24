#include<iostream>
#include<fstream>
#include<iomanip>
#include <ap_int.h>
#include "ap_axi_sdata.h"
#include<hls_stream.h>
#include</home/cag/work/pynq/ORB_FPGA_jianming/test_stream_in.h>
using namespace std;


int main(int argc,char** argv){
#ifdef TEST
	ifstream idata("/home/cag/work/pynq/ORB_FPGA_jianming/image/0.txt");
	int idx = 0;
	string data_out;
	while(getline( idata, data_out, ',')) {
		stringstream ss;
		ss << data_out;
		int data;
		ss >> data;
		idx++;
	}
#ifdef __SYNTHERSIS__
	int dataIn[WIDTH][HEIGHT];
#else
	int *dataIn = (int*) malloc(WIDTH*HEIGHT*sizeof(int));
#endif
#endif
//	  ofstream FILE;
//	  FILE.open("/home/cag/work/pynq/ORB_FPGA_jianming/rst.dat");
//	ap_uint<8> dataIn[HEIGHT][WIDTH] = {
//#include </home/cag/work/pynq/ORB_FPGA_jianming/image/0.txt>
//		};

    ofstream FILE;
    FILE.open("result.dat");
    int *dataIn = malloc( HEIGHT*WIDTH*sizeof(int));// [WIDTH*HEIGHT];
    ifstream idata("/home/cag/work/pynq/ORB_FPGA_jianming/image/0.txt");
    int cnt = 0, idx = 0;
    string data_out;
    while(getline( idata, data_out, ',')) {
        stringstream ss;
        ss << data_out;
        int data;
        ss >> data;
        cnt++;
        dataIn[idx] = data;
        idx++;
    }

	// test read data
	/*
	ofstream file_read_loop;
	file_read_loop.open("/home/cag/work/pynq/ORB_FPGA_jianming/loop.dat");
	for (int row= 0; row < WIDTH; row++){
			for (int col = 0; col < HEIGHT; col++) {
				file_read_loop << setbase(10) <<  dataIn[row][col]<< endl;
			}
		}
	 */

	ap_axiu<8,1,1,1> input_axiu;
	hls::stream<ap_axiu<8,1,1,1> > input;
	hls::stream<ap_axiu<16,1,1,1>  > output;

	for (int row= 0; row < WIDTH; row++){
		for (int col = 0; col < HEIGHT; col++) {
			input_axiu.data = dataIn[row*HEIGHT+col];
			input << input_axiu;
		}
	}

	test_stream_in(input, output);

	ap_uint<16> result;
	for (int i =0; i < WIDTH*HEIGHT; i++){
		result = (output.read()).data;
//		cout << setbase(10) << result[8] << " "<< result.range(7,0) << endl;
		FILE << setbase(10) << result[8] << " "<< result.range(7,0) << endl;
	}

	FILE.close();

	return 0;
}
