#include "FAST.h"

using namespace std;

ap_uint<32> width;
ap_uint<32> height;
ap_uint<WIDTH_BIT> unit_num;
ap_uint<WIDTH_BIT> padding_unit_num;
ap_uint<WIDTH_BIT> width_tc;

ap_int<9> my_abs(ap_int<9> &x) {
    ap_int<9> tmp = x;
    ap_int<9> neg_tmp = -x;
    return (tmp > 0) ? tmp : neg_tmp;
}

void process(hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream,
             hls::stream <ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outPixelStream,
             hls::stream <ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outFASTStream);

void process_cfg(hls::stream <ap_axiu<32, 1, 1, 1> > &cfgStream, hls::stream <ap_axiu<32, 1, 1, 1> > &cfgoutStream);

void process_input(hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream, hls::stream <ap_uint<PROCESS_BIT> > &pixelData);

void process_padding(hls::stream <ap_uint<INPUT_BIT> > &pixelData, hls::stream <ap_uint<(WIN_SZ - 1) * PIXEL_BIT> > &initData, hls::stream <ap_uint<INPUT_BIT> > &srcData);

void process_buf(hls::stream <ap_uint<(WIN_SZ - 1) * PIXEL_BIT> > &initData,
                 hls::stream <ap_uint<INPUT_BIT> > &srcData, 
                 hls::stream <ap_uint<PROCESS_BIT> > &gausData,
                 hls::stream <ap_uint<PROCESS_BIT> > &FASTData);
void
process_FAST(ap_uint<PIXEL_BIT> win_buf[WIN_SZ][WIN_SZ + PROCESS_NUM - 1], ap_uint<PIXEL_BIT> gaus_buf[PROCESS_NUM],
             ap_uint<PIXEL_BIT> FAST_buf[PROCESS_NUM],ap_uint<WIN_SZ_BIT> win_ind[WIN_SZ]);

void process_output(hls::stream <ap_uint<PROCESS_BIT> > &gausData, hls::stream <ap_uint<PROCESS_BIT> > &FASTData,
                    hls::stream <ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outPixelStream,
                    hls::stream <ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outFASTStream);

void FAST(hls::stream <ap_axiu<32, 1, 1, 1> > &cfgStream, hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream,
          hls::stream <ap_axiu<32, 1, 1, 1> > &cfgoutStream, hls::stream <ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outPixelStream,
          hls::stream <ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outFASTStream) {
#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS INTERFACE axis register both port = cfgStream
#pragma HLS INTERFACE axis register both port = srcStream
#pragma HLS INTERFACE axis register both port = cfgoutStream
#pragma HLS INTERFACE axis register both port = outPixelStream
#pragma HLS INTERFACE axis register both port = outFASTStream

    process_cfg(cfgStream, cfgoutStream);
    process(srcStream, outPixelStream, outFASTStream);
}

void process_cfg(hls::stream <ap_axiu<32, 1, 1, 1> > &cfgStream, hls::stream <ap_axiu<32, 1, 1, 1> > &cfgoutStream) {
#pragma HLS PIPELINE
    width = cfgStream.read().data;
    height = cfgStream.read().data;
    ap_axiu<32, 1, 1, 1> cfgout;
    ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> unit_num_ufixed = width;
    unit_num_ufixed = my_ceil<ap_ufixed < WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT >
                                                                                    (unit_num_ufixed / INPUT_PIXEL_NUM);
    unit_num = unit_num_ufixed.range(WIDTH_BIT + 7, 8);

    ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> padding_unit_num_ufixed = width + WIN_SZ - 1;
    padding_unit_num_ufixed = my_ceil<ap_ufixed < WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT >
                                                                                            (padding_unit_num_ufixed /
                                                                                             INPUT_PIXEL_NUM);
    padding_unit_num = padding_unit_num_ufixed.range(WIDTH_BIT + 7, 8);

    cfgout.data = width;
    cfgout.keep = 0xF;
    cfgout.last = 0;
    cfgoutStream.write(cfgout);
    cfgout.data = height;
    cfgout.keep = 0xF;
    cfgout.last = 1;
    cfgoutStream.write(cfgout);

}

void process(hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream,
             hls::stream <ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outPixelStream,
             hls::stream <ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outFASTStream) {
#pragma HLS DATAFLOW
    hls::stream <ap_uint<INPUT_BIT> > pixelData;
#pragma HLS STREAM variable = pixelData depth = 2
    hls::stream <ap_uint<PROCESS_BIT> > gausData;
#pragma HLS STREAM variable = gausData depth = 2
    hls::stream <ap_uint<PROCESS_BIT> > FASTData;
#pragma HLS STREAM variable = FASTData depth = 2
    hls::stream <ap_uint<(WIN_SZ - 1) * PIXEL_BIT> > initData;
#pragma HLS STREAM variable = initData depth = 2
    hls::stream <ap_uint<INPUT_BIT> > srcData;
#pragma HLS STREAM variable = srcData depth = 2
    process_input(srcStream, pixelData);
    process_padding(pixelData, initData, srcData);
    process_buf(initData, srcData, gausData, FASTData);
    process_output(gausData, FASTData, outPixelStream, outFASTStream);
#ifdef DEBUG
    for (int i = 0; i < height; i++)
    {
        ap_uint<(WIN_SZ - 1) * PIXEL_BIT> init_data = initData.read();
        for (int ii = 0; ii < WIN_SZ - 1; ii++)
        {
            cout << init_data.range(7, 0) << " ";
            init_data = init_data >> 8;
        }
    	for (int j = 0; j < unit_num; j++)
    	{
    		ap_uint<INPUT_BIT> data = srcData.read();
            for (int ii = 0; ii < INPUT_PIXEL_NUM; ii++)
            {
                cout << data.range(7, 0) << " ";
                data = data >> 8;
            }
    	}
    	cout << endl;
    }
#endif
}

void process_input(hls::stream <ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> > &srcStream, hls::stream <ap_uint<INPUT_BIT> > &pixelData) {
#pragma HLS INLINE off
    ap_uint<PIXEL_NUM_BIT> cnt = 0;
    ap_axiu<INPUT_STREAM_BIT, 1, 1, 1> dataIn;

    ap_uint<INPUT_BIT> data = 0;
    ap_uint<INPUT_BIT> prv_data = 0;
    ap_uint<11> rmn_num = 0;
    ap_uint<11> write_num = 0;
    ap_uint<INPUT_BIT> write_tmp = 0;
    for (ap_uint<HEIGHT_BIT> i = 0; i < height; i++) {
        for (ap_uint<WIDTH_BIT> j = 0; j < unit_num; j++) {
#pragma HLS PIPELINE
            if (j == unit_num - 1)
                write_num = (width - INPUT_PIXEL_NUM * (unit_num - 1)) * PIXEL_BIT;
            else
                write_num = INPUT_PIXEL_NUM * PIXEL_BIT;

            if (rmn_num >= write_num) {
                write_tmp = 0;
                write_tmp.range(write_num - 1, 0) = data.range(INPUT_BIT - rmn_num + write_num - 1,
                                                               INPUT_BIT - rmn_num);
                rmn_num = rmn_num - write_num;
            } else {
                dataIn = srcStream.read();
                prv_data = data;
                data = dataIn.data;
                if (rmn_num > 0) {
                    write_tmp = 0;
                    write_tmp.range(rmn_num - 1, 0) = prv_data.range(INPUT_BIT - 1, INPUT_BIT - rmn_num);
                    write_tmp.range(write_num - 1, rmn_num) = data.range(write_num - rmn_num - 1, 0);
                    rmn_num = INPUT_BIT - (write_num - rmn_num);
                } else {
                    write_tmp = 0;
                    write_tmp.range(write_num - 1, 0) = data.range(write_num - 1, 0);
                    rmn_num = INPUT_BIT - write_num;
                }
            }
            pixelData.write(write_tmp);
        }
    }
}

