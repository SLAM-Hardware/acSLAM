`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 	SJTU
// Engineer: 	Randy Suen
// 
// Create Date:    15:45:11 01/12/2018 
// Design Name: 
// Module Name:    data_store 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description:	
//		This file is used as the data storage element in the heap.
//		Two DPRAMs serve for left and right branches of the previous sorting node respectively, 
//		and together for up branch of the next sorting node.
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module data_store
#(
	parameter DATA_WIDTH = 32,
	parameter ADDR_WIDTH = 5,
	parameter LEVEL = 1
)
(
	input clk,
//ports to up sort node, left and right
	input [DATA_WIDTH-1:0] lm_din,  //×óÉÏÅÅÐòœÚµãÁ¬œÓ
	input [ADDR_WIDTH-1:0] lm_addr,
	input lm_we,
	output [DATA_WIDTH-1:0] lm_dout,
	
	input [DATA_WIDTH-1:0] rm_din,  //ÓÒ
	input [ADDR_WIDTH-1:0] rm_addr,
	input rm_we,
	output [DATA_WIDTH-1:0] rm_dout,
	
//ports to bottum sort node
	input [DATA_WIDTH-1:0] nl_din,   //ÏÂÅÅÐòœÚµãÁ¬œÓ
	input [ADDR_WIDTH-1:0] nl_addr,
	input nl_we,
	input nl_branch,	//0: left memory 1: right memory
	output [DATA_WIDTH-1:0] nl_dout
	
    );
//port a for up sort node
//port b for bottum sort node
	
	wire we_b_lm;   //×ó²àÏÂÊ¹ÄÜÐÅºÅ
	wire we_b_rm;   //ÓÒ²àÏÂÊ¹ÄÜÐÅºÅ
	wire [DATA_WIDTH-1:0] q_b_lm;  //×ó²àÏÂÊä³ö
	wire [DATA_WIDTH-1:0] q_b_rm;
	
	assign we_b_lm = nl_we & !nl_branch;  //ÏÂÊ¹ÄÜºÍÑ¡ÔñÐÅºÅÍ¬Ê±ÖžÏò
	assign we_b_rm = nl_we & nl_branch;
	assign nl_dout = nl_branch ? q_b_rm : q_b_lm;  //¶ÁÈ¡¶ÔÓŠÊýŸÝ
	
	
	
	dpram #(.DATA_WIDTH(DATA_WIDTH), .ADDR_WIDTH(ADDR_WIDTH), .LEVEL(LEVEL)) lm(
		.clk(clk), 
		.data_a(lm_din), 
		.we_a(lm_we), 
		.addr_a(lm_addr), 
		.q_a(lm_dout), 
		.data_b(nl_din), 
		.we_b(we_b_lm), 
		.addr_b(nl_addr), 
		.q_b(q_b_lm)
	);
	
	dpram #(.DATA_WIDTH(DATA_WIDTH), .ADDR_WIDTH(ADDR_WIDTH), .LEVEL(LEVEL)) rm(
		.clk(clk),
		.data_a(rm_din), 
		.we_a(rm_we), 
		.addr_a(rm_addr), 
		.q_a(rm_dout), 
		.data_b(nl_din), 
		.we_b(we_b_rm), 
		.addr_b(nl_addr), 
		.q_b(q_b_rm)
	);
	
endmodule