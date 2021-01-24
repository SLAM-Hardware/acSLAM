#include <iostream>
#include <bitset>
#include "RS_BRIEF.h"
#include "img_gray.h"
using namespace std;

int main(int argc, char **argv)
{
    hls::stream<ap_axiu<32,1,1,1> > srcStream;
    hls::stream<ap_axiu<512,1,1,1> >  descriptor_out;
//#ifdef HW_COSIM
   // Run the AutoESL matrix multiply block

//#endif
    for (int i=0;i<HEIGHT;i++)
        for (int j=0;j<WIDTH;j++)
        {
            ap_axiu<32,1,1,1> src;
            src.data.range(7,0) = img_gray[i][j];
            if (i == 12 && j ==12)
            	src.data.range(28,8)=0xbbb01;
            else
            	src.data.range(28,8)=0;
            src.data.range(31,29) = 0;
            src.keep=0xf;
            if (i==HEIGHT-1&&j==WIDTH-1)
                src.last=1;
            else
                src.last=0;
            srcStream.write(src);
   //         cout <<bitset<16>(src.data)<<endl;
        }
    RS_BRIEF(srcStream,descriptor_out);
    cout <<"-----------------------------------------------------\n";
    for (int i=0;i<2;i++)
//        for (int j=0;j<40;j++)
        {
            ap_axiu<512,1,1,1> out;
            out=descriptor_out.read();
            for (int n=9;n>=0;n--)
                cout << bitset<32>(out.data.range((n<<5)+31+12,(n<<5)+12));
            cout << bitset<12>(out.data.range(11,0));
            cout<<endl;
        }
    
}

