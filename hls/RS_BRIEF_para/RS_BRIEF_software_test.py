import cv2
import matplotlib.pyplot as plt
import numpy as np
import math
pattern = [
-5,4, -5,13,
    0,11, 4,1,
    -6,-2, -4,-1,
    -11,-4, -3,0,
    4,8, -5,2,
    5,3, -1,12,
    0,7, -5,5,
    9,-2, 8,9,
    -4,5, -2,14,
    2,11, 4,0,
    -6,-1, -4,0,
    -12,-2, -3,1,
    5,7, -5,3,
    5,2, 1,12,
    1,7, -4,6,
    8,-4, 10,7,
    -3,6, 0,14,
    4,10, 4,-1,
    -6,0, -4,1,
    -12,1, -3,1,
    7,6, -4,4,
    6,1, 4,11,
    3,6, -3,7,
    8,-5, 11,5,
    -2,6, 3,14,
    6,9, 4,-1,
    -6,2, -4,1,
    -11,3, -2,2,
    8,4, -3,4,
    6,0, 6,11,
    4,6, -1,7,
    6,-7, 12,3,
    -1,6, 6,13,
    8,8, 4,-2,
    -6,3, -4,2,
    -11,5, -2,2,
    8,3, -2,5,
    6,-1, 8,9,
    5,5, 0,7,
    5,-8, 12,1,
    1,6, 8,11,
    9,6, 3,-3,
    -5,4, -3,3,
    -9,7, -2,2,
    9,1, -1,5,
    5,-2, 9,7,
    6,4, 1,7,
    3,-9, 12,-2,
    2,6, 10,10,
    10,4, 2,-3,
    -4,5, -2,3,
    -8,9, -1,3,
    9,-1, 0,5,
    5,-3, 11,6,
    6,3, 3,7,
    2,-9, 11,-4,
    3,6, 12,7,
    11,2, 2,-4,
    -3,5, -2,4,
    -6,10, -1,3,
    9,-2, 1,5,
    4,-4, 12,3,
    7,1, 4,6,
    0,-9, 10,-6,
    4,5, 13,5,
    11,0, 1,-4,
    -2,6, -1,4,
    -4,11, 0,3,
    8,-4, 2,5,
    3,-5, 12,1,
    7,0, 5,5,
    -2,-9, 9,-8,
    5,4, 14,2,
    11,-2, 0,-4,
    -1,6, 0,4,
    -2,12, 1,3,
    7,-5, 3,5,
    2,-5, 12,-1,
    7,-1, 6,4,
    -4,-8, 7,-10,
    6,3, 14,0,
    10,-4, -1,-4,
    0,6, 1,4,
    1,12, 1,3,
    6,-7, 4,4,
    1,-6, 11,-4,
    6,-3, 7,3,
    -5,-8, 5,-11,
    6,2, 14,-3,
    9,-6, -1,-4,
    2,6, 1,4,
    3,11, 2,2,
    4,-8, 4,3,
    0,-6, 11,-6,
    6,-4, 7,1,
    -7,-6, 3,-12,
    6,1, 13,-6,
    8,-8, -2,-4,
    3,6, 2,4,
    5,11, 2,2,
    3,-8, 5,2,
    -1,-6, 9,-8,
    5,-5, 7,0,
    -8,-5, 1,-12,
    6,-1, 11,-8,
    6,-9, -3,-3,
    4,5, 3,3,
    7,9, 2,2,
    1,-9, 5,1,
    -2,-5, 7,-9,
    4,-6, 7,-1,
    -9,-3, -2,-12,
    6,-2, 10,-10,
    4,-10, -3,-2,
    5,4, 3,2,
    9,8, 3,1,
    -1,-9, 5,0,
    -3,-5, 6,-11,
    3,-6, 7,-3,
    -9,-2, -4,-11,
    6,-3, 7,-12,
    2,-11, -4,-2,
    5,3, 4,2,
    10,6, 3,1,
    -2,-9, 5,-1,
    -4,-4, 3,-12,
    1,-7, 6,-4,
    -9,0, -6,-10,
    5,-4, 5,-13,
    0,-11, -4,-1,
    6,2, 4,1,
    11,4, 3,0,
    -4,-8, 5,-2,
    -5,-3, 1,-12,
    0,-7, 5,-5,
    -9,2, -8,-9,
    4,-5, 2,-14,
    -2,-11, -4,0,
    6,1, 4,0,
    12,2, 3,-1,
    -5,-7, 5,-3,
    -5,-2, -1,-12,
    -1,-7, 4,-6,
    -8,4, -10,-7,
    3,-6, 0,-14,
    -4,-10, -4,1,
    6,0, 4,-1,
    12,-1, 3,-1,
    -7,-6, 4,-4,
    -6,-1, -4,-11,
    -3,-6, 3,-7,
    -8,5, -11,-5,
    2,-6, -3,-14,
    -6,-9, -4,1,
    6,-2, 4,-1,
    11,-3, 2,-2,
    -8,-4, 3,-4,
    -6,0, -6,-11,
    -4,-6, 1,-7,
    -6,7, -12,-3,
    1,-6, -6,-13,
    -8,-8, -4,2,
    6,-3, 4,-2,
    11,-5, 2,-2,
    -8,-3, 2,-5,
    -6,1, -8,-9,
    -5,-5, 0,-7,
    -5,8, -12,-1,
    -1,-6, -8,-11,
    -9,-6, -3,3,
    5,-4, 3,-3,
    9,-7, 2,-2,
    -9,-1, 1,-5,
    -5,2, -9,-7,
    -6,-4, -1,-7,
    -3,9, -12,2,
    -2,-6, -10,-10,
    -10,-4, -2,3,
    4,-5, 2,-3,
    8,-9, 1,-3,
    -9,1, 0,-5,
    -5,3, -11,-6,
    -6,-3, -3,-7,
    -2,9, -11,4,
    -3,-6, -12,-7,
    -11,-2, -2,4,
    3,-5, 2,-4,
    6,-10, 1,-3,
    -9,2, -1,-5,
    -4,4, -12,-3,
    -7,-1, -4,-6,
    0,9, -10,6,
    -4,-5, -13,-5,
    -11,0, -1,4,
    2,-6, 1,-4,
    4,-11, 0,-3,
    -8,4, -2,-5,
    -3,5, -12,-1,
    -7,0, -5,-5,
    2,9, -9,8,
    -5,-4, -14,-2,
    -11,2, 0,4,
    1,-6, 0,-4,
    2,-12, -1,-3,
    -7,5, -3,-5,
    -2,5, -12,1,
    -7,1, -6,-4,
    4,8, -7,10,
    -6,-3, -14,0,
    -10,4, 1,4,
    0,-6, -1,-4,
    -1,-12, -1,-3,
    -6,7, -4,-4,
    -1,6, -11,4,
    -6,3, -7,-3,
    5,8, -5,11,
    -6,-2, -14,3,
    -9,6, 1,4,
    -2,-6, -1,-4,
    -3,-11, -2,-2,
    -4,8, -4,-3,
    0,6, -11,6,
    -6,4, -7,-1,
    7,6, -3,12,
    -6,-1, -13,6,
    -8,8, 2,4,
    -3,-6, -2,-4,
    -5,-11, -2,-2,
    -3,8, -5,-2,
    1,6, -9,8,
    -5,5, -7,0,
    8,5, -1,12,
    -6,1, -11,8,
    -6,9, 3,3,
    -4,-5, -3,-3,
    -7,-9, -2,-2,
    -1,9, -5,-1,
    2,5, -7,9,
    -4,6, -7,1,
    9,3, 2,12,
    -6,2, -10,10,
    -4,10, 3,2,
    -5,-4, -3,-2,
    -9,-8, -3,-1,
    1,9, -5,0,
    3,5, -6,11,
    -3,6, -7,3,
    9,2, 4,11,
    -6,3, -7,12,
    -2,11, 4,2,
    -5,-3, -4,-2,
    -10,-6, -3,-1,
    2,9, -5,1,
    4,4, -3,12,
    -1,7, -6,4,
    9,0, 6,10]

