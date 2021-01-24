/***************************************************************************
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
may be used to endorse or promote products derived from this software 
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/
#ifndef _XF_HOUGHLINES_HPP_
#define _XF_HOUGHLINES_HPP_

#ifndef __cplusplus
#error C++ is needed to include this header
#endif


typedef unsigned short  uint16_t;

#include "hls_stream.h"
#include "common/xf_common.h"
#include "common/xf_utility.h"


#define	 pai 3.1415926
#define pai_by_360 0.008726
#define pai_by_180 0.017444

//--------------------------------------------------------------------------------------------
// Sin & Cos Tables for angles [0,180) or [0,179.5] in the steps of 0.5 degrees
// Sin(90) and Cos(90) are made close 1 (but NOT 1) to avoid using 2 bits for integer part
// The values are stored in 1.15 format
// Also, the vlaues 1.000000 and 0.500000 are made 0.999980 and 0.499980
// to make sure the cvRound does not create mismatch with orgin shift
// Example:
// When theta=90, W=240 H=135
// rho = (x - (W/2)Cos(theta) + (y - (H/2))Sin(Theta)
//     = 0 + (-67.5)Sin(90) (for the first row i.e y=0)
// We see .5 as the fractional part which makes cvRound to kick in and round the value
// cvRound is an irreverible computation, we can not get original value. So, to avoid cvRound
// getting kicked in because of H/2 or W/2, we chose 0.999980 and 0.499980 instead 1 & 0.5
//--------------------------------------------------------------------------------------------
static ap_fixed<16,1,AP_RND> sinval[360] = {0.000000,0.008727,0.017452,0.026177,0.034899,0.043619,0.052336,0.061049,0.069756,0.078459,0.087156,0.095846,0.104528,0.113203,0.121869,0.130526,0.139173,0.147809,0.156434,0.165048,0.173648,0.182236,0.190809,0.199368,0.207912,0.216440,0.224951,0.233445,0.241922,0.250380,0.258819,0.267238,0.275637,0.284015,0.292372,0.300706,0.309017,0.317305,0.325568,0.333807,0.342020,0.350207,0.358368,0.366501,0.374607,0.382684,0.390731,0.398749,0.406737,0.414693,0.422618,0.430511,0.438371,0.446198,0.453991,0.461749,0.469472,0.477159,0.484810,0.492424,0.499980,0.507539,0.515038,0.522499,0.529920,0.537300,0.544639,0.551937,0.559193,0.566407,0.573577,0.580703,0.587786,0.594823,0.601815,0.608762,0.615662,0.622515,0.629321,0.636079,0.642788,0.649448,0.656059,0.662620,0.669131,0.675591,0.681999,0.688355,0.694659,0.700910,0.707107,0.713251,0.719340,0.725375,0.731354,0.737278,0.743145,0.748956,0.754710,0.760406,0.766045,0.771625,0.777146,0.782609,0.788011,0.793354,0.798636,0.803857,0.809017,0.814116,0.819152,0.824127,0.829038,0.833886,0.838671,0.843392,0.848048,0.852641,0.857168,0.861629,0.866026,0.870356,0.874620,0.878817,0.882948,0.887011,0.891007,0.894934,0.898794,0.902585,0.906308,0.909961,0.913545,0.917060,0.920505,0.923879,0.927184,0.930417,0.933580,0.936672,0.939692,0.942641,0.945518,0.948323,0.951056,0.953717,0.956305,0.958820,0.961261,0.963630,0.965926,0.968147,0.970295,0.972370,0.974370,0.976296,0.978147,0.979924,0.981627,0.983255,0.984807,0.986285,0.987688,0.989016,0.990268,0.991445,0.992546,0.993572,0.994522,0.995396,0.996195,0.996917,0.997564,0.998135,0.998629,0.999048,0.999391,0.999657,0.999848,0.999962,0.999980,0.999962,0.999848,0.999657,0.999391,0.999048,0.998629,0.998135,0.997564,0.996917,0.996195,0.995396,0.994522,0.993572,0.992546,0.991445,0.990268,0.989016,0.987688,0.986285,0.984807,0.983255,0.981627,0.979924,0.978147,0.976296,0.97437,0.97237,0.970295,0.968147,0.965926,0.96363,0.961261,0.95882,0.956305,0.953717,0.951056,0.948323,0.945518,0.942641,0.939692,0.936672,0.93358,0.930417,0.927184,0.923879,0.920505,0.91706,0.913545,0.909961,0.906308,0.902585,0.898794,0.894934,0.891007,0.887011,0.882948,0.878817,0.87462,0.870356,0.866026,0.861629,0.857168,0.852641,0.848048,0.843392,0.838671,0.833886,0.829038,0.824127,0.819152,0.814116,0.809017,0.803857,0.798636,0.793354,0.788011,0.782609,0.777146,0.771625,0.766045,0.760406,0.75471,0.748956,0.743145,0.737278,0.731354,0.725375,0.71934,0.713251,0.707107,0.70091,0.694659,0.688355,0.681999,0.675591,0.669131,0.66262,0.656059,0.649448,0.642788,0.636079,0.629321,0.622515,0.615662,0.608762,0.601815,0.594823,0.587786,0.580703,0.573577,0.566407,0.559193,0.551937,0.544639,0.5373,0.52992,0.522499,0.515038,0.507539,0.5,0.492424,0.48481,0.477159,0.469472,0.461749,0.453991,0.446198,0.438371,0.430511,0.422618,0.414693,0.406737,0.398749,0.390731,0.382684,0.374607,0.366501,0.358368,0.350207,0.34202,0.333807,0.325568,0.317305,0.309017,0.300706,0.292372,0.284015,0.275637,0.267238,0.258819,0.25038,0.241922,0.233445,0.224951,0.21644,0.207912,0.199368,0.190809,0.182236,0.173648,0.165048,0.156434,0.147809,0.139173,0.130526,0.121869,0.113203,0.104528,0.095846,0.087156,0.078459,0.069756,0.061049,0.052336,0.043619,0.034899,0.026177,0.017452,0.008727};
static ap_fixed<16,1,AP_RND> cosval[360] = {0.999980,0.999962,0.999848,0.999657,0.999391,0.999048,0.998629,0.998135,0.997564,0.996917,0.996195,0.995396,0.994522,0.993572,0.992546,0.991445,0.990268,0.989016,0.987688,0.986285,0.984807,0.983255,0.981627,0.979924,0.978147,0.976296,0.97437,0.97237,0.970295,0.968147,0.965926,0.96363,0.961261,0.95882,0.956305,0.953717,0.951056,0.948323,0.945518,0.942641,0.939692,0.936672,0.93358,0.930417,0.927184,0.923879,0.920505,0.91706,0.913545,0.909961,0.906308,0.902585,0.898794,0.894934,0.891007,0.887011,0.882948,0.878817,0.87462,0.870356,0.866026,0.861629,0.857168,0.852641,0.848048,0.843392,0.838671,0.833886,0.829038,0.824127,0.819152,0.814116,0.809017,0.803857,0.798636,0.793354,0.788011,0.782609,0.777146,0.771625,0.766045,0.760406,0.75471,0.748956,0.743145,0.737278,0.731354,0.725375,0.71934,0.713251,0.707107,0.70091,0.694659,0.688355,0.681999,0.675591,0.669131,0.66262,0.656059,0.649448,0.642788,0.636079,0.629321,0.622515,0.615662,0.608762,0.601815,0.594823,0.587786,0.580703,0.573577,0.566407,0.559193,0.551937,0.544639,0.5373,0.52992,0.522499,0.515038,0.507539,0.5,0.492424,0.48481,0.477159,0.469472,0.461749,0.453991,0.446198,0.438371,0.430511,0.422618,0.414693,0.406737,0.398749,0.390731,0.382684,0.374607,0.366501,0.358368,0.350207,0.34202,0.333807,0.325568,0.317305,0.309017,0.300706,0.292372,0.284015,0.275637,0.267238,0.258819,0.25038,0.241922,0.233445,0.224951,0.21644,0.207912,0.199368,0.190809,0.182236,0.173648,0.165048,0.156434,0.147809,0.139173,0.130526,0.121869,0.113203,0.104528,0.095846,0.087156,0.078459,0.069756,0.061049,0.052336,0.043619,0.034899,0.026177,0.017452,0.008727,0.000000,-0.008727,-0.017452,-0.026177,-0.034899,-0.043619,-0.052336,-0.061049,-0.069756,-0.078459,-0.087156,-0.095846,-0.104528,-0.113203,-0.121869,-0.130526,-0.139173,-0.147809,-0.156434,-0.165048,-0.173648,-0.182236,-0.190809,-0.199368,-0.207912,-0.216440,-0.224951,-0.233445,-0.241922,-0.250380,-0.258819,-0.267238,-0.275637,-0.284015,-0.292372,-0.300706,-0.309017,-0.317305,-0.325568,-0.333807,-0.342020,-0.350207,-0.358368,-0.366501,-0.374607,-0.382684,-0.390731,-0.398749,-0.406737,-0.414693,-0.422618,-0.430511,-0.438371,-0.446198,-0.453991,-0.461749,-0.469472,-0.477159,-0.484810,-0.492424,-0.499980,-0.507539,-0.515038,-0.522499,-0.529920,-0.537300,-0.544639,-0.551937,-0.559193,-0.566407,-0.573577,-0.580703,-0.587786,-0.594823,-0.601815,-0.608762,-0.615662,-0.622515,-0.629321,-0.636079,-0.642788,-0.649448,-0.656059,-0.662620,-0.669131,-0.675591,-0.681999,-0.688355,-0.694659,-0.700910,-0.707107,-0.713251,-0.719340,-0.725375,-0.731354,-0.737278,-0.743145,-0.748956,-0.754710,-0.760406,-0.766045,-0.771625,-0.777146,-0.782609,-0.788011,-0.793354,-0.798636,-0.803857,-0.809017,-0.814116,-0.819152,-0.824127,-0.829038,-0.833886,-0.838671,-0.843392,-0.848048,-0.852641,-0.857168,-0.861629,-0.866026,-0.870356,-0.874620,-0.878817,-0.882948,-0.887011,-0.891007,-0.894934,-0.898794,-0.902585,-0.906308,-0.909961,-0.913545,-0.917060,-0.920505,-0.923879,-0.927184,-0.930417,-0.933580,-0.936672,-0.939692,-0.942641,-0.945518,-0.948323,-0.951056,-0.953717,-0.956305,-0.958820,-0.961261,-0.963630,-0.965926,-0.968147,-0.970295,-0.972370,-0.974370,-0.976296,-0.978147,-0.979924,-0.981627,-0.983255,-0.984807,-0.986285,-0.987688,-0.989016,-0.990268,-0.991445,-0.992546,-0.993572,-0.994522,-0.995396,-0.996195,-0.996917,-0.997564,-0.998135,-0.998629,-0.999048,-0.999391,-0.999657,-0.999848,-0.999962};


namespace xf{

/*****************************************************************
 * 		          Function for Voting process
 *****************************************************************/
