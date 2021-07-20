#include "resize.h"
#include "img_gray.h"
#include <fstream>
using namespace std;
int main()
{
	int width = 1241;
	int height = 376;

	hls::stream<ap_axiu<32, 1, 1, 1> > cfgStream;
	hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > srcStream;
	hls::stream<ap_axiu<32, 1, 1, 1> > cfgoutStream;
	hls::stream<ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > outStream;

	ap_axiu<32, 1, 1, 1> cfgin;
	cfgin.data = width;
	cfgin.keep = 0xF;
	cfgin.last = 0;
	cfgStream.write(cfgin);
	cfgin.data = height;
	cfgin.keep = 0xF;
	cfgin.last = 0;
	cfgStream.write(cfgin);
	ap_ufixed<16, 2> scale_in = SCALE;
	cfgin.data.range(15, 0) = scale_in.range(15, 0);
	cfgin.data.range(31, 16) = 0;
	cfgin.keep = 0xF;
	cfgin.last = 0;
	cfgStream.write(cfgin);
	scale_in = 1/SCALE;
	cfgin.data.range(15, 0) = scale_in.range(15, 0);
	cfgin.data.range(31, 16) = 0;
	cfgin.keep = 0xF;
	cfgin.last = 1;
	cfgStream.write(cfgin);

	ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> src;
	ap_uint<INPUT_BIT> data = 0;

	int cnt = 0;
	for (int i = 0; i< width*height; i++)
	{
		data.range((cnt+1)*PIXEL_BIT-1, cnt*PIXEL_BIT) = img_gray[i/width][i%width];
		cnt++;
		if (cnt == INPUT_PIXEL_NUM)
		{
			src.data = data;
			src.keep = 0xFFFFFFFFFFFFFFFF;
			if (i == width*height-1)
				src.last = 1;
			else
				src.last = 0;
			srcStream.write(src);
			cnt = 0;
			data= 0;
		}
	}
	if (cnt > 0)
	{
		src.data = data;
		src.keep = 0xFFFFFFFFFFFFFFFF;
		src.last = 1;
		srcStream.write(src);
	}


	resize(cfgStream, srcStream, cfgoutStream, outStream);

	int new_width = cfgoutStream.read().data;
	int new_height = cfgoutStream.read().data;
	cout << new_width << endl;
	cout << new_height << endl;

	ap_uint<PIXEL_BIT> new_img[HEIGHT][WIDTH];
	ap_axiu<OUTPUT_STREAM_BIT,1,1,1> outData;
	cnt = 0;
	do
	{
		outData = outStream.read();
		ap_uint<OUTPUT_BIT> tmp = outData.data;
		for (int i=0; i<OUTPUT_PIXEL_NUM; i++)
		{
			if (cnt < new_height*new_width)
			{
				new_img[cnt/new_width][cnt%new_width] = tmp.range(PIXEL_BIT-1,0);
				tmp = tmp >> PIXEL_BIT;
			}
			cnt++;
		}
	}while (outData.last == 0 && cnt < new_height*new_width);

	for (int i = 0; i < new_height; i++)
	{
		for (int j = 0; j < new_width; j++)
			 cout << new_img[i][j] << " ";
//			if (new_img[i][j]!=img_gray[i][j])
//				cout << "error" << endl;
		 cout << endl;
	}

	ofstream ofile;
	ofile.open("C:/slam/HLS_project/resize/result.txt");
	ofile << new_width << endl;
	ofile << new_height << endl;
	for (int i = 0; i < new_height; i++)
	{
		for (int j = 0; j < new_width; j++)
			ofile << new_img[i][j] << endl;
	}

	cout <<"-----------------------finish-------------------------";
}
