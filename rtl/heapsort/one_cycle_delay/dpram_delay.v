`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
module dpram_delay
#(
parameter DATA_WIDTH = 32,
parameter ADDR_WIDTH = 5,
parameter LEVEL = 1
)
(
	input clk,
//port a
	input [DATA_WIDTH-1:0] data_a, 
	input we_a,    
	input [ADDR_WIDTH-1:0] addr_a,
	output [DATA_WIDTH-1:0] q_a,  
//port b	
	input [DATA_WIDTH-1:0] data_b,  
	input we_b,    
	input [ADDR_WIDTH-1:0] addr_b,  
	output [DATA_WIDTH-1:0] q_b  
 );

 
reg [DATA_WIDTH-1:0] q_a_delay;
reg [DATA_WIDTH-1:0] q_b_delay;

(* ram_style = "block" *)
	localparam MEM_SIZE = 1<<LEVEL; 
	reg [DATA_WIDTH-1:0] ram[MEM_SIZE-1:0];  //body of the RAM	
generate
	if (MEM_SIZE==1) begin
		always@(posedge clk) begin
			if (we_a) begin  
//				q_a <= data_a;
//				q_b <= data_a;
				ram[0] <= data_a;
			end
			else if (we_b) begin
//				q_a <= data_b;
//				q_b <= data_b;
				ram[0] <= data_b;
			end
			else begin 
				ram[0] <= ram[0];
//				q_b <= ram[0];
			end
		end
        assign q_b = ram[0];
	    assign q_a = ram[0];
	end
	else begin	
		always@(posedge clk) begin		
			if (we_a) begin
				ram[addr_a] <= data_a;
			end        
//            q_a <= ram[addr_a];
		    else
			if (we_b) begin
				ram[addr_b] <= data_b;
			end
			
//			q_b <= ram[addr_b];
		end
	    assign q_b = ram[addr_b];
	    assign q_a = ram[addr_a];
	end
endgenerate

always@(posedge clk)
begin
	q_b_delay <= q_b;
	q_a_delay <= q_a;
end

endmodule