void process_padding(hls::stream <ap_uint<INPUT_BIT> > &pixelData, hls::stream <ap_uint<(WIN_SZ - 1) * PIXEL_BIT> > &initData, hls::stream <ap_uint<INPUT_BIT> > &srcData) {
    ap_uint<(WIN_SZ - 1) * PIXEL_BIT> initTmp = 0;
    ap_uint<HALF_WIN_SZ * PIXEL_BIT + INPUT_BIT * 2> readTmp = 0;
    ap_uint<HALF_WIN_SZ * PIXEL_BIT> endPaddingTmp = 0;
    
    for (ap_uint<HEIGHT_BIT> i = 0; i < height; i++) {
        for (ap_uint<3> j = 0; j < READ_NUM; j++){
#pragma HLS PIPELINE
            readTmp = pixelData.read();
            if (j < READ_NUM - 1){
                for (ap_uint<WIN_SZ_BIT> reflect_ind = 0; reflect_ind < INPUT_PIXEL_NUM; reflect_ind++)
                {
#pragma HLS UNROLL
                    if (j * INPUT_PIXEL_NUM + reflect_ind != 0){
                        ap_uint<WIN_SZ_BIT> padding_ind = HALF_WIN_SZ - j * INPUT_PIXEL_NUM - reflect_ind;
#ifdef BOARDER_101
                        initTmp.range((padding_ind + 1) * PIXEL_BIT - 1, padding_ind * PIXEL_BIT) = 
                            readTmp.range((reflect_ind + 1) * PIXEL_BIT - 1, reflect_ind * PIXEL_BIT);
#else
                        initTmp.range((padding_ind + 1) * PIXEL_BIT - 1, padding_ind * PIXEL_BIT) = 0;
#endif
                    }
                }
                initTmp.range((HALF_WIN_SZ + (j + 1) * INPUT_PIXEL_NUM) * PIXEL_BIT - 1, (HALF_WIN_SZ + j * INPUT_PIXEL_NUM) * PIXEL_BIT) =
                    readTmp.range(INPUT_PIXEL_NUM * PIXEL_BIT - 1, 0);
            }else{
                for (ap_uint<WIN_SZ_BIT> reflect_ind = 0; reflect_ind < HALF_WIN_SZ - (READ_NUM - 1) * INPUT_PIXEL_NUM + 1; reflect_ind++)
                {
#pragma HLS UNROLL
                    ap_uint<WIN_SZ_BIT> padding_ind = HALF_WIN_SZ - j * INPUT_PIXEL_NUM - reflect_ind;
#ifdef BOARDER_101
                    initTmp.range((padding_ind + 1) * PIXEL_BIT - 1, padding_ind * PIXEL_BIT) = 
                       readTmp.range((reflect_ind + 1) * PIXEL_BIT - 1, reflect_ind * PIXEL_BIT);
#else
                    initTmp.range((padding_ind + 1) * PIXEL_BIT - 1, padding_ind * PIXEL_BIT) = 0;
#endif
                }
                if (WIN_SZ - 1 > HALF_WIN_SZ + j * INPUT_PIXEL_NUM){
                    initTmp.range((WIN_SZ - 1) * PIXEL_BIT - 1, (HALF_WIN_SZ + j * INPUT_PIXEL_NUM) * PIXEL_BIT) =
                        readTmp.range((HALF_WIN_SZ - j * INPUT_PIXEL_NUM) * PIXEL_BIT - 1, 0);
                }
            }
        }

#ifdef DEBUG
    for (int i_ = 0; i_ < WIN_SZ - 1; i_++){
        cout << initTmp.range((i_+1)*PIXEL_BIT-1,i_*PIXEL_BIT) << " "; 
    }
    cout << "init---";
#endif

        initData.write(initTmp);
        readTmp = readTmp >> (REMAIN_NUM * PIXEL_BIT);
        for (ap_uint<WIDTH_BIT> j = READ_NUM; j < unit_num; j++) {
#pragma HLS PIPELINE
            readTmp.range(INPUT_BIT * 2 - REMAIN_NUM * PIXEL_BIT - 1, INPUT_BIT - REMAIN_NUM * PIXEL_BIT) = pixelData.read();
            for (ap_uint<8> pInd = 0; pInd < INPUT_PIXEL_NUM; pInd++)
            {
                ap_uint<WIDTH_BIT> gpInd = j * INPUT_PIXEL_NUM + pInd;
                if (width-HALF_WIN_SZ-1 <= gpInd && gpInd <= width-2)
                {
                    ap_uint<WIN_SZ_BIT> reflect_ind = width - gpInd - 2;
#ifdef BOARDER_101
                    endPaddingTmp.range((reflect_ind + 1) * PIXEL_BIT - 1, reflect_ind * PIXEL_BIT) = 
                       readTmp.range((pInd + 1) * PIXEL_BIT - 1 + (INPUT_PIXEL_NUM - REMAIN_NUM) * PIXEL_BIT, pInd * PIXEL_BIT + (INPUT_PIXEL_NUM - REMAIN_NUM) * PIXEL_BIT);
#else
                    endPaddingTmp.range((reflect_ind + 1) * PIXEL_BIT - 1, reflect_ind * PIXEL_BIT) = 0;
#endif
                }
            }
            if (j == unit_num - 1)
            {
                ap_uint<WIDTH_BIT> rmnPixelNum = width - (unit_num-1) * INPUT_PIXEL_NUM + INPUT_PIXEL_NUM - REMAIN_NUM;
                readTmp.range(rmnPixelNum * PIXEL_BIT + HALF_WIN_SZ * PIXEL_BIT - 1, rmnPixelNum * PIXEL_BIT) = endPaddingTmp;
            }
            srcData.write(readTmp.range(INPUT_BIT - 1, 0));
            readTmp = readTmp >> INPUT_BIT;
        }
        for (ap_uint<WIDTH_BIT> j = unit_num; j < unit_num + READ_NUM; j++) {
#pragma HLS PIPELINE
            srcData.write(readTmp.range(INPUT_BIT - 1, 0));
            readTmp = readTmp >> INPUT_BIT;        
        }
    }
}

void process_shift(ap_uint<PIXEL_BIT * MERGE_NUM> image_buf[WIN_SZ][WIDTH_AFTER_MERGE],
                   ap_uint<PIXEL_BIT> win_buf[WIN_SZ][WIN_SZ + PROCESS_NUM - 1],
                   ap_uint<PIXEL_BIT> read_buf[PROCESS_NUM],
                   ap_uint<PIXEL_BIT> gaus_buf[PROCESS_NUM],
                   ap_uint<PIXEL_BIT> FAST_buf[PROCESS_NUM],
                   ap_uint<WIN_SZ_BIT> win_ind[WIN_SZ],
                   hls::stream <ap_uint<PROCESS_BIT> > &gausData,
                   hls::stream <ap_uint<PROCESS_BIT> > &FASTData,
                   ap_uint<WIDTH_BIT> col_ind){
#pragma HLS INLINE
    for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
        for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < PROCESS_NUM / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter WAR false
            ap_uint<MERGE_NUM * PIXEL_BIT> read_tmp = image_buf[row_index][col_ind * PROCESS_NUM / MERGE_NUM + read_ind + (WIN_SZ - 1)/MERGE_NUM];
            for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                win_buf[row_index][read_ind * MERGE_NUM + pixel_ind + WIN_SZ - 1] =
                    read_tmp.range((pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
            }
        }
    }

    // compute
    process_FAST(win_buf, gaus_buf, FAST_buf, win_ind);
    ap_uint<PROCESS_BIT> gaus_write, FAST_write;

    for (ap_uint<10> pixel_ind = 0; pixel_ind < PROCESS_NUM; pixel_ind++) {
#pragma HLS UNROLL
        gaus_write.range((pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT) = gaus_buf[pixel_ind];
        FAST_write.range((pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT) = FAST_buf[pixel_ind];
    }
    gausData.write(gaus_write);
    FASTData.write(FAST_write);

#ifdef DEBUG
    for (int i = 0; i < WIN_SZ; i++)
    {
    for (int j = 0; j < WIN_SZ + PROCESS_NUM - 1; j++)
    {
    cout << win_buf[i][j] << " ";
    }
    cout << endl;
    }
    cout << "*----------------win_buf------------------" << endl;
#endif
    // slide win_buf
    for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
        for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < WIN_SZ - 1; pixel_ind++) {
#pragma HLS UNROLL
            win_buf[row_index][pixel_ind] = win_buf[row_index][pixel_ind + PROCESS_NUM];
        }
    }
}

