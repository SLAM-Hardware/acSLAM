#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <string>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>

using namespace cv;
//using namespace cv::features2d;

#define WRITE_IAMGE2FILE
//#define READ_INCLUDE_EXT_FILE

//#define  READ_EXTERN_FILE_IFSTREAM
//#define MALLOC_READ_EXTERN_FILE_IFSTREAM
//#define TEST_FAST
//#define TEST_FAST2
//#define TEST_FAST_FULL
//#define FAST_EXTRACT
using namespace std;

//#include <iomanip>
#define NUM_IMG 1
#define ROWS 480
#define COLS 640

//void getFiles(string path, vector<string> & files);
int main(int argc, char **argv) {

#ifdef WRITE_IAMGE2FILE
    // write image to a file (comma separated file)
    cv::Mat image;
    string path = "/home/cag/work/pynq/ORB_FPGA_jianming/image/";
    string path_image = "/home/cag/work/pynq/ORB_FPGA_July24/pynq_notebook/test_data/";
    ofstream ofile;

    for (int i = 0; i < NUM_IMG; i++) {
        stringstream s1;
        s1 << i;
        ofile.open(path + s1.str() + ".txt");
        stringstream ss;
        ss << i << ".jpg";
//        cout << path_image+ss.str();
        image = cv::imread(path_image + ss.str(), cv::IMREAD_GRAYSCALE);
//        cv::imshow("sd",image);
//        cv::waitKey();
//        cout << image.data;
        std::vector<KeyPoint> kp;
        FAST(image, kp, 40, true);
        for( auto kp_i: kp){
            cv::circle(image, kp_i.pt, 3, 255, 1, LINE_8, 0);
        }
        cv::imshow("win", image);
        cv::waitKey(0);

// write the whole image
        for (int row = 0 ; row < image.rows; row++){
            uchar* pImg = image.ptr<uchar>(row);
            for (int col = 0 ; col < image.cols; col++){
                ofile << setw(3) << setbase(10) << int(pImg[col]);
                if (col==image.cols-1 && row==image.rows-1) {}
                else  ofile << setw(1) << ", " ;// pImg[col];//image.at<uchar>(row,col);
            }
//            if(row != image.rows-1)  ofile << endl;
        }


        // write the leftupper 20*20
     /*   for (int row = 0; row < ROWS; row++) {
            uchar *pImg = image.ptr<uchar>(row);
            for (int col = 0; col < COLS; col++) {
                ofile << setw(3) << setbase(10) << int(pImg[col]);
                if (col == COLS - 1 && row == ROWS - 1) {}
                else ofile << setw(1) << ", ";// pImg[col];//image.at<uchar>(row,col);
            }
//            if(row != image.rows-1)  ofile << endl;
        }
        ofile.close();*/

    }

#endif



    // read from file method 1 include example
#ifdef READ_INCLUDE_EXT_FILE
    int dataIn[ROWS][COLS] = {
#include </home/cag/work/pynq/ORB_FPGA_jianming/image/0.txt>
    };

    for (int row= 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            cout << dataIn[row][col] << " ";
        }
        cout << endl;
    }

#endif

// this example could only read string from input file.
#ifdef READ_EXTERN_FILE_IFSTREAM
    int  dataIn[ROWS][COLS];
    ifstream idata("/home/cag/work/pynq/ORB_FPGA_jianming/image/0.txt");
    int row = 0;
    int col = 0;
    int cnt = 0;
    string data_out;
    while(getline( idata, data_out, ',')) {
        stringstream ss;
        ss << data_out;
        int data;
        ss >> data;
        cout << data << " ";
cnt++;
//        dataIn[row][col] = data;
//        if(col!=COLS-1) col++;
//        else{
//            row++;
//            col = 0;
//        }
    }
    cout << endl;
    cout << cnt << endl;
cout << row << " " << col << endl;
//    for(int row=0; row< ROWS; row++){
//        for(int col = 0; col < COLS; col++){
//            cout << dataIn[row][col] << " " ;
//        }
//        cout << endl;
//    }
#endif


#ifdef MALLOC_READ_EXTERN_FILE_IFSTREAM
    int* dataIn = (int*) malloc(ROWS*COLS*sizeof(int));
