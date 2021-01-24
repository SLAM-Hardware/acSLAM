#include "RS_BRIEF.h"
const unsigned char mask[25][25] =
{
		000, 000, 000, 000, 000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000, 000, 000, 000, 000,
		000, 000, 000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000, 000, 000,
		000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000,
		000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000,
		000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000,
		000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000,
		000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000,
		000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000,
		000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000,
		000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000,
		000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000,
		000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000,
		000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000,
		000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000,
		000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000,
		000, 000, 000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000, 000, 000,
		000, 000, 000, 000, 000, 000, 000, 000, 000, 255, 255, 255, 255, 255, 255, 255, 000, 000, 000, 000, 000, 000, 000, 000, 000,
};
const char bit_pattern_25_[256][4] =
{
		14 , 17 , 4 , 18 ,
		7 , 2 , 12 , 18 ,
		18 , 10 , 15 , 11 ,
		11 , 18 , 13 , 17 ,
		14 , 9 , 15 , 11 ,
		6 , 13 , 12 , 9 ,
		13 , 8 , 21 , 4 ,
		4 , 17 , 8 , 9 ,
		13 , 17 , 3 , 16 ,
		9 , 1 , 11 , 18 ,
		18 , 11 , 15 , 12 ,
		10 , 18 , 12 , 17 ,
		15 , 9 , 15 , 12 ,
		6 , 12 , 13 , 9 ,
		14 , 8 , 22 , 6 ,
		3 , 15 , 9 , 8 ,
		12 , 17 , 2 , 14 ,
		11 , 1 , 10 , 18 ,
		18 , 12 , 15 , 12 ,
		9 , 17 , 11 , 17 ,
		15 , 10 , 15 , 12 ,
		6 , 11 , 13 , 9 ,
		14 , 9 , 23 , 8 ,
		3 , 14 , 9 , 8 ,
		11 , 17 , 2 , 13 ,
		13 , 1 , 9 , 17 ,
		18 , 14 , 15 , 13 ,
		8 , 16 , 10 , 17 ,
		15 , 11 , 15 , 13 ,
		6 , 9 , 14 , 10 ,
		15 , 9 , 24 , 10 ,
		3 , 12 , 10 , 7 ,
		10 , 17 , 2 , 11 ,
		16 , 1 , 8 , 16 ,
		18 , 15 , 15 , 13 ,
		7 , 16 , 9 , 16 ,
		16 , 11 , 15 , 13 ,
		7 , 8 , 14 , 10 ,
		16 , 10 , 24 , 13 ,
		3 , 10 , 11 , 7 ,
		9 , 16 , 3 , 9 ,
		18 , 2 , 7 , 15 ,
		17 , 16 , 14 , 14 ,
		6 , 15 , 8 , 16 ,
		16 , 12 , 14 , 14 ,
		8 , 8 , 14 , 10 ,
		16 , 11 , 24 , 15 ,
		3 , 8 , 12 , 7 ,
		8 , 16 , 3 , 7 ,
		19 , 4 , 6 , 14 ,
		16 , 17 , 14 , 14 ,
		6 , 13 , 8 , 15 ,
		16 , 13 , 14 , 14 ,
		9 , 7 , 15 , 11 ,
		16 , 11 , 23 , 17 ,
		4 , 7 , 13 , 7 ,
		7 , 15 , 5 , 5 ,
		21 , 5 , 6 , 13 ,
		15 , 17 , 14 , 15 ,
		6 , 12 , 7 , 14 ,
		15 , 13 , 14 , 15 ,
		10 , 6 , 15 , 11 ,
		16 , 12 , 22 , 19 ,
		6 , 5 , 14 , 7 ,
		7 , 14 , 6 , 4 ,
		22 , 7 , 6 , 12 ,
		14 , 18 , 13 , 15 ,
		6 , 11 , 7 , 13 ,
		15 , 14 , 13 , 15 ,
		11 , 6 , 15 , 12 ,
		16 , 13 , 20 , 21 ,
		7 , 4 , 15 , 8 ,
		7 , 13 , 8 , 3 ,
		23 , 9 , 6 , 11 ,
		13 , 18 , 12 , 15 ,
		6 , 10 , 7 , 12 ,
		15 , 15 , 12 , 15 ,
		12 , 6 , 15 , 13 ,
		16 , 14 , 18 , 22 ,
		9 , 3 , 16 , 9 ,
		7 , 12 , 10 , 2 ,
		23 , 11 , 6 , 10 ,
		12 , 18 , 12 , 15 ,
		7 , 9 , 7 , 11 ,
		14 , 15 , 12 , 15 ,
		13 , 6 , 15 , 13 ,
		15 , 14 , 16 , 23 ,
		10 , 3 , 16 , 9 ,
		7 , 11 , 11 , 2 ,
		23 , 13 , 7 , 9 ,
		10 , 18 , 11 , 15 ,
		8 , 8 , 7 , 10 ,
		13 , 15 , 11 , 15 ,
		15 , 6 , 14 , 14 ,
		15 , 15 , 14 , 24 ,
		12 , 3 , 17 , 10 ,
		7 , 10 , 13 , 2 ,
		23 , 16 , 8 , 8 ,
		9 , 18 , 11 , 15 ,
		8 , 7 , 8 , 9 ,
		13 , 16 , 11 , 15 ,
		16 , 7 , 14 , 14 ,
		14 , 16 , 11 , 24 ,
		14 , 3 , 17 , 11 ,
		8 , 9 , 15 , 3 ,
		22 , 18 , 9 , 7 ,
		8 , 17 , 10 , 14 ,
		9 , 6 , 8 , 8 ,
		12 , 16 , 10 , 14 ,
		16 , 8 , 14 , 14 ,
		13 , 16 , 9 , 24 ,
		16 , 3 , 17 , 12 ,
		8 , 8 , 17 , 3 ,
		20 , 19 , 10 , 6 ,
		7 , 16 , 10 , 14 ,
		11 , 6 , 9 , 8 ,
		11 , 16 , 10 , 14 ,
		17 , 9 , 13 , 15 ,
		13 , 16 , 7 , 23 ,
		17 , 4 , 17 , 13 ,
		9 , 7 , 19 , 5 ,
		19 , 21 , 11 , 6 ,
		7 , 15 , 9 , 14 ,
		12 , 6 , 10 , 7 ,
		11 , 15 , 9 , 14 ,
		18 , 10 , 13 , 15 ,
		12 , 16 , 5 , 22 ,
		19 , 6 , 17 , 14 ,
		10 , 7 , 20 , 6 ,
		17 , 22 , 12 , 6 ,
		6 , 14 , 9 , 13 ,
		13 , 6 , 11 , 7 ,
		10 , 15 , 9 , 13 ,
		18 , 11 , 12 , 15 ,
		11 , 16 , 3 , 20 ,
		20 , 7 , 16 , 15 ,
		11 , 7 , 21 , 8 ,
		15 , 23 , 13 , 6 ,
		6 , 13 , 9 , 12 ,
		14 , 6 , 12 , 7 ,
		9 , 15 , 9 , 12 ,
		18 , 12 , 11 , 15 ,
		10 , 16 , 2 , 18 ,
		21 , 9 , 15 , 16 ,
		12 , 7 , 22 , 10 ,
		13 , 23 , 14 , 6 ,
		6 , 12 , 9 , 12 ,
		15 , 7 , 13 , 7 ,
		9 , 14 , 9 , 12 ,
		18 , 13 , 11 , 15 ,
		10 , 15 , 1 , 16 ,
		21 , 10 , 15 , 16 ,
		13 , 7 , 22 , 11 ,
		11 , 23 , 15 , 7 ,
		6 , 10 , 9 , 11 ,
		16 , 8 , 14 , 7 ,
		9 , 13 , 9 , 11 ,
		18 , 15 , 10 , 14 ,
		9 , 15 , 0 , 14 ,
		21 , 12 , 14 , 17 ,
		14 , 7 , 22 , 13 ,
		8 , 23 , 16 , 8 ,
		6 , 9 , 9 , 11 ,
		17 , 8 , 15 , 8 ,
		8 , 13 , 9 , 11 ,
		17 , 16 , 10 , 14 ,
		8 , 14 , 0 , 11 ,
		21 , 14 , 13 , 17 ,
		15 , 8 , 21 , 15 ,
		6 , 22 , 17 , 9 ,
		7 , 8 , 10 , 10 ,
		18 , 9 , 16 , 8 ,
		8 , 12 , 10 , 10 ,
		16 , 16 , 10 , 14 ,
		8 , 13 , 0 , 9 ,
		21 , 16 , 12 , 17 ,
		16 , 8 , 21 , 17 ,
		5 , 20 , 18 , 10 ,
		8 , 7 , 10 , 10 ,
		18 , 11 , 16 , 9 ,
		8 , 11 , 10 , 10 ,
		15 , 17 , 9 , 13 ,
		8 , 13 , 1 , 7 ,
		20 , 17 , 11 , 17 ,
		17 , 9 , 19 , 19 ,
		3 , 19 , 18 , 11 ,
		9 , 7 , 10 , 9 ,
		18 , 12 , 17 , 10 ,
		9 , 11 , 10 , 9 ,
		14 , 18 , 9 , 13 ,
		8 , 12 , 2 , 5 ,
		18 , 19 , 10 , 17 ,
		17 , 10 , 18 , 20 ,
		2 , 17 , 18 , 12 ,
		10 , 6 , 11 , 9 ,
		18 , 13 , 17 , 11 ,
		9 , 10 , 11 , 9 ,
		13 , 18 , 9 , 12 ,
		8 , 11 , 4 , 3 ,
		17 , 20 , 9 , 16 ,
		17 , 11 , 16 , 21 ,
		1 , 15 , 18 , 13 ,
		11 , 6 , 12 , 9 ,
		18 , 14 , 17 , 12 ,
		9 , 9 , 12 , 9 ,
		12 , 18 , 9 , 11 ,
		8 , 10 , 6 , 2 ,
		15 , 21 , 8 , 15 ,
		17 , 12 , 14 , 22 ,
		1 , 13 , 18 , 14 ,
		12 , 6 , 12 , 9 ,
		17 , 15 , 17 , 13 ,
		10 , 9 , 12 , 9 ,
		11 , 18 , 9 , 11 ,
		9 , 10 , 8 , 1 ,
		14 , 21 , 8 , 15 ,
		17 , 13 , 13 , 22 ,
		1 , 11 , 17 , 15 ,
		14 , 6 , 13 , 9 ,
		16 , 16 , 17 , 14 ,
		11 , 9 , 13 , 9 ,
		9 , 18 , 10 , 10 ,
		9 , 9 , 10 , 0 ,
		12 , 21 , 7 , 14 ,
		17 , 14 , 11 , 22 ,
		1 , 8 , 16 , 16 ,
		15 , 6 , 13 , 9 ,
		16 , 17 , 16 , 15 ,
		11 , 8 , 13 , 9 ,
		8 , 17 , 10 , 10 ,
		10 , 8 , 13 , 0 ,
		10 , 21 , 7 , 13 ,
		16 , 15 , 9 , 21 ,
		2 , 6 , 15 , 17 ,
		16 , 7 , 14 , 10 ,
		15 , 18 , 16 , 16 ,
		12 , 8 , 14 , 10 ,
		8 , 16 , 10 , 10 ,
		11 , 8 , 15 , 0 ,
		8 , 21 , 7 , 12 ,
		16 , 16 , 7 , 21 ,
		4 , 5 , 14 , 18 ,
		17 , 8 , 14 , 10 ,
		13 , 18 , 15 , 16 ,
		13 , 8 , 14 , 10 ,
		7 , 15 , 11 , 9 ,
		11 , 8 , 17 , 1 ,
		7 , 20 , 7 , 11 ,
		15 , 17 , 5 , 19 ,
		5 , 3 , 13 , 18 ,
		17 , 9 , 15 , 10 ,
		12 , 18 , 14 , 17 ,
		13 , 9 , 15 , 10 ,
		6 , 14 , 11 , 9 ,
		12 , 8 , 19 , 2 ,
		5 , 18 , 7 , 10
};
void Descriptors(
		hls::stream<ap_uint<292> > &dataPackStreamIn,
		hls::stream<ap_axiu<512,1,1,1> > &descriptor_out
		)
{
#pragma HLS ARRAY_PARTITION variable=mask complete dim=1
#pragma HLS ARRAY_PARTITION variable=mask complete dim=2

#pragma HLS RESOURCE variable=bit_pattern_25_ core=ROM_1P
#pragma HLS ARRAY_PARTITION variable=bit_pattern_25_ complete dim=2
	unsigned char src_buf0[25][25];
#pragma HLS ARRAY_PARTITION variable=src_buf0 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf0 complete dim=2

	ap_int<24> m_10_int,m_01_int;
	ap_fixed<24,23> m_10,m_01;
	ap_uint<320> descriptor = 0;
	ap_uint<256> desc;
	ap_uint<512> db_desc,shift_db_desc;
	ap_uint<12>posX,posY;
	unsigned char t0, t1;
	unsigned char validFlag;

	for(int ss = 0;ss<HEIGHT*(WIDTH+12);ss++)
	{
		ap_uint<292> datapack = dataPackStreamIn.read();
		ap_uint<200> winVal = datapack.range(199,0);
		m_10_int = datapack.range(223,200);
		m_01_int = datapack.range(247,224);
		posX = datapack.range(259,248);
		posY = datapack.range(271,260);
		validFlag = datapack.range(279,272);
		descriptor.range(31,0) = (int)posX;
		descriptor.range(63,32) = (int)posY;
		read_row_loop:
		for(ap_uint<6> k = 0;k<25;k++)
		{
#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT MAX=25
			for(ap_uint<8> s = 0;s<25;s++)
			{
#pragma HLS UNROLL
				if(k!=24)
				{
					src_buf0[s][k] = src_buf0[s][k+1];
				}
				else
				{
					src_buf0[s][24] = (unsigned char)winVal.range(s*8+7,s*8);
				}
			}
		}

		if(validFlag != 0)
		{
			m_10 = m_10_int;
			m_01 = m_01_int;
			ap_fixed<14,3> angle = hls::atan2(m_01, m_10);
			ap_fixed<14,3> PI = 3.14159265358;
			ap_fixed<14,3> PI_32 = 0.1963495408;
			angle = angle + PI;
			ap_fixed<17,14> div = angle / PI_32;
			ap_uint<8> bias;
			if (div.range(2,2) == 1)
				bias = div.range(7,3) + 1;
			else
				bias = div.range(7,3);

			//float angle = hls::atan2((float)m_01, (float)m_10);
			//float a = (hls::cos(angle)), b = (hls::sin(angle));
			compute_loop:
			for(unsigned int i =0;i<256;i++)
			{
#pragma HLS PIPELINE
				t0 = src_buf0[bit_pattern_25_[i][0] ][bit_pattern_25_[i][1] ];
				t1 = src_buf0[bit_pattern_25_[i][2] ][bit_pattern_25_[i][3] ];
				desc.range(i,i) = t0 < t1;
			}

			db_desc.range(255,0) = desc;
			db_desc.range(511,256) = desc;
			shift_db_desc = db_desc >> (bias<<3);
			descriptor.range(319,64)=shift_db_desc.range(255,0);


			ap_axiu<512,1,1,1> outVal;
			ap_uint<512> temp;
			temp.range(11,0) = datapack.range(291, 280);
			temp.range(331,12) = descriptor;
			temp.range(511,332) = 0;
			outVal.data = temp;
			outVal.keep = 0xffffffffffffffff;
			if(posX == 4095 && posY == 4095)
					outVal.last = 1;
				else
					outVal.last = 0;
			descriptor_out.write(outVal);
		}
	}
}

