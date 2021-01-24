`timescale 1ns / 1ps


module tb_max(

    );
    
    localparam NUM = 18;
    localparam LEN = 16;
    reg clk;
    reg [LEN-1:0] a_inner[NUM-1:0]; 
    reg [NUM*LEN-1:0] a;
    wire [LEN-1:0] max;
     max#(.NUM(NUM), .LEN(LEN)) dut (
     .clk(clk),
     .a(a),
     .max(max)
     );
    
    integer i;
    initial begin
        clk = 0;
        a = 0;
        for (i=0; i<NUM; i=i+1) 
        begin
            a_inner[i] = i;
        end
        
        for (i = 0; i < NUM; i = i + 1) begin
            a = {a,a_inner[i]}; 
        end
    end
    
    always #5 clk=~clk;
    

endmodule

