#include "/home/cag/work/pynq/ORB_FPGA_jianming/test_stream_in.h"


void processFAST( ap_uint<PIXEL_BIT> winReg[WIN_SZ][WIN_SZ + NPC -1], hls::stream<ap_axiu<16,1,1,1>  > &outStream, ap_uint<ROW_IMAGE_BIT> rowImage, ap_uint<COL_IMAGE_BIT> colImage, int threshold){

	// process FAST
	// here the 9 should be the length of the input + 1;
	ap_int<9> diff[16]={0};
#pragma HLS ARRAY_PARTITION variable=diff complete dim=1

	diff[0]  = winReg[3][3] - winReg[0][3];
	diff[1]  = winReg[3][3] - winReg[0][4];
	diff[2]  = winReg[3][3] - winReg[1][5];
	diff[3]  = winReg[3][3] - winReg[2][6];
	diff[4]  = winReg[3][3] - winReg[3][6];
	diff[5]  = winReg[3][3] - winReg[4][6];
	diff[6]  = winReg[3][3] - winReg[5][5];
	diff[7]  = winReg[3][3] - winReg[6][4];
	diff[8]  = winReg[3][3] - winReg[6][3];
	diff[9]  = winReg[3][3] - winReg[6][2];
	diff[10] = winReg[3][3] - winReg[5][1];
	diff[11] = winReg[3][3] - winReg[4][0];
	diff[12] = winReg[3][3] - winReg[3][0];
	diff[13] = winReg[3][3] - winReg[2][0];
	diff[14] = winReg[3][3] - winReg[1][1];
	diff[15] = winReg[3][3] - winReg[0][2];

	// feature point judgement the following two loop should be calculated simultaneously
	ap_uint<1> posJudge[24]={0};
#pragma HLS ARRAY_PARTITION variable=posJudge complete dim=1
	ap_uint<1> negJudge[24]={0};
#pragma HLS ARRAY_PARTITION variable=negJudge complete dim=1

	for (int i = 0; i < 16; i++){
#pragma HLS unroll
		posJudge[i] = (diff[i] >  threshold) ? 1:0;
		negJudge[i] = (diff[i] < -threshold) ? 1:0;
	}

	for (int i = 0; i < 8; i++)
	{
#pragma HLS unroll
		posJudge[i+16] = (diff[i] >  threshold) ? 1:0;
		negJudge[i+16] = (diff[i] < -threshold) ? 1:0;
	}
	// judge whether the center point is feature point (fp).

	ap_uint<9>  fpJudge[16];
	for (int i=0; i< 16; i++){
#pragma HLS unroll
		fpJudge[i] = (posJudge[i]&posJudge[i+1]&posJudge[i+2]&posJudge[i+3]&posJudge[i+4]&posJudge[i+5]&posJudge[i+6]&posJudge[i+7]&posJudge[i+8])|(negJudge[i]&negJudge[i+1]&negJudge[i+2]&negJudge[i+3]&negJudge[i+4]&negJudge[i+5]&negJudge[i+6]&negJudge[i+7]&negJudge[i+8]);
	}

	ap_uint<1> fpValid;
	// read new line && update line indicator/
	fpValid = (fpJudge[0]|fpJudge[1]|fpJudge[2]|fpJudge[3]|fpJudge[4]|fpJudge[5]|fpJudge[6]|fpJudge[7]|fpJudge[8]|fpJudge[9]|fpJudge[10]|fpJudge[11]|fpJudge[12]|fpJudge[13]|fpJudge[14]|fpJudge[15]);

	// output value
	ap_uint<16> fpOut;
	fpOut.range(15,9) = 0;
	fpOut[8] = fpValid;
	fpOut.range(7,0) = winReg[3][3];

	ap_axiu<16,1,1,1> fp_Out;
	fp_Out.data = fpOut;
	fp_Out.keep = 0x3;
	if(rowImage == HEIGHT-1 && colImage == WIDTH -1) fp_Out.last = 1;
	else fp_Out.last = 0;
	outStream.write(fp_Out);
}


