#include "RS_BRIEF.h"

using namespace std;

const ap_uint<8> mask[WIN_SZ][WIN_SZ] =
{
    000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000,
    000, 000, 000, 000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000, 000, 000, 000,
    000, 000, 000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000, 000, 000,
    000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000,
    000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000,
    000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000,
    000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000,
    000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000,
    000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000,
    000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000,
    000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000,
    000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000,
    000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000,
    000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000,
    000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000,
    000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000,
    000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000,
    000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000,
    000, 000, 000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000, 000, 000,
    000, 000, 000, 000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000, 000, 000, 000,
    000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000
};

const ap_int<6> bit_pattern_29_[256][4] =
{
    -5,4, -5,13,
    0,11, 4,1,
    -6,-2, -4,-1,
    -11,-4, -3,0,
    4,8, -5,2,
    5,3, -1,12,
    0,7, -5,5,
    9,-2, 8,9,
    -4,5, -2,14,
    2,11, 4,0,
    -6,-1, -4,0,
    -12,-2, -3,1,
    5,7, -5,3,
    5,2, 1,12,
    1,7, -4,6,
    8,-4, 10,7,
    -3,6, 0,14,
    4,10, 4,-1,
    -6,0, -4,1,
    -12,1, -3,1,
    7,6, -4,4,
    6,1, 4,11,
    3,6, -3,7,
    8,-5, 11,5,
    -2,6, 3,14,
    6,9, 4,-1,
    -6,2, -4,1,
    -11,3, -2,2,
    8,4, -3,4,
    6,0, 6,11,
    4,6, -1,7,
    6,-7, 12,3,
    -1,6, 6,13,
    8,8, 4,-2,
    -6,3, -4,2,
    -11,5, -2,2,
    8,3, -2,5,
    6,-1, 8,9,
    5,5, 0,7,
    5,-8, 12,1,
    1,6, 8,11,
    9,6, 3,-3,
    -5,4, -3,3,
    -9,7, -2,2,
    9,1, -1,5,
    5,-2, 9,7,
    6,4, 1,7,
    3,-9, 12,-2,
    2,6, 10,10,
    10,4, 2,-3,
    -4,5, -2,3,
    -8,9, -1,3,
    9,-1, 0,5,
    5,-3, 11,6,
    6,3, 3,7,
    2,-9, 11,-4,
    3,6, 12,7,
    11,2, 2,-4,
    -3,5, -2,4,
    -6,10, -1,3,
    9,-2, 1,5,
    4,-4, 12,3,
    7,1, 4,6,
    0,-9, 10,-6,
    4,5, 13,5,
    11,0, 1,-4,
    -2,6, -1,4,
    -4,11, 0,3,
    8,-4, 2,5,
    3,-5, 12,1,
    7,0, 5,5,
    -2,-9, 9,-8,
    5,4, 14,2,
    11,-2, 0,-4,
    -1,6, 0,4,
    -2,12, 1,3,
    7,-5, 3,5,
    2,-5, 12,-1,
    7,-1, 6,4,
    -4,-8, 7,-10,
    6,3, 14,0,
    10,-4, -1,-4,
    0,6, 1,4,
    1,12, 1,3,
    6,-7, 4,4,
    1,-6, 11,-4,
    6,-3, 7,3,
    -5,-8, 5,-11,
    6,2, 14,-3,
    9,-6, -1,-4,
    2,6, 1,4,
    3,11, 2,2,
    4,-8, 4,3,
    0,-6, 11,-6,
    6,-4, 7,1,
    -7,-6, 3,-12,
    6,1, 13,-6,
    8,-8, -2,-4,
    3,6, 2,4,
    5,11, 2,2,
    3,-8, 5,2,
    -1,-6, 9,-8,
    5,-5, 7,0,
    -8,-5, 1,-12,
    6,-1, 11,-8,
    6,-9, -3,-3,
    4,5, 3,3,
    7,9, 2,2,
    1,-9, 5,1,
    -2,-5, 7,-9,
    4,-6, 7,-1,
    -9,-3, -2,-12,
    6,-2, 10,-10,
    4,-10, -3,-2,
    5,4, 3,2,
    9,8, 3,1,
    -1,-9, 5,0,
    -3,-5, 6,-11,
    3,-6, 7,-3,
    -9,-2, -4,-11,
    6,-3, 7,-12,
    2,-11, -4,-2,
    5,3, 4,2,
    10,6, 3,1,
    -2,-9, 5,-1,
    -4,-4, 3,-12,
    1,-7, 6,-4,
    -9,0, -6,-10,
    5,-4, 5,-13,
    0,-11, -4,-1,
    6,2, 4,1,
    11,4, 3,0,
    -4,-8, 5,-2,
    -5,-3, 1,-12,
    0,-7, 5,-5,
    -9,2, -8,-9,
    4,-5, 2,-14,
    -2,-11, -4,0,
    6,1, 4,0,
    12,2, 3,-1,
    -5,-7, 5,-3,
    -5,-2, -1,-12,
    -1,-7, 4,-6,
    -8,4, -10,-7,
    3,-6, 0,-14,
    -4,-10, -4,1,
    6,0, 4,-1,
    12,-1, 3,-1,
    -7,-6, 4,-4,
    -6,-1, -4,-11,
    -3,-6, 3,-7,
    -8,5, -11,-5,
    2,-6, -3,-14,
    -6,-9, -4,1,
    6,-2, 4,-1,
    11,-3, 2,-2,
    -8,-4, 3,-4,
    -6,0, -6,-11,
    -4,-6, 1,-7,
    -6,7, -12,-3,
    1,-6, -6,-13,
    -8,-8, -4,2,
    6,-3, 4,-2,
    11,-5, 2,-2,
    -8,-3, 2,-5,
    -6,1, -8,-9,
    -5,-5, 0,-7,
    -5,8, -12,-1,
    -1,-6, -8,-11,
    -9,-6, -3,3,
    5,-4, 3,-3,
    9,-7, 2,-2,
    -9,-1, 1,-5,
    -5,2, -9,-7,
    -6,-4, -1,-7,
    -3,9, -12,2,
    -2,-6, -10,-10,
    -10,-4, -2,3,
    4,-5, 2,-3,
    8,-9, 1,-3,
    -9,1, 0,-5,
    -5,3, -11,-6,
    -6,-3, -3,-7,
    -2,9, -11,4,
    -3,-6, -12,-7,
    -11,-2, -2,4,
    3,-5, 2,-4,
    6,-10, 1,-3,
    -9,2, -1,-5,
    -4,4, -12,-3,
    -7,-1, -4,-6,
    0,9, -10,6,
    -4,-5, -13,-5,
    -11,0, -1,4,
    2,-6, 1,-4,
    4,-11, 0,-3,
    -8,4, -2,-5,
    -3,5, -12,-1,
    -7,0, -5,-5,
    2,9, -9,8,
    -5,-4, -14,-2,
    -11,2, 0,4,
    1,-6, 0,-4,
    2,-12, -1,-3,
    -7,5, -3,-5,
    -2,5, -12,1,
    -7,1, -6,-4,
    4,8, -7,10,
    -6,-3, -14,0,
    -10,4, 1,4,
    0,-6, -1,-4,
    -1,-12, -1,-3,
    -6,7, -4,-4,
    -1,6, -11,4,
    -6,3, -7,-3,
    5,8, -5,11,
    -6,-2, -14,3,
    -9,6, 1,4,
    -2,-6, -1,-4,
    -3,-11, -2,-2,
    -4,8, -4,-3,
    0,6, -11,6,
    -6,4, -7,-1,
    7,6, -3,12,
    -6,-1, -13,6,
    -8,8, 2,4,
    -3,-6, -2,-4,
    -5,-11, -2,-2,
    -3,8, -5,-2,
    1,6, -9,8,
    -5,5, -7,0,
    8,5, -1,12,
    -6,1, -11,8,
    -6,9, 3,3,
    -4,-5, -3,-3,
    -7,-9, -2,-2,
    -1,9, -5,-1,
    2,5, -7,9,
    -4,6, -7,1,
    9,3, 2,12,
    -6,2, -10,10,
    -4,10, 3,2,
    -5,-4, -3,-2,
    -9,-8, -3,-1,
    1,9, -5,0,
    3,5, -6,11,
    -3,6, -7,3,
    9,2, 4,11,
    -6,3, -7,12,
    -2,11, 4,2,
    -5,-3, -4,-2,
    -10,-6, -3,-1,
    2,9, -5,1,
    4,4, -3,12,
    -1,7, -6,4,
    9,0, 6,10
};