void process_win_ind(ap_uint<WIN_SZ_BIT> win_ind[WIN_SZ]){
#pragma HLS INLINE
    ap_uint<WIN_SZ_BIT> win_ind_tmp = win_ind[0];
    for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ - 1; i++)
#pragma HLS UNROLL
            win_ind[i] = win_ind[i + 1];
    win_ind[WIN_SZ - 1] = win_ind_tmp;
}

void process_buf(hls::stream <ap_uint<(WIN_SZ - 1) * PIXEL_BIT> > &initData,
                 hls::stream <ap_uint<INPUT_BIT> > &srcData, 
                 hls::stream <ap_uint<PROCESS_BIT> > &gausData,
                 hls::stream <ap_uint<PROCESS_BIT> > &FASTData) {
#pragma HLS INLINE off

    ap_uint<PIXEL_BIT * MERGE_NUM> image_buf[WIN_SZ][WIDTH_AFTER_MERGE];
#pragma HLS RESOURCE variable=image_buf core=RAM_T2P_BRAM
//#pragma HLS BIND_STORAGE variable=image_buf type=ram_t2p

#pragma HLS ARRAY_PARTITION variable = image_buf cyclic factor = 1 dim = 2
#pragma HLS ARRAY_PARTITION variable=image_buf complete dim=1

    ap_uint<PIXEL_BIT> win_buf[WIN_SZ][WIN_SZ + PROCESS_NUM - 1];
#pragma HLS ARRAY_PARTITION variable = win_buf complete dim = 0

    ap_uint<PIXEL_BIT> read_buf[PROCESS_NUM];
#pragma HLS ARRAY_PARTITION variable = read_buf complete dim = 0

    ap_uint<PIXEL_BIT> gaus_buf[PROCESS_NUM];
#pragma HLS ARRAY_PARTITION variable = gaus_buf complete dim = 0

    ap_uint<PIXEL_BIT> FAST_buf[PROCESS_NUM];
#pragma HLS ARRAY_PARTITION variable = FAST_buf complete dim = 0

    ap_uint<WIN_SZ_BIT> win_ind[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable = win_ind complete dim = 1

    ap_ufixed<WIDTH_BIT + 8, WIDTH_BIT> width_tc_ufixed = width + WIN_SZ - 1;
    width_tc_ufixed = my_ceil<ap_ufixed < WIDTH_BIT + 8, WIDTH_BIT>, WIDTH_BIT + 8, WIDTH_BIT >
                                                                                    (width_tc_ufixed / MERGE_NUM);
    width_tc = width_tc_ufixed.range(WIDTH_BIT + 7, 8);

    ap_uint<11> write_num = 0;
    ap_uint<8> write_pixel_num = 0;

    //initialization
    for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ; i++)
#pragma HLS UNROLL
            win_ind[i] = i;

    for (ap_uint<HEIGHT_BIT> row_ind = HALF_WIN_SZ; row_ind < WIN_SZ - 1; row_ind++) {
        ap_uint<(WIN_SZ - 1) * PIXEL_BIT> initIn = initData.read();
        for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
        {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 1
            ap_uint<8> offset = initInd * PIXEL_BIT * MERGE_NUM;
            ap_uint<PIXEL_BIT * MERGE_NUM> splitTmp = initIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
            image_buf[row_ind][initInd] = splitTmp;
        }
        for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < unit_num; read_ind++) {
#pragma HLS PIPELINE
            ap_uint<INPUT_BIT> srcIn = srcData.read();
            for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
            {
#pragma HLS UNROLL
                ap_uint<8> offset = srcInd * PIXEL_BIT * MERGE_NUM;
                ap_uint<PIXEL_BIT * MERGE_NUM> splitTmp = srcIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
                image_buf[row_ind][srcInd + read_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] = splitTmp;                
            }
        }
    }

    //    for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ; i++)
    //#pragma HLS UNROLL
    //        for (ap_uint<WIDTH_BIT> j = 0; j < WIN_SZ + PROCESS_NUM - 1; j++)
    //#pragma HLS UNROLL
    //            win_buf[i][j] = 0;

#ifdef DEBUG
    for (int i = 0; i < WIN_SZ; i++)
    {
        for (int j = 0; j < ceil((double)(width+WIN_SZ-1) / MERGE_NUM); j++)
        {
            ap_uint<MERGE_NUM * PIXEL_BIT> cout_tmp =image_buf[i][j];
            for (int k = 0; k < MERGE_NUM; k++)
            {
                cout << cout_tmp.range(7, 0) << " ";
                cout_tmp = cout_tmp >> PIXEL_BIT;
            }
        }
        cout << endl;
    }
    cout << "*----------------image_buf------------------" << endl;
    // for (int i = 0; i < WIN_SZ; i++)
    // {
    //     for (int j = 0; j < WIN_SZ; j++)
    //     {
    //         cout << win_buf[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // cout << "*----------------win_buf------------------" << endl;
#endif

    Row_Loop:
