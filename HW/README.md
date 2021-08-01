# Rebuild IP

## Generate Vivado IP from HLS

In order to change the parallelism of the ORB extractor, please modify the code according to the comments in the source files.
The default in the code is 4 parallelism.

Open Vivado HLS 2019.1 command terminal and run these commands :

```
cd <path-to-proj>/HW/hls

cd resize
vivado_hls -f script.tcl

cd ..

cd FAST_extractor
vivado_hls -f script.tcl

cd ..

cd RS_BRIEF
vivado_hls -f script.tcl
```

## Heapsort IP

The ```<path-to-proj>/HW/rtl/Heapsort/ip``` folder holds the IP of Heapsort packaged with Vivado 2019.1.

The source files for Heasort IP are in the ```<path-to-proj>/HW/rtl/Heapsort/src``` and ```<path-to-proj>/HW/rtl/Heapsort/hdl``` folders.