ap_uint<32> width;
ap_uint<32> height;
ap_uint<WIDTH_BIT> unit_num;
ap_uint<WIDTH_BIT> padding_unit_num;
void process_mdl(hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcPixelStream,
             hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcFASTStream, hls::stream <ap_axiu<512, 1, 1, 1> > &outStream);

void process_cfg(hls::stream <ap_axiu<32, 1, 1, 1> > &cfgStream);

void process_input(hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcPixelStream,
                   hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcFASTStream,
                   hls::stream <ap_uint<INPUT_BIT> > &pixelData, 
                   hls::stream <ap_uint<INPUT_BIT> > &FASTData);

void process_padding(hls::stream <ap_uint<INPUT_BIT> > &pixelData, 
                     hls::stream <ap_uint<(WIN_SZ - 1) * PIXEL_BIT> > &initData, 
                     hls::stream <ap_uint<INPUT_BIT> > &srcData);

void process_buf(hls::stream <ap_uint<(WIN_SZ - 1) * PIXEL_BIT> > &pixelInitData,
                 hls::stream <ap_uint<INPUT_BIT> > &pixelSrcData,
				 hls::stream <ap_uint<INPUT_BIT> > &FASTData,
                 hls::stream <ap_uint<PIXEL_BIT * (WIN_SZ + PROCESS_NUM - 1)> > bufData[WIN_SZ], 
                 hls::stream <ap_uint<PIXEL_BIT * PROCESS_NUM> > &FASTbufData, 
                 hls::stream <ap_uint<WIDTH_BIT + HEIGHT_BIT> > &posData);

void process_RS_BRIEF(hls::stream <ap_uint<PIXEL_BIT * (WIN_SZ + PROCESS_NUM - 1)> > bufData[WIN_SZ], hls::stream <ap_uint<PIXEL_BIT * PROCESS_NUM> > &FASTbufData, hls::stream <ap_uint<WIDTH_BIT + HEIGHT_BIT> > &posData, hls::stream <ap_axiu<512, 1, 1, 1> > &outStream);

void RS_BRIEF(hls::stream <ap_axiu<32, 1, 1, 1> > &cfgStream, hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcPixelStream,
              hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcFASTStream,
              hls::stream <ap_axiu<512, 1, 1, 1> > &outStream) {
#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS INTERFACE axis register both port = cfgStream
#pragma HLS INTERFACE axis register both port = srcPixelStream
#pragma HLS INTERFACE axis register both port = srcFASTStream
#pragma HLS INTERFACE axis register both port = outStream

//    width = cfgStream.read().data;
//    height = cfgStream.read().data;
//
//    ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> unit_num_ufixed = width;
//    unit_num_ufixed = my_ceil<ap_ufixed < WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT >
//                                                                                    (unit_num_ufixed / INPUT_PIXEL_NUM);
//    unit_num = unit_num_ufixed.range(WIDTH_BIT + 7, 8);
//
//    ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> padding_unit_num_ufixed = width + WIN_SZ - 1;
//    padding_unit_num_ufixed = my_ceil<ap_ufixed < WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT >
//                                                                                            (padding_unit_num_ufixed /
//                                                                                             INPUT_PIXEL_NUM);
//    padding_unit_num = padding_unit_num_ufixed.range(WIDTH_BIT + 7, 8);
	process_cfg(cfgStream);
    process_mdl(srcPixelStream, srcFASTStream, outStream);
}

void process_cfg(hls::stream <ap_axiu<32, 1, 1, 1> > &cfgStream) {
// #pragma HLS INLINE off
#pragma HLS PIPELINE
    width = cfgStream.read().data;
    height = cfgStream.read().data;

    ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> unit_num_ufixed = width;
    unit_num_ufixed = my_ceil<ap_ufixed < WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT >
                                                                                    (unit_num_ufixed / INPUT_PIXEL_NUM);
    unit_num = unit_num_ufixed.range(WIDTH_BIT + 7, 8);

    ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> padding_unit_num_ufixed = width + WIN_SZ - 1;
    padding_unit_num_ufixed = my_ceil<ap_ufixed < WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT >
                                                                                            (padding_unit_num_ufixed /
                                                                                             INPUT_PIXEL_NUM);
    padding_unit_num = padding_unit_num_ufixed.range(WIDTH_BIT + 7, 8);
}

