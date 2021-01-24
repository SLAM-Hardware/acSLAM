#include<iostream>
#include<fstream>
#include<iomanip>
#include "ap_int.h"
#include "ap_axi_sdata.h"
#include"hls_stream.h"
#include"FAST_extractor.h"
//#include <bitset>
using namespace std;


int main(int argc,char** argv){
#ifdef TEST
	ifstream idata("0.txt");
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

    ofstream FILE;
    FILE.open("../../../FAST_result.dat");
    int dataIn[HEIGHT*WIDTH]={0};// [WIDTH*HEIGHT];
    int* result_dataIn;
    result_dataIn=(int *)malloc(sizeof(int)*HEIGHT*WIDTH*2);// [WIDTH*HEIGHT];
    ifstream idata("../../../0.txt");
    ifstream result_data("../../../c_result.dat");
    int cnt = 0, idx = 0;
    string data_out;

    while(getline( result_data, data_out, ',')) {
		stringstream ss;
		ss << data_out;
		int data;
		ss >> data;
		cnt++;
		idx++;
	}
    cnt = 0;
    idx = 0;
    while(getline( idata, data_out, ',')) {
        stringstream ss;
        ss << data_out;
        int data;
        ss >> data;
        cnt++;
        dataIn[idx] = data;
		cout<<data<<" _ ";
		result_dataIn[idx] = data;
        idx++;
        if (idx == HEIGHT*WIDTH - 1)
        	break;
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
	hls::stream<ap_axiu<32,1,1,1>  > output;

	for (int row= 0; row < WIDTH; row++){
		for (int col = 0; col < HEIGHT; col++) {
			input_axiu.data = dataIn[row*HEIGHT+col];
			input << input_axiu;
		}
	}

	FAST_extractor(input, output);

	ap_uint<32> result;
	for (int i =0; i < WIDTH*HEIGHT; i++){
		result = (output.read()).data;
//		FILE << setbase(10) <<setw(5)<<result.range(7,0) << ",";
//		FILE << setbase(10) << result[8] << ",\t"<< result.range(7,0) << ",\t";
		if (!(result[8] == result_dataIn[i*2] && result.range(7,0) == result_dataIn[i*2+1])){
			cout << "error!";
			return -1;
		}
//		if (result[8] == 1){
			FILE << (result)<<endl;
//		}
//		if ((i+1)%WIDTH == 0) FILE <<endl;
	}
	cout << "pass!";
	FILE.close();

	return 0;
}
