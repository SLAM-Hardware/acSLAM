onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -radix hexadecimal /heap_tb/uut/clk
add wave -noupdate -radix hexadecimal /heap_tb/uut/rstn
add wave -noupdate -radix hexadecimal /heap_tb/uut/din
add wave -noupdate -radix hexadecimal /heap_tb/uut/en
add wave -noupdate -radix hexadecimal /heap_tb/uut/init
add wave -noupdate -radix hexadecimal /heap_tb/uut/flush
add wave -noupdate -radix hexadecimal /heap_tb/uut/valid
add wave -noupdate -radix hexadecimal /heap_tb/uut/ready
add wave -noupdate -radix hexadecimal /heap_tb/uut/din_1
add wave -noupdate -radix hexadecimal /heap_tb/uut/din_2
add wave -noupdate -radix hexadecimal /heap_tb/uut/din_r
add wave -noupdate -radix hexadecimal /heap_tb/uut/cnt
add wave -noupdate -radix hexadecimal /heap_tb/uut/d_comp1
add wave -noupdate -radix hexadecimal /heap_tb/uut/d_comp2
add wave -noupdate -radix hexadecimal /heap_tb/uut/en_1
add wave -noupdate -radix hexadecimal /heap_tb/uut/en_2
add wave -noupdate -radix hexadecimal /heap_tb/uut/en_r
add wave -noupdate -radix hexadecimal /heap_tb/uut/dout1
add wave -noupdate -radix hexadecimal /heap_tb/uut/dout2
add wave -noupdate -radix hexadecimal /heap_tb/uut/din_fifo
add wave -noupdate -radix hexadecimal /heap_tb/uut/wr_en
add wave -noupdate -radix hexadecimal /heap_tb/uut/rd_en
add wave -noupdate -radix hexadecimal -childformat {{{/heap_tb/uut/dout_fifo[11]} -radix hexadecimal} {{/heap_tb/uut/dout_fifo[10]} -radix hexadecimal} {{/heap_tb/uut/dout_fifo[9]} -radix hexadecimal} {{/heap_tb/uut/dout_fifo[8]} -radix hexadecimal} {{/heap_tb/uut/dout_fifo[7]} -radix hexadecimal} {{/heap_tb/uut/dout_fifo[6]} -radix hexadecimal} {{/heap_tb/uut/dout_fifo[5]} -radix hexadecimal} {{/heap_tb/uut/dout_fifo[4]} -radix hexadecimal} {{/heap_tb/uut/dout_fifo[3]} -radix hexadecimal} {{/heap_tb/uut/dout_fifo[2]} -radix hexadecimal} {{/heap_tb/uut/dout_fifo[1]} -radix hexadecimal} {{/heap_tb/uut/dout_fifo[0]} -radix hexadecimal}} -subitemconfig {{/heap_tb/uut/dout_fifo[11]} {-height 15 -radix hexadecimal} {/heap_tb/uut/dout_fifo[10]} {-height 15 -radix hexadecimal} {/heap_tb/uut/dout_fifo[9]} {-height 15 -radix hexadecimal} {/heap_tb/uut/dout_fifo[8]} {-height 15 -radix hexadecimal} {/heap_tb/uut/dout_fifo[7]} {-height 15 -radix hexadecimal} {/heap_tb/uut/dout_fifo[6]} {-height 15 -radix hexadecimal} {/heap_tb/uut/dout_fifo[5]} {-height 15 -radix hexadecimal} {/heap_tb/uut/dout_fifo[4]} {-height 15 -radix hexadecimal} {/heap_tb/uut/dout_fifo[3]} {-height 15 -radix hexadecimal} {/heap_tb/uut/dout_fifo[2]} {-height 15 -radix hexadecimal} {/heap_tb/uut/dout_fifo[1]} {-height 15 -radix hexadecimal} {/heap_tb/uut/dout_fifo[0]} {-height 15 -radix hexadecimal}} /heap_tb/uut/dout_fifo
add wave -noupdate -radix hexadecimal -childformat {{{/heap_tb/uut/fifo/din[35]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[34]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[33]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[32]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[31]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[30]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[29]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[28]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[27]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[26]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[25]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[24]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[23]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[22]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[21]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[20]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[19]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[18]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[17]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[16]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[15]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[14]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[13]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[12]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[11]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[10]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[9]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[8]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[7]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[6]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[5]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[4]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[3]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[2]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[1]} -radix hexadecimal} {{/heap_tb/uut/fifo/din[0]} -radix hexadecimal}} -subitemconfig {{/heap_tb/uut/fifo/din[35]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[34]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[33]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[32]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[31]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[30]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[29]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[28]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[27]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[26]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[25]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[24]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[23]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[22]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[21]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[20]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[19]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[18]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[17]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[16]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[15]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[14]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[13]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[12]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[11]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[10]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[9]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[8]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[7]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[6]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[5]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[4]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[3]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[2]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[1]} {-height 15 -radix hexadecimal} {/heap_tb/uut/fifo/din[0]} {-height 15 -radix hexadecimal}} /heap_tb/uut/fifo/din
add wave -noupdate -radix hexadecimal /heap_tb/uut/fifo/wr_en
add wave -noupdate -radix hexadecimal /heap_tb/uut/fifo/rd_en
add wave -noupdate -radix hexadecimal /heap_tb/uut/fifo/dout
add wave -noupdate -radix hexadecimal {/heap_tb/uut/Heap1/loop_a[0]/U_DS/lm/BRAM}
add wave -noupdate -radix hexadecimal {/heap_tb/uut/Heap1/loop_a[0]/U_DS/rm/BRAM}
add wave -noupdate -radix hexadecimal {/heap_tb/uut/Heap1/loop_a[1]/U_DS/lm/BRAM}
add wave -noupdate -radix hexadecimal {/heap_tb/uut/Heap1/loop_a[1]/U_DS/rm/BRAM}
add wave -noupdate -radix hexadecimal {/heap_tb/uut/Heap2/loop_a[0]/U_DS/lm/BRAM}
add wave -noupdate -radix hexadecimal {/heap_tb/uut/Heap2/loop_a[0]/U_DS/rm/BRAM}
add wave -noupdate -radix hexadecimal {/heap_tb/uut/Heap2/loop_a[1]/U_DS/lm/BRAM}
add wave -noupdate -radix hexadecimal {/heap_tb/uut/Heap2/loop_a[1]/U_DS/rm/BRAM}
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {1988966 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 278
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ns
update
WaveRestoreZoom {1737066 ps} {2202305 ps}
