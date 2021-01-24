#include "FAST_extractor.h"
void processFAST( ap_uint<PIXEL_BIT> winReg[WIN_SZ][WIN_SZ + NPC -1], hls::stream<ap_axiu<32,1,1,1> > &outStream, ap_uint<ROW_IMAGE_BIT> rowImage, ap_uint<COL_IMAGE_BIT> colImage, int threshold){
	//gaussian
#define winReg_center(x,y) winReg[x+1][y+1]
    ap_uint<18> psum[6];
#pragma HLS ARRAY_PARTITION variable=psum complete dim=1
    psum[0] = ((winReg_center(0,2) + winReg_center(0,3)) + (winReg_center(0,4) + winReg_center(6,2))) + ((winReg_center(6,3) + winReg_center(6,4)) + (winReg_center(2,6) + winReg_center(3,6))) + ((winReg_center(4,6) + winReg_center(2,0)) + (winReg_center(3,0) + winReg_center(4,0)));
    psum[1] = (winReg_center(1,1) + winReg_center(1,5)) + (winReg_center(5,1) + winReg_center(5,5));
    psum[2] = ((winReg_center(1,2) + winReg_center(1,4)) + (winReg_center(2,1) + winReg_center(4,1))) + ((winReg_center(2,5) + winReg_center(4,5)) + (winReg_center(5,2) + winReg_center(5,4)));
    psum[3] = (winReg_center(1,3) + winReg_center(3,1)) + (winReg_center(5,3) + winReg_center(3,5));
    psum[4] = (winReg_center(2,2) + winReg_center(4,2)) + (winReg_center(2,4) + winReg_center(4,4));
    psum[5] = (winReg_center(2,3) + winReg_center(3,2)) + (winReg_center(4,3) + winReg_center(3,4));

    ap_uint<18> S_psum[7];
#pragma HLS ARRAY_PARTITION variable=psum complete dim=1
    S_psum[0] = psum[0];
    S_psum[1] = psum[1] <<1 ;
    S_psum[2] = psum[2] + (psum[2] << 2);
    S_psum[3] = psum[3] + (psum[3] << 1) +( psum[3] <<2 );
    S_psum[4] = psum[4]  + (psum[4] <<2 ) + (psum[4] << 3);
    S_psum[5] = psum[5]  +( psum[5] << 4);
    S_psum[6] =winReg_center(3,3)  + (winReg_center(3,3) << 1) + (winReg_center(3,3) << 2) + (winReg_center(3,3) << 4);

    ap_uint<18> simsim = ((S_psum[0] + S_psum[1]) + (S_psum[2] + S_psum[3])) + ((S_psum[4] + S_psum[5]) + S_psum[6]);
    ap_uint<8> gaus = (simsim >> 8).range(7,0);
	// process FAST
	// here the 9 should be the length of the input + 1;
	ap_int<9> diff[9][16];
#pragma HLS ARRAY_PARTITION variable=diff complete dim=1
#pragma HLS ARRAY_PARTITION variable=diff complete dim=2
	for (int i=0; i<3; i++){
#pragma HLS UNROLL
		for (int j=0; j<3; j++){
#pragma HLS UNROLL
			int ind = i*3+j;
			diff[ind][0]  = winReg[i+3][j+3] - winReg[i+0][j+3];
			diff[ind][1]  = winReg[i+3][j+3] - winReg[i+0][j+4];
			diff[ind][2]  = winReg[i+3][j+3] - winReg[i+1][j+5];
			diff[ind][3]  = winReg[i+3][j+3] - winReg[i+2][j+6];
			diff[ind][4]  = winReg[i+3][j+3] - winReg[i+3][j+6];
			diff[ind][5]  = winReg[i+3][j+3] - winReg[i+4][j+6];
			diff[ind][6]  = winReg[i+3][j+3] - winReg[i+5][j+5];
			diff[ind][7]  = winReg[i+3][j+3] - winReg[i+6][j+4];
			diff[ind][8]  = winReg[i+3][j+3] - winReg[i+6][j+3];
			diff[ind][9]  = winReg[i+3][j+3] - winReg[i+6][j+2];
			diff[ind][10] = winReg[i+3][j+3] - winReg[i+5][j+1];
			diff[ind][11] = winReg[i+3][j+3] - winReg[i+4][j+0];
			diff[ind][12] = winReg[i+3][j+3] - winReg[i+3][j+0];
			diff[ind][13] = winReg[i+3][j+3] - winReg[i+2][j+0];
			diff[ind][14] = winReg[i+3][j+3] - winReg[i+1][j+1];
			diff[ind][15] = winReg[i+3][j+3] - winReg[i+0][j+2];
		}
	}


	// feature point judgement the following two loop should be calculated simultaneously
	ap_uint<1> posJudge[9][24];
#pragma HLS ARRAY_PARTITION variable=posJudge complete dim=1
#pragma HLS ARRAY_PARTITION variable=posJudge complete dim=2
	ap_uint<1> negJudge[9][24];
#pragma HLS ARRAY_PARTITION variable=negJudge complete dim=1
#pragma HLS ARRAY_PARTITION variable=negJudge complete dim=2
	ap_uint<12> FAST_score[9];
#pragma HLS ARRAY_PARTITION variable=FAST_score complete dim=1

	for (int ind=0; ind<9; ind++){
#pragma HLS unroll
		for (int i = 0; i < 16; i++){
#pragma HLS unroll
			posJudge[ind][i] = (diff[ind][i] >  threshold) ? 1:0;
			negJudge[ind][i] = (diff[ind][i] < -threshold) ? 1:0;
		}
	}

	for (int ind=0; ind<9; ind++){
#pragma HLS unroll
		for (int i = 0; i < 8; i++)
		{
#pragma HLS unroll
			posJudge[ind][i+16] = (diff[ind][i] >  threshold) ? 1:0;
			negJudge[ind][i+16] = (diff[ind][i] < -threshold) ? 1:0;
		}
	}
#define abs_diff(ind,i) hls::fabs(diff[ind][i])
	for (int ind = 0; ind<9; ind++){
#pragma HLS PIPELINE
		FAST_score[ind] = abs_diff(ind, 0) + abs_diff(ind, 1) + abs_diff(ind, 2) + abs_diff(ind, 3) + abs_diff(ind, 4) + abs_diff(ind, 5) + abs_diff(ind, 6) + abs_diff(ind, 7) + abs_diff(ind, 8) + abs_diff(ind, 9) + abs_diff(ind, 10) + abs_diff(ind, 11) + abs_diff(ind, 12) + abs_diff(ind, 13) + abs_diff(ind, 14) + abs_diff(ind, 15);
	}

	// judge whether the center point is feature point (fp).

	ap_uint<1>  fpJudge[9][16];
#pragma HLS ARRAY_PARTITION variable=fpJudge complete dim=1
#pragma HLS ARRAY_PARTITION variable=fpJudge complete dim=2

	for (int ind=0; ind<9; ind++){
#pragma HLS unroll
		for (int i=0; i< 16; i++){
#pragma HLS unroll
			fpJudge[ind][i] = (posJudge[ind][i]&posJudge[ind][i+1]&posJudge[ind][i+2]&posJudge[ind][i+3]&posJudge[ind][i+4]&posJudge[ind][i+5]&posJudge[ind][i+6]&posJudge[ind][i+7]&posJudge[ind][i+8])|(negJudge[ind][i]&negJudge[ind][i+1]&negJudge[ind][i+2]&negJudge[ind][i+3]&negJudge[ind][i+4]&negJudge[ind][i+5]&negJudge[ind][i+6]&negJudge[ind][i+7]&negJudge[ind][i+8]);
		}
	}

	ap_uint<1> fpValid[9];
#pragma HLS ARRAY_PARTITION variable=fpValid complete dim=1
	for (int ind=0; ind<9; ind++){
#pragma HLS unroll
		fpValid[ind] = (fpJudge[ind][0]|fpJudge[ind][1]|fpJudge[ind][2]|fpJudge[ind][3]|fpJudge[ind][4]|fpJudge[ind][5]|fpJudge[ind][6]|fpJudge[ind][7]|fpJudge[ind][8]|fpJudge[ind][9]|fpJudge[ind][10]|fpJudge[ind][11]|fpJudge[ind][12]|fpJudge[ind][13]|fpJudge[ind][14]|fpJudge[ind][15]);
	}

	for (int ind=0; ind<9; ind++){
#pragma HLS unroll
		if (fpValid[ind] == 0)
			FAST_score[ind] = 0;
	}

	//NMS
	ap_uint<1> scoreJudge[8];
#pragma HLS ARRAY_PARTITION variable=scoreJudge complete dim=1
	for (int i=0; i<4; i++){
#pragma HLS unroll
		scoreJudge[i] = (FAST_score[4] > FAST_score[i])? 1:0;
	}
	for (int i=5; i<9; i++){
#pragma HLS unroll
		scoreJudge[i-1] = (FAST_score[4] > FAST_score[i])? 1:0;
	}
	if ((scoreJudge[0]&scoreJudge[1]&scoreJudge[2]&scoreJudge[3]&scoreJudge[4]&scoreJudge[5]&scoreJudge[6]&scoreJudge[7])==0){
		fpValid[4] = 0;
		FAST_score[4] = 0;
	}

	// output value
	ap_uint<12> FAST_score_4 = FAST_score[4];
	ap_uint<1> fpValid_4 = fpValid[4];
	ap_axiu<32,1,1,1> fp_Out;
	fp_Out.data.range(31,28) = 0;
	fp_Out.data.range(27,16) = FAST_score_4;
	fp_Out.data.range(15,9) = 0;
	fp_Out.data.range(8,8) = fpValid_4;
	fp_Out.data.range(7,0) = gaus;
	fp_Out.keep = 0xf;
	if(rowImage == HEIGHT-1 && colImage == WIDTH -1) fp_Out.last = 1;
	else fp_Out.last = 0;
	outStream.write(fp_Out);
}


