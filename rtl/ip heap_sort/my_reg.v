`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2020/08/11 17:04:01
// Design Name: 
// Module Name: my_reg
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


module my_reg #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S_AXI
		parameter integer C_S_AXI_TDATA_WIDTH	= 64,
		// Parameters of Axi Master Bus Interface M_AXI
		parameter integer C_M_AXI_TDATA_WIDTH	= 64,
		parameter integer KEY_WIDTH = 8,
        parameter integer NLEVELS = 3

	)
	(

		// Ports of Axi Slave Bus Interface S_AXI
		input wire  aclk,
		input wire  s_axi_aresetn,
		output wire  s_axi_tready,
		input wire [C_S_AXI_TDATA_WIDTH-1 : 0] s_axi_tdata,
		input wire [(C_S_AXI_TDATA_WIDTH/8)-1 : 0] s_axi_tkeep,
		input wire  s_axi_tlast,
		input wire  s_axi_tvalid,

		// Ports of Axi Master Bus Interface M_AXI
		input wire  m_axi_aresetnQ,
		output wire  m_axi_tvalidQ,
		output wire [C_M_AXI_TDATA_WIDTH-1 : 0] m_axi_tdataQ,
		output wire [(C_M_AXI_TDATA_WIDTH/8)-1 : 0] m_axi_tkeepQ,
		output wire  m_axi_tlastQ,
		input wire  m_axi_treadyQ
	);

	// Add user logic here


	wire valid;
	reg m_axi_tlastQ_r;
	reg s_axi_tlast_r;
	wire [C_M_AXI_TDATA_WIDTH-1 : 0] m_axi_tdata;
	reg m_axi_tvalid_r;
	reg m_axi_tvalidQ_r;

	reg s_axi_tready_r;
	wire rstn;
	reg [(C_M_AXI_TDATA_WIDTH/8)-1 : 0] m_axi_tkeep_r;
    reg rstn_r; 
	assign m_axi_tvalidQ = m_axi_tvalidQ_r;
	assign m_axi_tkeepQ =  m_axi_tkeep_r;
	
	
	assign m_axi_tlastQ = m_axi_tlastQ_r;




	reg init_finished;
	reg [7 : 0] i;
	reg [7 : 0] j;
	reg [8 : 0] k;
	reg [C_M_AXI_TDATA_WIDTH-1:0] buffer [((1<<(NLEVELS+1))-3):0];
	reg en;
	reg flush;
	reg k_finished;
	always@(posedge aclk) begin
	if(!rstn) begin
	   j <= 0;
	   m_axi_tlastQ_r <= 0;
	   m_axi_tvalidQ_r <= 0; 
	   i <= 0;
	   m_axi_tkeep_r <= 0;
	   k<=0;
	   en<=0;
	   k_finished <= 0;
	end
	else begin
	   if(s_axi_tvalid&&s_axi_tready) begin
	       i <= i+1;
	       en <= 1;
	   end
	   else begin
	       i <= i;
	       en <=0 ;
	   end
	   if(i == 128) begin
	       flush <= 1;
	   end
	   else begin
	       flush <= 0;
	   end
	   if(!k_finished) begin
	       if(flush || k>=1) begin
	           k <= k+1;
	       end
	       else begin
	           k<=k;
	       end
	   end
	   if(k == 20) begin
	       m_axi_tvalidQ_r <= 1;
	       m_axi_tkeep_r <= s_axi_tkeep;
	   end
	   //if(k>=5&&k<=5+2*((1<<(NLEVELS+1))-3)) begin
	       //buffer[(k-5)/2] <= m_axi_tdata;
	   //end
	   if(k>=5&&k<=5+((1<<(NLEVELS+1))-3)) begin
	       buffer[k-5] <= m_axi_tdata;
	   end
	   else if(k==5+((1<<(NLEVELS+1))-2)) begin
	       k_finished <= 1;
	   end
	   if(m_axi_tvalidQ && m_axi_treadyQ) begin
	           j <= j+1;
	   end
	   else begin
	       j <= j;
	   end
	       if(j == ((1<<(NLEVELS+1))-3)) begin
	           m_axi_tvalidQ_r <= 0; 
	           m_axi_tlastQ_r <= 0;
	           j <= 0;
	       end
	       else if(j == ((1<<(NLEVELS+1))-4)) begin
	           m_axi_tlastQ_r <= 1;
	       end
	   end
	   end
	assign m_axi_tdataQ = (m_axi_tvalidQ && m_axi_treadyQ) ? buffer[j]:0;
	//init
	reg [7 : 0] cnt_i;
	reg init;
	
	reg rstn_finished;
	assign rstn = s_axi_aresetn && m_axi_aresetnQ && rstn_r;
	assign s_axi_tready = s_axi_tready_r;
	always@(posedge aclk) begin
	if(!rstn) begin
	   s_axi_tready_r <= 0;
	   init_finished <= 0;
	end
	else
	   if(cnt_i == (1<<NLEVELS-1)+3) begin
	       init <= 0;
	       init_finished <= 1;
	       s_axi_tready_r <= 1;
	   end
	   else if(init_finished == 1) begin
	       init <= 0;
	   end
	   else begin
	       init <=1;
	   end
	   if(cnt_i == 1) begin
	       rstn_r <= 1;
	       rstn_finished <= 1;
	   end
	   else if(rstn_finished) begin
	       rstn_r <= 1;
	   end
	   else begin
	       rstn_r <= 0;
	   end
	   if(s_axi_tvalid) begin
	       cnt_i <= cnt_i + 1;
	   end
	   else begin
	       cnt_i <= 0;
	   end
	end
	heap#(.DATA_WIDTH(C_S_AXI_TDATA_WIDTH),.KEY_WIDTH(KEY_WIDTH),.NLEVELS(NLEVELS)) uut (
		.clk(aclk), 
		.rstn(rstn), 
		.din(s_axi_tdata), 
		.en(en), 
		.init(init),
		.flush(flush),
		.dout(m_axi_tdata), 
		.valid(valid)
	);
	// User logic ends

	endmodule