void process_mdl(hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcPixelStream,
             hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcFASTStream,
             hls::stream <ap_axiu<512, 1, 1, 1> > &outStream) {
// #pragma HLS INLINE off
#pragma HLS DATAFLOW
    hls::stream <ap_uint<INPUT_BIT> > pixelData;
#pragma HLS STREAM variable = pixelData depth = 2
    hls::stream <ap_uint<INPUT_BIT> > FASTData;
#pragma HLS STREAM variable = FASTData depth = 5000
    hls::stream <ap_uint<PIXEL_BIT * (WIN_SZ + PROCESS_NUM - 1)> > bufData[WIN_SZ];
#pragma HLS STREAM variable = bufData depth = 2
    hls::stream <ap_uint<PIXEL_BIT * PROCESS_NUM> > FASTbufData;
#pragma HLS STREAM variable = FASTbufData depth =2
    hls::stream <ap_uint<WIDTH_BIT + HEIGHT_BIT> > posData;
#pragma HLS STREAM variable = posData depth =2
    hls::stream <ap_uint<(WIN_SZ - 1) * PIXEL_BIT> > pixelInitData;
#pragma HLS STREAM variable = pixelInitData depth = 2
    hls::stream <ap_uint<INPUT_BIT> > pixelSrcData;
#pragma HLS STREAM variable = pixelSrcData depth = 2
    hls::stream <ap_uint<(WIN_SZ - 1) * PIXEL_BIT> > FASTInitData;
#pragma HLS STREAM variable = FASTInitData depth = 2
    hls::stream <ap_uint<INPUT_BIT> > FASTSrcData;
#pragma HLS STREAM variable = FASTSrcData depth = 2
    

    process_input(srcPixelStream, srcFASTStream, pixelData, FASTData);
    process_padding(pixelData, pixelInitData, pixelSrcData);
    process_buf(pixelInitData, pixelSrcData, FASTData, bufData, FASTbufData, posData);
    process_RS_BRIEF(bufData, FASTbufData, posData, outStream);
#ifdef DEBUG
    for (int i = 0; i < height; i++)
    {
        ap_uint<(WIN_SZ - 1) * PIXEL_BIT> initTmp =  pixelInitData.read();
        for (int j = 0; j < WIN_SZ - 1; j++)
            cout << initTmp.range((j+1)*PIXEL_BIT-1,j*PIXEL_BIT) << " ";
    	for (int j = 0; j < unit_num; j++)
    	{
    		ap_uint<INPUT_BIT> data = pixelSrcData.read();
    		for (int read_ind = 0; read_ind<INPUT_PIXEL_NUM; read_ind++)
    			cout << data.range((read_ind+1)*PIXEL_BIT-1,read_ind*PIXEL_BIT) << " ";
    	}
    	cout << endl;
    }
    for (int i = 0; i < height; i++)
    {
        ap_uint<(WIN_SZ - 1) * PIXEL_BIT> initTmp =  FASTInitData.read();
        for (int j = 0; j < WIN_SZ - 1; j++)
            cout << initTmp.range((j+1)*PIXEL_BIT-1,j*PIXEL_BIT) << " ";
    	for (int j = 0; j < unit_num; j++)
    	{
    		ap_uint<INPUT_BIT> data = FASTSrcData.read();
    		for (int read_ind = 0; read_ind<INPUT_PIXEL_NUM; read_ind++)
    			cout << data.range((read_ind+1)*PIXEL_BIT-1,read_ind*PIXEL_BIT) << " ";
    	}
    	cout << endl;
    }
#endif
}

void process_input(hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcPixelStream,
                   hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcFASTStream,
                   hls::stream <ap_uint<INPUT_BIT> > &pixelData, hls::stream <ap_uint<INPUT_BIT> > &FASTData) {
#pragma HLS INLINE off

//     input from PS or tb
    ap_uint<PIXEL_NUM_BIT> cnt = 0;
    ap_uint<INPUT_BIT> pixel_data = 0;
    ap_uint<INPUT_BIT> pixel_prv_data = 0;
    ap_uint<INPUT_BIT> FAST_data = 0;
    ap_uint<INPUT_BIT> FAST_prv_data = 0;
    ap_uint<11> rmn_num = 0;
    ap_uint<11> write_num = 0;
    ap_uint<INPUT_BIT> pixel_write_tmp = 0;
    ap_uint<INPUT_BIT> FAST_write_tmp = 0;
    for (ap_uint<HEIGHT_BIT> i = 0; i < height; i++) {
        for (ap_uint<WIDTH_BIT> j = 0; j < unit_num; j++) {
#pragma HLS PIPELINE
            if (j == unit_num - 1)
                write_num = (width - INPUT_PIXEL_NUM * (unit_num - 1)) * PIXEL_BIT;
            else
                write_num = INPUT_PIXEL_NUM * PIXEL_BIT;

            if (rmn_num >= write_num) {
                pixel_write_tmp = 0;
                FAST_write_tmp = 0;
                pixel_write_tmp.range(write_num - 1, 0) = pixel_data.range(INPUT_BIT - rmn_num + write_num - 1,
                                                                           INPUT_BIT - rmn_num);
                FAST_write_tmp.range(write_num - 1, 0) = FAST_data.range(INPUT_BIT - rmn_num + write_num - 1,
                                                                         INPUT_BIT - rmn_num);
                rmn_num = rmn_num - write_num;
            } else {
                pixel_prv_data = pixel_data;
                pixel_data = srcPixelStream.read().data;
                FAST_prv_data = FAST_data;
                FAST_data = srcFASTStream.read().data;
                if (rmn_num > 0) {
                    pixel_write_tmp = 0;
                    FAST_write_tmp = 0;
                    pixel_write_tmp.range(rmn_num - 1, 0) = pixel_prv_data.range(INPUT_BIT - 1, INPUT_BIT - rmn_num);
                    pixel_write_tmp.range(write_num - 1, rmn_num) = pixel_data.range(write_num - rmn_num - 1, 0);
                    FAST_write_tmp.range(rmn_num - 1, 0) = FAST_prv_data.range(INPUT_BIT - 1, INPUT_BIT - rmn_num);
                    FAST_write_tmp.range(write_num - 1, rmn_num) = FAST_data.range(write_num - rmn_num - 1, 0);
                    rmn_num = INPUT_BIT - (write_num - rmn_num);
                } else {
                    pixel_write_tmp = 0;
                    FAST_write_tmp = 0;
                    pixel_write_tmp.range(write_num - 1, 0) = pixel_data.range(write_num - 1, 0);
                    FAST_write_tmp.range(write_num - 1, 0) = FAST_data.range(write_num - 1, 0);
                    rmn_num = INPUT_BIT - write_num;
                }
            }
            pixelData.write(pixel_write_tmp);
            FASTData.write(FAST_write_tmp);
        }
    }

//     input from PL FAST module
//     for (ap_uint<HEIGHT_BIT> i = 0; i < height; i++) {
//         for (ap_uint<WIDTH_BIT> j = 0; j < unit_num; j++) {
// #pragma HLS PIPELINE
//             ap_uint<INPUT_BIT> pixel_data = srcPixelStream.read().data;
//             ap_uint<INPUT_BIT> FAST_data = srcFASTStream.read().data;
//             pixelData.write(pixel_data);
//             FASTData.write(FAST_data);
//         }
//     }
}

