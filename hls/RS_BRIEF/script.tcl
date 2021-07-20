############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project RS_BRIEF
set_top RS_BRIEF
add_files ./FAST.h
add_files ./RS_BRIEF.cpp
add_files ./RS_BRIEF.h
add_files ./img_gray.h
add_files -tb ./tb_RS_BRIEF.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1"
set_part {xczu7ev-ffvc1156-2-e}
create_clock -period 8 -name default
config_sdx -target none
config_export -format ip_catalog -rtl verilog -vivado_optimization_level 2 -vivado_phys_opt place -vivado_report_level 0
#source "./RS_BRIEF/solution1/directives.tcl"
csim_design -clean -O
csynth_design
cosim_design -O
export_design -rtl verilog -format ip_catalog