static void register_rho_stg1_sin(ap_fixed<28,13,AP_RND> &rho_stg1_sino, ap_fixed<28,13,AP_RND> rho_stg1_sini, ap_fixed<16,1,AP_RND> sinvals, bool j_eq_width)
{
#pragma HLS INLINE OFF
	if (j_eq_width) // Update when it is the last pixel of row
		{rho_stg1_sino = rho_stg1_sini + sinvals;}
	else
		{rho_stg1_sino = rho_stg1_sini;}
}


template<int SRC_T,int ROWS,int COLS,int DEPTH,int NPC,int WORDWIDTH,unsigned int theta,unsigned int rho,int AngleN,int rhoN,int MINTHETA>
void xfVoting(
		xf::Mat<SRC_T, ROWS, COLS, NPC> & _src_mat,
		ap_uint<12> accum[AngleN+1][rhoN+1],
		ap_uint<12> height,
		ap_uint<12> width
)
{
#pragma HLS INLINE OFF


	//accumulator matrix initialization
	// Make all votes as "0"
	loop_init_r: for(ap_uint<13> r=0;r<rhoN+1;r++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=rhoN
#pragma HLS PIPELINE
		loop_init_n: for(ap_uint<13> n=0;n<AngleN+1;n++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=AngleN
			accum[n][r]=0;
		}
	}


	ap_fixed<16,1,AP_RND> sinvals[AngleN]; //To store fixed point sin angle values 1.15
	ap_fixed<16,1,AP_RND> cosvals[AngleN]; //To store fixed point cos angle values 1.15

	// Angle values scaling based on rho step and theta step
	// Top level arguments are required to be integers.
	// But we consider those values to be in 6.1 format
	// i.e if input is 5 that means theta = 5>>1 = 5/2 = 2.5

	unsigned char rhoval = rho;

	loop_init: for(ap_uint<10> n = 0,ang=(MINTHETA*2); n < AngleN ; ang=ang+(theta),n++ ) // Assumtion is theta is in 6.1 format
	{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=1 max=AngleN
		sinvals[n]=sinval[ang]/rhoval;
		cosvals[n]=cosval[ang]/rhoval;

	}



	ap_uint<13> rho_prev_set1[(AngleN)];
	ap_uint<13> rho_stg3,rho_stg3_reg[AngleN],rho_set2_reg;
	ap_fixed<14,13> rho_stg3_apfixedp1;
	ap_fixed<13,13> rho_stg3_apfixedp0;
	ap_fixed<28,13,AP_RND> rho_stg1_sin[(AngleN)],rho_stg1_cos[(AngleN)],rho_stg1_sin_wire[AngleN],rho_stg2[(AngleN)]; // sin and cos value register (13.15)
	ap_uint<12> accval_reg_set1[AngleN]; // [MINTHETA, MAXTHETA) angles

#pragma HLS ARRAY_PARTITION variable=accval_reg_set1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=rho_stg1_cos complete dim=0
#pragma HLS ARRAY_PARTITION variable=rho_stg1_sin complete dim=0
#pragma HLS ARRAY_PARTITION variable=rho_stg1_sin_wire complete dim=0
#pragma HLS ARRAY_PARTITION variable=rho_stg2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=sinvals complete dim=0
#pragma HLS ARRAY_PARTITION variable=cosvals complete dim=0
#pragma HLS ARRAY_PARTITION variable=rho_stg3_reg complete dim=0
#pragma HLS ARRAY_PARTITION variable=rho_prev_set1 complete dim=0

	// Our computation (with origin shifted to center) generates rho= -rM/2 to (rM/2)-1
	// we need to add (rM/2) to make inexing/addressing of accumulator matrix easy
	// with offest added, the range will be 0 to (rM)-1
	ap_fixed<14,13,AP_RND> diag_offset = (rhoN)/2;
	ap_fixed<14,13,AP_RND> roundval = 0.5;

	ap_fixed<14,13,AP_RND> rnd_Const_m0p5 = diag_offset - roundval;
	ap_fixed<14,13,AP_RND> rnd_Const_p0p5 = diag_offset + roundval;
	ap_uint<13> hei = (height/2) ;
	ap_uint<13> wdt = (width/2);
	ap_fixed<28,13,AP_RND> rho_stg1_sin_sin, rho_stg1_sin_cos;

	// Truncate height/2 & width/2 so that center falls on a pixel (not inbetween pixels)
	// When image size WxH, the co-ordinates ranges are
	// Origin				Co-ordinates range
	// Top left(0,0)		(0,0) to (W-1 x H-1)
	// Center (W/2, H/2)	(-W/2,-H/2) to (H/2 -1 , W/2 - 1)
	//
	// Example:
	// For WxH = 6x4
	// Top left - (0,0) to (5,3)
	// Center (3,2) - (-3,-2) to (2,1)
	//
	// For WxH = 5x4
	// Top left - (0,0) to (4,3)
	// Center (2,2) - (-2,-2) to (2,1)

	//FILE *fpang = fopen("hlsang.txt","w");
	// Initialization of all registers
	for(ap_uint<10> ki=0;ki< (AngleN);ki++)
	{
#pragma HLS pipeline
		rho_stg1_sin[ki] = (-wdt * cosvals[ki]) + (-hei * sinvals[ki]); // 13.15

		//fprintf(fpang,"sin:%f cos%f\n",(float)(-wdt * cosvals[ki]),(float)(-hei * sinvals[ki]));
		// Splitting the computation seems to be helping in timing
		//rho_stg1_sin_sin = (-hei * sinvals[ki]); // 13.15
		//rho_stg1_sin_cos = (-wdt * cosvals[ki]); // 13.15
		//rho_stg1_sin[ki] = rho_stg1_sin_sin + rho_stg1_sin_cos;

		rho_prev_set1[ki] = 0;
		accval_reg_set1[ki]=0;
		rho_stg3_reg[ki]=0;
	}
	//fclose(fpang);

	ap_fixed<14,13,AP_RND> rho_offset;
	ap_fixed<14,13,AP_RND> rho_stg2_rsh;

	ap_uint<12> acc_val_set1,upd_accval_set1=0;
	ap_uint<23> row_index =0;
	bool rho_stg2_lsbs;
	bool j_eq_width, j_eq_0,delay_1edge=0;
	XF_PTNAME(DEPTH) img_pixel_val_reg =0;




	// Row loop
	LOOPI:for(ap_uint<13> i = 0; i < height; i++ )
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
		XF_PTNAME(DEPTH) img_pixel_val;

		// Column loop
		LOOPJ:for(ap_uint<12> j = 0; j < width; j++ )
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE  array inter false
#pragma HLS LOOP_FLATTEN off

			img_pixel_val = _src_mat.data[row_index]; // Reading one pixel at a time (address auto incremented) in raster scan order
			j_eq_width = ( j == (width-1))? 1:0;
			j_eq_0 = ( j == 0)? 1:0;


			LOOPN1:for(ap_uint<10> n = 0; n < (AngleN); n++ ) // angle loop
			{
#pragma HLS UNROLL

				//rho_stg1_sin_wire[n] = rho_stg1_sin[n];

				// Check if it is a new row (by checking if col=0)
				if(j_eq_0){
					rho_stg2[n] = rho_stg1_sin[n];
				}
				else {
					rho_stg2[n] = rho_stg1_cos[n];
				}

				//-------------------------------------------------
				// Prepare value for the next pixel
				rho_stg1_cos[n] = rho_stg2[n] + cosvals[n];
				if (j_eq_width) // Update when it is the last pixel of row
					rho_stg1_sin[n] = rho_stg1_sin[n] + sinvals[n];

				//register_rho_stg1_sin(rho_stg1_sin[n], rho_stg1_sin_wire[n], sinvals[n], j_eq_width);


				//-------------------------------------------------

				//----------------------------------------------------------------
				// Logic to select rounding consant to implement cvRound function
				rho_stg2_lsbs = (rho_stg2[n].range(15,0) == 16384)? 1:0;
				rho_offset = (rho_stg2_lsbs)? rnd_Const_m0p5 : rnd_Const_p0p5;
				//----------------------------------------------------------------

				// Prepare rho in 13.1 format to avoid big adder
				// as cvRound operation needs addition/subtration of 0.5
				// and diag offset is added after cvRound operation
				rho_stg2_rsh.range(13,0) = rho_stg2[n].range(27,14);


				// cvRound rho and add diag_offset to shift rho values from [-rM/2,(rM/2)-1) to [0,rM-1)
				// So that we can address/index the accumulator cells easily
				rho_stg3_apfixedp1 = rho_stg2_rsh + rho_offset;
				rho_stg3_apfixedp0 = rho_stg3_apfixedp1.range(13,1);
				rho_stg3 = rho_stg3_apfixedp0;



				//---------------------------------------------------------------------------------------------
				// This logic ensures the voting increment happend even if the previous pixel is also
				// an edge pixel and has same rho. This logic will ensure that accumulator matrix is not read
				// but to use the recently incremeneted vote value
				if(( img_pixel_val_reg != 0 ))// If pixel is a edge pixel, then update the vote
				{
					acc_val_set1 = accum[n][rho_stg3_reg[n]];

					upd_accval_set1 = accval_reg_set1[n] + 1;


					if(rho_stg3_reg[n] == rho_prev_set1[n])
					{
						accval_reg_set1[n] = upd_accval_set1;
					}
					else
					{
						accval_reg_set1[n] = acc_val_set1;
					}

					// Writing is pipelined. So, we start writing from the 2nd edge pixel
					if (delay_1edge){
						accum[n][rho_prev_set1[n]] = upd_accval_set1; // Writing set 1

					}

					rho_prev_set1[n] = rho_stg3_reg[n];


				}
				//---------------------------------------------------------------------------------------------

				rho_stg3_reg[n] = rho_stg3;


			}// Angle Loop
			if (img_pixel_val_reg != 0 )
			{delay_1edge = 1;}

			row_index = row_index + 1;

			img_pixel_val_reg =img_pixel_val;

		} // Column loop
	} //Row Loop




	/* Left over pixel computation. Flushing the pipeline*/
	LOOPN2:for(ap_uint<10> n = 0; n < (AngleN); n++ )
	{
#pragma HLS UNROLL

		upd_accval_set1 = accval_reg_set1[n] + 1;

		accum[n][rho_prev_set1[n]] = upd_accval_set1; // Writing set 1

	}
}