void process_padding(hls::stream <ap_uint<INPUT_BIT> > &pixelData, hls::stream <ap_uint<(WIN_SZ - 1) * PIXEL_BIT> > &initData, hls::stream <ap_uint<INPUT_BIT> > &srcData) {
    ap_uint<(WIN_SZ - 1) * PIXEL_BIT> initTmp = 0;
    ap_uint<HALF_WIN_SZ * PIXEL_BIT + INPUT_BIT * 2> readTmp = 0;
    ap_uint<HALF_WIN_SZ * PIXEL_BIT> endPaddingTmp = 0;
    
    for (ap_uint<HEIGHT_BIT> i = 0; i < height; i++) {
        for (ap_uint<3> j = 0; j < READ_NUM; j++){
#pragma HLS PIPELINE
            readTmp = pixelData.read();
            if (j < READ_NUM - 1){
                for (ap_uint<WIN_SZ_BIT> reflect_ind = 0; reflect_ind < INPUT_PIXEL_NUM; reflect_ind++)
                {
#pragma HLS UNROLL
                    if (j * INPUT_PIXEL_NUM + reflect_ind != 0){
                        ap_uint<WIN_SZ_BIT> padding_ind = HALF_WIN_SZ - j * INPUT_PIXEL_NUM - reflect_ind;
#ifdef BOARDER_101
                        initTmp.range((padding_ind + 1) * PIXEL_BIT - 1, padding_ind * PIXEL_BIT) = 
                            readTmp.range((reflect_ind + 1) * PIXEL_BIT - 1, reflect_ind * PIXEL_BIT);
#else
                        initTmp.range((padding_ind + 1) * PIXEL_BIT - 1, padding_ind * PIXEL_BIT) = 0;
#endif
                    }
                }
                initTmp.range((HALF_WIN_SZ + (j + 1) * INPUT_PIXEL_NUM) * PIXEL_BIT - 1, (HALF_WIN_SZ + j * INPUT_PIXEL_NUM) * PIXEL_BIT) =
                    readTmp.range(INPUT_PIXEL_NUM * PIXEL_BIT - 1, 0);
            }else{
                for (ap_uint<WIN_SZ_BIT> reflect_ind = 0; reflect_ind < HALF_WIN_SZ - (READ_NUM - 1) * INPUT_PIXEL_NUM + 1; reflect_ind++)
                {
#pragma HLS UNROLL
                    ap_uint<WIN_SZ_BIT> padding_ind = HALF_WIN_SZ - j * INPUT_PIXEL_NUM - reflect_ind;
#ifdef BOARDER_101
                    initTmp.range((padding_ind + 1) * PIXEL_BIT - 1, padding_ind * PIXEL_BIT) = 
                       readTmp.range((reflect_ind + 1) * PIXEL_BIT - 1, reflect_ind * PIXEL_BIT);
#else
                    initTmp.range((padding_ind + 1) * PIXEL_BIT - 1, padding_ind * PIXEL_BIT) = 0;
#endif
                }
                
                if (WIN_SZ - 1 > HALF_WIN_SZ + j * INPUT_PIXEL_NUM){
                    initTmp.range((WIN_SZ - 1) * PIXEL_BIT - 1, (HALF_WIN_SZ + j * INPUT_PIXEL_NUM) * PIXEL_BIT) =
                        readTmp.range((HALF_WIN_SZ - j * INPUT_PIXEL_NUM) * PIXEL_BIT - 1, 0);
                }
            }
        }
        initData.write(initTmp);
        
        readTmp = readTmp >> (REMAIN_NUM * PIXEL_BIT);
        
        for (ap_uint<WIDTH_BIT> j = READ_NUM; j < unit_num; j++) {
#pragma HLS PIPELINE
            readTmp.range(INPUT_BIT * 2 - REMAIN_NUM * PIXEL_BIT - 1, INPUT_BIT - REMAIN_NUM * PIXEL_BIT) = pixelData.read();
        
            for (ap_uint<8> pInd = 0; pInd < INPUT_PIXEL_NUM; pInd++)
            {
                ap_uint<WIDTH_BIT> gpInd = j * INPUT_PIXEL_NUM + pInd;
                if (width-HALF_WIN_SZ-1 <= gpInd && gpInd <= width-2)
                {
                    ap_uint<WIN_SZ_BIT> reflect_ind = width - gpInd - 2;
#ifdef BOARDER_101
                    endPaddingTmp.range((reflect_ind + 1) * PIXEL_BIT - 1, reflect_ind * PIXEL_BIT) = 
                       readTmp.range((pInd + 1) * PIXEL_BIT - 1 + (INPUT_PIXEL_NUM - REMAIN_NUM) * PIXEL_BIT, pInd * PIXEL_BIT + (INPUT_PIXEL_NUM - REMAIN_NUM) * PIXEL_BIT);
#else
                    endPaddingTmp.range((reflect_ind + 1) * PIXEL_BIT - 1, reflect_ind * PIXEL_BIT) = 0;
#endif
                }
            }
            if (j == unit_num - 1)
            {
                ap_uint<WIDTH_BIT> rmnPixelNum = width - (unit_num-1) * INPUT_PIXEL_NUM + INPUT_PIXEL_NUM - REMAIN_NUM;
                readTmp.range(rmnPixelNum * PIXEL_BIT + HALF_WIN_SZ * PIXEL_BIT - 1, rmnPixelNum * PIXEL_BIT) = endPaddingTmp;
            }
            srcData.write(readTmp.range(INPUT_BIT - 1, 0));
            
            readTmp = readTmp >> INPUT_BIT;
        }
        for (ap_uint<WIDTH_BIT> j = unit_num; j < unit_num + READ_NUM; j++) {
#pragma HLS PIPELINE
            srcData.write(readTmp.range(INPUT_BIT - 1, 0));
            readTmp = readTmp >> INPUT_BIT;        
        }
    }
}

void process_shift(ap_uint<PIXEL_BIT * MERGE_NUM> image_buf[WIN_SZ][WIDTH_AFTER_MERGE],
                   ap_uint<PIXEL_BIT> win_buf[WIN_SZ][WIN_SZ + PROCESS_NUM - 1],
                   ap_uint<WIN_SZ_BIT> win_ind[WIN_SZ],
                   hls::stream <ap_uint<PROCESS_BIT> > &FASTData,
                   hls::stream <ap_uint<PIXEL_BIT * (WIN_SZ + PROCESS_NUM - 1)> > bufData[WIN_SZ], 
                   hls::stream <ap_uint<PIXEL_BIT * PROCESS_NUM> > &FASTbufData, 
                   hls::stream <ap_uint<WIDTH_BIT + HEIGHT_BIT> > &posData,
                   ap_uint<HEIGHT_BIT> row_ind,
                   ap_uint<WIDTH_BIT> col_ind){
#pragma HLS INLINE
    for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
        for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < PROCESS_NUM / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
            ap_uint<MERGE_NUM * PIXEL_BIT> pixelReadtmp = image_buf[row_index][col_ind * PROCESS_NUM / MERGE_NUM + read_ind + (WIN_SZ - 1)/MERGE_NUM];
            for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                win_buf[row_index][read_ind * MERGE_NUM + pixel_ind + WIN_SZ - 1] = 
                    pixelReadtmp.range((pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
            }
        }
    }

    //compute
    ap_uint<8> FAST_tmp[INPUT_PIXEL_NUM];
