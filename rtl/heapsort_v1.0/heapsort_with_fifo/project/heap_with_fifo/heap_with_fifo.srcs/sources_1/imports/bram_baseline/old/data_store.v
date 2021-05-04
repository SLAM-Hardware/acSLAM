`timescale 1ns / 1ps
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
	input [DATA_WIDTH-1:0] lm_din, 
	input [ADDR_WIDTH-1:0] lm_addr,
	input lm_we,
	output [DATA_WIDTH-1:0] lm_dout,
	
	input [DATA_WIDTH-1:0] rm_din,  //ÓÒ
	input [ADDR_WIDTH-1:0] rm_addr,
	input rm_we,
	output [DATA_WIDTH-1:0] rm_dout,
	
//ports to bottum sort node
	input [DATA_WIDTH-1:0] nl_din, 
	input [ADDR_WIDTH-1:0] nl_addr,
	input nl_we,
	input nl_branch,	//0: left memory 1: right memory
	output [DATA_WIDTH-1:0] nl_dout
	
    );
//port a for up sort node
//port b for bottum sort node

	localparam MEM_SIZE = 2<<LEVEL;
	
	wire we_b_lm;  
	wire we_b_rm; 
	wire [DATA_WIDTH-1:0] q_b_lm;
	wire [DATA_WIDTH-1:0] q_b_rm;
	
	assign we_b_lm = (lm_we & (lm_addr == nl_addr)) ? 1'b0 : nl_we & !nl_branch;  
	assign we_b_rm = (rm_we & (rm_addr == nl_addr)) ? 1'b0 : nl_we & nl_branch;
	assign nl_dout = nl_branch ? q_b_rm : q_b_lm; 
	
	block_ram_rtl #(.RAM_WIDTH(DATA_WIDTH),.RAM_DEPTH(MEM_SIZE),.ADDR_WIDTH(ADDR_WIDTH),.RAM_PERFORMANCE("LOW_LATENCY"),.INIT_FILE(""))
	lm (
	    .addra(lm_addr),   // Port A address bus, width determined from RAM_DEPTH
	    .addrb(nl_addr),   // Port B address bus, width determined from RAM_DEPTH
	    .dina(lm_din),     // Port A RAM input data, width determined from RAM_WIDTH
	    .dinb(nl_din),     // Port B RAM input data, width determined from RAM_WIDTH
	    .clka(clk),     // Clock
	    .wea(lm_we),       // Port A write enable
	    .web(we_b_lm),       // Port B write enable
	    .rsta(1'b0),     // Port A output reset (does not affect memory contents)
	    .rstb(1'b0),     // Port B output reset (does not affect memory contents)
	    .regcea(1'b0), // Port A output register enable
	    .regceb(1'b0), // Port B output register enable
	    .douta(lm_dout),   // Port A RAM output data, width determined from RAM_WIDTH
	    .doutb(q_b_lm)    // Port B RAM output data, width determined from RAM_WIDTH
  	);

  	block_ram_rtl #(.RAM_WIDTH(DATA_WIDTH),.RAM_DEPTH(MEM_SIZE),.ADDR_WIDTH(ADDR_WIDTH),.RAM_PERFORMANCE("LOW_LATENCY"),.INIT_FILE(""))
	rm (
	    .addra(rm_addr),   // Port A address bus, width determined from RAM_DEPTH
	    .addrb(nl_addr),   // Port B address bus, width determined from RAM_DEPTH
	    .dina(rm_din),     // Port A RAM input data, width determined from RAM_WIDTH
	    .dinb(nl_din),     // Port B RAM input data, width determined from RAM_WIDTH
	    .clka(clk),     // Clock
	    .wea(rm_we),       // Port A write enable
	    .web(we_b_rm),       // Port B write enable
	    .rsta(1'b0),     // Port A output reset (does not affect memory contents)
	    .rstb(1'b0),     // Port B output reset (does not affect memory contents)
	    .regcea(1'b0), // Port A output register enable
	    .regceb(1'b0), // Port B output register enable
	    .douta(rm_dout),   // Port A RAM output data, width determined from RAM_WIDTH
	    .doutb(q_b_rm)    // Port B RAM output data, width determined from RAM_WIDTH
  	);

endmodule