/*****************************************************************
 * 		            For Thinning process
 *****************************************************************/
template<int AngleN>
void thinningCompare(ap_uint<12> vote_at_rho_theta[AngleN+1],ap_uint<12> vote_at_rho_theta_reg[AngleN+1],
		bool cond1,bool cond2[AngleN],bool cond3,bool cond4,bool four_conds[AngleN],short threshold)
{
#pragma HLS INLINE OFF
	CONDLOOP:for (ap_uint<10> ang2 =0; ang2 < AngleN; ang2++)
	{
#pragma HLS PIPELINE
		cond1 = (vote_at_rho_theta[ang2+1] > vote_at_rho_theta[ang2])? 1:0;
		cond2[ang2+1] = (vote_at_rho_theta[ang2+1] > vote_at_rho_theta_reg[ang2+1])? 1:0;
		cond3 = (vote_at_rho_theta[ang2+1] > threshold)? 1:0;
		cond4 = (vote_at_rho_theta[ang2+2] > vote_at_rho_theta[ang2+1])? 0:1;
		four_conds[ang2+1] = (cond1 && cond2[ang2+1] && cond3 && cond4)? 1:0;
	}
}
/*****************************************************************
 * 		                Thinning process
 *****************************************************************
 * xfThinning
 *
 *  | --  | x01 | --  |
 *
 *  | x10 | x11 | x12 |
 *
 *  | --  | x21 | --  |
 *
 * Comparing x11 with x01,x10,x12 and x21 
 * ((x11 > x01) && (x11 > x10) && (x11 >= x12) && (x11 >= x21) && x11>threshold)
 *
 *****************************************************************/