#pragma HLS ARRAY_PARTITION variable = FAST_tmp complete dim = 1
    ap_uint<1> FAST_judge = 0;
    ap_uint<INPUT_BIT> FASTIn = FASTData.read();
    for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < INPUT_PIXEL_NUM; read_ind++)
    {
#pragma HLS UNROLL
        FAST_tmp[read_ind] = FASTIn.range((read_ind + 1) * PIXEL_BIT - 1, read_ind * PIXEL_BIT);
    }

    for (ap_uint<8> FAST_ind = 0; FAST_ind < PROCESS_NUM; FAST_ind++){
        FAST_judge = FAST_judge | FAST_tmp[FAST_ind].range(0, 0);
    }

    if (FAST_judge) 
    {
        for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ; i++)
        {
            ap_uint<PIXEL_BIT * (WIN_SZ+PROCESS_NUM-1)> bufData_out;
            for (ap_uint<WIN_SZ_BIT + 5> j = 0; j < WIN_SZ + PROCESS_NUM - 1; j++)
            {
                bufData_out.range((j+1)*PIXEL_BIT-1, j*PIXEL_BIT) = win_buf[win_ind[i]][j];
            }
            bufData[i].write(bufData_out);
        }
        ap_uint<WIDTH_BIT + HEIGHT_BIT> posData_out;
        posData_out.range(WIDTH_BIT-1, 0) = col_ind * PROCESS_NUM;
        posData_out.range(WIDTH_BIT + HEIGHT_BIT - 1, WIDTH_BIT) = row_ind;

        ap_uint<PIXEL_BIT * PROCESS_NUM> FASTbufData_out;
        for (ap_uint<6> i = 0; i < PROCESS_NUM; i++)
            FASTbufData_out.range((i+1)*PIXEL_BIT-1, i*PIXEL_BIT) = FAST_tmp[i];

        posData.write(posData_out);
        FASTbufData.write(FASTbufData_out);
    }

#ifdef DEBUG
for (int i = 0; i < WIN_SZ; i++)
{
for (int j = 0; j < WIN_SZ + PROCESS_NUM - 1; j++)
{
    cout << win_buf[i][j] << " ";
}
cout << endl;
}
cout << "*----------------win_buf------------------" << endl;
#endif

    // slide win_buf
    for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
        for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < WIN_SZ - 1; pixel_ind++) {
#pragma HLS UNROLL
            win_buf[row_index][pixel_ind] = win_buf[row_index][pixel_ind + PROCESS_NUM];
        }
    }
}

void process_win_ind(ap_uint<WIN_SZ_BIT> win_ind[WIN_SZ]){
#pragma HLS INLINE
    ap_uint<WIN_SZ_BIT> win_ind_tmp = win_ind[0];
    for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ - 1; i++)
#pragma HLS UNROLL
            win_ind[i] = win_ind[i + 1];
    win_ind[WIN_SZ - 1] = win_ind_tmp;
}

void process_buf(hls::stream <ap_uint<(WIN_SZ - 1) * PIXEL_BIT> > &pixelInitData,
                 hls::stream <ap_uint<INPUT_BIT> > &pixelSrcData,
				 hls::stream <ap_uint<INPUT_BIT> > &FASTData,
                 hls::stream <ap_uint<PIXEL_BIT * (WIN_SZ + PROCESS_NUM - 1)> > bufData[WIN_SZ], 
                 hls::stream <ap_uint<PIXEL_BIT * PROCESS_NUM> > &FASTbufData, 
                 hls::stream <ap_uint<WIDTH_BIT + HEIGHT_BIT> > &posData){
#pragma HLS INLINE off
    ap_uint<PIXEL_BIT * MERGE_NUM> image_buf[WIN_SZ][WIDTH_AFTER_MERGE];
#pragma HLS RESOURCE variable=image_buf core=RAM_T2P_BRAM
//#pragma HLS BIND_STORAGE variable=image_buf type=ram_t2p impl=bram
#pragma HLS ARRAY_PARTITION variable = image_buf cyclic factor = 2 dim = 2
#pragma HLS ARRAY_PARTITION variable = image_buf block factor = 29 dim = 1

    ap_uint<PIXEL_BIT> win_buf[WIN_SZ][WIN_SZ + PROCESS_NUM - 1];
#pragma HLS ARRAY_PARTITION variable = win_buf complete dim = 0

    ap_uint<WIN_SZ_BIT> win_ind[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable = win_ind complete dim = 1

    ap_uint<11> write_num = 0;
    ap_uint<8> write_pixel_num = 0;

    //initialization
    for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ; i++)
#pragma HLS UNROLL
            win_ind[i] = i;

    for (ap_uint<HEIGHT_BIT> row_ind = HALF_WIN_SZ; row_ind < WIN_SZ - 1; row_ind++) {
        ap_uint<(WIN_SZ - 1) * PIXEL_BIT> pixelInitIn = pixelInitData.read();
        for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
        {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 2
            ap_uint<8> offset = initInd * PIXEL_BIT * MERGE_NUM;
            ap_uint<PIXEL_BIT * MERGE_NUM> pixelSplitTmp = pixelInitIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
            image_buf[row_ind][initInd] = pixelSplitTmp;
        }
        for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < unit_num; read_ind++) {
#pragma HLS PIPELINE
            ap_uint<INPUT_BIT> pixelSrcIn = pixelSrcData.read();
            for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
            {
#pragma HLS UNROLL
                ap_uint<8> offset = srcInd * PIXEL_BIT * MERGE_NUM;
                ap_uint<PIXEL_BIT * MERGE_NUM> pixelSplitTmp = pixelSrcIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
                image_buf[row_ind][srcInd + read_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] = pixelSplitTmp;
            }
        }
    }

#ifdef BOARDER_101
   for (ap_uint<HEIGHT_BIT> row_ind = 1; row_ind < HALF_WIN_SZ; row_ind++)
   {
#pragma HLS UNROLL
       for (ap_uint<WIDTH_BIT> read_ind = 0;
           read_ind < padding_unit_num * INPUT_PIXEL_NUM / MERGE_NUM; read_ind++) {
#pragma HLS DEPENDENCE variable=image_buf inter RAW false
#pragma HLS UNROLL factor = 2
#pragma HLS PIPELINE
           ap_uint<PIXEL_BIT * MERGE_NUM> pixelCopyTmp = image_buf[WIN_SZ-row_ind-1][read_ind];
           image_buf[row_ind][read_ind] = pixelCopyTmp;
       }
   }

#ifdef DEBUG
    for (int i = 0; i < WIN_SZ; i++)
    {
        for (int j = 0; j < ceil((double)(width + WIN_SZ - 1) / MERGE_NUM); j++)
        {
            ap_uint<MERGE_NUM *PIXEL_BIT> cout_tmp = image_buf[i][j];
            for (int k = 0; k < MERGE_NUM; k++)
            {
                cout << cout_tmp.range(7, 0) << " ";
                cout_tmp = cout_tmp >> PIXEL_BIT;
            }
        }
        cout << endl;
    }
    cout << "*----------------image_buf------------------" << endl;