//    int *dataIn = new int[ROWS*COLS];
    ifstream idata("/home/cag/work/pynq/ORB_FPGA_jianming/image/0.txt");
    int cnt = 0, idx = 0;
    string data_out;
    while(getline( idata, data_out, ',')) {
        stringstream ss;
        ss << data_out;
        int data;
        ss >> data;
//        cout << data << " ";
        cnt++;
        dataIn[idx] = data;
        idx++;
    }

    cout << endl << cnt << endl;
    for(int row=0; row< ROWS; row++){
        for(int col = 0; col < COLS; col++){
            cout << dataIn[row*COLS+col] << " " ;
        }
        cout << endl;
    }
#endif


#ifdef TEST_FAST

    //    int* dataIn = (int*) malloc(ROWS*COLS*sizeof(int));
        ofstream file;
        file.open("result.dat");
        int *dataIn = new int[ROWS*COLS];
        ifstream idata("/home/cag/work/pynq/ORB_FPGA_jianming/image/0.txt");
        int cnt = 0, idx = 0;
        string data_out;
        while(getline( idata, data_out, ',')) {
            stringstream ss;
            ss << data_out;
            int data;
            ss >> data;
            cnt++;
            dataIn[idx] = data;
            idx++;
        }

#define WIN_SZ 7
#define NPC 1
#undef ROWS
#undef COLS
#define ROWS 20
#define COLS 20
        int lineBuffer[WIN_SZ][COLS] = {0};
        idx=0;
        for (int row =  WIN_SZ >> 1; row < WIN_SZ - 1; row++){
            for(int col = 0; col < COLS; col++){
                lineBuffer[row][col] = dataIn[idx++];
            }
        }
        int rowIdx[WIN_SZ];
        // process all rows of input images
        ROW_IMAGE:for (int rowImage = 0; rowImage < ROWS; rowImage++){
            // set up line buffer pointer
            if(rowImage == 0){
                for (int i = 0; i < WIN_SZ; i++ ){
                    rowIdx[i] = i;
                }
            }
            else
            {
                int temp = rowIdx[0];
                for (int i = 0; i < WIN_SZ-1; i++ ){
                    rowIdx[i] = rowIdx[i+1];
                }
                rowIdx[WIN_SZ-1] = temp;
            }

            // initialize register window in the corresponding row
            int winReg[WIN_SZ][WIN_SZ + NPC -1];
            for (int row = 0; row < WIN_SZ-1; row++){
                for (int col = WIN_SZ >> 1; col < WIN_SZ; col++){
                    winReg[row][col] = lineBuffer[rowIdx[row]][col];
                }
            }

            // udpate the first three column value in the rowImage + WIN_SZ>>1 line.
            for (int col =WIN_SZ>>1; col < WIN_SZ; col++){
                int data_temp = dataIn[idx++];
                lineBuffer[rowIdx[WIN_SZ-1]][col] = data_temp;
                winReg[WIN_SZ-1][col] = data_temp;
            }

            COL_IMAGE:for (int colImage = 0; colImage < COLS; colImage++){

                // process FAST
                // here the 9 should be the length of the input + 1;
        int diff[16]={0};

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
                int posJudge[24]={0};
                int negJudge[24]={0};
                int threshold = 40;
                for (int i = 0; i < 16; i++){

                    //
                    posJudge[i] = (diff[i] >  threshold) ? 1:0;
                    negJudge[i] = (diff[i] < -threshold) ? 1:0;
                }

                for (int i = 0; i < 8; i++)
                {
                    //
                    posJudge[i+16] = (diff[i] >  threshold) ? 1:0;
                    negJudge[i+16] = (diff[i] < -threshold) ? 1:0;
                }

                // judge whether the center point is feature point (fp).

                int fpJudge[16];
                for (int i=0; i< 16; i++){
    //
                    fpJudge[i] = (posJudge[i]&posJudge[i+1]&posJudge[i+2]&posJudge[i+3]&posJudge[i+4]&posJudge[i+5]&posJudge[i+6]&posJudge[i+7])|(negJudge[i]&negJudge[i+1]&negJudge[i+2]&negJudge[i+3]&negJudge[i+4]&negJudge[i+5]&negJudge[i+6]&negJudge[i+7]);
                }

                int fpValid;
                // read new line && update line indicator/

                fpValid = (fpJudge[0]|fpJudge[1]|fpJudge[2]|fpJudge[3]|fpJudge[4]|fpJudge[5]|fpJudge[6]|fpJudge[7]);

                // output value
                int fpOut[2];

                fpOut[1] = fpValid;
                fpOut[0] = winReg[3][3];
                cout << setbase(10) << fpOut[1] << " " <<  fpOut[0] << endl;
                file << setbase(10) << fpOut[1] << " " <<  fpOut[0] << endl;

                // update line buffer & winReg
                if (colImage <= COLS - WIN_SZ>>1 - 1){
                    int newPixelIn = dataIn[idx++];

                    lineBuffer[rowIdx[WIN_SZ-1]][colImage + WIN_SZ>>1 + 1] = newPixelIn;
                    winReg[WIN_SZ-1][WIN_SZ-1] = newPixelIn;

                    for (int row =0; row < WIN_SZ; row++){
    //
                        for (int col =0; col < WIN_SZ - 1; col++){
    //
                            winReg[row][col] = winReg[row][col+1];
                        }
                    }

                    for (int row_win = 0; row_win < WIN_SZ - 1; row_win++){
    //
                        winReg[row_win][WIN_SZ-1] = lineBuffer[rowIdx[row_win]][colImage + WIN_SZ>>1 +1];
                    }
                }

                else{
                    for (int row =0; row < WIN_SZ; row++){
    //
                        for (int col =0; col < WIN_SZ - 1; col++){
    //
                            winReg[row][col] = winReg[row][col+1];
                        }
                    }

                    for (int row_win = 0; row_win < WIN_SZ; row_win++){
    //
                        winReg[row_win][WIN_SZ-1] = 0;
                    }
                }
            }
        }
        cout<< endl<< idx;
