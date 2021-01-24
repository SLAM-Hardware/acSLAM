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


module my_reg_v1 #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S_AXI
		parameter integer C_S_AXI_TDATA_WIDTH	= 64,
		// Parameters of Axi Master Bus Interface M_AXI
		parameter integer C_M_AXI_TDATA_WIDTH	= 64

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


	//reg valid;
	reg m_axi_tlastQ_r;
	reg s_axi_tlast_r;
	wire [C_M_AXI_TDATA_WIDTH-1 : 0] m_axi_tdata;
	reg m_axi_tvalid_r;
	reg m_axi_tvalidQ_r;
	wire valid;

	reg s_axi_tready_r;
	wire rstn;
	reg [(C_M_AXI_TDATA_WIDTH/8)-1 : 0] m_axi_tkeep_r;

	assign m_axi_tvalidQ = m_axi_tvalidQ_r;//&&valid;
	assign m_axi_tkeepQ =  m_axi_tkeep_r;
	assign rstn = s_axi_aresetn && m_axi_aresetnQ;
	assign s_axi_tready = 1;
	assign m_axi_tlastQ = m_axi_tlastQ_r;




	reg init_finished;
	reg [7 : 0] i;
	reg [7 : 0] j;
	reg [C_M_AXI_TDATA_WIDTH-1:0] buffer [127:0];
	reg rstn_finished;
	always@(posedge aclk) begin
	if(!rstn) begin
	   j <= 0;
	   m_axi_tlastQ_r <= 0;
	   m_axi_tvalidQ_r <= 0; 
	   i <= 0;
	   m_axi_tkeep_r <= 0;
	end
	else begin
	   if(s_axi_tvalid&&s_axi_tready) begin
	       buffer[i] <= s_axi_tdata;
	       i <= i+1;
	       
	   end
	   else begin
	       i <= i;
	   end
	   if(i == 1) begin
	       m_axi_tvalidQ_r = 1;
	       m_axi_tkeep_r = s_axi_tkeep;
	   end
	   if(m_axi_tvalidQ && m_axi_treadyQ &&(j!=i-1)) begin
	           j <= j+1;
	   end
	   else begin
	       j <= j;
	   end
	       if(j == 127) begin
	           m_axi_tvalidQ_r <= 0; 
	           m_axi_tlastQ_r <= 0;
	           j <= 0;
	       end
	       else if(j == 126) begin
	           m_axi_tlastQ_r <= 1;
	       end
	   end
	   end
	assign m_axi_tdataQ = (m_axi_tvalidQ && m_axi_treadyQ) ? buffer[j]:0;
	// User logic ends

	endmodule
