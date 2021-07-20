# SLAM-Hardware
FPGA Hardware Implementation for SLAM

## Generate Vivado IP from HLS
```
cd hls

cd resize
vivado_hls -f script.tcl

cd ..

cd FAST_extractor
vivado_hls -f script.tcl

cd ..

cd RS_BRIEF
vivado_hls -f script.tcl
```