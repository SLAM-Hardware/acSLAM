# SLAM-Hardware
FPGA Hardware Implementation for SLAM

## How to rebuild Vivado project
### Step 1. Generate Vivado IP from HLS
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
### Step 2. Generate Heapsort IP
### Step 3. Rebuild the Vivado Project
### Step 4. Building ORB-SLAM2
### Step 5. TUM Example
### Step 6. KITTI Example