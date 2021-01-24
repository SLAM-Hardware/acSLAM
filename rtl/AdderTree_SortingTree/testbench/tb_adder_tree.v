`timescale 1ns / 1ps


module tb_adder_tree(

    );
    
    localparam NUM = 18;
    localparam LEN = 16;
    reg clk;
    reg signed [LEN-1:0] in_inner[NUM-1:0]; 
    reg [NUM*LEN-1:0] in;
    wire [LEN-1:0] sum;
     adder_tree#(.NUM(NUM), .LEN(LEN)) dut (
     .clk(clk),
     .in(in),
     .sum(sum)
     );
    
    integer i;
    initial begin
        clk = 0;
        in = 0;
        for (i=0; i<NUM; i=i+1) 
        begin
            in_inner[i] = 1;
        end
        
        for (i = 0; i < NUM; i = i + 1) begin
            in = {in,in_inner[i]}; 
        end
    end
    
    always #5 clk=~clk;
    

endmodule