PATCH_SIZE = 29
HALF_PATCH_SIZE = PATCH_SIZE>>1
umax = np.zeros(HALF_PATCH_SIZE + 1)
vmax = math.floor(HALF_PATCH_SIZE * math.sqrt((float)(2))/2+ 1 )
vmin = math.ceil(HALF_PATCH_SIZE * math.sqrt((float)(2)) / 2)
hp2 = HALF_PATCH_SIZE*HALF_PATCH_SIZE
for v in range(0,(int)(vmax)+1):
    umax[int(v)]= round(math.sqrt(hp2 - v*v))
v0 = 0
for v in range(HALF_PATCH_SIZE,int(vmin)-1,-1):
    while (umax[v0] == umax[v0+1]):
        v0 = v0 + 1
    umax[int(v)]=v0
    v0 = v0+1
mask = [[0 for i in range(PATCH_SIZE)]for j in range(PATCH_SIZE)]
for i in range(-HALF_PATCH_SIZE,HALF_PATCH_SIZE+1):
    for j in range(-HALF_PATCH_SIZE,HALF_PATCH_SIZE+1):
        if (umax[abs(j)]>=abs(i)):
            mask[i+HALF_PATCH_SIZE][j+HALF_PATCH_SIZE] = 255

# img_ori = cv2.imread('000000'+'.png')
# img_gray = cv2.cvtColor(img_ori,cv2.COLOR_BGR2GRAY)
# print(img_gray)
f = open("C:/slam/HLS_project/fast_para_copy/result_test.txt")
data = f.readlines()

