############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project ORB_FPGA_jianming
set_top test_stream_in
add_files test_stream_in.h -cflags "-I../include/."
add_files include/test_stream_in.cpp -cflags "-I../include/."
add_files -tb image/0.txt
add_files -tb include/tb_test_stream_in.cpp
open_solution "solution6"
set_part {xczu7ev-ffvc1156-2-e}
create_clock -period 10 -name default
config_export -format ip_catalog -rtl verilog
#source "./ORB_FPGA_jianming/solution6/directives.tcl"
csim_design -O
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