#ifdef BOARDER_101

    for (ap_uint<HEIGHT_BIT> row_ind = 1; row_ind < HALF_WIN_SZ; row_ind++)
    {
#pragma HLS UNROLL
        for (ap_uint<WIDTH_BIT> read_ind = 0;
            read_ind < padding_unit_num * INPUT_PIXEL_NUM / MERGE_NUM; read_ind++) {
#pragma HLS DEPENDENCE variable=image_buf inter RAW false
#pragma HLS UNROLL factor = 1
#pragma HLS PIPELINE
            ap_uint<PIXEL_BIT * MERGE_NUM> copyTmp = image_buf[WIN_SZ-row_ind-1][read_ind];
            image_buf[row_ind][read_ind] = copyTmp;
        }
    }

    for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < 1; row_ind++) {
        ap_uint<(WIN_SZ - 1) * PIXEL_BIT> initIn = initData.read();
        for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
        {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 1
            ap_uint<8> offset = initInd * PIXEL_BIT * MERGE_NUM;
            image_buf[win_ind[WIN_SZ - 1]][initInd] = 
                initIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
            image_buf[0][initInd] = initIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
        }

        for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
            for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < (WIN_SZ - 1) / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
                ap_uint<MERGE_NUM * PIXEL_BIT> read_tmp = image_buf[row_index][read_ind];
                for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                    win_buf[row_index][read_ind * MERGE_NUM + pixel_ind] = read_tmp.range(
                            (pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
                }
            }
        }

        for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < unit_num; col_ind++) {
#pragma HLS PIPELINE
            ap_uint<INPUT_BIT> srcIn = srcData.read();
            for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
            {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter WAW  false
                ap_uint<8> offset = srcInd * PIXEL_BIT * MERGE_NUM;
                image_buf[win_ind[WIN_SZ - 1]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] =
                    srcIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
                image_buf[0][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] =
                    srcIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
            }

            process_shift(image_buf, win_buf, read_buf, gaus_buf, FAST_buf, win_ind, gausData, FASTData, col_ind);
        }
        process_win_ind(win_ind);
    }
 //-------------------------------------------------------------------------------------
 for (ap_uint<HEIGHT_BIT> row_ind = 1; row_ind < height - HALF_WIN_SZ; row_ind++) {
        //init
        ap_uint<(WIN_SZ - 1) * PIXEL_BIT> initIn = initData.read();
        for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
        {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 1
            ap_uint<8> offset = initInd * PIXEL_BIT * MERGE_NUM;
            image_buf[win_ind[WIN_SZ - 1]][initInd] = 
                initIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
        }

        for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
            for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < (WIN_SZ - 1) / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
                ap_uint<MERGE_NUM * PIXEL_BIT> read_tmp = image_buf[row_index][read_ind];
                for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                    win_buf[row_index][read_ind * MERGE_NUM + pixel_ind] = read_tmp.range(
                            (pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
                }
            }
        }

        for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < unit_num; col_ind++) {
#pragma HLS PIPELINE
            ap_uint<INPUT_BIT> srcIn = srcData.read();
            for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
            {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter WAW  false
                ap_uint<8> offset = srcInd * PIXEL_BIT * MERGE_NUM;
                image_buf[win_ind[WIN_SZ - 1]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] =
                    srcIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
            }
            
            process_shift(image_buf, win_buf, read_buf, gaus_buf, FAST_buf, win_ind, gausData, FASTData, col_ind);
        }
        process_win_ind(win_ind);
    }
 //-------------------------------------------------------------------------------------
    for (ap_uint<HEIGHT_BIT> row_ind = height - HALF_WIN_SZ; row_ind < height; row_ind++) {
        //init
        for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
        {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 1
            image_buf[win_ind[WIN_SZ - 1]][initInd] = 
                image_buf[win_ind[WIN_SZ-3-(row_ind-(height-HALF_WIN_SZ))*2]][initInd];
        }


        for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
            for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < (WIN_SZ - 1) / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
                ap_uint<MERGE_NUM * PIXEL_BIT> read_tmp = image_buf[row_index][read_ind];
                for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                    win_buf[row_index][read_ind * MERGE_NUM + pixel_ind] = read_tmp.range(
                            (pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
                }
            }
        }

        for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < unit_num; col_ind++) {
#pragma HLS PIPELINE
            for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
            {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter RAW false
                image_buf[win_ind[WIN_SZ - 1]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] =
                    image_buf[win_ind[WIN_SZ-3-(row_ind-(height-HALF_WIN_SZ))*2]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM];
            }

            process_shift(image_buf, win_buf, read_buf, gaus_buf, FAST_buf, win_ind, gausData, FASTData, col_ind);
        }
        process_win_ind(win_ind);
    }
#else

    for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < WIDTH_AFTER_MERGE; col_ind++)
#pragma HLS UNROLL factor = 1
#pragma HLS PIPELINE
        for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < HALF_WIN_SZ; row_ind++)
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter WAW  false
            image_buf[row_ind][col_ind] = 0;

    for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < height; row_ind++) {
        //init
        if (row_ind < height - HALF_WIN_SZ) {
            ap_uint<(WIN_SZ - 1) * PIXEL_BIT> initIn = initData.read();
            for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
            {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 1
                ap_uint<8> offset = initInd * PIXEL_BIT * MERGE_NUM;
                image_buf[win_ind[WIN_SZ - 1]][initInd] = 
                    initIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
            }
        }
        else
        {
            for (ap_uint<8> initInd = 0; initInd < (WIN_SZ - 1) / MERGE_NUM; initInd++)
            {
#pragma HLS PIPELINE
#pragma HLS UNROLL factor = 1
                image_buf[win_ind[WIN_SZ - 1]][initInd] = 0;
            }
        }

        for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
            for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < (WIN_SZ - 1) / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
                ap_uint<MERGE_NUM * PIXEL_BIT> read_tmp = image_buf[row_index][read_ind];
                for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                    win_buf[row_index][read_ind * MERGE_NUM + pixel_ind] = read_tmp.range(
                            (pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
                }
            }
        }

        for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < unit_num; col_ind++) {
#pragma HLS PIPELINE
            if (row_ind < height - HALF_WIN_SZ) {
                ap_uint<INPUT_BIT> srcIn = srcData.read();
                for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
                {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter WAW  false
                    ap_uint<8> offset = srcInd * PIXEL_BIT * MERGE_NUM;
                    image_buf[win_ind[WIN_SZ - 1]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] =
                        srcIn.range(PIXEL_BIT * MERGE_NUM - 1 + offset, 0 + offset);
                }
            }
            else
            {
                for (ap_uint<8> srcInd = 0; srcInd < INPUT_PIXEL_NUM / MERGE_NUM; srcInd++)
                {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter RAW false
                	image_buf[win_ind[WIN_SZ - 1]][srcInd + col_ind * INPUT_PIXEL_NUM / MERGE_NUM + (WIN_SZ - 1) / MERGE_NUM] = 0;
                }
            }

            for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
                for (ap_uint<WIDTH_BIT> read_ind = 0; read_ind < PROCESS_NUM / MERGE_NUM; read_ind++) {
#pragma HLS UNROLL
#pragma HLS DEPENDENCE variable=image_buf inter WAR false
                    ap_uint<MERGE_NUM * PIXEL_BIT> read_tmp = image_buf[row_index][col_ind * PROCESS_NUM / MERGE_NUM + read_ind + (WIN_SZ - 1)/MERGE_NUM];
                    for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < MERGE_NUM; pixel_ind++) {
#pragma HLS UNROLL
                        win_buf[row_index][read_ind * MERGE_NUM + pixel_ind + WIN_SZ - 1] =
                            read_tmp.range((pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT);
                    }
                }
            }

            // compute
            process_FAST(win_buf, gaus_buf, FAST_buf, win_ind);
            ap_uint<PROCESS_BIT> gaus_write, FAST_write;

            for (ap_uint<10> pixel_ind = 0; pixel_ind < PROCESS_NUM; pixel_ind++) {
#pragma HLS UNROLL
                gaus_write.range((pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT) = gaus_buf[pixel_ind];
                FAST_write.range((pixel_ind + 1) * PIXEL_BIT - 1, pixel_ind * PIXEL_BIT) = FAST_buf[pixel_ind];
            }
            gausData.write(gaus_write);
            FASTData.write(FAST_write);

#ifdef DEBUG
    for (int i = 0; i < WIN_SZ; i++)
    {
        for (int j = 0; j < WIN_SZ + PROCESS_NUM - 1; j++)
        {
            cout << win_buf[i][j] << " ";
        }
        cout << endl;
    }
    cout << "*----------------win_buf------------------" << endl;
#endif
            // slide win_buf
            for (ap_uint<WIN_SZ_BIT> row_index = 0; row_index < WIN_SZ; row_index++) {
#pragma HLS UNROLL
                for (ap_uint<WIDTH_BIT> pixel_ind = 0; pixel_ind < WIN_SZ - 1; pixel_ind++) {
#pragma HLS UNROLL
                    win_buf[row_index][pixel_ind] = win_buf[row_index][pixel_ind + PROCESS_NUM];
                }
            }
        }
        ap_uint<WIN_SZ_BIT> win_ind_tmp = win_ind[0];
        for (ap_uint<WIN_SZ_BIT> i = 0; i < WIN_SZ - 1; i++)
#pragma HLS UNROLL
                win_ind[i] = win_ind[i + 1];
        win_ind[WIN_SZ - 1] = win_ind_tmp;
    }
#endif
}

