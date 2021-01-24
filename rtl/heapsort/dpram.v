`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    17:17:04 01/09/2018 
// Design Name: 
// Module Name:    dpram_l 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module dpram
#(
parameter DATA_WIDTH = 32,
parameter ADDR_WIDTH = 5,
parameter LEVEL = 1
)
(
	input clk,  //Ê±ÖÓÖÜÆÚ
//port a
	input [DATA_WIDTH-1:0] data_a,  //ÊäÈëÊýŸÝA
	input we_a,    //ÐŽÊ¹ÄÜÐÅºÅ
	input [ADDR_WIDTH-1:0] addr_a,//µØÖ·A
	output reg [DATA_WIDTH-1:0] q_a,   //Êä³öÊýŸÝA
//port b	
	input [DATA_WIDTH-1:0] data_b,   //ÊäÈëÊýŸÝB
	input we_b,     //ÐŽÊ¹ÄÜÐÅºÅB
	input [ADDR_WIDTH-1:0] addr_b,   //µØÖ·b
	output reg [DATA_WIDTH-1:0] q_b    //Êä³öÊýŸÝB
 );
(* ram_style = "block" *)//ÌáÊŸ×ÛºÏ¹€ŸßÊ¹ÓÃBlock RAMÀŽÊµÏÖ
	localparam MEM_SIZE = 1<<LEVEL; //Ž¢ŽæÆ÷ŽóÐ¡/ÈÝÁ¿
	reg [DATA_WIDTH-1:0] ram[MEM_SIZE-1:0];  //body of the RAM	
generate
	if (MEM_SIZE==1) begin
		always@(posedge clk) begin
			if (we_a) begin   //ÐŽÊ¹ÄÜÐÅºÅAŽŠÀí
				q_a <= data_a;
				q_b <= data_a;
				ram[0] <= data_a;
			end
			else if (we_b) begin //ÐŽÊ¹ÄÜÐÅºÅBŽŠÀí
				q_a <= data_b;
				q_b <= data_b;
				ram[0] <= data_b;
			end
			else begin  //Ê¹ÄÜÐÅºÅÈ«ÊÇ0£¬Ïàµ±ÓÚ¶ÁÊ¹ÄÜÐÅºÅ
				q_a <= ram[0];
				q_b <= ram[0];
			end
		end
	end
	else begin	
		always@(posedge clk) begin		
			if (we_a) begin  //aÊýŸÝÐŽœø¶ÔÓŠÎ»ÖÃ
				ram[addr_a] <= data_a;
			end           //aÊýŸÝÊä³ö
				q_a <= ram[addr_a];
		end
		always@(posedge clk) begin   //Í¬Àí
			if (we_b) begin
				ram[addr_b] <= data_b;
			end
			q_b <= ram[addr_b];
		end
	end
endgenerate

endmodule