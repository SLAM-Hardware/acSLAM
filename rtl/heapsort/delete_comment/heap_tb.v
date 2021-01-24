`timescale 1ns / 1ps

//////////////////////////////////////////////////////////////////////////////////
module heap_tb;
parameter DATA_WIDTH = 8;
parameter KEY_WIDTH = 4;
parameter NLEVELS = 3;


	// Inputs
	reg clk;
	reg rstn;
	reg [DATA_WIDTH-1:0] din;
	reg en;
	reg init;
	reg flush;
	reg waiting;
	// Outputs
	wire [DATA_WIDTH-1:0] dout;
	wire valid;

	// Instantiate the Unit Under Test (UUT)
	heap#(.DATA_WIDTH(DATA_WIDTH),.KEY_WIDTH(KEY_WIDTH),.NLEVELS(NLEVELS)) uut (
		.clk(clk), 
		.rstn(rstn), 
		.din(din), 
		.en(en), 
		.init(init),
		.flush(flush),
		.dout(dout), 
		.valid(valid)
	);

	initial begin
		// Initialize Inputs
		clk = 0;
		rstn = 0;
		din = 0;
		en = 0;
		init = 0;
		waiting = 1;
		flush = 0;
		// Wait 100 ns for global reset to finish
		#100;
      rstn = 1;
		
		// Add stimulus here
		#40;
		init = 1;
		#15;
		init = 0;
		
		#1000;
		waiting = 0;
		#800;
		waiting = 1;
		#200;
		flush = 1;
		#15;
		flush = 0;
		#1000;
		$stop;
	end
	

always#7 clk = ~clk;

always@(posedge clk or negedge rstn) begin
	if (!rstn) begin
		din <= 0;//({$random}%256)<<`VAL_WIDTH;
		en <= 0;
	end
	else if (!waiting) begin
		en <= ~en;
		if (en) begin
			din[KEY_WIDTH-1:0] <= ({$random}%256);
			din[DATA_WIDTH-1:KEY_WIDTH] <= 0;
        end
	end
	else
	   en <= 0;
end
		
wire [KEY_WIDTH-1:0] din_key;
wire [KEY_WIDTH-1:0] dout_key;

assign din_key = din[KEY_WIDTH-1:0];
assign dout_key = dout[KEY_WIDTH-1:0];

   
endmodule