template<int ROWS,int COLS,int DEPTH,int NPC,int WORDWIDTH,unsigned int AngleN,int rhoN>
void xfThinning(ap_uint<12>  accumulator[AngleN+1][rhoN+1],	short threshold)
{
	ap_uint<12> vote_at_rho_theta[AngleN+2], vote_at_rho_theta_reg[AngleN+2];
	bool cond1,cond2[AngleN+1],cond3,cond4, four_conds[AngleN+1],four_conds_reg[AngleN+1],four_conds_reg_2[AngleN+1];

#pragma HLS ARRAY_PARTITION variable=cond2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=vote_at_rho_theta complete dim=0
#pragma HLS ARRAY_PARTITION variable=vote_at_rho_theta_reg complete dim=0
#pragma HLS ARRAY_PARTITION variable=four_conds complete dim=0
#pragma HLS ARRAY_PARTITION variable=four_conds_reg complete dim=0
#pragma HLS ARRAY_PARTITION variable=four_conds_reg_2 complete dim=0

	// Initialization
	for (ap_uint<10> ang1 =0; ang1 < AngleN +1; ang1++)
	{
#pragma HLS UNROLL
		vote_at_rho_theta_reg[ang1] = 0;
		four_conds_reg[ang1] = 0;
		four_conds_reg_2[ang1] = 1;
	}
	vote_at_rho_theta[0] =0;
	vote_at_rho_theta[AngleN+1] =0;

	RHOLOOPTHINNING:for(ap_uint<13> r = 0; r < rhoN+1; r++ )
	{
#pragma HLS LOOP_FLATTEN off
#pragma HLS PIPELINE
		THINNINGINIT:for (ap_uint<10> ang1 =0; ang1 < AngleN; ang1++)
		{
#pragma HLS UNROLL
			vote_at_rho_theta[ang1+1] = accumulator[ang1][r];
			// use ang1+1 for assigning so that we can avoid the pad data bram (ang1=0)
		}

		// Compare flags for all Angles
		thinningCompare<AngleN>(vote_at_rho_theta,vote_at_rho_theta_reg,cond1,cond2,cond3,cond4,four_conds,threshold);

		THINWRITELOOP:for (ap_uint<10> ang3 =0; ang3 < AngleN; ang3++)
		{
#pragma HLS UNROLL
			if (four_conds_reg_2[ang3+1] && (r>1))
				accumulator[ang3][r-2] = 0;

			vote_at_rho_theta_reg[ang3+1] = vote_at_rho_theta[ang3+1];

			four_conds_reg_2[ang3+1] = (!four_conds_reg[ang3+1] || cond2[ang3+1])? 1 : 0;
			four_conds_reg[ang3+1] = four_conds[ang3+1];

		}
	}
}

