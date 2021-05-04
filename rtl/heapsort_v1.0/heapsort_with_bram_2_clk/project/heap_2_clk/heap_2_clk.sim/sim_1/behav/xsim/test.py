import operator as op
import os

dinfile = "data_in.dat"
doutfile = "data_out.dat"
dinlist = []
doutlist = []


with open(dinfile) as din:
    for line in din:
        temp = line[0:-1]
        dinlist.append(temp)
        
        
with open(doutfile) as dout:
    for line in dout:
        temp = line[0:-1]
        doutlist.append(temp)
        
din = []        
for each in dinlist:
    din.append(int(each,16))
    
din.sort(reverse=True)
    
dout = []
for each in doutlist:
    dout.append(int(each,16))
    
dout.sort(reverse=True)

dinsort = []

print("期望输出",end = "    ")
print("实际输出")
for i in range(0,len(doutlist)):
    print(" "+hex(din[i]),end = "        ")
    print(hex(dout[i]))
    dinsort.append(din[i])
    # if op.eq(din[i],dout[i]) == False :
    #     break
   
print(int(op.eq(dinsort,dout)))