#endif


#ifdef TEST_FAST2

    //    int* dataIn = (int*) malloc(ROWS*COLS*sizeof(int));
    ofstream file;
    file.open("result.dat");
    int *dataIn = new int[ROWS*COLS];
    ifstream idata("/home/cag/work/pynq/ORB_FPGA_jianming/image/0.txt");
    int cnt = 0, idx = 0;
    string data_out;
    while(getline( idata, data_out, ',')) {
        stringstream ss;
        ss << data_out;
        int data;
        ss >> data;
        cnt++;
        dataIn[idx] = data;
        idx++;
    }

#define WIN_SZ 7
#define NPC 1
#undef ROWS
#undef COLS
#define ROWS 20
#define threshold 40
#define COLS 20
    
        int col_loop_var = NPC;
        idx = 0;
        int row_cout_idx = 3;
        // row index initialization
        int rowIdx[WIN_SZ];

        // initialize the line_buffer
        int lineBuffer[WIN_SZ][COLS] = {0};

        for (int row =  WIN_SZ >> 1; row < WIN_SZ - 1; row++){
            for(int col = 0; col < COLS; col++){
                lineBuffer[row][col] = dataIn[idx++];
            }
        }
        cout <<endl<< idx << ' ' << row_cout_idx;
        // process all rows of input images
        ROW_IMAGE:
    for (int rowImage = 0; rowImage < ROWS; rowImage++){
        // set up line buffer pointer
        if(rowImage == 0){
            for (int i = 0; i < WIN_SZ; i++ ){
                rowIdx[i] = i;
            }
        }
        else
        {
            int temp = rowIdx[0];
            for (int i = 0; i < WIN_SZ-1; i++ ){
                rowIdx[i] = rowIdx[i+1];
            }
            rowIdx[WIN_SZ-1] = temp;
        }

        // initialize WIN_REG in the corresponding row
        int winReg[WIN_SZ][WIN_SZ + NPC -1];
        for (int row = 0; row < WIN_SZ-1; row++){
            for (int col = (WIN_SZ >> 1); col < WIN_SZ; col++){
                winReg[row][col] = lineBuffer[rowIdx[row]][col];
            }
        }

        // read new data into linebuffer & update the last row of WIN_REG
        if(rowImage + (WIN_SZ>>1) < ROWS ){
            for (int col =(WIN_SZ>>1); col < WIN_SZ; col++){
                int data_temp =dataIn[idx++];
                lineBuffer[rowIdx[WIN_SZ-1]][col] = data_temp;
                winReg[WIN_SZ-1][col] = data_temp;
            }
        }
        else{
            for (int col =(WIN_SZ>>1); col < WIN_SZ; col++){
                lineBuffer[rowIdx[WIN_SZ-1]][col] = 0;
                winReg[WIN_SZ-1][col] = 0;
            }
        }


        COL_IMAGE:
        for (int colImage = 0; colImage < COLS; colImage++){
            // process FAST
            // here the 9 should be the length of the input + 1;
            int diff[16]={0};

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
            int posJudge[24]={0};
            int negJudge[24]={0};
            {
                for (int i = 0; i < 16; i++){


                    posJudge[i] = (diff[i] >  threshold) ? 1:0;
                    negJudge[i] = (diff[i] < -threshold) ? 1:0;
                }

                for (int i = 0; i < 8; i++)
                {

                    posJudge[i+16] = (diff[i] >  threshold) ? 1:0;
                    negJudge[i+16] = (diff[i] < -threshold) ? 1:0;
                }
            }
            // judge whether the center point is feature point (fp).

            int  fpJudge[16];
            for (int i=0; i< 16; i++){
                fpJudge[i] = (posJudge[i]&posJudge[i+1]&posJudge[i+2]&posJudge[i+3]&posJudge[i+4]&posJudge[i+5]&posJudge[i+6]&posJudge[i+7])|(negJudge[i]&negJudge[i+1]&negJudge[i+2]&negJudge[i+3]&negJudge[i+4]&negJudge[i+5]&negJudge[i+6]&negJudge[i+7]);
            }

            int fpValid;
            // read new line && update line indicator/

            fpValid = (fpJudge[0]|fpJudge[1]|fpJudge[2]|fpJudge[3]|fpJudge[4]|fpJudge[5]|fpJudge[6]|fpJudge[7]);

            // output value
            int fpOut[2];
            fpOut[1] = fpValid;
            fpOut[0] = winReg[3][3];

            // update line buffer & winReg
            if (((WIN_SZ>>1) + 1 + colImage) < COLS){
                int newPixelIn;

                if(rowImage + (WIN_SZ>>1) < ROWS ){
                    newPixelIn =dataIn[idx++];
                }
                else{
                    newPixelIn = 0;
                }

                lineBuffer[rowIdx[WIN_SZ-1]][colImage + WIN_SZ>>1 + 1] = newPixelIn;
                winReg[WIN_SZ-1][WIN_SZ-1] = newPixelIn;

                for (int row =0; row < WIN_SZ; row++){
                    for (int col =0; col < WIN_SZ - 1; col++){
                        winReg[row][col] = winReg[row][col+1];
                    }
                }
                for (int row_win = 0; row_win < WIN_SZ - 1; row_win++){
                    winReg[row_win][WIN_SZ-1] = lineBuffer[rowIdx[row_win]][colImage + WIN_SZ>>1 +1];
                }
            }

            else
            {
                for (int row =0; row < WIN_SZ; row++){
                    for (int col =0; col < WIN_SZ - 1; col++){
                        winReg[row][col] = winReg[row][col+1];
                    }
                }

                for (int row_win = 0; row_win < WIN_SZ; row_win++){
                    winReg[row_win][WIN_SZ-1] = 0;
                }
            }
        }
        row_cout_idx++;
        cout <<endl<< idx << " "<< row_cout_idx;
    }

    cout << endl << idx << " "<< row_cout_idx;
