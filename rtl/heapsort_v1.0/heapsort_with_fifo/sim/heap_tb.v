`timescale 1ns / 1ps

//////////////////////////////////////////////////////////////////////////////////
module heap_tb;
parameter DATA_WIDTH = 16;
parameter KEY_WIDTH = 12;
parameter NLEVELS = 7;


	// Inputs
	reg clk;
	reg rstn;
	reg [DATA_WIDTH-1:0] din;
	reg data_en;
	reg init;
	reg flush;
	reg waiting;
	// Outputs
	//wire [DATA_WIDTH-1:0] dout1,dout2;
	wire valid;

	`ifdef _MAX_  
	localparam INIT_DATA = {2'b11,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b1}}};
	localparam FLUSH_DATA = {2'b01,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b0}}};
	`else
	localparam INIT_DATA = {2'b01,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b0}}};
	localparam FLUSH_DATA = {2'b11,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b1}}};
	`endif

	// Instantiate the Unit Under Test (UUT)
	heap_rtl#(.DATA_WIDTH(DATA_WIDTH),.KEY_WIDTH(KEY_WIDTH),.NLEVELS(NLEVELS)) uut (
		.clk(clk), 
		.rstn(rstn), 
		.din(din), 
		.en(data_en), 
		.init(init),
		.flush(flush), 
		.valid(valid)
	);
    integer fp_w, fout_w;
    integer seed;
	initial begin
	
	    fp_w=$fopen("data_in.dat","w");//以写的方式打开文件
	    fout_w=$fopen("data_out.dat","w");
		// Initialize Inputs
		seed = 100;//75
		clk = 0;
		rstn = 0;
		din = 0;
		data_en = 0;
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
		data_en = 1;
		#10000;
		waiting = 1;
		data_en = 0;
		#6000;
		flush = 1;
		#15;
		flush = 0;
		#4000;	
		$fclose(fp_w);
		$fclose(fout_w);
		$stop;
	end
	
	reg [DATA_WIDTH-1:0] data;
	reg [9:0] cnt;

always#5 clk = ~clk;

always@(posedge clk or negedge rstn) begin
	if (!rstn) begin
		din <= 0;//({$random}%256)<<`VAL_WIDTH;
		data_en <= 0;
		data <= 0;
		//datar <= 0;
		cnt <= 0;
	end
	else if (data_en) begin
        din[KEY_WIDTH-1:0] <= data;
        din[DATA_WIDTH-1:KEY_WIDTH] <= 0;
        data <= ({$random(seed)}%4096);
        //datar <= data;
        $fwrite(fp_w,"%h\n",data) ;
    end
    else begin
        data <= 0;
    end
end

always@(posedge clk or negedge rstn) begin
    if(heap_tb.uut.valid) begin
        $fwrite(fout_w,"%h\n",heap_tb.uut.dout1);
        $fwrite(fout_w,"%h\n",heap_tb.uut.dout2);
    end  
end

endmodule