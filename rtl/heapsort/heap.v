`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/18/2018 04:19:35 PM
// Design Name: 
// Module Name: heap
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
//`define _MAX_   //°ŽÀíËµÊÇ²ÎÊý¶šÒå£¬¿ÉÒÔ¿çÎÄŒþŽ«µÝ²ÎÊý
module heap
#(
	parameter DATA_WIDTH = 32,   //ÊýŸÝ¿í¶È£¿
	parameter KEY_WIDTH = 16,     //ºËÐÄ¿í¶È£¿
	parameter NLEVELS = 4           //µÈŒ¶J
	)
(
	input clk,    //Ê±ÖÓÖÜÆÚ
	input rstn,   //žŽÎ»ÐÅºÅ
	input [DATA_WIDTH-1:0] din,  //ÊäÈëÊýŸÝ
	input en, //Ê¹ÄÜÐÅºÅ
	input init, //³õÊŒ»¯
	input flush, //Çå³ý
	
	output reg [DATA_WIDTH-1:0] dout,  //Êä³ö
	output reg valid  //ÓÐÐ§ÐÅºÅ

    );
localparam ADDR_WIDTH = NLEVELS;  //µØÖ·¿í¶È
`ifdef _MAX_   //Èç¹ûÊÇŽó¶Ñ£¿
localparam INIT_DATA = {2'b11,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b1}}};  //³õÊŒ»¯ÊýŸÝ Ç°ÁœÎ»±êÖŸÎ»£¬01×îÐ¡ 11×îŽó 00 normal ×îºókeyÈ¡×îŽó
localparam FLUSH_DATA = {2'b01,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b0}}};
`else
localparam INIT_DATA = {2'b01,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b0}}};
localparam FLUSH_DATA = {2'b11,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b1}}};
`endif
localparam HEAP_SIZE = (1<<(NLEVELS+1))-1; //¶ÑŽóÐ¡
//wire between up memory and sorting node
wire [(NLEVELS+1)*DATA_WIDTH-1:0] um_in;  //œ«ÉÏŽ¢ŽæÆ÷ºÍÅÅÐòœÚµãÁ¬ÆðÀŽ
wire [(NLEVELS+1)*DATA_WIDTH-1:0] um_out;
wire [(NLEVELS+1)*ADDR_WIDTH-1:0] um_addr;
wire [(NLEVELS+1)-1:0] um_we;
//wire between bottum left memory and sorting node
wire [NLEVELS*DATA_WIDTH-1:0] lm_in;  //×óŽ¢ŽæÆ÷
wire [NLEVELS*DATA_WIDTH-1:0] lm_out;
wire [NLEVELS*ADDR_WIDTH-1:0] lm_addr;
wire [NLEVELS-1:0] lm_we;
//wire between bottum right memory and sorting node
wire [NLEVELS*DATA_WIDTH-1:0] rm_in;  //ÓÒŽ¢ŽæÆ÷
wire [NLEVELS*DATA_WIDTH-1:0] rm_out;
wire [NLEVELS*ADDR_WIDTH-1:0] rm_addr;
wire [NLEVELS-1:0] rm_we;
//wire between up by-pass channel and sorting node
wire [NLEVELS-1:0] pl_update_in;   //ÉÏÅÔÂ·ÍšµÀ
wire [NLEVELS*ADDR_WIDTH-1:0] pl_addr_in;
wire [NLEVELS-1:0] pl_branch_in;
wire [NLEVELS*DATA_WIDTH-1:0] pl_in;
wire [NLEVELS*DATA_WIDTH-1:0] pl_out;
wire [NLEVELS-1:0] pl_update_out;
wire [NLEVELS*ADDR_WIDTH-1:0] pl_addr_out;
wire [NLEVELS+1-1:0] pl_branch_out;
//wire between bottum by-pass channel and sorting node
wire [NLEVELS-1:0] nl_update_in;    //ÏÂÅÔÂ·ÍšµÀ
wire [NLEVELS*ADDR_WIDTH-1:0] nl_addr_in;
wire [NLEVELS-1:0] nl_branch_in;
wire [NLEVELS*DATA_WIDTH-1:0] nl_in;
wire [NLEVELS*DATA_WIDTH-1:0] nl_out;
wire [NLEVELS*ADDR_WIDTH-1:0] nl_addr_out;
wire [NLEVELS-1:0] nl_update_out;
wire [NLEVELS-1:0] nl_branch_out;

assign um_out[NLEVELS*DATA_WIDTH +: DATA_WIDTH] = 0;  //ŽÓžÃÎ»¿ªÊŒµœºóÃæ¶àÉÙÎ» £¿£¿£¿£¿
assign um_addr[NLEVELS*ADDR_WIDTH +: ADDR_WIDTH] = 0;
assign um_we[NLEVELS] = 0;
assign pl_branch_out[NLEVELS] = 0;

reg [DATA_WIDTH-1:0] pl_in_r;  //input to sort node 0
reg [DATA_WIDTH-1:0] pl_out_r;  //output from sort node 0


reg [DATA_WIDTH-1:0] din_r;
reg en_r;
reg flush_flag;
reg flush_en;
reg [NLEVELS:0] flush_cnt;

always@(posedge clk or negedge rstn) begin
	if (!rstn) begin
		//pl_in_r <= 0;
		pl_out_r <= 0;
		din_r <= 0;
		dout <= 0;
		en_r <= 0;
		valid <= 0;
		flush_flag <= 0;
		flush_cnt <= 0;
		flush_en <= 0;
	end
	else begin
		//valid <= (en_r && cmp_lt(pl_out_r, din_r) || flush_en) && pl_out_r[DATA_WIDTH-1:DATA_WIDTH-2]==2'b00;
		//en_r <= en && cmp_lt(pl_out_r, din);
		valid <= (en_r || flush_en) && pl_out_r[DATA_WIDTH-1:DATA_WIDTH-2]==2'b00;  //Êä³öÊýŸÝÊÇ·ñÓÐÐ§
		en_r <= en;
		din_r <= din;
		if (init) begin  //³õÊŒ»¯ÊýŸÝŽ«Èë
			pl_out_r <= INIT_DATA;
			flush_flag <= 0;
		end
	//	else if (en_r && cmp_lt(pl_out_r, din_r) || flush_en) begin
	//		dout <= pl_out_r;
	//		if (!flush_flag)
	//			pl_in_r <= din;
	/*	if (en_r || flush_en) begin
		    if (cmp_lt(pl_out_r, din_r))
		        dout <= pl_out_r;
            else
                dout <= din_r;
		end*/
		if (en_r)
`ifdef _MAX_
            if (cmp_lt(din_r, pl_out_r))  //±ÈœÏº¯Êý Ç°ÃæÐ¡ÓÚºóÃæ·µ»ØÕæ
`else
		    if (cmp_lt(pl_out_r, din_r))
		        
`endif
                dout <= pl_out_r;   //œ«ŽóÊýÊä³ö ²»ÒªÐ¡ÊýÁË
            else
                dout <= din_r;
        else if (flush_en)  //Çå³ýÊ¹ÄÜ
            dout <= pl_out_r;        //°ÑÊý×éÀïŽ¢ŽæµÄ¶«Î÷Êä³ö
            
            
	    if (flush) begin   //£¿flushµÄÒ»ÏµÁÐ²Ù×÷
	//		pl_in_r <= FLUSH_DATA;
			flush_flag <= 1;
		end
		else if (flush_cnt==HEAP_SIZE)
			flush_flag <= 0;
			
		if (pl_update_out[0])
			pl_out_r <= pl_out[DATA_WIDTH-1:0];  //£¿£¿£¿
			
		if (flush_flag) begin
			flush_en <= flush_cnt == HEAP_SIZE ? 0 : ~flush_en;
			if (flush_en)
			    flush_cnt <= flush_cnt == HEAP_SIZE ? 0 : flush_cnt+1;
        end
		else begin
			flush_en <= 0;
			flush_cnt <= 0;
        end
	end
end

reg [DATA_WIDTH-1:0] pl_in_reg;
always@(*) begin
/*    if (en_r && cmp_lt(pl_out_r, din_r) || flush_en) begin
        if (!flush_flag)
            pl_in_r = din_r;
        else
            pl_in_r = pl_in_reg;
    end
    else if (flush)
        pl_in_r = FLUSH_DATA;
    else
        pl_in_r = pl_in_reg;*/
    if (en_r)  //ÍùÏÂ
`ifdef _MAX_
        if (cmp_lt(din_r, pl_out_r))
`else
        if (cmp_lt(pl_out_r, din_r))
`endif
            pl_in_r = din_r;
        else
            pl_in_r = pl_out_r;
 //   else if (flush)
 //       pl_in_r = FLUSH_DATA;
    else if (flush_en & flush_flag)
        pl_in_r = FLUSH_DATA;
    else
        pl_in_r = pl_in_reg;
    
end

always@(posedge clk or negedge rstn) begin
    if (!rstn)
        pl_in_reg <= 0;
    else
        pl_in_reg <= pl_in_r;
end

genvar i;
generate for (i=0; i<NLEVELS; i=i+1) begin: loop_a   //Ñ­»·ÊµÀý»¯ž÷žöÄ£¿é
	sort_node 
# (.DATA_WIDTH(DATA_WIDTH),
	.KEY_WIDTH(KEY_WIDTH),
	.ADDR_WIDTH(ADDR_WIDTH),
	.INIT_DATA(INIT_DATA),
	.LEVEL(i)) U_SN   //pl previous nl next  bypass
	(
		.clk(clk), 
		.rstn(rstn), 
		.init(init), 
		.um_in(um_in[i*DATA_WIDTH +: DATA_WIDTH]), 
		.um_out(um_out[i*DATA_WIDTH +: DATA_WIDTH]), 
		.um_addr(um_addr[i*ADDR_WIDTH +: ADDR_WIDTH]), 
		.um_we(um_we[i]), 
		.lm_in(lm_in[i*DATA_WIDTH +: DATA_WIDTH]), 
		.lm_out(lm_out[i*DATA_WIDTH +: DATA_WIDTH]), 
		.lm_addr(lm_addr[i*ADDR_WIDTH +: ADDR_WIDTH]), 
		.lm_we(lm_we[i]), 
		.rm_in(rm_in[i*DATA_WIDTH +: DATA_WIDTH]), 
		.rm_out(rm_out[i*DATA_WIDTH +: DATA_WIDTH]), 
		.rm_addr(rm_addr[i*ADDR_WIDTH +: ADDR_WIDTH]), 
		.rm_we(rm_we[i]), 
		.pl_update_in(pl_update_in[i]), 
		.pl_addr_in(pl_addr_in[i*ADDR_WIDTH +: ADDR_WIDTH]), 
		.pl_branch_in(pl_branch_in[i]), 
		.pl_in(pl_in[i*DATA_WIDTH +: DATA_WIDTH]), 
		.pl_out(pl_out[i*DATA_WIDTH +: DATA_WIDTH]), 
		.pl_update_out(pl_update_out[i]),
		.pl_addr_out(pl_addr_out[i*ADDR_WIDTH +: ADDR_WIDTH]),
		.pl_branch_out(pl_branch_out[i]), 
		.nl_update_in(nl_update_in[i]), 
		.nl_addr_in(nl_addr_in[i*ADDR_WIDTH +: ADDR_WIDTH]), 
		.nl_branch_in(nl_branch_in[i]),
		.nl_in(nl_in[i*DATA_WIDTH +: DATA_WIDTH]), 
		.nl_out(nl_out[i*DATA_WIDTH +: DATA_WIDTH]), 
		.nl_update_out(nl_update_out[i]), 
		.nl_addr_out(nl_addr_out[i*ADDR_WIDTH +: ADDR_WIDTH]),
		.nl_branch_out(nl_branch_out[i])
	);

	data_store #(.DATA_WIDTH(DATA_WIDTH),.ADDR_WIDTH(ADDR_WIDTH),.LEVEL(i)) U_DS   //lmºÍrm·Ö±ðÊÇ×óÓÒ£¬nlÊÇtranslation layer
	(
		.clk(clk), 
		.lm_din(lm_out[i*DATA_WIDTH +: DATA_WIDTH]),  //lmoutŽæÁËÃ¿Ò»Œ¶µÄËùÓÐÊä³ö
		.lm_addr(lm_addr[i*ADDR_WIDTH +: ADDR_WIDTH]), 
		.lm_we(lm_we[i]), 
		.lm_dout(lm_in[i*DATA_WIDTH +: DATA_WIDTH]), 
		.rm_din(rm_out[i*DATA_WIDTH +: DATA_WIDTH]), 
		.rm_addr(rm_addr[i*ADDR_WIDTH +: ADDR_WIDTH]), 
		.rm_we(rm_we[i]), 
		.rm_dout(rm_in[i*DATA_WIDTH +: DATA_WIDTH]), 
		.nl_din(um_out[(i+1)*DATA_WIDTH +: DATA_WIDTH]),   //ÅÔÂ·Á¬ÉÏÃæÊä³ö
		.nl_addr(um_addr[(i+1)*ADDR_WIDTH +: ADDR_WIDTH]),
		.nl_we(um_we[i+1]), 
		.nl_branch(pl_branch_out[i+1]), 
		.nl_dout(um_in[(i+1)*DATA_WIDTH +: DATA_WIDTH])
	);	
	
	if (i==0) begin
		assign pl_in[0*DATA_WIDTH +: DATA_WIDTH] = pl_in_r;//din;
		assign pl_update_in[0] = en_r|flush_en;
		assign pl_branch_in[0] = 0;
		assign pl_addr_in[0*ADDR_WIDTH +: ADDR_WIDTH] = 0;
		assign um_in[0*DATA_WIDTH +: DATA_WIDTH] = pl_in_r;
	end
	else begin
		assign pl_in[i*DATA_WIDTH +: DATA_WIDTH] = nl_out[(i-1)*DATA_WIDTH +: DATA_WIDTH];
		assign pl_update_in[i] = nl_update_out[i-1];
		assign pl_branch_in[i] = nl_branch_out[i-1];
		assign pl_addr_in[i*ADDR_WIDTH +: ADDR_WIDTH] = nl_addr_out[(i-1)*ADDR_WIDTH +: ADDR_WIDTH];
	end
	
	if (i==NLEVELS-1) begin
		assign nl_in[i*DATA_WIDTH +: DATA_WIDTH] = 0;
		assign nl_update_in[i] = 0;
		assign nl_branch_in[i] = 0;
		assign nl_addr_in[i*ADDR_WIDTH +: ADDR_WIDTH] = 0;	
	end
	else begin
		assign nl_in[i*DATA_WIDTH +: DATA_WIDTH] = pl_out[(i+1)*DATA_WIDTH +: DATA_WIDTH];
		assign nl_update_in[i] = pl_update_out[i+1];
		assign nl_branch_in[i] = pl_branch_out[i+1];
		assign nl_addr_in[i*ADDR_WIDTH +: ADDR_WIDTH] = pl_addr_out[(i+1)*ADDR_WIDTH +: ADDR_WIDTH];
	end
end
endgenerate
function cmp_lt;        //return true if d1 key < d2 key
	input [DATA_WIDTH-1:0] d1; 
	input [DATA_WIDTH-1:0] d2;
	reg [1:0] d1_flag;
	reg [1:0] d2_flag;
	reg [KEY_WIDTH-1:0] d1_key;
	reg [KEY_WIDTH-1:0] d2_key;	
	begin
		d1_flag = d1[DATA_WIDTH-1:DATA_WIDTH-2];
		d2_flag = d2[DATA_WIDTH-1:DATA_WIDTH-2];
		d1_key = d1[KEY_WIDTH-1:0];
		d2_key = d2[KEY_WIDTH-1:0];
		case (d1_flag)
		2'b01: begin    //d1 is min
		    case (d2_flag)
		    2'b11: cmp_lt = 1;  //d2 is max
		    2'b00: cmp_lt = 1;  //d2 is normal
		    2'b01: cmp_lt = 0;  //d2 is min
		    default: cmp_lt = 0;
		    endcase
        end
        2'b11: begin    //d1 is max
            cmp_lt = 0;
        end
        2'b00: begin    //d1 is normal
            case(d2_flag)
            2'b11: cmp_lt = 1;  //d2 is max
            2'b01: cmp_lt = 0;  //d2 is min
            2'b00: cmp_lt = d1_key < d2_key;  //d2 is normal
            default: cmp_lt = 0;
            endcase
        end
        default:
            cmp_lt = 0;
        endcase
    end
endfunction
/*
function cmp_lt;        //return true if d1 key < d2 key
	input [DATA_WIDTH-1:0] d1; 
	input [DATA_WIDTH-1:0] d2;
	reg [1:0] d1_flag;
	reg [1:0] d2_flag;
	reg [KEY_WIDTH-1:0] d1_key;
	reg [KEY_WIDTH-1:0] d2_key;	
	begin
		d1_flag = d1[DATA_WIDTH-1:DATA_WIDTH-2];
		d2_flag = d2[DATA_WIDTH-1:DATA_WIDTH-2];
		d1_key = d1[KEY_WIDTH-1:0];
		d2_key = d2[KEY_WIDTH-1:0];
		//d1 is initial data
		if (d1_flag==2'b01)
			cmp_lt = 1;
		//d1 is flush data
		else if (d1_flag==2'b11)
			cmp_lt = 0;
		//d1 is variable
		else if (d1_flag==2'b00) begin
			//d2 is initial data
			if (d2_flag==2'b01)
				cmp_lt = 0;
			//d2 is flush data
			else if (d2_flag==2'b11)
				cmp_lt = 1;
			//d2 is variable
			else if (d2_flag==2'b00)
				cmp_lt = d1_key<d2_key;
			else
				cmp_lt = 0;
		end
		else
			cmp_lt = 0;
	end
endfunction
*/
endmodule