#endif

#ifdef TEST_FAST_FULL


    //    int* dataIn = (int*) malloc(ROWS*COLS*sizeof(int));
    ofstream file;
    file.open("result.dat");
    int *dataIn = new int[ROWS*COLS];
    ifstream idata("/home/cag/work/pynq/ORB_FPGA_jianming/image/0.txt");
    int cnt = 0, idx = 0;
    string data_out;
    while(getline( idata, data_out, ',')) {
        stringstream ss;
        ss << data_out;
        int data;
        ss >> data;
        cnt++;
        dataIn[idx] = data;
        idx++;
    }

#define WIN_SZ 7
#define NPC 1
#define threshold 40

    int col_loop_var = NPC;
    idx = 0;
    int row_cout_idx = 3;
    // row index initialization
    int rowIdx[WIN_SZ];

    // initialize the line_buffer
    int lineBuffer[WIN_SZ][COLS] = {0};

    for (int row =  WIN_SZ >> 1; row < WIN_SZ - 1; row++){
        for(int col = 0; col < COLS; col++){
            lineBuffer[row][col] = dataIn[idx++];
        }
    }
    cout <<endl<< idx << ' ' << row_cout_idx;
    // process all rows of input images
    ROW_IMAGE:
    for (int rowImage = 0; rowImage < ROWS; rowImage++){
        // set up line buffer pointer
        if(rowImage == 0){
            for (int i = 0; i < WIN_SZ; i++ ){
                rowIdx[i] = i;
            }
        }
        else
        {
            int temp = rowIdx[0];
            for (int i = 0; i < WIN_SZ-1; i++ ){
                rowIdx[i] = rowIdx[i+1];
            }
            rowIdx[WIN_SZ-1] = temp;
        }

        // initialize WIN_REG in the corresponding row
        int winReg[WIN_SZ][WIN_SZ + NPC -1];
        for (int row = 0; row < WIN_SZ-1; row++){
            for (int col = (WIN_SZ >> 1); col < WIN_SZ; col++){
                winReg[row][col] = lineBuffer[rowIdx[row]][col];
            }
        }

        // read new data into linebuffer & update the last row of WIN_REG
        if(rowImage + (WIN_SZ>>1) < ROWS ){
            for (int col =(WIN_SZ>>1); col < WIN_SZ; col++){
                int data_temp =dataIn[idx++];
                lineBuffer[rowIdx[WIN_SZ-1]][col] = data_temp;
                winReg[WIN_SZ-1][col] = data_temp;
            }
        }
        else{
            for (int col =(WIN_SZ>>1); col < WIN_SZ; col++){
                lineBuffer[rowIdx[WIN_SZ-1]][col] = 0;
                winReg[WIN_SZ-1][col] = 0;
            }
        }


        COL_IMAGE:
        for (int colImage = 0; colImage < COLS; colImage++){
            // process FAST
            // here the 9 should be the length of the input + 1;
            int diff[16]={0};

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
            int posJudge[24]={0};
            int negJudge[24]={0};
            {
                for (int i = 0; i < 16; i++){


                    posJudge[i] = (diff[i] >  threshold) ? 1:0;
                    negJudge[i] = (diff[i] < -threshold) ? 1:0;
                }

                for (int i = 0; i < 8; i++)
                {

                    posJudge[i+16] = (diff[i] >  threshold) ? 1:0;
                    negJudge[i+16] = (diff[i] < -threshold) ? 1:0;
                }
            }
            // judge whether the center point is feature point (fp).

            int  fpJudge[16];
            for (int i=0; i< 16; i++){
                fpJudge[i] = (posJudge[i]&posJudge[i+1]&posJudge[i+2]&posJudge[i+3]&posJudge[i+4]&posJudge[i+5]&posJudge[i+6]&posJudge[i+7])|(negJudge[i]&negJudge[i+1]&negJudge[i+2]&negJudge[i+3]&negJudge[i+4]&negJudge[i+5]&negJudge[i+6]&negJudge[i+7]);
            }

            int fpValid;
            // read new line && update line indicator/

            fpValid = (fpJudge[0]|fpJudge[1]|fpJudge[2]|fpJudge[3]|fpJudge[4]|fpJudge[5]|fpJudge[6]|fpJudge[7]);

            // output value
            int fpOut[2];
            fpOut[1] = fpValid;
            fpOut[0] = winReg[3][3];

            // update line buffer & winReg
            if (((WIN_SZ>>1) + 1 + colImage) < COLS){
                int newPixelIn;

                if(rowImage + (WIN_SZ>>1) < ROWS ){
                    newPixelIn =dataIn[idx++];
                }
                else{
                    newPixelIn = 0;
                }

                lineBuffer[rowIdx[WIN_SZ-1]][colImage + WIN_SZ>>1 + 1] = newPixelIn;
                winReg[WIN_SZ-1][WIN_SZ-1] = newPixelIn;

                for (int row =0; row < WIN_SZ; row++){
                    for (int col =0; col < WIN_SZ - 1; col++){
                        winReg[row][col] = winReg[row][col+1];
                    }
                }
                for (int row_win = 0; row_win < WIN_SZ - 1; row_win++){
                    winReg[row_win][WIN_SZ-1] = lineBuffer[rowIdx[row_win]][colImage + WIN_SZ>>1 +1];
                }
            }

            else
            {
                for (int row =0; row < WIN_SZ; row++){
                    for (int col =0; col < WIN_SZ - 1; col++){
                        winReg[row][col] = winReg[row][col+1];
                    }
                }

                for (int row_win = 0; row_win < WIN_SZ; row_win++){
                    winReg[row_win][WIN_SZ-1] = 0;
                }
            }
        }
        row_cout_idx++;
        cout <<endl<< idx << " "<< row_cout_idx;
    }

    cout << endl << idx << " "<< row_cout_idx;