/*****************************************************************
 * 		              For Sorting process
 * 	This function outputs the max Vote value and its rho location
 // in a given Angle memory
 *****************************************************************/
template<int rhoN>
void get_maxval_index(ap_uint<12>  input_array[rhoN+1],ap_uint<12> &maxval,ap_uint<12> &max_index)
{
#pragma HLS INLINE OFF

	ap_uint<12> local_max =0;
	ap_uint<12> input_array_reg =0;

	RHOLOOP:for(ap_uint<13> r=0;r<rhoN;r++) // diagonal loop; Value @rhoN+1 is 0 anyway
	{
#pragma HLS PIPELINE

		input_array_reg = input_array[r]; // Regsister BRAM output
		if(input_array_reg > local_max)   //comparing rho values to find max rho value
		{
			local_max = input_array_reg;
			max_index = r;
		}

	}

	maxval = local_max;
}

/*****************************************************************
 * 		                Sorting process
 *****************************************************************
 * xfSorting
 *
 *
 * Finding the Top linesMax values in the accumulator buffer.
 * Inside linesMax loop,for all the theta values we are doing rho comparision in parallel.
 * and the rho comparisions are in pipeline loop.
 *
 * Once the final rho and theta values are computed, Returning theta in radians and rho values as float
 ****************************************************************/
