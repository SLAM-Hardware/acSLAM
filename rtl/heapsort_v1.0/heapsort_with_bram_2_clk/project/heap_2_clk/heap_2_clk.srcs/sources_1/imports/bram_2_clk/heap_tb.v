`timescale 1ns / 1ps

//////////////////////////////////////////////////////////////////////////////////
module heap_tb;
parameter DATA_WIDTH = 12;
parameter KEY_WIDTH = 8;
parameter NLEVELS = 7;


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



    integer fp_w, fout_w, num_w;
    integer seed;
    reg [9:0] mem [1:0];

	initial begin
	   fp_w=$fopen("data_in.dat","w");//以写的方式打开文件
	   fout_w=$fopen("data_out.dat","w");
		// Initialize Inputs
		clk = 0;
		rstn = 0;
		din = 0;
		en = 0;
		init = 0;
		waiting = 1;
		flush = 0;
		cnt = 0;
		$readmemh("num.dat", mem); 
		seed = 50;
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
		en = 1;
		#10000;
		waiting = 1;
		en = 0;
		#200;
		flush = 1;
		#15;
		flush = 0;
		#10000;
		
		$fclose(fp_w);
		$fclose(fout_w);
		$stop;
	end
	

always#5 clk = ~clk;

reg [KEY_WIDTH-1:0] data;
reg cnt;
reg en_in;

always@(posedge clk or negedge rstn) begin
	if (!rstn) begin
		cnt <= 0;
	end
	else begin
	   cnt <= ~cnt;
   end
end

always@(posedge clk or negedge rstn) begin
	if (!rstn) begin
		din <= 0;//({$random}%256)<<`VAL_WIDTH;
		data <= 0;
		en_in <= 0;
		//datar <= 0;
	end
	else if (en) begin
        din[KEY_WIDTH-1:0] <= data;
        din[DATA_WIDTH-1:KEY_WIDTH] <= 0;
        data <= ({$random(seed)}%256);
        en_in <= 1;
        //datar <= data;
        $fwrite(fp_w,"%h\n",data) ;
    end
    else begin
        en_in <= 0;
        end

end

always@(posedge clk or negedge rstn) begin
    if(heap_tb.uut.valid && heap_tb.uut.flush_flag) begin
        $fwrite(fout_w,"%h\n",heap_tb.uut.dout);
    end  
end
		
// Instantiate the Unit Under Test (UUT)
heap#(.DATA_WIDTH(DATA_WIDTH),.KEY_WIDTH(KEY_WIDTH),.NLEVELS(NLEVELS)) uut (
    .clk(clk), 
    .rstn(rstn), 
    .din(din), 
    .en(en_in), 
    .init(init),
    .flush(flush),
    .dout(dout), 
    .valid(valid)
	);

   
endmodule