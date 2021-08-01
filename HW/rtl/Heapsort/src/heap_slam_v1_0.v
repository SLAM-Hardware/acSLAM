
`timescale 1 ns / 1 ps

	module heap_ip_v1_0 #
	(
		// Users to add parameters here
        parameter DATA_WIDTH = 292,   
	    parameter KEY_WIDTH = 7, 
	    parameter NLEVELS = 7, 
		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXIS
		parameter integer C_S00_AXIS_TDATA_WIDTH	= 512,

		// Parameters of Axi Master Bus Interface M00_AXIS
		parameter integer C_M00_AXIS_TDATA_WIDTH	= 512
		//parameter integer C_M00_AXIS_START_COUNT	= 32
	)
	(
		// Users to add ports here

		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S00_AXIS
		input wire  s00_axis_aclk,
		input wire  s00_axis_aresetn,
		output wire  s00_axis_tready,
		input wire [C_S00_AXIS_TDATA_WIDTH-1 : 0] s00_axis_tdata,
		input wire [(C_S00_AXIS_TDATA_WIDTH/8)-1 : 0] s00_axis_tstrb,
		input wire [(C_S00_AXIS_TDATA_WIDTH/8)-1 : 0]  s00_axis_tkeep,
		input wire  s00_axis_tlast,
		input wire  s00_axis_tvalid,

		// Ports of Axi Master Bus Interface M00_AXIS
		//input wire  m00_axis_aclk,
		//input wire  m00_axis_aresetn,
		output wire  m00_axis_tvalid,
		output wire [C_M00_AXIS_TDATA_WIDTH-1 : 0] m00_axis_tdata,
		output wire [(C_M00_AXIS_TDATA_WIDTH/8)-1 : 0] m00_axis_tstrb,
		output wire [(C_S00_AXIS_TDATA_WIDTH/8)-1 : 0]  m00_axis_tkeep,
		output wire  m00_axis_tlast,
		input wire  m00_axis_tready
	);
//// Instantiation of Axi Bus Interface S00_AXIS
//	heap_ip_v1_0_S00_AXIS # ( 
//		.C_S_AXIS_TDATA_WIDTH(C_S00_AXIS_TDATA_WIDTH)
//	) heap_ip_v1_0_S00_AXIS_inst (
//		.S_AXIS_ACLK(s00_axis_aclk),
//		.S_AXIS_ARESETN(s00_axis_aresetn),
//		.S_AXIS_TREADY(s00_axis_tready),
//		.S_AXIS_TDATA(s00_axis_tdata),
//		.S_AXIS_TSTRB(s00_axis_tstrb),
//		.S_AXIS_TLAST(s00_axis_tlast),
//		.S_AXIS_TVALID(s00_axis_tvalid)
//	);

//// Instantiation of Axi Bus Interface M00_AXIS
//	heap_ip_v1_0_M00_AXIS # ( 
//		.C_M_AXIS_TDATA_WIDTH(C_M00_AXIS_TDATA_WIDTH),
//		.C_M_START_COUNT(C_M00_AXIS_START_COUNT)
//	) heap_ip_v1_0_M00_AXIS_inst (
//		.M_AXIS_ACLK(m00_axis_aclk),
//		.M_AXIS_ARESETN(m00_axis_aresetn),
//		.M_AXIS_TVALID(m00_axis_tvalid),
//		.M_AXIS_TDATA(m00_axis_tdata),
//		.M_AXIS_TSTRB(m00_axis_tstrb),
//		.M_AXIS_TLAST(m00_axis_tlast),
//		.M_AXIS_TREADY(m00_axis_tready)
//	);

	// Add user logic here
	
	reg rstn_r,init,init_flag,ready;
	wire in_end;
	reg [NLEVELS-2:0] init_cnt;
	wire emp1,emp2;
	
	
	assign m00_axis_tstrb = {(C_M00_AXIS_TDATA_WIDTH/8){1'b1}};
	assign m00_axis_tkeep = {(C_M00_AXIS_TDATA_WIDTH/8){1'b1}};
	//assign m00_axis_tdata[C_M00_AXIS_TDATA_WIDTH-1:DATA_WIDTH] = 0;
	
	reg init_r;
	
	always @(posedge s00_axis_aclk or negedge s00_axis_aresetn) begin
	   if(~s00_axis_aresetn) begin
            init <= 0;
            rstn_r <= 0;
            init_r <= 0;
       end
       else begin
            rstn_r <= s00_axis_aresetn;
            init_r <= init;
            if((~rstn_r && s00_axis_aresetn) || (emp1 && emp2)) begin
                init <= 1;
            end
            else begin
                init <= 0;
            end
        end
    end
    	
	always @(posedge s00_axis_aclk or negedge s00_axis_aresetn) begin
	   if(~s00_axis_aresetn) begin
            init_flag <= 0;
            init_cnt <= 0;
       end
       else begin
	       if(init_r) begin
	           init_flag <= 1;
	       end
	       else if(init_cnt == (1<<(NLEVELS-1))-1) begin
	           init_flag <= 0;
	       end
	       if(init_flag) begin
	           init_cnt <= (init_cnt == (1<<(NLEVELS-1))-1) ? 0:init_cnt + 1;
	       end
	   end
	end
	
	assign in_end = s00_axis_tlast;
	
	reg ready_r,in_end_r;
	
	always @(posedge s00_axis_aclk or negedge s00_axis_aresetn) begin
	   if(~s00_axis_aresetn) begin
           ready <= 0;
           ready_r <= 0;
           in_end_r <= 0;
           //ready_rr <= 0;
       end
       else begin
           ready_r <= ready;
           in_end_r <= in_end;
           //ready_rr <= ready_r;
           if(init_cnt == (1<<(NLEVELS-1))-1) begin
            ready <= 1;
           end
           else if(!in_end_r && in_end) begin
            ready <= 0;
           end
       end
    end
    
    assign s00_axis_tready = ready_r;
	
//	reg en;
    
//    always @(posedge s00_axis_aclk or negedge s00_axis_aresetn) begin
//        if(~s00_axis_aresetn) begin
//            en <= 0;
//        end
//        else begin
//            if(s00_axis_tready && s00_axis_tvalid) begin
//                en <= 1;
//            end
//            else begin
//                en <= 0;
//            end
//        end
//    end

    assign en = s00_axis_tready && s00_axis_tvalid;
    
    reg emp_1r,emp_2r;
    
    always @(posedge s00_axis_aclk or negedge s00_axis_aresetn) begin
        if(~s00_axis_aresetn) begin
            emp_1r <= 0;
            emp_2r <= 0;
        end
        else begin
            emp_1r <= emp1;
            emp_2r <= emp2;
        end
    end
    
    reg valid,valid_r;
    
    always @(posedge s00_axis_aclk or negedge s00_axis_aresetn) begin
        if(~s00_axis_aresetn) begin
            valid <= 0;
            valid_r <= 0;
        end
        else begin
            valid_r <= valid;
            if(sort_end) begin
                valid <= 1;
            end
            else if(emp_1r && emp_2r) begin
                valid <= 0;
            end
        end
    end
    
    assign m00_axis_tvalid = valid || valid_r;
    
    wire flush;
    
    assign flush = valid &&  m00_axis_tready;
    
    
    assign m00_axis_tlast = (emp_1r && emp_2r) && init_r;
    
    assign m00_axis_tdata[C_M00_AXIS_TDATA_WIDTH-1:DATA_WIDTH] = 0;
    
     heap_rtl
#(.DATA_WIDTH(DATA_WIDTH),.KEY_WIDTH(KEY_WIDTH),.NLEVELS(NLEVELS)) heap_core
(
	.clk(s00_axis_aclk),    
	.rstn(s00_axis_aresetn),   
	.din(s00_axis_tdata), 
	.en(en),
	.init(init_r),
	.flush(flush), 
	.in_end(in_end),

	.dout(m00_axis_tdata[DATA_WIDTH-1:0]),
	.heap_emp1(emp1),//flush finish
	.heap_emp2(emp2),
	.sort_end(sort_end)
    );
	// User logic ends

	endmodule
