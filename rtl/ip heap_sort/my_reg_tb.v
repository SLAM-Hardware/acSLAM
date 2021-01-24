`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2020/08/11 17:07:50
// Design Name: 
// Module Name: my_reg_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module my_reg_tb();
		//parameter C_M_AXI_START_COUNT	= 32;
		parameter C_M_AXI_TDATA_WIDTH	= 16;
		parameter C_S_AXI_TDATA_WIDTH	= 16;
		parameter KEY_WIDTH = 8;
		parameter NLEVELS = 3;
        reg clk;
    	reg s_axi_aresetn;
		wire s_axi_tready;
		reg [C_S_AXI_TDATA_WIDTH-1 : 0] s_axi_tdata;
		wire [(C_M_AXI_TDATA_WIDTH/8)-1 : 0] s_axi_tkeep;
		reg s_axi_tlast;
		reg s_axi_tvalid;
		reg m_axi_aresetn;
		wire  m_axi_tvalid;
		wire [C_M_AXI_TDATA_WIDTH-1 : 0] m_axi_tdata;
		wire [(C_M_AXI_TDATA_WIDTH/8)-1 : 0] m_axi_tkeep;
		wire  m_axi_tlast;
		reg  m_axi_tready;
		//reg en;
	    //reg init;
	    //reg flush;
	    assign s_axi_tkeep	= {(C_M_AXI_TDATA_WIDTH/8){1'b1}};
	my_reg #
   (

        .C_S_AXI_TDATA_WIDTH(C_S_AXI_TDATA_WIDTH),
		.C_M_AXI_TDATA_WIDTH(C_M_AXI_TDATA_WIDTH))  uut
		(
		.aclk(clk),
		.s_axi_aresetn(s_axi_aresetn),
		.s_axi_tready(s_axi_tready),
		.s_axi_tdata(s_axi_tdata),
		.s_axi_tkeep(s_axi_tkeep),
		.s_axi_tlast(s_axi_tlast),
		.s_axi_tvalid(s_axi_tvalid),
		.m_axi_aresetnQ(m_axi_aresetn),
		.m_axi_tvalidQ(m_axi_tvalid),
		.m_axi_tdataQ(m_axi_tdata),
		.m_axi_tkeepQ(m_axi_tkeep),
		.m_axi_tlastQ(m_axi_tlast),
		.m_axi_treadyQ(m_axi_tready)
	);
	initial begin
		// Initialize Inputs
	 clk = 0;
	 m_axi_aresetn = 1;
	 s_axi_aresetn = 1;
	 s_axi_tdata = 0;
	 s_axi_tlast = 0;
	 s_axi_tvalid = 0;
	 m_axi_tready = 1;
	 //init = 0;
	 //flush = 0;
	 //en = 0;
	 #100
	 //s_axi_aresetn = 0;
	 #40
	 s_axi_tvalid = 1;
	 #147
	 s_axi_tvalid = 0;
	 #140
	 s_axi_tvalid = 1;
	 #1750
	 s_axi_tlast = 1;
	 #14;
	 s_axi_tvalid = 0;
	 s_axi_tlast = 0;
	 #191;
	 //flush = 1;
	 #15;
	 //flush = 0;
	 #144
	 //m_axi_tready = 0;
	 #1000;
	 $stop;
	end
	always#7 clk = ~clk;
    always@(posedge clk) begin
        if (s_axi_tvalid && s_axi_tready) begin
            //({$random}%256)<<`VAL_WIDTH;
            s_axi_tdata[KEY_WIDTH-1:0] <= ({$random}%256);
            s_axi_tdata[C_S_AXI_TDATA_WIDTH-1:KEY_WIDTH] <= 0;
        end
        else begin
            s_axi_tdata[C_S_AXI_TDATA_WIDTH-1 : 0] <= s_axi_tdata;
        end
           //en <= 0;
    end
endmodule