void
process_FAST(ap_uint<PIXEL_BIT> win_buf[WIN_SZ][WIN_SZ + PROCESS_NUM - 1], ap_uint<PIXEL_BIT> gaus_buf[PROCESS_NUM],
             ap_uint<PIXEL_BIT> FAST_buf[PROCESS_NUM],ap_uint<WIN_SZ_BIT> win_ind[WIN_SZ]) {
#pragma HLS INLINE
#define win_buf_center(x, y) win_buf[win_ind[x+i]][y+j]
    ap_uint<8> NMS_score_buf[3][PROCESS_NUM + 2];
#pragma HLS ARRAY_PARTITION variable = NMS_score_buf complete dim = 0
    ap_uint<1> NMS_FAST_buf[3][PROCESS_NUM + 2];
#pragma HLS ARRAY_PARTITION variable = NMS_FAST_buf complete dim = 0
        for (ap_uint<11> loop_ind = 0; loop_ind < (2 + PROCESS_NUM); loop_ind++) {
#pragma HLS UNROLL
            ap_uint<3> i = loop_ind / (2 + PROCESS_NUM);
            ap_uint<8> j = loop_ind - i*(2 + PROCESS_NUM);
            short int diff[25];
#pragma HLS ARRAY_PARTITION variable=diff complete dim=1

            diff[0] = win_buf_center(3, 3) - win_buf_center(0, 3);
            diff[1] = win_buf_center(3, 3) - win_buf_center(0, 4);
            diff[2] = win_buf_center(3, 3) - win_buf_center(1, 5);
            diff[3] = win_buf_center(3, 3) - win_buf_center(2, 6);
            diff[4] = win_buf_center(3, 3) - win_buf_center(3, 6);
            diff[5] = win_buf_center(3, 3) - win_buf_center(4, 6);
            diff[6] = win_buf_center(3, 3) - win_buf_center(5, 5);
            diff[7] = win_buf_center(3, 3) - win_buf_center(6, 4);
            diff[8] = win_buf_center(3, 3) - win_buf_center(6, 3);
            diff[9] = win_buf_center(3, 3) - win_buf_center(6, 2);
            diff[10] = win_buf_center(3, 3) - win_buf_center(5, 1);
            diff[11] = win_buf_center(3, 3) - win_buf_center(4, 0);
            diff[12] = win_buf_center(3, 3) - win_buf_center(3, 0);
            diff[13] = win_buf_center(3, 3) - win_buf_center(2, 0);
            diff[14] = win_buf_center(3, 3) - win_buf_center(1, 1);
            diff[15] = win_buf_center(3, 3) - win_buf_center(0, 2);

            diff[16] = diff[0];
            diff[17] = diff[1];
            diff[18] = diff[2];
            diff[19] = diff[3];
            diff[20] = diff[4];
            diff[21] = diff[5];
            diff[22] = diff[6];
            diff[23] = diff[7];
            diff[24] = diff[8];

            short int flag_d_min2[NUM - 1];
            short int flag_d_max2[NUM - 1];
            short int flag_d_min4[NUM - 3];
            short int flag_d_max4[NUM - 3];
            short int flag_d_min8[NUM - 7];
            short int flag_d_max8[NUM - 7];

            for (ap_uint<5> i = 0; i < NUM - 1; i++) {
#pragma HLS UNROLL
                flag_d_min2[i] = __MIN(diff[i], diff[i + 1]);
                flag_d_max2[i] = __MAX(diff[i], diff[i + 1]);
            }

            for (ap_uint<5> i = 0; i < NUM - 3; i++) {
#pragma HLS UNROLL
                flag_d_min4[i] = __MIN(flag_d_min2[i], flag_d_min2[i + 2]);
                flag_d_max4[i] = __MAX(flag_d_max2[i], flag_d_max2[i + 2]);
            }

            for (ap_uint<5> i = 0; i < NUM - 7; i++) {
#pragma HLS UNROLL
                flag_d_min8[i] = __MIN(flag_d_min4[i], flag_d_min4[i + 4]);
                flag_d_max8[i] = __MAX(flag_d_max4[i], flag_d_max4[i + 4]);
            }

            uchar_t a0 = THRESHOLD;

            for (ap_uint<5> i = 0; i < PSize; i += 2) {
#pragma HLS UNROLL
                short int a = 255;
                if (PSize == 16) {
                    a = flag_d_min8[i + 1];
                }
                //		else {
                //			for(ap_uint<5> j=1;j<PSize/2+1;j++)
                //			{
                //				a=__MIN(a,flag_d[i+j]);
                //			}
                //		}
                a0 = __MAX(a0, __MIN(a, diff[i])); // a0 >= THRESHOLD
                a0 = __MAX(a0, __MIN(a, diff[i + PSize / 2 + 1]));
            }
            short int b0 = -THRESHOLD;
            for (ap_uint<5> i = 0; i < PSize; i += 2) {
#pragma HLS UNROLL
                short int b = -255;
                if (PSize == 16) {
                    b = flag_d_max8[i + 1];
                }
                //		} else {
                //			for(ap_uint<5> j=1;j<PSize/2+1;j++)
                //			{
                //				b=__MAX(b,flag_d[i+j]);
                //			}
                //		}
                b0 = __MIN(b0, __MAX(b, diff[i])); // b0 <= -THRESHOLD
                b0 = __MIN(b0, __MAX(b, diff[i + PSize / 2 + 1]));
            }
            NMS_score_buf[i][j] = __MAX(a0, -b0) - 1;


            ap_uint<1> posJudge[24];
#pragma HLS ARRAY_PARTITION variable=posJudge complete dim=1
            ap_uint<1> negJudge[24];
#pragma HLS ARRAY_PARTITION variable=negJudge complete dim=1

            for (ap_uint<5> ii = 0; ii < 16; ii++) {
#pragma HLS unroll
                posJudge[ii] = (diff[ii] > THRESHOLD) ? 1 : 0;
                negJudge[ii] = (diff[ii] < -THRESHOLD) ? 1 : 0;
            }


            for (ap_uint<5> ii = 0; ii < 8; ii++) {
#pragma HLS unroll
                posJudge[ii + 16] = (diff[ii] > THRESHOLD) ? 1 : 0;
                negJudge[ii + 16] = (diff[ii] < -THRESHOLD) ? 1 : 0;
            }


            ap_uint<1> fpJudge[16];
#pragma HLS ARRAY_PARTITION variable=fpJudge complete dim=1
            for (ap_uint<5> ii = 0; ii < 16; ii++) {
#pragma HLS unroll
                fpJudge[ii] =
                        (posJudge[ii] & posJudge[ii + 1] & posJudge[ii + 2] & posJudge[ii + 3] & posJudge[ii + 4] &
                         posJudge[ii + 5] & posJudge[ii + 6] & posJudge[ii + 7] & posJudge[ii + 8]) |
                        (negJudge[ii] & negJudge[ii + 1] & negJudge[ii + 2] & negJudge[ii + 3] & negJudge[ii + 4] &
                         negJudge[ii + 5] & negJudge[ii + 6] & negJudge[ii + 7] & negJudge[ii + 8]);
            }
            NMS_FAST_buf[i][j] = (fpJudge[0] | fpJudge[1] | fpJudge[2] | fpJudge[3] | fpJudge[4] | fpJudge[5] |
                                  fpJudge[6] | fpJudge[7] | fpJudge[8] | fpJudge[9] | fpJudge[10] | fpJudge[11] |
                                  fpJudge[12] | fpJudge[13] | fpJudge[14] | fpJudge[15]);
            if (NMS_FAST_buf[i][j] == 0)
                NMS_score_buf[i][j] = 0;

        }
        ///////////////////////////
        for (ap_uint<11> loop_ind = 2 + PROCESS_NUM; loop_ind < (2 + PROCESS_NUM)*2; loop_ind++) {
#pragma HLS UNROLL
            ap_uint<3> i = loop_ind / (2 + PROCESS_NUM);
            ap_uint<8> j = loop_ind - i*(2 + PROCESS_NUM);
            short int diff[25];
#pragma HLS ARRAY_PARTITION variable=diff complete dim=1

            diff[0] = win_buf_center(3, 3) - win_buf_center(0, 3);
            diff[1] = win_buf_center(3, 3) - win_buf_center(0, 4);
            diff[2] = win_buf_center(3, 3) - win_buf_center(1, 5);
            diff[3] = win_buf_center(3, 3) - win_buf_center(2, 6);
            diff[4] = win_buf_center(3, 3) - win_buf_center(3, 6);
            diff[5] = win_buf_center(3, 3) - win_buf_center(4, 6);
            diff[6] = win_buf_center(3, 3) - win_buf_center(5, 5);
            diff[7] = win_buf_center(3, 3) - win_buf_center(6, 4);
            diff[8] = win_buf_center(3, 3) - win_buf_center(6, 3);
            diff[9] = win_buf_center(3, 3) - win_buf_center(6, 2);
            diff[10] = win_buf_center(3, 3) - win_buf_center(5, 1);
            diff[11] = win_buf_center(3, 3) - win_buf_center(4, 0);
            diff[12] = win_buf_center(3, 3) - win_buf_center(3, 0);
            diff[13] = win_buf_center(3, 3) - win_buf_center(2, 0);
            diff[14] = win_buf_center(3, 3) - win_buf_center(1, 1);
            diff[15] = win_buf_center(3, 3) - win_buf_center(0, 2);

            diff[16] = diff[0];
            diff[17] = diff[1];
            diff[18] = diff[2];
            diff[19] = diff[3];
            diff[20] = diff[4];
            diff[21] = diff[5];
            diff[22] = diff[6];
            diff[23] = diff[7];
            diff[24] = diff[8];

            short int flag_d_min2[NUM - 1];
            short int flag_d_max2[NUM - 1];
            short int flag_d_min4[NUM - 3];
            short int flag_d_max4[NUM - 3];
            short int flag_d_min8[NUM - 7];
            short int flag_d_max8[NUM - 7];

            for (ap_uint<5> i = 0; i < NUM - 1; i++) {
#pragma HLS UNROLL
                flag_d_min2[i] = __MIN(diff[i], diff[i + 1]);
                flag_d_max2[i] = __MAX(diff[i], diff[i + 1]);
            }

            for (ap_uint<5> i = 0; i < NUM - 3; i++) {
#pragma HLS UNROLL
                flag_d_min4[i] = __MIN(flag_d_min2[i], flag_d_min2[i + 2]);
                flag_d_max4[i] = __MAX(flag_d_max2[i], flag_d_max2[i + 2]);
            }

            for (ap_uint<5> i = 0; i < NUM - 7; i++) {
#pragma HLS UNROLL
                flag_d_min8[i] = __MIN(flag_d_min4[i], flag_d_min4[i + 4]);
                flag_d_max8[i] = __MAX(flag_d_max4[i], flag_d_max4[i + 4]);
            }

            uchar_t a0 = THRESHOLD;

            for (ap_uint<5> i = 0; i < PSize; i += 2) {
#pragma HLS UNROLL
                short int a = 255;
                if (PSize == 16) {
                    a = flag_d_min8[i + 1];
                }
                //		else {
                //			for(ap_uint<5> j=1;j<PSize/2+1;j++)
                //			{
                //				a=__MIN(a,flag_d[i+j]);
                //			}
                //		}
                a0 = __MAX(a0, __MIN(a, diff[i])); // a0 >= THRESHOLD
                a0 = __MAX(a0, __MIN(a, diff[i + PSize / 2 + 1]));
            }
            short int b0 = -THRESHOLD;
            for (ap_uint<5> i = 0; i < PSize; i += 2) {
#pragma HLS UNROLL
                short int b = -255;
                if (PSize == 16) {
                    b = flag_d_max8[i + 1];
                }
                //		} else {
                //			for(ap_uint<5> j=1;j<PSize/2+1;j++)
                //			{
                //				b=__MAX(b,flag_d[i+j]);
                //			}
                //		}
                b0 = __MIN(b0, __MAX(b, diff[i])); // b0 <= -THRESHOLD
                b0 = __MIN(b0, __MAX(b, diff[i + PSize / 2 + 1]));
            }
            NMS_score_buf[i][j] = __MAX(a0, -b0) - 1;


            ap_uint<1> posJudge[24];
#pragma HLS ARRAY_PARTITION variable=posJudge complete dim=1
            ap_uint<1> negJudge[24];
#pragma HLS ARRAY_PARTITION variable=negJudge complete dim=1

            for (ap_uint<5> ii = 0; ii < 16; ii++) {
#pragma HLS unroll
                posJudge[ii] = (diff[ii] > THRESHOLD) ? 1 : 0;
                negJudge[ii] = (diff[ii] < -THRESHOLD) ? 1 : 0;
            }


            for (ap_uint<5> ii = 0; ii < 8; ii++) {
#pragma HLS unroll
                posJudge[ii + 16] = (diff[ii] > THRESHOLD) ? 1 : 0;
                negJudge[ii + 16] = (diff[ii] < -THRESHOLD) ? 1 : 0;
            }


            ap_uint<1> fpJudge[16];
#pragma HLS ARRAY_PARTITION variable=fpJudge complete dim=1
            for (ap_uint<5> ii = 0; ii < 16; ii++) {
#pragma HLS unroll
                fpJudge[ii] =
                        (posJudge[ii] & posJudge[ii + 1] & posJudge[ii + 2] & posJudge[ii + 3] & posJudge[ii + 4] &
                         posJudge[ii + 5] & posJudge[ii + 6] & posJudge[ii + 7] & posJudge[ii + 8]) |
                        (negJudge[ii] & negJudge[ii + 1] & negJudge[ii + 2] & negJudge[ii + 3] & negJudge[ii + 4] &
                         negJudge[ii + 5] & negJudge[ii + 6] & negJudge[ii + 7] & negJudge[ii + 8]);
            }
            NMS_FAST_buf[i][j] = (fpJudge[0] | fpJudge[1] | fpJudge[2] | fpJudge[3] | fpJudge[4] | fpJudge[5] |
                                  fpJudge[6] | fpJudge[7] | fpJudge[8] | fpJudge[9] | fpJudge[10] | fpJudge[11] |
                                  fpJudge[12] | fpJudge[13] | fpJudge[14] | fpJudge[15]);
            if (NMS_FAST_buf[i][j] == 0)
                NMS_score_buf[i][j] = 0;

        }
        ///////////////////////////
        for (ap_uint<11> loop_ind = (2 + PROCESS_NUM)*2; loop_ind < (2 + PROCESS_NUM)*3; loop_ind++) {
#pragma HLS UNROLL
            ap_uint<3> i = loop_ind / (2 + PROCESS_NUM);
            ap_uint<8> j = loop_ind - i*(2 + PROCESS_NUM);
            short int diff[25];
#pragma HLS ARRAY_PARTITION variable=diff complete dim=1

            diff[0] = win_buf_center(3, 3) - win_buf_center(0, 3);
            diff[1] = win_buf_center(3, 3) - win_buf_center(0, 4);
            diff[2] = win_buf_center(3, 3) - win_buf_center(1, 5);
            diff[3] = win_buf_center(3, 3) - win_buf_center(2, 6);
            diff[4] = win_buf_center(3, 3) - win_buf_center(3, 6);
            diff[5] = win_buf_center(3, 3) - win_buf_center(4, 6);
            diff[6] = win_buf_center(3, 3) - win_buf_center(5, 5);
            diff[7] = win_buf_center(3, 3) - win_buf_center(6, 4);
            diff[8] = win_buf_center(3, 3) - win_buf_center(6, 3);
            diff[9] = win_buf_center(3, 3) - win_buf_center(6, 2);
            diff[10] = win_buf_center(3, 3) - win_buf_center(5, 1);
            diff[11] = win_buf_center(3, 3) - win_buf_center(4, 0);
            diff[12] = win_buf_center(3, 3) - win_buf_center(3, 0);
            diff[13] = win_buf_center(3, 3) - win_buf_center(2, 0);
            diff[14] = win_buf_center(3, 3) - win_buf_center(1, 1);
            diff[15] = win_buf_center(3, 3) - win_buf_center(0, 2);

            diff[16] = diff[0];
            diff[17] = diff[1];
            diff[18] = diff[2];
            diff[19] = diff[3];
            diff[20] = diff[4];
            diff[21] = diff[5];
            diff[22] = diff[6];
            diff[23] = diff[7];
            diff[24] = diff[8];

            short int flag_d_min2[NUM - 1];
            short int flag_d_max2[NUM - 1];
            short int flag_d_min4[NUM - 3];
            short int flag_d_max4[NUM - 3];
            short int flag_d_min8[NUM - 7];
            short int flag_d_max8[NUM - 7];

            for (ap_uint<5> i = 0; i < NUM - 1; i++) {
#pragma HLS UNROLL
                flag_d_min2[i] = __MIN(diff[i], diff[i + 1]);
                flag_d_max2[i] = __MAX(diff[i], diff[i + 1]);
            }

            for (ap_uint<5> i = 0; i < NUM - 3; i++) {
#pragma HLS UNROLL
                flag_d_min4[i] = __MIN(flag_d_min2[i], flag_d_min2[i + 2]);
                flag_d_max4[i] = __MAX(flag_d_max2[i], flag_d_max2[i + 2]);
            }

            for (ap_uint<5> i = 0; i < NUM - 7; i++) {
#pragma HLS UNROLL
                flag_d_min8[i] = __MIN(flag_d_min4[i], flag_d_min4[i + 4]);
                flag_d_max8[i] = __MAX(flag_d_max4[i], flag_d_max4[i + 4]);
            }

            uchar_t a0 = THRESHOLD;

            for (ap_uint<5> i = 0; i < PSize; i += 2) {
#pragma HLS UNROLL
                short int a = 255;
                if (PSize == 16) {
                    a = flag_d_min8[i + 1];
                }
                //		else {
                //			for(ap_uint<5> j=1;j<PSize/2+1;j++)
                //			{
                //				a=__MIN(a,flag_d[i+j]);
                //			}
                //		}
                a0 = __MAX(a0, __MIN(a, diff[i])); // a0 >= THRESHOLD
                a0 = __MAX(a0, __MIN(a, diff[i + PSize / 2 + 1]));
            }
            short int b0 = -THRESHOLD;
            for (ap_uint<5> i = 0; i < PSize; i += 2) {
#pragma HLS UNROLL
                short int b = -255;
                if (PSize == 16) {
                    b = flag_d_max8[i + 1];
                }
                //		} else {
                //			for(ap_uint<5> j=1;j<PSize/2+1;j++)
                //			{
                //				b=__MAX(b,flag_d[i+j]);
                //			}
                //		}
                b0 = __MIN(b0, __MAX(b, diff[i])); // b0 <= -THRESHOLD
                b0 = __MIN(b0, __MAX(b, diff[i + PSize / 2 + 1]));
            }
            NMS_score_buf[i][j] = __MAX(a0, -b0) - 1;


            ap_uint<1> posJudge[24];
#pragma HLS ARRAY_PARTITION variable=posJudge complete dim=1
            ap_uint<1> negJudge[24];
#pragma HLS ARRAY_PARTITION variable=negJudge complete dim=1

            for (ap_uint<5> ii = 0; ii < 16; ii++) {
#pragma HLS unroll
                posJudge[ii] = (diff[ii] > THRESHOLD) ? 1 : 0;
                negJudge[ii] = (diff[ii] < -THRESHOLD) ? 1 : 0;
            }


            for (ap_uint<5> ii = 0; ii < 8; ii++) {
#pragma HLS unroll
                posJudge[ii + 16] = (diff[ii] > THRESHOLD) ? 1 : 0;
                negJudge[ii + 16] = (diff[ii] < -THRESHOLD) ? 1 : 0;
            }


            ap_uint<1> fpJudge[16];
#pragma HLS ARRAY_PARTITION variable=fpJudge complete dim=1
            for (ap_uint<5> ii = 0; ii < 16; ii++) {
#pragma HLS unroll
                fpJudge[ii] =
                        (posJudge[ii] & posJudge[ii + 1] & posJudge[ii + 2] & posJudge[ii + 3] & posJudge[ii + 4] &
                         posJudge[ii + 5] & posJudge[ii + 6] & posJudge[ii + 7] & posJudge[ii + 8]) |
                        (negJudge[ii] & negJudge[ii + 1] & negJudge[ii + 2] & negJudge[ii + 3] & negJudge[ii + 4] &
                         negJudge[ii + 5] & negJudge[ii + 6] & negJudge[ii + 7] & negJudge[ii + 8]);
            }
            NMS_FAST_buf[i][j] = (fpJudge[0] | fpJudge[1] | fpJudge[2] | fpJudge[3] | fpJudge[4] | fpJudge[5] |
                                  fpJudge[6] | fpJudge[7] | fpJudge[8] | fpJudge[9] | fpJudge[10] | fpJudge[11] |
                                  fpJudge[12] | fpJudge[13] | fpJudge[14] | fpJudge[15]);
            if (NMS_FAST_buf[i][j] == 0)
                NMS_score_buf[i][j] = 0;

        }
        ///////////////////////////
    ap_uint<3> i = 1;
    for (ap_uint<8> j = 1; j < PROCESS_NUM + 1; j++) {
#pragma HLS unroll
        ap_uint<18> psum[6];
#pragma HLS ARRAY_PARTITION variable=psum complete dim=1
        psum[0] = ((win_buf_center(0, 2) + win_buf_center(0, 3)) + (win_buf_center(0, 4) + win_buf_center(6, 2))) +
                  ((win_buf_center(6, 3) + win_buf_center(6, 4)) + (win_buf_center(2, 6) + win_buf_center(3, 6))) +
                  ((win_buf_center(4, 6) + win_buf_center(2, 0)) + (win_buf_center(3, 0) + win_buf_center(4, 0)));
        psum[1] = (win_buf_center(1, 1) + win_buf_center(1, 5)) + (win_buf_center(5, 1) + win_buf_center(5, 5));
        psum[2] = ((win_buf_center(1, 2) + win_buf_center(1, 4)) + (win_buf_center(2, 1) + win_buf_center(4, 1))) +
                  ((win_buf_center(2, 5) + win_buf_center(4, 5)) + (win_buf_center(5, 2) + win_buf_center(5, 4)));
        psum[3] = (win_buf_center(1, 3) + win_buf_center(3, 1)) + (win_buf_center(5, 3) + win_buf_center(3, 5));
        psum[4] = (win_buf_center(2, 2) + win_buf_center(4, 2)) + (win_buf_center(2, 4) + win_buf_center(4, 4));
        psum[5] = (win_buf_center(2, 3) + win_buf_center(3, 2)) + (win_buf_center(4, 3) + win_buf_center(3, 4));

        ap_uint<18> S_psum[7];
#pragma HLS ARRAY_PARTITION variable=psum complete dim=1
        S_psum[0] = psum[0];
        S_psum[1] = psum[1] << 1;
        S_psum[2] = psum[2] + (psum[2] << 2);
        S_psum[3] = psum[3] + (psum[3] << 1) + (psum[3] << 2);
        S_psum[4] = psum[4] + (psum[4] << 2) + (psum[4] << 3);
        S_psum[5] = psum[5] + (psum[5] << 4);
        S_psum[6] = win_buf_center(3, 3) + (win_buf_center(3, 3) << 1) + (win_buf_center(3, 3) << 2) +
                    (win_buf_center(3, 3) << 4);

        ap_uint<18> simsim =
                ((S_psum[0] + S_psum[1]) + (S_psum[2] + S_psum[3])) + ((S_psum[4] + S_psum[5]) + S_psum[6]);
        gaus_buf[j - 1] = (simsim >> 8).range(7, 0);

        ap_uint<1> scoreJudge[3][3];
        for (ap_uint<3> ii = i - 1; ii < i + 2; ii++) {
#pragma HLS unroll
            for (ap_uint<8> jj = j - 1; jj < j + 2; jj++) {
#pragma HLS unroll
                scoreJudge[ii - i + 1][jj - j + 1] = (NMS_score_buf[ii][jj] >= NMS_score_buf[i][j]) ? 0 : 1;
            }
        }
        FAST_buf[j - 1].range(0, 0) =
                NMS_FAST_buf[i][j] & scoreJudge[0][0] & scoreJudge[0][1] & scoreJudge[0][2] & scoreJudge[1][0] &
                scoreJudge[1][2] & scoreJudge[2][0] & scoreJudge[2][1] & scoreJudge[2][2];
        FAST_buf[j - 1].range(7, 1) = NMS_score_buf[i][j] >> 1;
    }
}

