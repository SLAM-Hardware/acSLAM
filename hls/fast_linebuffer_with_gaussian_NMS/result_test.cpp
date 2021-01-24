#include<iostream>
#include<fstream>
#include<iomanip>
using namespace std;


int main(int argc,char** argv){

    ifstream idata("result.dat");
    int ind_155 = 0;
    int cnt = 0, idx = 0;
    int *dataIn=(int*)malloc(640*480*2*sizeof(int));
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

    ifstream idata1("result_7.dat");
    int cnt1 = 0, idx1 = 0;
    int *dataIn1=(int*)malloc(640*480*2*sizeof(int));
    string data_out1;
    while(getline( idata1, data_out1, ',')) {
        stringstream ss;
        ss << data_out1;
        int data;
        ss >> data;
        cnt1++;
        dataIn1[idx1] = data;
        idx1++;
    }
    idata.close();
    idata1.close();
    // #define x 10
    // #define y 3
    // for (int i = x - 3; i < x + 4; i++){
    //     for (int j = y-3; j < y+4; j++){
    //         cout << dataIn[i * 640 + j] << ", ";
    //     }
    //     cout << endl;
    // }
    
        // cout << dataIn[10 * 640 + 622];
    for (int i = 0; i < 480 * 640*2; i++){
        // cout << dataIn1[i] << " ";
        if (dataIn[i] != dataIn1[i]){
            if (dataIn1[i]==0 && dataIn[i]==1)
                cout << "error";
            else
                cout << dataIn[i] << " " << dataIn1[i] << endl;
            // cout << "error!  " << dataIn[i + 1] << " "<< endl;
            //system("pause");
            //return (-1);            
        }            
    }
    cout << "pass!";
    system("pause");
    return 0;
}