#endif


#ifdef FAST_EXTRACT


    //    int* dataIn = (int*) malloc(ROWS*COLS*sizeof(int));

    int *dataIn = new int[ROWS * COLS];
    ifstream idata("/home/cag/work/pynq/ORB_FPGA_jianming/image/0.txt");
    int cnt = 0, idx = 0;
    string data_out;
    while (getline(idata, data_out, ',')) {
        stringstream ss;
        ss << data_out;
        int data;
        ss >> data;
        cnt++;
        dataIn[idx] = data;
        idx++;
    }
    idx = 0;
#define WIN_SZ 7
#define NPC 1
#define threshold 40
#define COL_IMAGE_BIT 10
#define ROW_IMAGE_BIT 9
#define WIDTH    640
#define HEIGHT    480

#define WIN_SZ 7
#define WIN_SZ_IDX_BIT 4
#define NPC 1
#define PIXEL_BIT 8

    ofstream _file;
    _file.open("rst.dat");
    ofstream _file2;
    _file2.open("rst_first_col.dat");

    // row index initialization
    int rowIdx[WIN_SZ];

    // initialize the line_buffer
    int lineBuffer[WIN_SZ][WIDTH] = {0};
    for (int row = WIN_SZ >> 1; row < WIN_SZ - 1; row++) {
        for (int col = 0; col < WIDTH; col++) {
            lineBuffer[row][col] = dataIn[idx++];
        }
    }

    // process all HEIGHT of input images
    ROW_IMAGE:
    for (int rowImage = 0; rowImage < HEIGHT; rowImage++) {
        // set up line buffer pointer
        if (rowImage == 0) {
            for (int i = 0; i < WIN_SZ; i++) {
                rowIdx[i] = i;
            }
        } else {
            int temp = rowIdx[0];
            for (int i = 0; i < WIN_SZ - 1; i++) {
                rowIdx[i] = rowIdx[i + 1];
            }
            rowIdx[WIN_SZ - 1] = temp;
        }

        // initialize WIN_REG in the corresponding row
        int winReg[WIN_SZ][WIN_SZ + NPC - 1];
        for (int col = WIN_SZ >> 1; col < WIN_SZ; col++) {
            for (int row = 0; row < WIN_SZ - 1; row++) {
                winReg[row][col] = lineBuffer[rowIdx[row]][col];
            }
        }

        // read new data into linebuffer & update the last row of WIN_REG
        if (rowImage + (WIN_SZ >> 1) < HEIGHT) {
            for (int col = (WIN_SZ >> 1); col < WIN_SZ; col++) {
                int data_temp = dataIn[idx++];
                lineBuffer[rowIdx[WIN_SZ - 1]][col] = data_temp;
                winReg[WIN_SZ - 1][col] = data_temp;
            }
        } else {
            for (int col = (WIN_SZ >> 1); col < WIN_SZ; col++) {
                lineBuffer[rowIdx[WIN_SZ - 1]][col] = 0;
                winReg[WIN_SZ - 1][col] = 0;
            }
        }


        COL_IMAGE:
        for (int colImage = 0; colImage < WIDTH; colImage++) {
            // process FAST
            // here the 9 should be the length of the input + 1;
            int diff[16] = {0};
            diff[0] = winReg[3][3] - winReg[0][3];
            diff[1] = winReg[3][3] - winReg[0][4];
            diff[2] = winReg[3][3] - winReg[1][5];
            diff[3] = winReg[3][3] - winReg[2][6];
            diff[4] = winReg[3][3] - winReg[3][6];
            diff[5] = winReg[3][3] - winReg[4][6];
            diff[6] = winReg[3][3] - winReg[5][5];
            diff[7] = winReg[3][3] - winReg[6][4];
            diff[8] = winReg[3][3] - winReg[6][3];
            diff[9] = winReg[3][3] - winReg[6][2];
            diff[10] = winReg[3][3] - winReg[5][1];
            diff[11] = winReg[3][3] - winReg[4][0];
            diff[12] = winReg[3][3] - winReg[3][0];
            diff[13] = winReg[3][3] - winReg[2][0];
            diff[14] = winReg[3][3] - winReg[1][1];
            diff[15] = winReg[3][3] - winReg[0][2];

            // feature point judgement the following two loop should be calculated simultaneously
            int posJudge[24] = {0};
            int negJudge[24] = {0};

            for (int i = 0; i < 16; i++) {
                posJudge[i] = (diff[i] > threshold) ? 1 : 0;
                negJudge[i] = (diff[i] < -threshold) ? 1 : 0;
            }

            for (int i = 0; i < 8; i++) {
                posJudge[i + 16] = (diff[i] > threshold) ? 1 : 0;
                negJudge[i + 16] = (diff[i] < -threshold) ? 1 : 0;
            }
            // judge whether the center point is feature point (fp).

            int fpJudge[16];
            for (int i = 0; i < 16; i++) {
                fpJudge[i] = (posJudge[i] & posJudge[i + 1] & posJudge[i + 2] & posJudge[i + 3] & posJudge[i + 4] &
                              posJudge[i + 5] & posJudge[i + 6] & posJudge[i + 7]) |
                             (negJudge[i] & negJudge[i + 1] & negJudge[i + 2] & negJudge[i + 3] & negJudge[i + 4] &
                              negJudge[i + 5] & negJudge[i + 6] & negJudge[i + 7]);
            }

            int fpValid;
            // read new line && update line indicator/
            fpValid = (fpJudge[0] | fpJudge[1] | fpJudge[2] | fpJudge[3] | fpJudge[4] | fpJudge[5] | fpJudge[6] |
                       fpJudge[7]);

            // output value
            int fpOut[2];
            fpOut[1] = fpValid;
            fpOut[0] = winReg[3][3];
            _file << fpOut[1] << " "  << fpOut[0] << endl;
            if(colImage == 0) {
                _file2 << fpOut[1] *256 + fpOut[0] << endl;
            }
            // update line buffer & winReg
            if (colImage + (WIN_SZ >> 1) + 1 < WIDTH) {
                int newPixelIn;

                if (rowImage + (WIN_SZ >> 1) < HEIGHT) {
                    newPixelIn = dataIn[idx++];
                } else {
                    newPixelIn = 0;
                }

                lineBuffer[rowIdx[WIN_SZ - 1]][colImage + (WIN_SZ >> 1) + 1] = newPixelIn;
                winReg[WIN_SZ - 1][WIN_SZ - 1] = newPixelIn;
                for (int col = 0; col < WIN_SZ - 1; col++) {
                    for (int row = 0; row < WIN_SZ; row++) {
                        winReg[row][col] = winReg[row][col + 1];
                    }
                }

                for (int row_win = 0; row_win < WIN_SZ - 1; row_win++) {
                    winReg[row_win][WIN_SZ - 1] = lineBuffer[rowIdx[row_win]][colImage + (WIN_SZ >> 1) + 1];
                }
            }
            else
            if (colImage < WIDTH) {
                for (int col = 0; col < WIN_SZ - 1; col++) {
                    for (int row = 0; row < WIN_SZ; row++) {
                        winReg[row][col] = winReg[row][col + 1];
                    }
                }

                for (int row_win = 0; row_win < WIN_SZ; row_win++) {
                    winReg[row_win][WIN_SZ - 1] = 0;
                }
            }
        }

    }
#endif

    return 0;
}