#endif

    Row_Loop:
    for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < 1; row_ind++) {
        if (row_ind < height - HALF_WIN_SZ) {
            ap_uint<(WIN_SZ - 1) * PIXEL_BIT> pixelInitIn = pixelInitData.read();
            for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
            {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 2
                ap_uint<8> offset = initInd * PIXEL_BIT * MERGE_NUM;
                image_buf[win_ind[WIN_SZ - 1]][initInd] = 
                    pixelInitIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
                image_buf[0][initInd] = pixelInitIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
            }
        }else{
            for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
            {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 2
                image_buf[win_ind[WIN_SZ - 1]][initInd] =
               		image_buf[win_ind[WIN_SZ-3-(row_ind-(height-HALF_WIN_SZ))*2]][initInd];
            }
        }

        for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
            for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < (WIN_SZ - 1) / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
                ap_uint<MERGE_NUM * PIXEL_BIT> pixelReadTmp = image_buf[row_index][read_ind];
                for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                    win_buf[row_index][read_ind * MERGE_NUM + pixel_ind] = pixelReadTmp.range(
                            (pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
                }
            }
        }

        for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < unit_num; col_ind++) {
#pragma HLS PIPELINE
            ap_uint<INPUT_BIT> pixelSrcIn = pixelSrcData.read();
            for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
            {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter WAW  false
                ap_uint<8> offset = srcInd * PIXEL_BIT * MERGE_NUM;
                image_buf[win_ind[WIN_SZ - 1]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] =
                    pixelSrcIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
                image_buf[0][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] =
                    pixelSrcIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
            }
            process_shift(image_buf, win_buf, win_ind, FASTData, bufData, FASTbufData, posData, row_ind, col_ind);
        }

        process_win_ind(win_ind);
    }
    //---------------------------------------------------------------

    for (ap_uint<HEIGHT_BIT> row_ind = 1; row_ind < height - HALF_WIN_SZ; row_ind++) {
        ap_uint<(WIN_SZ - 1) * PIXEL_BIT> pixelInitIn = pixelInitData.read();
        for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
        {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 2
            ap_uint<8> offset = initInd * PIXEL_BIT * MERGE_NUM;
            image_buf[win_ind[WIN_SZ - 1]][initInd] = 
                pixelInitIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
        }

        for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
            for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < (WIN_SZ - 1) / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
                ap_uint<MERGE_NUM * PIXEL_BIT> pixelReadTmp = image_buf[row_index][read_ind];
                for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                    win_buf[row_index][read_ind * MERGE_NUM + pixel_ind] = pixelReadTmp.range(
                            (pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
                }
            }
        }

        for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < unit_num; col_ind++) {
#pragma HLS PIPELINE
            ap_uint<INPUT_BIT> pixelSrcIn = pixelSrcData.read();
            for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
            {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter WAW  false
                ap_uint<8> offset = srcInd * PIXEL_BIT * MERGE_NUM;
                image_buf[win_ind[WIN_SZ - 1]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] =
                    pixelSrcIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
            }
            process_shift(image_buf, win_buf, win_ind, FASTData, bufData, FASTbufData, posData, row_ind, col_ind);
        }

        process_win_ind(win_ind);
    }
    //---------------------------------------------------------------

    for (ap_uint<HEIGHT_BIT> row_ind = height - HALF_WIN_SZ; row_ind < height; row_ind++) {
        for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
        {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 2
            image_buf[win_ind[WIN_SZ - 1]][initInd] =
                image_buf[win_ind[WIN_SZ-3-(row_ind-(height-HALF_WIN_SZ))*2]][initInd];
        }

        for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
            for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < (WIN_SZ - 1) / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
                ap_uint<MERGE_NUM * PIXEL_BIT> pixelReadTmp = image_buf[row_index][read_ind];
                for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                    win_buf[row_index][read_ind * MERGE_NUM + pixel_ind] = pixelReadTmp.range(
                            (pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
                }
            }
        }

        for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < unit_num; col_ind++) {
#pragma HLS PIPELINE
            for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
            {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter RAW false
                image_buf[win_ind[WIN_SZ - 1]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] = 
                    image_buf[win_ind[WIN_SZ-3-(row_ind-(height-HALF_WIN_SZ))*2]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM];
            }
            process_shift(image_buf, win_buf, win_ind, FASTData, bufData, FASTbufData, posData, row_ind, col_ind);
        }

        process_win_ind(win_ind);
    }
    //---------------------------------------------------------------
    
#else

    for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < WIDTH_AFTER_MERGE; col_ind++)
#pragma HLS UNROLL factor = 2
#pragma HLS PIPELINE
		for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < HALF_WIN_SZ; row_ind++)
#pragma HLS UNROLL
			image_buf[row_ind][col_ind] = 0;
    for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < height; row_ind++) {
        if (row_ind < height - HALF_WIN_SZ) {
            ap_uint<(WIN_SZ - 1) * PIXEL_BIT> pixelInitIn = pixelInitData.read();
            for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
            {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 2
                ap_uint<8> offset = initInd * PIXEL_BIT * MERGE_NUM;
                image_buf[win_ind[WIN_SZ - 1]][initInd] = 
                    pixelInitIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
            }
        }else{
            for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
            {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 2
                image_buf[win_ind[WIN_SZ - 1]][initInd] = 0;
            }
        }

        for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
            for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < (WIN_SZ - 1) / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
                ap_uint<MERGE_NUM * PIXEL_BIT> pixelReadTmp = image_buf[row_index][read_ind];
                for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                    win_buf[row_index][read_ind * MERGE_NUM + pixel_ind] = pixelReadTmp.range(
                            (pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
                }
            }
        }

#ifdef DEBUG
        for (int i = 0; i < WIN_SZ; i++)
        {
            for (int j = 0; j < ceil((double)(width + WIN_SZ - 1) / MERGE_NUM); j++)
            {
                ap_uint<MERGE_NUM *PIXEL_BIT> cout_tmp = image_buf[i][j];
                for (int k = 0; k < MERGE_NUM; k++)
                {
                    cout << cout_tmp.range(7, 0) << " ";
                    cout_tmp = cout_tmp >> PIXEL_BIT;
                }
            }
            cout << endl;
        }
        cout << "-----------------image_buf------------------" << endl;
#endif

        for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < unit_num; col_ind++) {
#pragma HLS PIPELINE
            if (row_ind < height - HALF_WIN_SZ) {
                ap_uint<INPUT_BIT> pixelSrcIn = pixelSrcData.read();
                for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
                {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter WAW  false
                    ap_uint<8> offset = srcInd * PIXEL_BIT * MERGE_NUM;
                    image_buf[win_ind[WIN_SZ - 1]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] =
                        pixelSrcIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
                }
            }
            else
            {
                for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
                {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter RAW false
                	image_buf[win_ind[WIN_SZ - 1]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] = 0;
                }
            }

            for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
                for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < PROCESS_NUM / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
                	ap_uint<MERGE_NUM * PIXEL_BIT> pixelReadtmp = image_buf[row_index][col_ind * PROCESS_NUM / MERGE_NUM + read_ind + (WIN_SZ - 1)/MERGE_NUM];
                    for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                        win_buf[row_index][read_ind * MERGE_NUM + pixel_ind + WIN_SZ - 1] = 
                            pixelReadtmp.range((pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
                    }
                }
            }

            //compute
            ap_uint<8> FAST_tmp[INPUT_PIXEL_NUM];