template<int ROWS,int COLS,int DEPTH,int NPC,int WORDWIDTH,unsigned int theta,unsigned int rho,int AngleN,int rhoN,int linesMax,int MINTHETA>
void xfSorting(ap_uint<12>  accumulator[AngleN+1][rhoN+1],
		float linesrho[linesMax],
		float linestheta[linesMax],short linesmax)
{

	ap_uint<12> local_max[AngleN];
	ap_uint<12> local_max_rho[AngleN];
	ap_uint<12> maxrho =0;
	ap_uint<10> maxangle =0;

#pragma HLS ARRAY_PARTITION variable=local_max complete
#pragma HLS ARRAY_PARTITION variable=local_max_rho complete

	// Initialize local max (make max vote vale as 0 for each angle)
	for(ap_uint<10> i=0;i<AngleN;i++)
	{
#pragma HLS UNROLL
		local_max[i]=0;
	}

	ap_fixed<14,13,AP_RND> diag_offset = (rhoN)/2.0;



	MAINL:for(ap_uint<12> li=0;li<linesmax;li++) //linesMax loop
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=linesMax
		for(ap_uint<10> n=0;n<AngleN;n++) //Theta loop
		{
#pragma HLS UNROLL
			get_maxval_index<rhoN>(accumulator[n],local_max[n],local_max_rho[n]);
		}

		// Find the global maxima (of vote value) of the local maxima
		ap_uint<12> maxfinal=0;
		THETAL:for(ap_uint<10> n=0;n<AngleN;n++)
		{
#pragma HLS PIPELINE
			if(local_max[n] > maxfinal) // Vote comparision
			{
				maxfinal = local_max[n];
				maxangle = n;
				maxrho   = local_max_rho[n];
			}
		}

		// Make the location of that localMaxima 0 if that localMaxima becomes globalMaxima
		accumulator[maxangle][maxrho] = 0;

		float mintheta_radn = (MINTHETA * pai_by_180);

		float angle_radn = (maxangle * pai_by_360*(theta));

		float _rho = (maxrho- diag_offset) * rho; //rho computation
		float _angle = angle_radn + mintheta_radn;// Theta computation
		linesrho[li]=_rho; //updating rho value
		linestheta[li]=_angle; //updating theta value

	}


}

