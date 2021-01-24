`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 05/02/2020 08:56:30 PM
// Design Name: 
// Module Name: tb_single_buffer
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


module tb_single_buffer(

    );
      reg clk;
    
    single_buffer dut(
        .clk(clk)
    );
    
    initial begin
     clk = 0;
    end

    always#5 clk= ~clk;
    
endmodule
