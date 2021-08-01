# Software
## jupyter_notebooks
This folder holds jupyter_notebook file and the .bit and .hwh files for PYNQ.

Run code in ```<path-to-proj>/SW/jupyter_notebooks/ORB/ORB.ipynb``` to overlay the bitstream and test the hardware.

## ORB_SLAM2_FPGA
Please install the dependency library according to the https://github.com/raulmur/ORB_SLAM2 first.

Build ORB_SLAM2_FPGA system:
```
cd <path-to-proj>/SW/ORB_SLAM2_FPGA
./build.sh
```

TUM example:

**Note: Needs to be run with the bitstream already overlayed** 

```
./Examples/RGB-D/rgbd_tum Vocabulary/RSBvoc.txt Examples/RGB-D/TUMX.yaml PATH_TO_SEQUENCE_FOLDER ASSOCIATIONS_FILE
```
