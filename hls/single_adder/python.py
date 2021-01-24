from pynq import Overlay
from pynq import allocate
import numpy as np
overlay = Overlay("./si_ad_3.bit")
overlay?
dma = overlay.axi_dma_0
dma.recvchannel.start()
dma.sendchannel.start()
dataIn = allocate((20,),np.uint8)
# dataIn = xlnk.cma_array((10,),np.uint8)
for i in range(20):
    dataIn[i] = 2*i
for i in range(20):
    print(dataIn[i])
dst1 = allocate((10,), np.uint16)
dst2 = allocate((10,), np.uint16)
dma.recvchannel.transfer(dst1)
dma.sendchannel.transfer(dataIn)
dma.sendchannel.wait()
dma.recvchannel.wait()
dma.recvchannel.transfer(dst2)
dma.recvchannel.wait()
dst1.flush()
dst2.flush()
dst1
dst2