void process_output(hls::stream <ap_uint<PROCESS_BIT> > &gausData, hls::stream <ap_uint<PROCESS_BIT> > &FASTData,
                    hls::stream <ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outPixelStream,
                    hls::stream <ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> > &outFASTStream) {
//     output to PS
//     ap_uint<PROCESS_BIT> pixel_data = 0;
//     ap_uint<PROCESS_BIT> FAST_data = 0;
//     ap_uint<OUTPUT_BIT> pixel_write_tmp = 0;
//     ap_uint<OUTPUT_BIT> FAST_write_tmp = 0;
//     ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> outPixel, outFAST;
//     ap_uint<8> pixel_num = 0;
//     ap_uint<8> rmn_num = 0;
//     ap_uint<PIXEL_NUM_BIT> p_cnt = 0;

//     for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < height; row_ind++) {
//         ap_uint<WIDTH_BIT> read_ind = 0;
//         while (read_ind < unit_num || rmn_num >= OUTPUT_PIXEL_NUM) {
// #pragma HLS PIPELINE

//             if (rmn_num >= OUTPUT_PIXEL_NUM) {
//                 rmn_num = rmn_num - OUTPUT_PIXEL_NUM;
//                 outPixel.data = pixel_write_tmp;
//                 outFAST.data = FAST_write_tmp;
//                 for (ap_uint<8> i = 0; i < OUTPUT_BIT >> 3; i++) {
// #pragma HLS UNROLL
//                     outPixel.keep.range(i, i) = 1;
//                     outFAST.keep.range(i, i) = 1;
//                 }
//                 if (p_cnt == width * height && rmn_num == 0) {
//                     outPixel.last = 1;
//                     outFAST.last = 1;
//                 } else {
//                     outPixel.last = 0;
//                     outFAST.last = 0;
//                 }
//                 outPixelStream.write(outPixel);
//                 outFASTStream.write(outFAST);
//                 pixel_write_tmp = 0;
//                 FAST_write_tmp = 0;
//                 if (rmn_num > 0) {
//                     pixel_write_tmp = pixel_data.range(pixel_num * PIXEL_BIT - 1, (pixel_num - rmn_num) * PIXEL_BIT);
//                     FAST_write_tmp = FAST_data.range(pixel_num * PIXEL_BIT - 1, (pixel_num - rmn_num) * PIXEL_BIT);
//                 }
//             } else {
//                 pixel_data = gausData.read();
//                 FAST_data = FASTData.read();
//                 if (read_ind == unit_num - 1)
//                     pixel_num = width - read_ind * PROCESS_NUM;
//                 else
//                     pixel_num = PROCESS_NUM;
//                 p_cnt = p_cnt + pixel_num;
//                 read_ind = read_ind + 1;
//                 if (rmn_num + pixel_num >= OUTPUT_PIXEL_NUM) {
//                     pixel_write_tmp.range(OUTPUT_BIT - 1, rmn_num * PIXEL_BIT) = pixel_data.range(
//                             OUTPUT_BIT - 1 - rmn_num * PIXEL_BIT, 0);
//                     FAST_write_tmp.range(OUTPUT_BIT - 1, rmn_num * PIXEL_BIT) = FAST_data.range(
//                             OUTPUT_BIT - 1 - rmn_num * PIXEL_BIT, 0);
//                     rmn_num = rmn_num + pixel_num - OUTPUT_PIXEL_NUM;
//                     outPixel.data = pixel_write_tmp;
//                     outFAST.data = FAST_write_tmp;
//                     for (ap_uint<8> i = 0; i < OUTPUT_BIT >> 3; i++) {
// #pragma HLS UNROLL
//                         outPixel.keep.range(i, i) = 1;
//                         outFAST.keep.range(i, i) = 1;
//                     }
//                     if (p_cnt == width * height && rmn_num == 0) {
//                         outPixel.last = 1;
//                         outFAST.last = 1;
//                     } else {
//                         outPixel.last = 0;
//                         outFAST.last = 0;
//                     }
//                     outPixelStream.write(outPixel);
//                     outFASTStream.write(outFAST);
//                     pixel_write_tmp = 0;
//                     FAST_write_tmp = 0;
//                     if (rmn_num > 0) {
//                         pixel_write_tmp = pixel_data.range(pixel_num * PIXEL_BIT - 1,
//                                                            (pixel_num - rmn_num) * PIXEL_BIT);
//                         FAST_write_tmp = FAST_data.range(pixel_num * PIXEL_BIT - 1, (pixel_num - rmn_num) * PIXEL_BIT);
//                     }
//                 } else {
//                     if (pixel_num > 0) {
//                         pixel_write_tmp.range((rmn_num + pixel_num) * PIXEL_BIT - 1,
//                                               rmn_num * PIXEL_BIT) = pixel_data.range(pixel_num * PIXEL_BIT - 1, 0);
//                         FAST_write_tmp.range((rmn_num + pixel_num) * PIXEL_BIT - 1,
//                                              rmn_num * PIXEL_BIT) = FAST_data.range(pixel_num * PIXEL_BIT - 1, 0);
//                     }
//                     rmn_num = rmn_num + pixel_num;
//                 }
//             }
//         }
//     }
//     if (rmn_num > 0) {
//         outPixel.data = pixel_write_tmp;
//         outFAST.data = FAST_write_tmp;
//         for (ap_uint<8> i = 0; i < OUTPUT_BIT >> 3; i++) {
// #pragma HLS UNROLL
//             outPixel.keep.range(i, i) = 1;
//             outFAST.keep.range(i, i) = 1;
//         }
//         outPixel.last = 1;
//         outFAST.last = 1;
//         outPixelStream.write(outPixel);
//         outFASTStream.write(outFAST);
//     }

//     output to PL, PROCESS_BIT must equal to OUTPUT_BIT
    ap_axiu<OUTPUT_STREAM_BIT, 1, 1, 1> outPixel, outFAST;
    for (ap_uint<HEIGHT_BIT> row_ind = 0; row_ind < height; row_ind++) {
        for (ap_uint<WIDTH_BIT> col_ind = 0; col_ind < unit_num; col_ind++) {
            ap_uint<PROCESS_BIT> pixel_data = gausData.read();
            ap_uint<PROCESS_BIT> FAST_data = FASTData.read();
            outPixel.data = pixel_data;
            outFAST.data = FAST_data;
            for (ap_uint<8> i = 0; i < OUTPUT_BIT >> 3; i++) {
#pragma HLS UNROLL
                outPixel.keep.range(i, i) = 1;
                outFAST.keep.range(i, i) = 1;
            }
            if (row_ind == height - 1 && col_ind == unit_num - 1){
                outPixel.last = 1;
                outFAST.last = 1;
            }else{
                outPixel.last = 0;
                outFAST.last = 0;
            }
            outPixelStream.write(outPixel);
            outFASTStream.write(outFAST);
        }
    }
}
