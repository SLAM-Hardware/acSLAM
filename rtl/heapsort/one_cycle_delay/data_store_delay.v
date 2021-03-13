`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
module data_store_delay
#(
	parameter DATA_WIDTH = 32,
	parameter ADDR_WIDTH = 5,
	parameter LEVEL = 1
)
(
	input clk,
//ports to up sort node, left and right
	input [DATA_WIDTH-1:0] lm_din, 
	input [ADDR_WIDTH-1:0] lm_addr,
	input lm_we,
	output [DATA_WIDTH-1:0] lm_dout,
	
	input [DATA_WIDTH-1:0] rm_din, 
	input [ADDR_WIDTH-1:0] rm_addr,
	input rm_we,
	output [DATA_WIDTH-1:0] rm_dout,
	
//ports to bottum sort node
	input [DATA_WIDTH-1:0] nl_din, 
	input [ADDR_WIDTH-1:0] nl_addr,
	input nl_we,
	input nl_branch	//0: left memory 1: right memory
    );
//port a for up sort node
//port b for bottum sort node
	
	wire we_b_lm;  
	wire we_b_rm; 
	wire [DATA_WIDTH-1:0] q_b_lm;
	wire [DATA_WIDTH-1:0] q_b_rm;
	
	assign we_b_lm = nl_we & !nl_branch;  
	assign we_b_rm = nl_we & nl_branch;
	
	
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