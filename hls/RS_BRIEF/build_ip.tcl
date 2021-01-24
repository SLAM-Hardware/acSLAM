############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project RS_BRIEF
set_top RS_BRIEF
add_files ./RS_BRIEF.cpp -cflags "-I ../xfopencv/include -D__SDSVHLS__"
add_files ./RS_BRIEF.h
open_solution "solution1"
set_part {xczu3eg-sbva484-1-e} -tool vivado
create_clock -period 3 -name default
config_export -format ip_catalog -rtl verilog
csynth_design
export_design -flow impl -rtl verilog -format ip_catalog
close_project

