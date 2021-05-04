`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
//`define _MAX_   
module heap_rtl
#(
	parameter DATA_WIDTH = 16,   
	parameter KEY_WIDTH = 4, 
	parameter NLEVELS = 7      
	)
(
	input clk,    
	input rstn,   
	input [DATA_WIDTH-1:0] din, 
	input en,
	input init,
	input flush, 
	
	//output [DATA_WIDTH-1:0] dout1, dout2,
	output reg [DATA_WIDTH-1:0] dout,  
	output valid
    );

	localparam ADDR_WIDTH = NLEVELS-1;
	`ifdef _MAX_  
	localparam INIT_DATA = {2'b11,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b1}}};
	localparam FLUSH_DATA = {2'b01,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b0}}};
	`else
	localparam INIT_DATA = {2'b01,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b0}}};
	localparam FLUSH_DATA = {2'b11,{(DATA_WIDTH-2-KEY_WIDTH){1'b0}},{KEY_WIDTH{1'b1}}};
	`endif

	reg [DATA_WIDTH-1:0] din_1,din_2;

	reg cnt;

	wire [DATA_WIDTH-1:0] comp1,comp2;

	wire en_1,en_2;

	wire [DATA_WIDTH-1:0] dout1,dout2;
	
	wire valid_1,valid_2;

	reg ext, fifo;

	always @(posedge clk or negedge rstn) begin
		if(~rstn) begin
			ext <= 0;
			fifo <= 0;
		end 
		else begin
			if(en) begin
				ext <= 1;
				fifo <= 0;
			end
			else if(!en && ext) begin
				ext <= 0;
				fifo <= 1;
			end
			else if(fifo_emptyr4 && fifo) begin
				ext <= 0;
				fifo <= 0;
			end
		end
	end

	reg ext_r,fifo_r,ext_in,fifo_in,ext_out,fifo_out;

	always @(posedge clk or negedge rstn) begin
		if(~rstn) begin
			ext_r <= 0;
			fifo_r <= 0;
			ext_in <= 0;
			fifo_in <= 0;
			ext_out <= 0;
			fifo_out <= 0;
		end else begin
			ext_r <= ext;
			fifo_r <= fifo;
			ext_in <= ext_r;
			fifo_in <= fifo_r;
			ext_out <= ext_in;
			fifo_out <= fifo_in;
		end
	end

    //?????????fifo???????cnt?????§Ý?????????????????
	always @(posedge clk or negedge rstn) begin
		if(~rstn) begin
			cnt <= 1'b0;
		end 
		else if(ext | ext_out | fifo_out | flush_flag) begin
			cnt <= ~cnt;
		end
		else begin
			cnt <= 1'b0;
		end
	end
	
	wire empty1, empty2, fifo_emptyrr;

	reg [3:0] empty1r, empty2r;

	//??empty????????????????????§Ø?fifoin??????
	always @(posedge clk or negedge rstn) begin
		if(~rstn) begin
			empty1r <= 4'b0;
			empty2r <= 4'b0;
		end 
		else begin
			empty1r[0] <= empty1;
			empty2r[0] <= empty2;
			empty1r[3:1] <= empty1r[2:0];
			empty2r[3:1] <= empty2r[2:0];
		end
	end
     
	assign fifo_emptyr4 = empty1r[3] && empty2r[3];

	//?§Ø?fifoin?????????fifo????FIFO???rr????
	assign fifo_en1 = (fifo | fifo_out) & ~empty1r[3];
	assign fifo_en2 = (fifo | fifo_out) & ~empty2r[3];
	
	assign en_1 = (ext | ext_out | fifo_en1) & cnt;
	assign en_2 = (ext | ext_out | fifo_en2) & ~cnt;

//	assign en_1 = (ext_out | fifo_en1);
//	assign en_2 = (ext_out | fifo_en2);
	assign valid = valid_1 & valid_2;

	wire [DATA_WIDTH-1:0] dout_fifo1, dout_fifo2;
	reg [DATA_WIDTH-1:0] dout_buf1, dout_buf2;
	reg ready1, ready2;

	//????fifo????????????????????????????????????????????????fifo???????????????????????
	always @(posedge clk or negedge rstn) begin
        if(~rstn)
            dout_buf1 <= 0;
        else
            dout_buf1 <= dout_fifo1;
    end
    
	always @(*) begin
	   if(~rstn) begin
	       ready1 = 1'b0;
	   end
	   else begin
	       if((cmp_lt(dout_buf1,comp1) && !empty1) && ext) begin
	           ready1 = 1'b1;
	       end
/**/       else if(fifo_en1 && cnt) begin
	           ready1 = 1'b1;
	       end
	       else begin
	           ready1 = 1'b0;
	       end
	   end
	end

	assign rd_en1 = ready1;

	always @(posedge clk or negedge rstn) begin
        if(~rstn)
            dout_buf2 <= 0;
        else
            dout_buf2 <= dout_fifo2;
    end
    
	always @(*) begin
	   if(~rstn) begin
	       ready2 = 1'b0;
	   end
	   else begin
	       if((cmp_lt(dout_buf2,comp2) && !empty2) && ext) begin
	           ready2 = 1'b1;
	       end
/**/       else if(fifo_en2 && ~cnt) begin
	           ready2 = 1'b1;
	       end
	       else
	           ready2 = 1'b0;
	       end
	   end

	assign rd_en2 = ready2;

	reg wr_en1,wr_en2;

	reg [DATA_WIDTH-1:0] din_fifo1,din_fifo2;

	//fifo§Õ????????ext_out????????????????FIFO??
	always @(posedge clk or negedge rstn) begin
        if(~rstn) begin
                din_fifo1 <= 0;
                wr_en1 <= 1'b0;
                din_fifo2 <= 0;
                wr_en2 <= 1'b0;
        end
        else begin
            if(ext_out && cnt && cmp_lt(comp2,dout1) && comp2 != INIT_DATA) begin
                din_fifo2 <= dout1;
                wr_en2 <= 1'b1;
                wr_en1 <= 1'b0;
            end
            else if(ext_out && ~cnt && cmp_lt(comp1,dout2) && comp1 != INIT_DATA) begin
                wr_en2 <= 1'b0;
                din_fifo1 <= dout2;
                wr_en1 <= 1'b1;
            end
            else begin
                din_fifo1 <= 0;
                wr_en1 <= 1'b0;
                din_fifo2 <= 0;
                wr_en2 <= 1'b0;
            end
         end
      end

    //????????????????????????????????§Ö?fifo????????
	//ext = 1,??din1din2??din?§Ý?????fifo = 1????fifo_en1?din_1 = dout_buf1??
    always @(posedge clk or negedge rstn) begin
        if(~rstn) begin
            din_1 <= 0;
            din_2 <= 0;
        end
        else begin
            if(cnt)begin
                if(ext) begin
                    din_1 <= din;
                end
                else if(fifo_en1) begin
                    if(empty1r[2]) begin
                    din_1 <= 0;
                    end
                    else begin
                        din_1 <= dout_buf1;
                    end
                end
                else begin
                    din_1 <= 0;
                end
            end            
            else begin
                if(ext) begin
                    din_2 <= din;
                end
                else if(fifo_en2) begin
                    if(empty2r[2]) begin
                    din_2 <= 0;
                    end
                    else begin
                        din_2 <= dout_buf2;
                    end
                end
                else begin
                    din_2 <= 0;
                end
            end 
        end
    end
    
    assign flush_flag = flush_flag_1 | flush_flag_2;
    
    always @(*) begin
        if(!rstn) begin
            dout = 0;
        end
        else if(flush_flag) begin
            if(cnt) begin
                dout = dout2;
            end
            else begin
                dout = dout1;
            end
         end
         else begin
            dout = 0;
         end
     end

	sub_heap # (.SUB_NUM(0),.DATA_WIDTH(DATA_WIDTH),.KEY_WIDTH(KEY_WIDTH),.NLEVELS(NLEVELS-1)) Heap1
	(
		.clk(clk),
		.rstn(rstn),
		.din(din_1),
		.en(en_1),
		.init(init),
		.flush(flush),
		.dout(dout1),
		.d_comp(comp1),
		.valid(valid_1),
		.flush_flag(flush_flag_1)
		);

	sub_heap # (.SUB_NUM(1),.DATA_WIDTH(DATA_WIDTH),.KEY_WIDTH(KEY_WIDTH),.NLEVELS(NLEVELS-1)) Heap2
	(
		.clk(clk),
		.rstn(rstn),
		.din(din_2),
		.en(en_2),
		.init(init),
		.flush(flush),
		.dout(dout2),
		.d_comp(comp2),
		.valid(valid_2),
		.flush_flag(flush_flag_2)
		);
		
	fifo_generator_0 fifo_1 (
		.clk(clk),                  // input wire clk
		.srst(~rstn),                // input wire srst
		.din(din_fifo1),                  // input wire [35 : 0] din
		.wr_en(wr_en1),              // input wire wr_en
		.rd_en(rd_en1),              // input wire rd_en
		.dout(dout_fifo1),                // output wire [35 : 0] dout
		.full(),                // output wire full
		.overflow(),        // output wire overflow
		.empty(empty1),              // output wire empty
		.wr_rst_busy(),  // output wire wr_rst_busy
		.rd_rst_busy()  // output wire rd_rst_busy
	);
	
	fifo_generator_0 fifo_2 (
		.clk(clk),                  // input wire clk
		.srst(~rstn),                // input wire srst
		.din(din_fifo2),                  // input wire [35 : 0] din
		.wr_en(wr_en2),              // input wire wr_en
		.rd_en(rd_en2),              // input wire rd_en
		.dout(dout_fifo2),                // output wire [35 : 0] dout
		.full(),                // output wire full
		.overflow(),        // output wire overflow
		.empty(empty2),              // output wire empty
		.wr_rst_busy(),  // output wire wr_rst_busy
		.rd_rst_busy()  // output wire rd_rst_busy
	);

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
            
            
function cmp_lte;        //return true if d1 key <= d2 key
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
		d1_flag = d1[DATA_WIDTH-1:DATA_WIDTH-2];
        d2_flag = d2[DATA_WIDTH-1:DATA_WIDTH-2];
        d1_key = d1[KEY_WIDTH-1:0];
        d2_key = d2[KEY_WIDTH-1:0];
        case (d1_flag)
        2'b01: begin    //d1 is min
            case (d2_flag)
            2'b11: cmp_lte = 1;  //d2 is max
            2'b00: cmp_lte = 1;  //d2 is normal
            2'b01: cmp_lte = 1;  //d2 is min
            default: cmp_lte = 0;
            endcase
        end
        2'b11: begin    //d1 is max
            case (d2_flag)
            2'b11: cmp_lte = 1;  //d2 is max
            default: cmp_lte = 0;
            endcase
        end
        2'b00: begin    //d1 is normal
            case(d2_flag)
            2'b11: cmp_lte = 1;  //d2 is max
            2'b01: cmp_lte = 0;  //d2 is min
            2'b00: cmp_lte = d1_key < d2_key || d1_key == d2_key;  //d2 is normal
            default: cmp_lte = 0;
            endcase
        end
        default:
            cmp_lte = 0;
        endcase
    end
endfunction

endmodule