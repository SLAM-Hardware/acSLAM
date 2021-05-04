# README

heapsort_with_fifo是通过fifo进行一周期输入排序的代码，heapsort_with_bram_2_clk是两周期排序的代码。

下面的测试方法可以在heapsort_with_bram_2_clk里使用。若在heapsort_with_fifo中测试，需要把datatest.tcl复制到目录下，并参照修改testbench（声明一个seed，并从num.dat中读取）。

测试方法：

打开heapsort_with_bram_2_clk工程，tcl命令行执行：

```tcl
cd ./heap_2_clk.sim/sim_1/behav/xsim
source datatest.tcl
```

其中data.tcl内容：

```tcl
set n 1
set f [open ./res.dat w]
close $f
set f [open ./fault.dat w]
close $f
while {$n < 100} {
	set f [open ./num.dat w]
	set hex [format "%16x" $n]
	puts $f $hex
	close $f
	launch_simulation
	run all
	close_sim
	set result [exec python test.py]
	set f [open ./res.dat a]
	puts $f $result
	close $f
	if {$result == 0} {
		set f [open ./fault.dat a]
		puts $f $n
		close $f
	}
	incr n
}
```

脚本进行连续n次仿真。每次向num.dat写入一个数字n作为随机数种子的初始值，然后运行仿真。testbench中读取num.dat里数字写入seed中进行仿真，并将输入输出数据写入data_in.dat和data_out.dat中，随后脚本调用test.py对数据进行比对；若结果匹配返回1，结果不匹配返回0，并将对应的seed写入fault.dat中。test.py中的print取消注释可以查看输入输出数据。