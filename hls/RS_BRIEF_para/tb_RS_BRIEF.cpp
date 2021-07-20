#include "img_gray.h"
#include "FAST.h"
#include "RS_BRIEF.h"
#include <fstream>
using namespace std;
int main()
{
	// int width = 111;
	// int height = 111;
	ifstream ifile;
	ifile.open("C:/slam/HLS_project/fast_para_copy/result.txt");
	int width, height;
	ifile >> width;
	ifile >> height;
	cout << width << " " << height << endl;

	hls::stream<ap_axiu<32, 1, 1, 1> > cfgStream;
	hls::stream<ap_axiu<INPUT_BIT, 1, 1, 1> > srcPixelStream;
	hls::stream<ap_axiu<INPUT_BIT, 1, 1, 1> > srcFASTStream;
	hls::stream<ap_axiu<512, 1, 1, 1> > outStream;

	ap_axiu<32, 1, 1, 1> cfgin;
	cfgin.data = width;
	cfgin.keep = 0xF;
	cfgin.last = 0;
	cfgStream.write(cfgin);
	cfgin.data = height;
	cfgin.keep = 0xF;
	cfgin.last = 1;
	cfgStream.write(cfgin);

	ap_axiu<INPUT_BIT, 1, 1, 1> pixel_src;
    ap_axiu<INPUT_BIT, 1, 1, 1> FAST_src;
	ap_uint<INPUT_BIT> pixel_data = 0;
    ap_uint<INPUT_BIT> FAST_data = 0;

// 	for (int i = 0; i < height * ceil(float(width) / INPUT_PIXEL_NUM); i++){
// 		ap_uint<INPUT_BIT> read_i, read_F;
// 		ifile >> read_i >> read_F;
// //		cout << read_i << " " << read_F <<endl;
// 		pixel_src.data = read_i;
// 		FAST_src.data = read_F;
// 		if (i == height * ceil(float(width) / INPUT_PIXEL_NUM)-1)
// 		{
// 			pixel_src.last = 1;
// 			FAST_src.last = 1;
// 		}
// 		else
// 		{
// 			pixel_src.last = 0;
// 			FAST_src.last = 0;
// 		}
// 		srcPixelStream.write(pixel_src);
// 		srcFASTStream.write(FAST_src);
// 	}

	int cnt = 0;
	for (int i = 0; i< width*height; i++)
	{
		pixel_data.range((cnt+1)*PIXEL_BIT-1, cnt*PIXEL_BIT) = img_gray[i/width][i%width];
		FAST_data.range((cnt+1)*PIXEL_BIT-1, cnt*PIXEL_BIT) = FAST[i/width][i%width];
		cnt++;
		if (cnt == INPUT_PIXEL_NUM)
		{
			pixel_src.data = pixel_data;
			FAST_src.data = FAST_data;
			pixel_src.keep = 0xFFFFFFFFFFFFFFFF;
			FAST_src.keep = 0xFFFFFFFFFFFFFFFF;
			if (i == width*height-1)
            {
				pixel_src.last = 1;
				FAST_src.last = 1;
            }
			else
            {
				pixel_src.last = 0;
				FAST_src.last = 0;
            }
			srcPixelStream.write(pixel_src);
			srcFASTStream.write(FAST_src);
			cnt = 0;
			pixel_data= 0;
			FAST_data= 0;
		}
	}
	if (cnt > 0)
	{
        pixel_src.data = pixel_data;
        FAST_src.data = FAST_data;
        pixel_src.keep = 0xFFFFFFFFFFFFFFFF;
        FAST_src.keep = 0xFFFFFFFFFFFFFFFF;
        pixel_src.last = 1;
        FAST_src.last = 1;
        srcPixelStream.write(pixel_src);
        srcFASTStream.write(FAST_src);
	}
	ifile.close();

	RS_BRIEF(cfgStream, srcPixelStream, srcFASTStream, outStream);
	ap_axiu<512, 1, 1, 1> outVal = outStream.read();

	int kp_num = 0;
	while (outVal.last!=1)
	{
		ap_uint<512> outValData = outVal.data;
		cout << "FASTscore\t" << outValData.range(6, 0) << endl;
		cout << "angle\t" << outValData.range(15, 7) << endl;
		cout << "row_ind\t" << outValData.range(16+HEIGHT_BIT-1, 16) << endl;
		cout << "col_ind\t" << outValData.range(16+HEIGHT_BIT+WIDTH_BIT-1, 16+HEIGHT_BIT) << endl;
		cout << "desc\t";
		for (int i = 0; i < 256/32; i++)
			cout << outValData.range(16+HEIGHT_BIT+WIDTH_BIT+(i+1)*32-1, 16+HEIGHT_BIT+WIDTH_BIT+i*32) << " ";
		cout<<"\n----------------------\n";

		// cout << outValData.range(16+HEIGHT_BIT-1, 16) << " " << outValData.range(16+HEIGHT_BIT+WIDTH_BIT-1, 16+HEIGHT_BIT)  <<endl;
		kp_num++;
		outVal = outStream.read();
	}
	cout << "total " << kp_num << " keypoints" <<endl;

	//  for (int i = 0; i< width*height; i++)
	//  {
	//  	if (FAST[i/width][i%width]  & 1)
	//  		cout << i/width << " " << i%width << endl;
	//  }
}