//template<int NMS, int SRC_T, int HEIGHT, int WIDTH, int WIN_SZ, int WIN_SZSQ, int NPC>
void myFastScore(hls::stream<ap_axiu<8,1,1,1> > &srcStream, hls::stream<ap_axiu<32,1,1,1>  > &outStream, int threshold)
{
	int col_loop_var = NPC;

	// row index initialization
	ap_uint<WIN_SZ_IDX_BIT> rowIdx[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable=rowIdx complete dim=1

	// initialize the line_buffer
	ap_uint<PIXEL_BIT> lineBuffer[WIN_SZ][WIDTH];
#pragma HLS ARRAY_PARTITION variable=lineBuffer complete dim=1

	for (int col = 0; col < WIDTH; col++){
#pragma HLS PIPELINE
		for (int row = 0; row < WIN_SZ>>1; row++){
			lineBuffer[row][col] = 0;
		}
	}
	for (int col = 0; col < WIDTH; col++){
#pragma HLS PIPELINE
		lineBuffer[WIN_SZ-1][col] = 0;
	}

	for (int row =  WIN_SZ >> 1; row < WIN_SZ - 1; row++){
		for(int col = 0; col < WIDTH; col++){
#pragma HLS PIPELINE
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
#pragma HLS ARRAY_PARTITION variable=winReg complete dim=0
		if (rowImage<WIN_SZ>>1){
			for (int col = WIN_SZ >> 1; col < WIN_SZ; col++){
				for (int row = 0; row < (WIN_SZ>>1)-rowImage; row++){
#pragma HLS PIPELINE
					winReg[row][col] = lineBuffer[rowIdx[(WIN_SZ>>1)-rowImage]][col-(WIN_SZ>>1)];
				}
			}
			for (int col = WIN_SZ >> 1; col < WIN_SZ; col++){
				for (int row = (WIN_SZ>>1)-rowImage; row < WIN_SZ-1; row++){
#pragma HLS PIPELINE
					winReg[row][col] = lineBuffer[rowIdx[row]][col-(WIN_SZ>>1)];
				}
			}
		}
		else {
			for (int col = WIN_SZ >> 1; col < WIN_SZ; col++){
#pragma HLS PIPELINE
				for (int row = 0; row < WIN_SZ-1; row++){
					winReg[row][col] = lineBuffer[rowIdx[row]][col-(WIN_SZ>>1)];
				}
			}
		}
		for (int col = 0; col < WIN_SZ >> 1; col++){
#pragma HLS UNROLL
			for (int row = 0; row < WIN_SZ-1; row++){
#pragma HLS UNROLL
				winReg[row][col] = winReg[row][WIN_SZ>>1];
			}
		}
		// read new data into linebuffer & update the last row of WIN_REG
		if(rowImage + (WIN_SZ>>1) < HEIGHT ){
			for (int col =(WIN_SZ>>1); col < WIN_SZ; col++){
#pragma HLS PIPELINE
				ap_uint<PIXEL_BIT> data_temp = (srcStream.read()).data;
				lineBuffer[rowIdx[WIN_SZ-1]][col-(WIN_SZ>>1)] = data_temp;
				winReg[WIN_SZ-1][col] = data_temp;
			}
		}
		else{
			for (int col =(WIN_SZ>>1); col < WIN_SZ; col++){
#pragma HLS PIPELINE
				lineBuffer[rowIdx[WIN_SZ-1]][col-(WIN_SZ>>1)] = lineBuffer[rowIdx[WIN_SZ-2]][col-(WIN_SZ>>1)];
				winReg[WIN_SZ-1][col] = lineBuffer[rowIdx[WIN_SZ-2]][col-(WIN_SZ>>1)];
			}
		}
		for (int col = 0; col < WIN_SZ>>1; col++){
#pragma HLS UNROLL
			winReg[WIN_SZ-1][col] = winReg[WIN_SZ-1][WIN_SZ>>1];
		}

		COL_IMAGE:for (ap_uint<COL_IMAGE_BIT> colImage = 0; colImage < WIDTH; colImage++){
			processFAST( winReg,  outStream, rowImage, colImage,  threshold);


			// update line buffer & winReg
			if (colImage + (WIN_SZ>>1) + 1 < WIDTH){

				for (int col =0; col < WIN_SZ - 1; col++){
#pragma HLS unroll
					for (int row =0; row < WIN_SZ; row++){
#pragma HLS unroll
						winReg[row][col] = winReg[row][col+1];
					}
				}
				ap_uint<PIXEL_BIT> newPixelIn;

				if(rowImage + (WIN_SZ>>1) < HEIGHT ){
					newPixelIn = (srcStream.read()).data;
				}
				else{
					newPixelIn = lineBuffer[rowIdx[WIN_SZ-2]][colImage + (WIN_SZ>>1) + 1];
				}

				lineBuffer[rowIdx[WIN_SZ-1]][colImage + (WIN_SZ>>1) + 1] = newPixelIn;
				winReg[WIN_SZ-1][WIN_SZ-1] = newPixelIn;

				if (rowImage < WIN_SZ>>1){
					for (int row_win = 0; row_win < (WIN_SZ>>1)-rowImage; row_win++){
#pragma HLS PIPELINE
						winReg[row_win][WIN_SZ-1] = lineBuffer[rowIdx[(WIN_SZ>>1)-rowImage]][colImage + (WIN_SZ>>1) +1];
					}
					for (int row_win = (WIN_SZ>>1)-rowImage; row_win < WIN_SZ-1; row_win++){
#pragma HLS PIPELINE
						winReg[row_win][WIN_SZ-1] = lineBuffer[rowIdx[row_win]][colImage + (WIN_SZ>>1) +1];
					}
				}
				else{
					for (int row_win = 0; row_win < WIN_SZ - 1; row_win++){
#pragma HLS unroll
						winReg[row_win][WIN_SZ-1] = lineBuffer[rowIdx[row_win]][colImage + (WIN_SZ>>1) +1];
					}
				}
			}

			else if(colImage < WIDTH)
			{

				for (int col =0; col < WIN_SZ - 1; col++){
#pragma HLS unroll
					for (int row =0; row < WIN_SZ; row++){
#pragma HLS unroll
						winReg[row][col] = winReg[row][col+1];
					}
				}
			}
		}
	}
}


void FAST_extractor(hls::stream<ap_axiu<8,1,1,1> > &srcStream, hls::stream<ap_axiu<32,1,1,1> > &outStream){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=srcStream
#pragma HLS INTERFACE axis register both port=outStream
	myFastScore(srcStream, outStream, 40);
}

