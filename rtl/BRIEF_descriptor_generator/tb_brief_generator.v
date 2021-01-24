`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 05/11/2020 08:13:10 AM
// Design Name: 
// Module Name: tb_brief_generator
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


module tb_brief_generator(

    );
    
    reg clk;
    reg start;
    wire busy;
    
    brief_generator_pipeline dut(
        .clk(clk),
        .start(start),
        .busy(busy)
    );
    
    initial
    begin
       clk = 0;
       start = 0;
       @(posedge clk);
       start = 1;
       @(posedge clk);
       start = 0;
    end
    
    
    always #5 clk = ~ clk;
    
endmodule
