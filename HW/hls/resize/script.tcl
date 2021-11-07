############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project resize
set_top resize
add_files ./resize.h
add_files ./resize.cpp
add_files -tb ./resize_tb.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1"
set_part {xczu7ev-ffvc1156-2-e}
create_clock -period 8 -name default
config_export -format ip_catalog -rtl verilog -vivado_optimization_level 2 -vivado_phys_opt place -vivado_report_level 0
config_sdx -optimization_level none -target none
#source "./resize/solution1/directives.tcl"
#csim_design -O
csynth_design
#cosim_design
export_design -rtl verilog -format ip_catalog
