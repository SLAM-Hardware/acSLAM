############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project single_adder
set_top adder
add_files adder.cpp -cflags "-I/home/work/cag/vivado/single_adder/."
add_files -tb adder_tb.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution4"
set_part {xczu7ev-ffvc1156-2-e}
create_clock -period 10 -name default
config_export -format ip_catalog -rtl verilog
#source "./single_adder/solution4/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
