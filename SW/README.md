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

**Note: It needs to run with the bitstream already overlayed and the board needs to be connected to the monitor. If there is no monitor, you need to turn off the display by changing ```ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::RGBD,true);``` to ```ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::RGBD,false);``` in the ```<path-to-proj>/SW/ORB_SLAM2_FPGA/Examples/RGB-D/rgbd_tum.cc.``` and rebuild the project.** 

```
./Examples/RGB-D/rgbd_tum Vocabulary/RSBvoc.txt Examples/RGB-D/TUMX.yaml PATH_TO_SEQUENCE_FOLDER ASSOCIATIONS_FILE
```
