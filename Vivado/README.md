# Vivado

Open Vivado 2019.1 command terminal and run these commands to rebuild Vivado project:
```
cd <path-to-proj>/Vivado
source ./ORB_extractor_1_bd.tcl
```
The script will generate a project with a block design. Then please create the hdl wrapper and generate the bitstream in vivado.
```
make_wrapper -files [get_files <path-to-proj>/Vivado/myproj/project_1.srcs/sources_1/bd/ORB_extractor_1/ORB_extractor_1.bd] -top
add_files -norecurse <path-to-proj>/Vivado/myproj/project_1.srcs/sources_1/bd/ORB_extractor_1/hdl/ORB_extractor_1_wrapper.v
launch_runs impl_1 -to_step write_bitstream -jobs 48
```