template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH, int TC, int WIN_SZ, int WIN_SZ_SQ>
void lineProcess(hls::stream<ap_axiu<32,1,1,1> > & _src_mat,
		hls::stream<ap_uint<292> > &dataPackStreamOut,
		XF_SNAME(WORDWIDTH) buf[WIN_SZ][(COLS >> XF_BITSHIFT(NPC))],
		ap_uint<20> FAST_buf[WIN_SZ][(COLS >> XF_BITSHIFT(NPC))],
		XF_PTNAME(DEPTH) src_buf[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)],
		ap_uint<20> FAST_src_buf[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)],
		uint16_t img_width,
		uint16_t img_height,
		ap_uint<13> row_ind[WIN_SZ],
		ap_uint<13> row,
		ap_uint<8> win_size)
{
#pragma HLS INLINE

	XF_SNAME(WORDWIDTH) buf_cop[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable=buf_cop complete dim=1
	ap_uint<20> FAST_buf_cop[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable=FAST_buf_cop complete dim=1

	uint16_t npc = XF_NPIXPERCYCLE(NPC);
	uint16_t col_loop_var = 0;
	ap_uint<200> winVal = 0;
	if(npc == 1)
	{
		col_loop_var = (WIN_SZ>>1);
	}
	else
	{
		col_loop_var = 1;
	}
	for(int extract_px=0;extract_px<WIN_SZ;extract_px++)
	{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
#pragma HLS unroll
		for(int ext_copy=0; ext_copy<npc + WIN_SZ - 1; ext_copy++)
		{
#pragma HLS unroll
			src_buf[extract_px][ext_copy] = 0;
			FAST_src_buf[extract_px][ext_copy] = 0;
		}
	}

	Col_Loop:
	for(ap_uint<13> col = 0; col < ((img_width)>>XF_BITSHIFT(NPC)) + col_loop_var; col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
#pragma HLS LOOP_FLATTEN OFF
		if(row < img_height && col < (img_width>>XF_BITSHIFT(NPC)))
		{
			ap_axiu<32,1,1,1> _src_mat_in = _src_mat.read();
			ap_uint<32> _src_mat_data = _src_mat_in.data;
			buf[row_ind[win_size-1]][col] = _src_mat_data.range(7,0); // Read data
			FAST_buf[row_ind[win_size-1]][col] = _src_mat_data.range(27,8);
		}
		if(NPC == XF_NPPC8)
		{
		}
		else
		{
			for(int copy_buf_var=0;copy_buf_var<WIN_SZ;copy_buf_var++)
			{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
#pragma HLS UNROLL
				if(	(row >(img_height-1)) && (copy_buf_var>(win_size-1-(row-(img_height-1)))))
				{
					buf_cop[copy_buf_var] = buf[(row_ind[win_size-1-(row-(img_height-1))])][col];
					FAST_buf_cop[copy_buf_var] = FAST_buf[(row_ind[win_size-1-(row-(img_height-1))])][col];
				}
				else
				{
					buf_cop[copy_buf_var] = buf[(row_ind[copy_buf_var])][col];
					FAST_buf_cop[copy_buf_var] = FAST_buf[(row_ind[copy_buf_var])][col];
				}
			}
			for(int extract_px=0;extract_px<WIN_SZ;extract_px++)
			{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
#pragma HLS UNROLL
				if(col<img_width)
				{
					src_buf[extract_px][win_size-1] = buf_cop[extract_px];
					FAST_src_buf[extract_px][win_size-1] = FAST_buf_cop[extract_px];
					winVal.range((extract_px<<3)+7,extract_px<<3) = buf_cop[extract_px];
				}
				else
				{
					src_buf[extract_px][win_size-1] = src_buf[extract_px][win_size-2];
					FAST_src_buf[extract_px][win_size-1] = FAST_src_buf[extract_px][win_size-2];
					winVal.range((extract_px<<3)+7,extract_px<<3) = src_buf[extract_px][win_size-2];
				}
			}

			ap_int<24> m_10 = 0;
			ap_int<24> m_01 = 0;
			if ((FAST_src_buf[win_size-1-(WIN_SZ>>1)][win_size-1-(WIN_SZ>>1)].range(8,8) != 0) || (HEIGHT-1 == row-(WIN_SZ>>1) && WIDTH-1 == col-(WIN_SZ>>1)))
			{
				for(ap_int<8> i = 0;i<25;i++)
				{
#pragma HLS PIPELINE
				unsigned char vres0[32];
#pragma HLS ARRAY_PARTITION variable=vres0 complete dim=1
				ap_uint<9> vres1[16];
#pragma HLS ARRAY_PARTITION variable=vres1 complete dim=1
				ap_uint<10> vres2[8];
#pragma HLS ARRAY_PARTITION variable=vres2 complete dim=1
				ap_uint<11> vres3[4];
#pragma HLS ARRAY_PARTITION variable=vres3 complete dim=1
				ap_uint<12> vres4[2];
#pragma HLS ARRAY_PARTITION variable=vres4 complete dim=1
				ap_uint<13> vres5;
				ap_int<19> vres;
					for(ap_uint<6>im = 0;im<32;im++)
					{
						if(im<25)
							vres0[im] = src_buf[i][im]&mask[i][im];
						else
							vres0[im] = 0;
					}
					for(ap_uint<6> i0 = 0;i0<16;i0++)
					{
						vres1[i0] = vres0[i0<<1]+vres0[(i0<<1)+1];
					}
					for(ap_uint<6> i1 = 0;i1<8;i1++)
					{
						vres2[i1] = vres1[i1<<1]+vres1[(i1<<1)+1];
					}
					for(ap_uint<6> i3 = 0;i3<4;i3++)
					{
						vres3[i3] = vres2[i3<<1]+vres2[(i3<<1)+1];
					}
					for(ap_uint<6> i4 = 0;i4<2;i4++)
					{
						vres4[i4] = vres3[i4<<1]+vres3[(i4<<1)+1];
					}
					vres5 = vres4[0]+vres4[1];
					vres = vres5*(i-12);
					m_01 += vres;

					unsigned char ures0[32];
#pragma HLS ARRAY_PARTITION variable=ures0 complete dim=1
					ap_uint<9> ures1[16];
#pragma HLS ARRAY_PARTITION variable=ures1 complete dim=1
					ap_uint<10> ures2[8];
#pragma HLS ARRAY_PARTITION variable=ures2 complete dim=1
					ap_uint<11> ures3[4];
#pragma HLS ARRAY_PARTITION variable=ures3 complete dim=1
					ap_uint<12> ures4[2];
#pragma HLS ARRAY_PARTITION variable=ures4 complete dim=1
					ap_uint<13> ures5;
					ap_int<19> ures;
					for(ap_uint<6> im = 0;im<32;im++)
					{
						if(im<25)
							ures0[im] = src_buf[im][i]&mask[im][i];
						else
							ures0[im] = 0;
					}
					for(ap_uint<6> i0 = 0;i0<16;i0++)
					{
							ures1[i0] = ures0[i0<<1]+ures0[(i0<<1)+1];
					}
					for(ap_uint<6> i1 = 0;i1<8;i1++)
						{
						ures2[i1] = ures1[i1<<1]+ures1[(i1<<1)+1];
					}
					for(ap_uint<6> i3 = 0;i3<4;i3++)
					{
						ures3[i3] = ures2[i3<<1]+ures2[(i3<<1)+1];
					}
					for(ap_uint<6> i4 = 0;i4<2;i4++)
					{
						ures4[i4] = ures3[i4<<1]+ures3[(i4<<1)+1];
					}
					ures5 = ures4[0]+ures4[1];
					ures = ures5*(i-12);
					m_10 += ures;

				}
			}

			ap_uint<92> data_tmp;
			if(HEIGHT-1 == row-(WIN_SZ>>1) && WIDTH-1 == col-(WIN_SZ>>1))
			{
				data_tmp.range(23,0) = m_10;
				data_tmp.range(47,24) = m_01;
				data_tmp.range(59,48) = 4095;
				data_tmp.range(71,60) = 4095;
				data_tmp.range(79,72) = 255;
				data_tmp.range(91,80) = 0;
			}
			else if(col >= (WIN_SZ>>1))
			{
				data_tmp.range(23,0) = m_10;
				data_tmp.range(47,24) = m_01;
				data_tmp.range(59,48) = row-(WIN_SZ>>1);
				data_tmp.range(71,60) = col-(WIN_SZ>>1);
				data_tmp.range(91,72) = FAST_src_buf[win_size-1-(WIN_SZ>>1)][win_size-1-(WIN_SZ>>1)];
			}
			else
			{
				data_tmp = 0;
			}
			ap_uint<292> temp;
			temp.range(199,0) = winVal;
			temp.range(291,200) = data_tmp;
			dataPackStreamOut.write(temp);
			for(int wrap_buf=0;wrap_buf<WIN_SZ;wrap_buf++)
			{
#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
				for(int col_warp=0; col_warp<WIN_SZ-1;col_warp++)
				{
#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
					if(col==0)
					{
						src_buf[wrap_buf][col_warp] = src_buf[wrap_buf][win_size-1];
						FAST_src_buf[wrap_buf][col_warp] = FAST_src_buf[wrap_buf][win_size-1];
					}
					else
					{
						src_buf[wrap_buf][col_warp] = src_buf[wrap_buf][col_warp+1];
						FAST_src_buf[wrap_buf][col_warp] = FAST_src_buf[wrap_buf][col_warp+1];
					}
				}
			}
		}
	} // Col_Loop
}



template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH, int TC,int WIN_SZ, int WIN_SZ_SQ>
void windowSliderProc(hls::stream<ap_axiu<32,1,1,1> > &img,
		hls::stream<ap_uint<292> > &dataPackStreamOut,
		ap_uint<8> win_size,
		uint16_t img_height,
		uint16_t img_width
		)
{
	ap_uint<13> row_ind[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable=row_ind complete dim=1


	uint16_t shift_x = 0;
	ap_uint<13> row, col;

	XF_PTNAME(DEPTH) src_buf[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)];
#pragma HLS ARRAY_PARTITION variable=src_buf complete dim=1
	// src_buf1 et al merged

	ap_uint<20> FAST_src_buf[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)];
#pragma HLS ARRAY_PARTITION variable=FAST_src_buf complete dim=1

	XF_SNAME(WORDWIDTH) buf[WIN_SZ][(COLS >> XF_BITSHIFT(NPC))];
#pragma HLS ARRAY_PARTITION variable=buf complete dim=1
#pragma HLS RESOURCE variable=buf core=RAM_S2P_BRAM

	ap_uint<20> FAST_buf[WIN_SZ][(COLS >> XF_BITSHIFT(NPC))];
#pragma HLS ARRAY_PARTITION variable=FAST_buf complete dim=1
#pragma HLS RESOURCE variable=FAST_buf core=RAM_S2P_BRAM

	//initializing row index

	for(int init_row_ind=0; init_row_ind<win_size; init_row_ind++)
	{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
		row_ind[init_row_ind] = init_row_ind;
	}

	read_lines:
	for(int init_buf=row_ind[win_size>>1]; init_buf <row_ind[win_size-1] ;init_buf++)
	{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
		for(col = 0; col < img_width>>XF_BITSHIFT(NPC) ; col++)
		{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
#pragma HLS pipeline
#pragma HLS LOOP_FLATTEN OFF
			ap_axiu<32,1,1,1> img_in = img.read();
			ap_uint<32> img_data = img_in.data;
			FAST_buf[init_buf][col] = img_data.range(27,8);
			buf[init_buf][col] = img_data.range(7,0);
		}
	}

	//takes care of top borders
	for(col = 0; col < img_width>>XF_BITSHIFT(NPC); col++)
	{
#pragma HLS LOOP_TRIPCOUNT min=TC max=TC
		for(int init_buf=0; init_buf < WIN_SZ>>1;init_buf++)
		{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
#pragma HLS UNROLL
			buf[init_buf][col] = 0;//buf[row_ind[win_size>>1]][col];
			FAST_buf[init_buf][col] = 0;
		}
	}

	Row_Loop:
	for(row = (win_size>>1); row < img_height+(win_size>>1); row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=ROWS max=ROWS

		lineProcess<ROWS, COLS, DEPTH, NPC, WORDWIDTH, TC, WIN_SZ, WIN_SZ_SQ>(img, dataPackStreamOut,buf,FAST_buf, src_buf,FAST_src_buf,img_width, img_height, row_ind, row,win_size);

		//update indices
		ap_uint<13> zero_ind = row_ind[0];
		for(int init_row_ind=0; init_row_ind<WIN_SZ-1; init_row_ind++)
		{
#pragma HLS LOOP_TRIPCOUNT min=WIN_SZ max=WIN_SZ
#pragma HLS UNROLL
			row_ind[init_row_ind] = row_ind[init_row_ind + 1];
		}
		row_ind[win_size-1] = zero_ind;
	} // Row_Loop
}

template<int NMS,int SRC_T,int ROWS, int COLS,int NPC=1>
void windowSlider(
		hls::stream<ap_axiu<32,1,1,1> > &src,
		hls::stream<ap_uint<292> > &dataPackStreamOut)
{
#pragma HLS inline off
#pragma HLS DATAFLOW
	windowSliderProc<ROWS,COLS,XF_DEPTH(SRC_T,NPC),NPC,XF_WORDWIDTH(SRC_T,NPC),(COLS>>XF_BITSHIFT(NPC))+(25>>1),25, 25*25>(src, dataPackStreamOut,25,ROWS,COLS);
}

void RS_BRIEF(hls::stream<ap_axiu<32,1,1,1> > &srcStream,hls::stream<ap_axiu<512,1,1,1> > & descriptor_out)
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis register both port=srcStream
#pragma HLS INTERFACE axis register both port=descriptor_out

#pragma HLS DATAFLOW
	hls::stream<ap_uint<292> > dataPackStream;
	windowSlider<1,XF_8UC1,HEIGHT,WIDTH,1>(srcStream,dataPackStream);
	Descriptors(dataPackStream,descriptor_out);
}