//template<int NMS, int SRC_T, int HEIGHT, int WIDTH, int WIN_SZ, int WIN_SZSQ, int NPC>
void myFastScore(hls::stream<ap_axiu<8,1,1,1> > &srcStream, hls::stream<ap_axiu<16,1,1,1>  > &outStream, int threshold)
{
	int col_loop_var = NPC;

	// row index initialization
	ap_uint<WIN_SZ_IDX_BIT> rowIdx[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable=rowIdx complete dim=1

	// initialize the line_buffer
	ap_uint<PIXEL_BIT> lineBuffer[WIN_SZ][WIDTH] = {0};
#pragma HLS ARRAY_PARTITION variable=lineBuffer complete dim=1

	for (int row =  WIN_SZ >> 1; row < WIN_SZ - 1; row++){
		for(int col = 0; col < WIDTH; col++){
			lineBuffer[row][col] = (srcStream.read()).data;
		}
	}

	// process all HEIGHT of input images
	ROW_IMAGE:for (ap_uint<ROW_IMAGE_BIT> rowImage = 0; rowImage < HEIGHT; rowImage++){
//#pragma HLS PIPELINE
		// set up line buffer pointer
		if(rowImage == 0){
			for (int i = 0; i < WIN_SZ; i++ ){
#pragma HLS UNROLL
				rowIdx[i] = i;
			}
		}
		else
		{
			ap_uint<WIN_SZ_IDX_BIT> temp = rowIdx[0];
			for (int i = 0; i < WIN_SZ-1; i++ ){
#pragma HLS UNROLL
				rowIdx[i] = rowIdx[i+1];
			}
			rowIdx[WIN_SZ-1] = temp;
		}

		// initialize WIN_REG in the corresponding row
		ap_uint<PIXEL_BIT> winReg[WIN_SZ][WIN_SZ + NPC -1];
#pragma HLS ARRAY_PARTITION variable=winReg complete dim=1
		for (int col = WIN_SZ >> 1; col < WIN_SZ; col++){
#pragma HLS PIPELINE
			for (int row = 0; row < WIN_SZ-1; row++){
				winReg[row][col] = lineBuffer[rowIdx[row]][col];
			}
		}

		// read new data into linebuffer & update the last row of WIN_REG
		if(rowImage + (WIN_SZ>>1) < HEIGHT ){
			for (int col =(WIN_SZ>>1); col < WIN_SZ; col++){
#pragma HLS PIPELINE
				ap_uint<PIXEL_BIT> data_temp = (srcStream.read()).data;
				lineBuffer[rowIdx[WIN_SZ-1]][col] = data_temp;
				winReg[WIN_SZ-1][col] = data_temp;
			}
		}
		else{
			for (int col =(WIN_SZ>>1); col < WIN_SZ; col++){
#pragma HLS PIPELINE
				lineBuffer[rowIdx[WIN_SZ-1]][col] = 0;
				winReg[WIN_SZ-1][col] = 0;
			}
		}


		COL_IMAGE:for (ap_uint<COL_IMAGE_BIT> colImage = 0; colImage < WIDTH; colImage++){

			processFAST( winReg,  outStream, rowImage, colImage,  threshold);

/*
			// process FAST
			// here the 9 should be the length of the input + 1;
			// process FAST
			// here the 9 should be the length of the input + 1;
			ap_int<9> diff[16]={0};
#pragma HLS ARRAY_PARTITION variable=diff complete dim=1

			diff[0]  = winReg[3][3] - winReg[0][3];
			diff[1]  = winReg[3][3] - winReg[0][4];
			diff[2]  = winReg[3][3] - winReg[1][5];
			diff[3]  = winReg[3][3] - winReg[2][6];
			diff[4]  = winReg[3][3] - winReg[3][6];
			diff[5]  = winReg[3][3] - winReg[4][6];
			diff[6]  = winReg[3][3] - winReg[5][5];
			diff[7]  = winReg[3][3] - winReg[6][4];
			diff[8]  = winReg[3][3] - winReg[6][3];
			diff[9]  = winReg[3][3] - winReg[6][2];
			diff[10] = winReg[3][3] - winReg[5][1];
			diff[11] = winReg[3][3] - winReg[4][0];
			diff[12] = winReg[3][3] - winReg[3][0];
			diff[13] = winReg[3][3] - winReg[2][0];
			diff[14] = winReg[3][3] - winReg[1][1];
			diff[15] = winReg[3][3] - winReg[0][2];

			// feature point judgement the following two loop should be calculated simultaneously
			ap_uint<1> posJudge[24]={0};
#pragma HLS ARRAY_PARTITION variable=posJudge complete dim=1
			ap_uint<1> negJudge[24]={0};
#pragma HLS ARRAY_PARTITION variable=negJudge complete dim=1

			for (int i = 0; i < 16; i++){
#pragma HLS unroll
				posJudge[i] = (diff[i] >  threshold) ? 1:0;
				negJudge[i] = (diff[i] < -threshold) ? 1:0;
			}

			for (int i = 0; i < 8; i++)
			{
#pragma HLS unroll
				posJudge[i+16] = (diff[i] >  threshold) ? 1:0;
				negJudge[i+16] = (diff[i] < -threshold) ? 1:0;
			}
			// judge whether the center point is feature point (fp).

			ap_uint<9>  fpJudge[16];
			for (int i=0; i< 16; i++){
#pragma HLS unroll
				fpJudge[i] = (posJudge[i]&posJudge[i+1]&posJudge[i+2]&posJudge[i+3]&posJudge[i+4]&posJudge[i+5]&posJudge[i+6]&posJudge[i+7])|(negJudge[i]&negJudge[i+1]&negJudge[i+2]&negJudge[i+3]&negJudge[i+4]&negJudge[i+5]&negJudge[i+6]&negJudge[i+7]);
			}

			ap_uint<1> fpValid;
			// read new line && update line indicator/
			fpValid = (fpJudge[0]|fpJudge[1]|fpJudge[2]|fpJudge[3]|fpJudge[4]|fpJudge[5]|fpJudge[6]|fpJudge[7]);

			// output value
			ap_uint<16> fpOut;
			fpOut.range(15,9) = 0;
			fpOut[8] = fpValid;
			fpOut.range(7,0) = winReg[3][3];

			ap_axiu<16,1,1,1> fp_Out;
			fp_Out.data = fpOut;
			fp_Out.keep = 0x3;
			if(rowImage == HEIGHT-1 && colImage == WIDTH -1) fp_Out.last = 1;
			else fp_Out.last = 0;
			outStream.write(fp_Out);
*/
			// update line buffer & winReg
			if (colImage + (WIN_SZ>>1) + 1 < WIDTH){
				ap_uint<PIXEL_BIT> newPixelIn;

				if(rowImage + (WIN_SZ>>1) < HEIGHT ){
					newPixelIn = (srcStream.read()).data;
				}
				else{
					newPixelIn = 0;
				}

				lineBuffer[rowIdx[WIN_SZ-1]][colImage + (WIN_SZ>>1) + 1] = newPixelIn;
				winReg[WIN_SZ-1][WIN_SZ-1] = newPixelIn;
				for (int col =0; col < WIN_SZ - 1; col++){
#pragma HLS pipeline
					for (int row =0; row < WIN_SZ; row++){
#pragma HLS unroll
						winReg[row][col] = winReg[row][col+1];
					}
				}

				for (int row_win = 0; row_win < WIN_SZ - 1; row_win++){
#pragma HLS unroll
					winReg[row_win][WIN_SZ-1] = lineBuffer[rowIdx[row_win]][colImage + (WIN_SZ>>1) +1];
				}
			}

			else if(colImage < WIDTH)
			{

				for (int col =0; col < WIN_SZ - 1; col++){
#pragma HLS pipeline
					for (int row =0; row < WIN_SZ; row++){
#pragma HLS unroll
						winReg[row][col] = winReg[row][col+1];
					}
				}

				for (int row_win = 0; row_win < WIN_SZ; row_win++){
#pragma HLS unroll
					winReg[row_win][WIN_SZ-1] = 0;
				}
			}
		}


	}
}


void test_stream_in(hls::stream<ap_axiu<8,1,1,1> > &srcStream, hls::stream<ap_axiu<16,1,1,1> > &outStream){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=srcStream
#pragma HLS INTERFACE axis register both port=outStream
//	ap_uint<8> internal_reg;
//	internal_reg = (srcStream.read()).data;
	// feature point extraction & score generation
//	hls::stream<ap_uint<9>> outFastScoreStream;
	// FAST_SCORE

	myFastScore(srcStream, outStream, 40); // @suppress("Function cannot be resolved")
	//	myFAST<> (hls::stream<ap_axiu<8,1,1,1>>);
	// feature points selection

	// description generation
}