template<int SRC_T,int ROWS,int COLS,int DEPTH,int NPC,int WORDWIDTH,unsigned int theta,unsigned int rho,int linesMax,int DIAG,int MINTHETA,int MAXTHETA>
void xfHoughLines(
		xf::Mat<SRC_T, ROWS, COLS, NPC> & _src_mat,
		float outputrho[linesMax],
		float outputtheta[linesMax],
		short _threshold,
		ap_uint<12> height,
		ap_uint<12> width,
		short linesmax
)
{
#pragma HLS INLINE OFF

	// Accumulator buffer declaration
	// we need theta=[0,AngleN-1] and rho=[0,rhoN-1];
	// Also, we need theta=AngleN & r=rhoN for padding
	// But, BRAM for AngleN is avoided by handling it in design
	// using vote_at_rho_theta[AngleN] =0 in Thinning
	//
	// Example : MAXTHETA = 5; MINTHETA = 2; THETA=1 i.e ThetaStep=0.5
	//           (5-2)/0.5 = 6 ; {2, 2.5, 3, 3.5, 4, 4.5} 
	//			 i.e MAXTHETA is not included
	//
	//			If width=4 and Height=3, then Diag=5
	//			so, if rhoStep=1, then rho takes value 0,1,2,3,4
	//			i.e., {0,1,, ...Diag-1}
	//
	// rho=0 and theta=0  will have actual values
	// Padding on the left side (zero side) of rho and theta
	// are handled in the design. But we need memory for pad data on right
	ap_uint<12> accum[((2*(MAXTHETA-MINTHETA))/theta)+1][(DIAG)+1];


#pragma HLS ARRAY_PARTITION variable=accum complete dim=1
#pragma HLS RESOURCE variable=accum core=RAM_T2P_BRAM


	xfVoting<SRC_T,ROWS,COLS,DEPTH,NPC,WORDWIDTH,theta,rho,((2*(MAXTHETA-MINTHETA))/theta),((DIAG)),MINTHETA>(_src_mat,accum,height,width); //votes updation
	xfThinning<ROWS,COLS,DEPTH,NPC,WORDWIDTH,((2*(MAXTHETA-MINTHETA))/theta),((DIAG))>(accum,_threshold); // thinning -->NMS
	xfSorting<ROWS,COLS,DEPTH,NPC,WORDWIDTH,theta,rho,((2*(MAXTHETA-MINTHETA))/theta),((DIAG)),linesMax,MINTHETA>(accum,outputrho,outputtheta,linesmax); // finding the linesmax lines from the complete accum buffer

}

