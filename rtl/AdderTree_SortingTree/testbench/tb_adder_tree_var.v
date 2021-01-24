`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/26/2020 11:06:37 PM
// Design Name: 
// Module Name: tb_adder_tree_var
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module tb_adder_tree_var(

    );
      localparam NUM = 18;
    localparam LEN = 16;
    localparam LEVEL = 5;
    localparam LEN_LAST = LEN + LEVEL ;
    
    reg clk;
    reg signed [LEN-1:0] in_inner[NUM-1:0]; 
    reg [NUM*LEN-1:0] in;
    wire [LEN_LAST-1:0] sum;
     adder_tree_var_bit#(.NUM(NUM), .LEN(LEN)) dut (
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