width = int(data[0][:-1])
height = int(data[1][:-1])

img_gray = np.zeros([height, width], dtype=np.uint8)
fast = np.zeros([height, width], dtype=np.uint8)
cnt = 0
for line in data[2:]:
    l = line[:-1].split(' ')
    img_gray[int(cnt / width), cnt % width] = int(l[0])
    fast[int(cnt / width), cnt % width] = int(l[1])
    cnt = cnt + 1
print(img_gray)

img_gray=cv2.copyMakeBorder(img_gray,14,14,14,14,cv2.BORDER_CONSTANT,value=0)
x = 149
y = 495
x=x+14
y=y+14
m01 = 0
m10 = 0
buf = [[0 for i in range(PATCH_SIZE)]for j in range(PATCH_SIZE)]
for i in range(x-HALF_PATCH_SIZE,x+HALF_PATCH_SIZE+1):
    for j in range(y-HALF_PATCH_SIZE,y+HALF_PATCH_SIZE+1):
        buf[i-x+HALF_PATCH_SIZE][j-y+HALF_PATCH_SIZE]=img_gray[i][j]
for i in buf:
    print(i)
for i in range(PATCH_SIZE):
    tmp1 = 0
    tmp2 = 0
    for im in range(PATCH_SIZE):
        tmp1 = tmp1 + (buf[i][im]&mask[i][im])
        # print((buf[i][im]&mask[i][im]), end=' ')
        tmp2 = tmp2 + (buf[im][i]&mask[im][i])
    # print("")
    m01 = m01 + tmp1*(i-HALF_PATCH_SIZE)
    m10 = m10 + tmp2*(i-HALF_PATCH_SIZE)
print(m01)
print(m10)

angle = math.atan2(m01,m10)
if angle < 0:
    angle = angle + 3.14159265358*2
bias = round(angle / 0.1963495408)
print(angle)
print(bias)
if (bias == 32):
    bias = 0
des=np.zeros(256)
for i in range(256):
    tmp = int((i +(32-bias)*8)%256)
    t0 = HALF_PATCH_SIZE+pattern[i*4]
    t1 = HALF_PATCH_SIZE+pattern[i*4+1]
    t2 = HALF_PATCH_SIZE+pattern[i*4+2]
    t3 = HALF_PATCH_SIZE+pattern[i*4+3]
    des[tmp]=buf[t0][t1]<buf[t2][t3]
    # print(t0, t1)
# print(des)

for i in range(8):
    tmp = 0
    for j in range(32):
        tmp += des[i*32+j]*pow(2,j)
    print(tmp, end = ' ')

    