#pragma HLS ARRAY_PARTITION variable = FAST_tmp complete dim = 1
            ap_uint<1> FAST_judge = 0;
            ap_uint<INPUT_BIT> FASTIn = FASTData.read();
            for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < INPUT_PIXEL_NUM; read_ind++)
            {
#pragma HLS UNROLL
				FAST_tmp[read_ind] = FASTIn.range((read_ind + 1) * PIXEL_BIT - 1, read_ind * PIXEL_BIT);
            }

            for (ap_uint<8> FAST_ind = 0; FAST_ind < PROCESS_NUM; FAST_ind++){
                FAST_judge = FAST_judge | FAST_tmp[FAST_ind].range(0, 0);
            }

            if (FAST_judge) 
            {
                for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ; i++)
                {
                    ap_uint<PIXEL_BIT * (WIN_SZ+PROCESS_NUM-1)> bufData_out;
                    for (ap_uint<WIN_SZ_BIT + 5> j = 0; j < WIN_SZ + PROCESS_NUM - 1; j++)
                    {
                        bufData_out.range((j+1)*PIXEL_BIT-1, j*PIXEL_BIT) = win_buf[win_ind[i]][j];
                    }
                    bufData[i].write(bufData_out);
                }
                ap_uint<WIDTH_BIT + HEIGHT_BIT> posData_out;
                posData_out.range(WIDTH_BIT-1, 0) = col_ind * PROCESS_NUM;
                posData_out.range(WIDTH_BIT + HEIGHT_BIT - 1, WIDTH_BIT) = row_ind;

                ap_uint<PIXEL_BIT * PROCESS_NUM> FASTbufData_out;
                for (ap_uint<6> i = 0; i < PROCESS_NUM; i++)
                    FASTbufData_out.range((i+1)*PIXEL_BIT-1, i*PIXEL_BIT) = FAST_tmp[i];

                posData.write(posData_out);
                FASTbufData.write(FASTbufData_out);
            }

#ifdef DEBUG
    for (int i = 0; i < WIN_SZ; i++)
    {
        for (int j = 0; j < WIN_SZ + PROCESS_NUM - 1; j++)
        {
            cout << win_buf[i][j] << " ";
        }
        cout << endl;
    }
    cout << "*----------------win_buf------------------" << endl;
#endif

            // slide win_buf
            for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
                for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < WIN_SZ - 1; pixel_ind++) {
#pragma HLS UNROLL
                    win_buf[row_index][pixel_ind] = win_buf[row_index][pixel_ind + PROCESS_NUM];
                }
            }
        }

        ap_uint<WIN_SZ_BIT> win_ind_tmp = win_ind[0];
        for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ - 1; i++)
#pragma HLS UNROLL
                win_ind[i] = win_ind[i + 1];
        win_ind[WIN_SZ - 1] = win_ind_tmp;
    }
#endif

    ap_uint<WIDTH_BIT + HEIGHT_BIT> posData_out;
    posData_out.range(WIDTH_BIT-1, 0) = 2047;
    posData_out.range(WIDTH_BIT + HEIGHT_BIT - 1, WIDTH_BIT) = 511;

    ap_uint<PIXEL_BIT * PROCESS_NUM> FASTbufData_out = 1;
    for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ; i++)
        bufData[i].write(0);
    posData.write(posData_out);
    FASTbufData.write(FASTbufData_out);
}

