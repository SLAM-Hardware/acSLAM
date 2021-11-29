/**
* This file is part of ac^2SLAM.
*
* Copyright (C) 2021 Cheng Wang <wangcheng at stu dot xjtu dot edu dot cn> (Xi'an Jiaotong University)
* For more information see <https://github.com/SLAM-Hardware/acSLAM>
*
* ac^2SLAM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ac^2SLAM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ac^2SLAM. If not, see <http://www.gnu.org/licenses/>.
*/

// #include "img_gray.h"
#include "FAST.h"
#include <fstream>
using namespace std;
int main()
{
    ifstream ifile;
    ifile.open("../../../../../resize/result.txt");
    int width, height;
    ifile >> width;
    ifile >> height;
    cout << width << " " << height << endl;

    hls::stream<ap_axiu<32, 1, 1, 1> > cfgStream;
    hls::stream<ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > srcStream;
    hls::stream<ap_axiu<32, 1, 1, 1> > cfgoutStream;
    hls::stream<ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > outPixelStream;
    hls::stream<ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > outFASTStream;

    ap_axiu<32, 1, 1, 1> cfgin;
    cfgin.data = width;
    cfgin.keep = 0xF;
    cfgin.last = 0;
    cfgStream.write(cfgin);
    cfgin.data = height;
    cfgin.keep = 0xF;
    cfgin.last = 1;
    cfgStream.write(cfgin);

    ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> src;
    ap_uint<INPUT_BIT> data = 0;

    int cnt = 0;
    for (int i = 0; i< width*height; i++)
    {
        int p;
        ifile >> p;
        data.range((cnt+1)*PIXEL_BIT-1, cnt*PIXEL_BIT) = p; //img_gray[i/width][i%width];
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


    FAST(cfgStream, srcStream, cfgoutStream, outPixelStream, outFASTStream);

    int new_width = cfgoutStream.read().data;
    int new_height = cfgoutStream.read().data;
    cout << new_width << endl;
    cout << new_height << endl;

    ap_uint<PIXEL_BIT> new_img[HEIGHT][WIDTH];
    ap_uint<PIXEL_BIT> FAST_buf[HEIGHT][WIDTH];
    ap_axiu<OUTPUT_STREAM_BIT,1,1,1> outData;
    ap_axiu<OUTPUT_STREAM_BIT,1,1,1> outData1;
    cnt = 0;

    ofstream ofile;
    ofile.open("../../../../result.txt");
    ofile << new_width << endl;
    ofile << new_height << endl;
    for (int row = 0; row < new_height; row++){
        cnt = 0;
        for (int col = 0; col < ceil(float(new_width) / INPUT_PIXEL_NUM); col++){
            outData = outPixelStream.read();
            ap_uint<OUTPUT_BIT> tmp = outData.data;

            outData1 = outFASTStream.read();
            ap_uint<OUTPUT_BIT> tmp1 = outData1.data;

            ofile << tmp << " " << tmp1 << endl;

            for (int i=0; i<OUTPUT_PIXEL_NUM; i++)
            {
                if (cnt < new_width)	
                {
                    new_img[row][cnt] = tmp.range(PIXEL_BIT-1,0);
                    tmp = tmp >> PIXEL_BIT;
                    FAST_buf[row][cnt] = tmp1.range(PIXEL_BIT-1,0);
                    tmp1 = tmp1 >> PIXEL_BIT;
                }
                cnt++;
            }
        }
    }
    ofile.close();

    ofile.open("../../../../result_test.txt");
    ofile << new_width << endl;
    ofile << new_height << endl;
    for (int i = 0; i < new_height; i++)
    {
        for (int j = 0; j < new_width; j++)
        {
            cout << FAST_buf[i][j] << " ";
            ofile << new_img[i][j] << " " << FAST_buf[i][j] << endl;
        }
        cout << endl;
    }
    ofile.close();
}
