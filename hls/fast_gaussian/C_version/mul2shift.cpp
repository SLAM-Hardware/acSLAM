#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <chrono>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

#define NUM_IMG 1
#define ROWS 480
#define COLS 640

#define TEST_SW
#define TEST_HW_SIM

int main(int argc, char **argv) {
    const int weight[7][7] = {
        0 , 0 , 1 , 1 , 1 , 0 , 0 ,
        0 , 2 , 5 , 7 , 5 , 2 , 0 ,
        1 , 5 , 13 , 17 , 13 , 5 , 1 ,
        1 , 7 , 17 , 23 , 17 , 7 , 1 ,
        1 , 5 , 13 , 17 , 13 , 5 , 1 ,
        0 , 2 , 5 , 7 , 5 , 2 , 0 ,
        0 , 0 , 1 , 1 , 1 , 0 , 0 ,
    };
    const int input[7][7] = {100};
    const int w[7] = {
        18,34,49,55,49,34,18
    };

    double w_m[7][7];

    for (int i=0; i<7; i++){
        for (int j=0; j<7; j++){
            w_m[i][j] = w[i]*w[j];
        }
    }

/*    for (int i=0; i<7; i++){
        for (int j=0; j<7; j++){
            cout << w_m[i][j]/65536 << " ";
        }
        cout << endl;
    }
*/

#ifdef TEST_SW
    int input_base[7] = {1,2,5,7,13,17,23};
    for(int input_ptr = 0; input_ptr < 7; input_ptr++){
        int exp_binary[7]={0};
        int input = input_base[input_ptr];
        int bit_ptr = 0;
        int itr_ub = log2(input)+1;
        int base = 8;

        for(int _itr = 0; _itr < itr_ub ; _itr++){
            if(input % 2 == 1) {
                exp_binary[bit_ptr] = 1;
            }
            else
            {
                exp_binary[bit_ptr] = 0;
            }
            input = input/ 2;
            bit_ptr++;
        }

    //    int i = 0;
    //    while(i++ < 8){
    //        cout << "'0', " ;
    //    }
        if(exp_binary[0]) cout << "sum[" << input_ptr << "] = (psum["<< input_ptr << "] >> " << base << ')';
        else   cout << "sum[" << input_ptr << "]" ;
        for(int i = 1 ; i< 7; i++){
            if(exp_binary[i]){
                char mask[16] ={'0', '0', '0', '0', '0', '0', '0', '0','0', '0', '0', '0', '0', '0', '0', '0'};
                int itr = 0;
                while (itr < 16){
                    if(itr < (16-i)) mask[itr] = '1';
                    else mask[itr] = '0';
                    itr++;
                }
                if(i==1 && exp_binary[0]==0 ){
                      cout << " = ((psum[" << input_ptr << "] >> " << base - i << ") & 0b" ;
                }
                else  cout << " + ((psum[" << input_ptr << "] >> " << base - i << ") & 0b" ;
                itr = 0;
                while(itr< 16){
                    cout<< mask[itr]   ;
                    itr++;
                }
                cout << ")";
            }
        }
        cout << ";" << endl;
        cout << endl << endl;
    }
/*
    int mod_;
    if(mod_){
        bit_ptr++;
        char* mask;
        for (int bit = 7; bit>bit_ptr; bit++){
            mask[bit] =  (char)bit;
        }
        cout << "(psum[] >> " << bit_ptr << ") & 0x" << mask << endl;
    }
    input = input/2;*/
    double sum = 0;
    for(int i = 0; i < 7; i++){
        for(int j=0; j<7; j++){
//            cout << input[i][j] << " ";
            sum+= 100* ((double)weight[i][j]/pow(2,8));
        }
    }

//    sum = sum >> 8;
    cout << sum;

    int sum_int = 0;
    for(int i = 0; i < 7; i++){
        for(int j=0; j<7; j++){
//            cout << input[i][j] << " ";
            sum_int+= 100* weight[i][j];
        }
    }

//    sum = sum >> 8;
    cout << sum_int;

#endif
    unsigned int winReg[7][7];
    for(int i = 0; i < 7; i++){
        for(int j=0; j<7; j++){
//            cout << input[i][j] << " ";
            winReg[i][j] = 100;
        }
    }

    unsigned int psum[6];
    unsigned int S_psum[7];
    unsigned int S_psum2[7];
    unsigned int S_psum3[7];
//	psum[0] = ((100 + 100) + (100 + 100)) + ((100 + 100) + (100 + 100)) + ((100 + 100) + (100 + 100));
    psum[0] = ((winReg[0][2] + winReg[0][3]) + (winReg[0][4] + winReg[6][2])) + ((winReg[6][3] + winReg[6][4]) + (winReg[2][6] + winReg[3][6])) + ((winReg[4][6] + winReg[2][0]) + (winReg[3][0] + winReg[4][0]));
    psum[1] = (winReg[1][1] + winReg[1][5]) + (winReg[5][1] + winReg[5][5]);
    psum[2] = ((winReg[1][2] + winReg[1][4]) + (winReg[2][1] + winReg[4][1])) + ((winReg[2][5] + winReg[4][5]) + (winReg[5][2] + winReg[5][4]));
    psum[3] = (winReg[1][3] + winReg[3][1]) + (winReg[5][3] + winReg[3][5]);
    psum[4] = (winReg[2][2] + winReg[4][2]) + (winReg[2][4] + winReg[4][4]);
    psum[5] = (winReg[2][3] + winReg[3][2]) + (winReg[4][3] + winReg[3][4]);

//    S_psum[0] = (psum[0] >> 8);
//    S_psum[1] = (psum[1] >> 7) & 0xfe;
//    S_psum[2] = (psum[2] >> 8) + ((psum[2] >> 6) & 0xfc);
//    S_psum[3] = (psum[3] >> 8) + ((psum[3] >> 7) & 0xfe) + ((psum[3] >> 6) & 0xfc);
//    S_psum[4] = (psum[4] >> 8) + ((psum[4] >> 6) & 0xfc) + ((psum[4] >> 5) & 0xf8);
//    S_psum[5] = (psum[5] >> 8) + ((psum[5] >> 4) & 0xf0);
//    S_psum[6] =((winReg[3][3] >> 8) + ((winReg[3][3] >> 7) & 0xfe)) + (((winReg[3][3] >> 6) & 0xfc) + ((winReg[3][3] >> 4) & 0xf0));

//    S_psum[0] = (psum[0] >> 8);
//    S_psum[1] = (psum[1] >> 7);
//    S_psum[2] = (psum[2] >> 8) + ((psum[2] >> 6));
//    S_psum[3] = (psum[3] >> 8) + ((psum[3] >> 7)) + ((psum[3] >> 6));
//    S_psum[4] = (psum[4] >> 8) + ((psum[4] >> 6)) + ((psum[4] >> 5));
//    S_psum[5] = (psum[5] >> 8) + ((psum[5] >> 4));
//    S_psum[6] =((winReg[3][3] >> 8) + ((winReg[3][3] >> 7))) + (((winReg[3][3] >> 6)) + ((winReg[3][3] >> 4)));

    S_psum[0] = psum[0];
    S_psum[1] =  psum[1] <<1 ;
    S_psum[2] = psum[2] + (psum[2] << 2);
    S_psum[3] = psum[3] + (psum[3] << 1) +( psum[3] <<2 );
    S_psum[4] = psum[4]  + (psum[4] <<2 ) + (psum[4] << 3);
    S_psum[5] = psum[5]  +( psum[5] << 4);
    S_psum[6] =winReg[3][3]  + (winReg[3][3] << 1) + (winReg[3][3] << 2) + (winReg[3][3] << 4);

    S_psum2[0] = (psum[0] * weight[0][2]) >> 8;
    S_psum2[1] = (psum[1] * weight[1][1])>> 8;
    S_psum2[2] = (psum[2] * weight[1][2])>> 8;
    S_psum2[3] = (psum[3]  * weight[1][3])>> 8;
    S_psum2[4] = (psum[4]  * weight[2][2])>> 8;
    S_psum2[5] = (psum[5]  * weight[2][3])>> 8;
    S_psum2[6] = (winReg[3][3] * weight[3][3])>> 8;

    S_psum3[0] = (psum[0] * weight[0][2]);
    S_psum3[1] = (psum[1] * weight[1][1]);
    S_psum3[2] = (psum[2] * weight[1][2]);
    S_psum3[3] = (psum[3]  * weight[1][3]);
    S_psum3[4] = (psum[4]  * weight[2][2]);
    S_psum3[5] = (psum[5]  * weight[2][3]);
    S_psum3[6] =(winReg[3][3] * weight[3][3]);
//	sum = psum * weights[0] * weights[0];
    unsigned int simsim =((S_psum[0] + S_psum[1]) + (S_psum[2] + S_psum[3])) + ((S_psum[4] + S_psum[5]) + S_psum[6]);
    unsigned int simsim2 =((S_psum2[0] + S_psum2[1]) + (S_psum2[2] + S_psum2[3])) + ((S_psum2[4] + S_psum2[5]) + S_psum2[6]);
    unsigned int simsim3 =((S_psum3[0] + S_psum3[1]) + (S_psum3[2] + S_psum3[3])) + ((S_psum3[4] + S_psum3[5]) + S_psum3[6]);
    simsim3 = simsim3 >> 8;
    simsim = simsim >> 8;
    cout << endl << simsim ;
    return 0;
}