void process_RS_BRIEF(hls::stream <ap_uint<PIXEL_BIT * (WIN_SZ + PROCESS_NUM - 1)> > bufData[WIN_SZ], hls::stream <ap_uint<PIXEL_BIT * PROCESS_NUM> > &FASTbufData, hls::stream <ap_uint<WIDTH_BIT + HEIGHT_BIT> > &posData, hls::stream <ap_axiu<512, 1, 1, 1> > &outStream){
//#pragma HLS INLINE
#ifdef DEBUG
    for (int i = 0; i< WIN_SZ; i++)
    {
        for (int j = 0; j < WIN_SZ; j++)
            cout << (win_buf[win_ind[i]][j] & mask[i][j]) << " ";
        cout << endl;
    }
#endif
    ap_uint<HEIGHT_BIT> row_ind;
    ap_uint<WIDTH_BIT> col_ind;
    do
    {
        ap_uint<PIXEL_BIT> win_buf[WIN_SZ][WIN_SZ + PROCESS_NUM - 1];
#pragma HLS ARRAY_PARTITION variable = win_buf complete dim = 0
        for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ; i++)
        {
#pragma HLS UNROLL
            ap_uint<PIXEL_BIT * (WIN_SZ+PROCESS_NUM-1)> bufData_in = bufData[i].read();
            for (ap_uint<WIN_SZ_BIT + 5> j = 0; j < WIN_SZ + PROCESS_NUM - 1; j++)
            {
#pragma HLS UNROLL
                win_buf[i][j] = bufData_in.range((j+1)*PIXEL_BIT-1, j*PIXEL_BIT);
            }
        }

        ap_uint<WIDTH_BIT + HEIGHT_BIT> posData_in = posData.read();
        col_ind = posData_in.range(WIDTH_BIT-1, 0);
        row_ind = posData_in.range(WIDTH_BIT + HEIGHT_BIT - 1, WIDTH_BIT);

        ap_uint<PIXEL_BIT * PROCESS_NUM> FASTbufData_in = FASTbufData.read();
        ap_uint<PIXEL_BIT> FAST_buf[PROCESS_NUM];
#pragma HLS ARRAY_PARTITION variable = FAST_buf complete dim = 0
        for (ap_uint<6> i = 0; i < PROCESS_NUM; i++)
            FAST_buf[i] = FASTbufData_in.range((i+1)*PIXEL_BIT-1, i*PIXEL_BIT);

        for (ap_uint<8> prc_ind = 0; prc_ind < PROCESS_NUM; prc_ind++)
#pragma HLS PIPELINE
            if (FAST_buf[prc_ind] & 1) {
                ap_uint<PIXEL_BIT> win_buf_tmp[WIN_SZ][WIN_SZ];
#pragma HLS ARRAY_PARTITION variable = win_buf_tmp complete dim = 0
                for (ap_uint<WIN_SZ_BIT> win_row = 0; win_row < WIN_SZ; win_row++)
                {
                    for (ap_uint<WIN_SZ_BIT> win_col = 0; win_col < WIN_SZ; win_col++)
                        win_buf_tmp[win_row][win_col] = win_buf[win_row][win_col+prc_ind];
                }

#ifdef DEBUG
                for(int i = 0; i < WIN_SZ; i++){
                    for(int j = 0; j < WIN_SZ; j++)
                        cout << win_buf_tmp[i][j] << " ";
                    cout << endl;
                }
                cout << "win_buf_tmp------------------------------------\n";
#endif

                ap_int<24> m_10 = 0;
                ap_int<24> m_01 = 0;
                for (ap_int<8> i = 0; i < WIN_SZ; i++){
                    ap_int<24> vres = 0;
                    for (ap_uint<6> im = 0; im < WIN_SZ; im++)
                        vres += win_buf_tmp[i][im] & mask[i][im];
                    vres *= (i - HALF_WIN_SZ);
                    m_01 += vres;

                    ap_int<24> ures = 0;
                    for (ap_uint<6> im = 0; im < WIN_SZ; im++)
                        ures += win_buf_tmp[im][i] & mask[im][i];
                    ures *= (i - HALF_WIN_SZ);
                    m_10 += ures;
                }
    //         for (ap_int<8> i = 0; i < 25; i++) {
    // #pragma HLS UNROLL
    //             ap_uint<8> vres0[32];
    // #pragma HLS ARRAY_PARTITION variable = vres0 complete dim = 1
    //             ap_uint<9> vres1[16];
    // #pragma HLS ARRAY_PARTITION variable = vres1 complete dim = 1
    //             ap_uint<10> vres2[8];
    // #pragma HLS ARRAY_PARTITION variable = vres2 complete dim = 1
    //             ap_uint<11> vres3[4];
    // #pragma HLS ARRAY_PARTITION variable = vres3 complete dim = 1
    //             ap_uint<12> vres4[2];
    // #pragma HLS ARRAY_PARTITION variable = vres4 complete dim = 1
    //             ap_uint<13> vres5;
    //             ap_int<19> vres;
    //             for (ap_uint<6> im = 0; im < 32; im++) {
    // #pragma HLS UNROLL
    //                 if (im < 25)
    //                     vres0[im] = win_buf_tmp[win_ind[i]][im] & mask[i][im];
    //                 else
    //                     vres0[im] = 0;
    //             }
    //             for (ap_uint<6> i0 = 0; i0 < 16; i0++) {
    // #pragma HLS UNROLL
    //                 vres1[i0] = vres0[i0 << 1] + vres0[(i0 << 1) + 1];
    //             }
    //             for (ap_uint<6> i1 = 0; i1 < 8; i1++) {
    // #pragma HLS UNROLL
    //                 vres2[i1] = vres1[i1 << 1] + vres1[(i1 << 1) + 1];
    //             }
    //             for (ap_uint<6> i3 = 0; i3 < 4; i3++) {
    // #pragma HLS UNROLL
    //                 vres3[i3] = vres2[i3 << 1] + vres2[(i3 << 1) + 1];
    //             }
    //             for (ap_uint<6> i4 = 0; i4 < 2; i4++) {
    // #pragma HLS UNROLL
    //                 vres4[i4] = vres3[i4 << 1] + vres3[(i4 << 1) + 1];
    //             }
    //             vres5 = vres4[0] + vres4[1];
    //             vres = vres5;
    //             vres = vres * (i - 12);
    //             m_01 += vres;

    //             unsigned char ures0[32];
    // #pragma HLS ARRAY_PARTITION variable = ures0 complete dim = 1
    //             ap_uint<9> ures1[16];
    // #pragma HLS ARRAY_PARTITION variable = ures1 complete dim = 1
    //             ap_uint<10> ures2[8];
    // #pragma HLS ARRAY_PARTITION variable = ures2 complete dim = 1
    //             ap_uint<11> ures3[4];
    // #pragma HLS ARRAY_PARTITION variable = ures3 complete dim = 1
    //             ap_uint<12> ures4[2];
    // #pragma HLS ARRAY_PARTITION variable = ures4 complete dim = 1
    //             ap_uint<13> ures5;
    //             ap_int<19> ures;
    //             for (ap_uint<6> im = 0; im < 32; im++) {
    // #pragma HLS UNROLL
    //                 if (im < 25)
    //                     ures0[im] = win_buf_tmp[win_ind[im]][i] & mask[im][i];
    //                 else
    //                     ures0[im] = 0;
    //             }
    //         for (ap_uint<6> i0 = 0; i0 < 16; i0++) {
    // #pragma HLS UNROLL
    //                 ures1[i0] = ures0[i0 << 1] + ures0[(i0 << 1) + 1];
    //             }
    //             for (ap_uint<6> i1 = 0; i1 < 8; i1++) {
    // #pragma HLS UNROLL
    //                 ures2[i1] = ures1[i1 << 1] + ures1[(i1 << 1) + 1];
    //             }
    //                 for (ap_uint<6> i3 = 0; i3 < 4; i3++) {
    // #pragma HLS UNROLL
    //                 ures3[i3] = ures2[i3 << 1] + ures2[(i3 << 1) + 1];
    //             }
    //         for (ap_uint<6> i4 = 0; i4 < 2; i4++) {
    // #pragma HLS UNROLL
    //                 ures4[i4] = ures3[i4 << 1] + ures3[(i4 << 1) + 1];
    //             }
    //             ures5 = ures4[0] + ures4[1];
    //             ures = ures5;
    //             ures = ures * (i - 12);
    //             m_10 += ures;
    //         }
                ap_uint<9> angle;
                ap_uint<256> desc;
                ap_fixed<24, 23> m_10_f, m_01_f;
                m_10_f = m_10;
                m_01_f = m_01;
                ap_fixed<15, 4> angle_out = hls::atan2(m_01_f, m_10_f);
                ap_fixed<15, 4> PI = 3.14159265358;
                ap_fixed<15, 4> PI_32 = 0.1963495408;
                if (angle_out < 0)
                    angle_out = angle_out + PI * 2;
                angle.range(8, 0) = angle_out.range(14, 6);
                ap_fixed<19, 15> div = angle_out / PI_32;
                ap_uint<8> bias;

                if (div.range(3, 3) == 1)
                    bias = div.range(8, 4) + 1;
                else
                    bias = div.range(8, 4);
                compute_loop:
                for (ap_uint<9> i = 0; i < 256; i++) {
                    ap_uint<8> t0, t1;
#pragma HLS UNROLL
                    t0 = win_buf_tmp[bit_pattern_29_[i][0]+HALF_WIN_SZ][bit_pattern_29_[i][1]+HALF_WIN_SZ];
                    t1 = win_buf_tmp[bit_pattern_29_[i][2]+HALF_WIN_SZ][bit_pattern_29_[i][3]+HALF_WIN_SZ];
                    desc.range(i, i) = t0 < t1;
                }

                ap_uint<512> db_desc, shift_db_desc;
                db_desc.range(255, 0) = desc;
                db_desc.range(511, 256) = desc;
                shift_db_desc = db_desc >> (bias << 3);
                desc = shift_db_desc.range(255, 0);

                ap_uint<512> write_tmp;
                write_tmp.range(6, 0) = FAST_buf[prc_ind] >> 1;
                write_tmp.range(15, 7) = angle;
                write_tmp.range(16+HEIGHT_BIT-1, 16) = row_ind;
                write_tmp.range(16+HEIGHT_BIT+WIDTH_BIT-1, 16+HEIGHT_BIT) = col_ind + prc_ind;
                write_tmp.range(16+HEIGHT_BIT+WIDTH_BIT+256-1, 16+HEIGHT_BIT+WIDTH_BIT) = desc;
                write_tmp.range(511, 16+HEIGHT_BIT+WIDTH_BIT+256) = 0;
                ap_axiu<512, 1, 1, 1> outVal;
                outVal.data = write_tmp;
                outVal.keep = 0xffffffffffffffff;
                if (row_ind!=511 && col_ind!=2047){
                    outVal.last = 0;
                    if (col_ind + prc_ind < width)
                        outStream.write(outVal);
                }else{
                    outVal.last = 1;
                    outStream.write(outVal);
                }
            }
        // cout << row_ind << " " << col_ind << endl;
    }
    while (row_ind!=511 && col_ind!=2047);
}