#pragma SDS data data_mover("_src_mat.data":AXIDMA_SIMPLE)
#pragma SDS data data_mover(outputrho:AXIDMA_SIMPLE)
#pragma SDS data data_mover(outputtheta:AXIDMA_SIMPLE)

#pragma SDS data access_pattern("_src_mat.data":SEQUENTIAL)
#pragma SDS data copy("_src_mat.data"[0:"_src_mat.size"])

#pragma SDS data access_pattern(outputrho:SEQUENTIAL)
#pragma SDS data access_pattern(outputtheta:SEQUENTIAL)

#pragma SDS data copy(outputrho[0:linesmax])
#pragma SDS data copy(outputtheta[0:linesmax])


/**************************************************************************
 * HoughLines : Wrapper function which calls the kernel function
 * 				depending upon the configurations.
 *
 * 				RHO = RHOSTEP
 * 				THETA=THETASTEP
 * 				MAXLINES=MAXIMUM NUMBER OF TOP LINES
 * 				DIAG=DIAGONAL OF THE IMAGE
 * 				MINTHETA = MINIMUM THETA
 * 				MAXTHETA = MAXIMUM THETA
 * 				SRC_T = SOURCE TYPE
 * 				ROWS = HEIGHT OF THE IMAGE
 * 				COLS = WIDTH OF THE IMAGE
 * 				NPC = NUMBER OF PIXELS PROCESSED PER CYCLE
 *
 * 				_src_mat: input image
 * 				outputrho:output rho values
 * 				outputtheta:output theta values
 * 				threshold:threshold value to supress votes in the kernel
 *
 **************************************************************************/

template<unsigned int RHO,unsigned int THETA,int MAXLINES,int DIAG,int MINTHETA,int MAXTHETA,int SRC_T, int ROWS, int COLS,int NPC>
void HoughLines(xf::Mat<SRC_T, ROWS, COLS, NPC> & _src_mat,float outputrho[MAXLINES],float outputtheta[MAXLINES],short threshold,short linesmax)
{
#pragma HLS INLINE OFF

	assert(((_src_mat.rows <= ROWS ) && (_src_mat.cols <= COLS)) && "ROWS and COLS should be greater than input image");
	assert((NPC == XF_NPPC1) && "NPC must be XF_NPPC1");

	assert((((MAXTHETA-MINTHETA) > 0)) && "MINTHETA must be less than MAXTHETA");
	assert(((MINTHETA >= 0) && (MINTHETA<180)) && "MINTHETA must be between 0 to 180");
	assert(((MAXTHETA > 0) && (MAXTHETA<=180)) && "MAXTHETA must be between 0 to 180");

	// Main function calling
	xfHoughLines<SRC_T,ROWS,COLS,XF_DEPTH(SRC_T,NPC),NPC,XF_WORDWIDTH(SRC_T,NPC),THETA,RHO,MAXLINES,DIAG,MINTHETA,MAXTHETA>(_src_mat,outputrho,outputtheta,threshold,_src_mat.rows,_src_mat.cols,linesmax);

}
}
#endif //_XF_HOUGHLINES_HPP_
