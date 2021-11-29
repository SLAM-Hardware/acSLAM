/**
* This file is part of ac^2SLAM.
*
* Copyright (C) 2021 Cheng Wang <wangcheng at stu dot xjtu dot edu dot cn> (Xi'an Jiaotong University)
* For more information see <https://github.com/SLAM-Hardware/acSLAM>
*
* ac^2SLAM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ac^2SLAM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ac^2SLAM. If not, see <http://www.gnu.org/licenses/>.
*/

`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
//`define _MAX_   
module heap_rtl
#(
	parameter DATA_WIDTH = 292,   
	parameter KEY_WIDTH = 7, 
	parameter NLEVELS = 6      
	)
(
	input clk,    
	input rstn,   
	input [DATA_WIDTH-1:0] din, 
	input en,
	input init,
	input flush, 
	input in_end,
	
	//output [DATA_WIDTH-1:0] dout1, dout2,
	output reg [DATA_WIDTH-1:0] dout,
	output heap_emp1,heap_emp2,
	//output heap_emp1r,heap_emp2r,
	output sort_end,
	output valid
    );

	localparam ADDR_WIDTH = NLEVELS-1;
	`ifdef _MAX_  
	localparam INIT_DATA = {{(DATA_WIDTH-KEY_WIDTH){1'b0}},{(KEY_WIDTH){1'b1}}};
	localparam FLUSH_DATA = {{(DATA_WIDTH-KEY_WIDTH){1'b0}},{(KEY_WIDTH){1'b0}}};
	`else
	localparam INIT_DATA = {{(DATA_WIDTH-KEY_WIDTH){1'b0}},{(KEY_WIDTH){1'b0}}};
	localparam FLUSH_DATA = {{(DATA_WIDTH-KEY_WIDTH){1'b0}},{(KEY_WIDTH){1'b1}}};
	`endif

	reg [DATA_WIDTH-1:0] din_1,din_2;

	reg cnt;

	wire [DATA_WIDTH-1:0] comp1,comp2;

	wire en_1,en_2;

	wire [DATA_WIDTH-1:0] dout1,dout2;
	
	wire valid_1,valid_2;

	reg ext, fifo, fifo_delay;

	always @(posedge clk or negedge rstn) begin
		if(~rstn) begin
			ext <= 0;
			fifo <= 0;
		end 
		else begin
		    if(!fifo && (!inend_r && in_end)) begin
			    ext <= 0;
			    fifo <= 1;
			end
			else if(en) begin
				ext <= 1;
				//fifo <= 0;
			end
			else if(!en && ext) begin
				ext <= 0;
				fifo <= 0;
			end
			else if(fifo_emptyr4 && fifo && fifo_delay) begin
				ext <= 0;
				fifo <= 0;
				fifo_delay <= 0;
			end
			
			if(fifo_t1) begin
			    fifo_delay <= 1;
			end
		end
	end

	reg ext_r,fifo_r,ext_in,fifo_in,ext_out,fifo_out,fifo_t1,inend_r;

	always @(posedge clk or negedge rstn) begin
		if(~rstn) begin
			ext_r <= 0;
			fifo_r <= 0;
			ext_in <= 0;
			fifo_in <= 0;
			ext_out <= 0;
			fifo_out <= 0;
			fifo_t1 <= 0;
			inend_r <= 0;
		end else begin
			ext_r <= ext;
			fifo_r <= fifo;
			ext_in <= ext_r;
			fifo_in <= fifo_r;
			ext_out <= ext_in;
			fifo_out <= fifo_in;
			fifo_t1 <= fifo_out;
			inend_r <= in_end;
		end
	end

    //?????????fifo???????cnt?????��??????????????????
	always @(posedge clk or negedge rstn) begin
		if(~rstn) begin
			cnt <= 1'b0;
		end 
		else if(ext | ext_out | fifo_out | flush) begin
			cnt <= ~cnt;
		end
		else if(init) begin
			cnt <= 1'b0;
		end
	end
	
	reg [4:0] sort_cnt;
    reg sort_flag;
	
	always @(posedge clk or negedge rstn) begin
	   if(~rstn) begin
	       sort_cnt <= 0;
	       sort_flag <= 0;
	   end
	   else begin
	       if(~fifo_in && fifo_out) begin
	           sort_flag <= 1;
	       end
	       else if(sort_cnt == 2*(NLEVELS-1))begin
	           sort_flag <= 0;
	       end
	       if(sort_flag) begin
	           sort_cnt <= (sort_cnt == 2*(NLEVELS-1)) ? 0 : sort_cnt + 1;
	       end
	   end
	end
	   
    assign sort_end = (sort_cnt == 2*(NLEVELS-1)) ? 1 : 0;
	
	wire empty1, empty2, fifo_emptyrr;

	reg [3:0] empty1r, empty2r;

	//??empty????????????????????��??fifoin??????
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

	//?��??fifoin?????????fifo????FIFO???rr????
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
            if(init) begin
                dout_buf1 <= 0;
            end
            else begin
                dout_buf1 <= dout_fifo1;
            end
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

	assign rd_en1 = ready1 || heap_emp1;

	always @(posedge clk or negedge rstn) begin
        if(~rstn)
            dout_buf2 <= 0;
        else
            if(init) begin
                dout_buf2 <= 0;
            end
            else begin
                dout_buf2 <= dout_fifo2;
            end
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

	assign rd_en2 = ready2 || heap_emp2;

	reg wr_en1,wr_en2;

	reg [DATA_WIDTH-1:0] din_fifo1,din_fifo2;

	//fifo��?????????ext_out????????????????FIFO??
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
            else if(sort_end) begin
                din_fifo1 <= 0;
                wr_en1 <= 1'b1;
                din_fifo2 <= 0;
                wr_en2 <= 1'b1;
            end
            else begin
                din_fifo1 <= 0;
                wr_en1 <= 1'b0;
                din_fifo2 <= 0;
                wr_en2 <= 1'b0;
            end
         end
      end

    //????????????????????????????????��??fifo????????
	//ext = 1,??din1din2??din?��??????fifo = 1????fifo_en1?din_1 = dout_buf1??
    always @(posedge clk or negedge rstn) begin
        if(~rstn) begin
            din_1 <= 0;
            din_2 <= 0;
        end
        else begin
            if(cnt)begin
                if(ext || en) begin
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
                if(ext || en) begin
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
    
    always @(*) begin
        if(!rstn) begin
            dout = 0;
        end
        else if(flush) begin
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
		.heap_empty(heap_emp1)
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
		.heap_empty(heap_emp2)
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
		.empty(empty1)             // output wire empty
		//.wr_rst_busy(),  // output wire wr_rst_busy
		//.rd_rst_busy()  // output wire rd_rst_busy
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
		.empty(empty2)              // output wire empty
		//.wr_rst_busy(),  // output wire wr_rst_busy
		//.rd_rst_busy()  // output wire rd_rst_busy
	);

function cmp_lt;        //return true if d1 key < d2 key
	input [DATA_WIDTH-1:0] d1; 
	input [DATA_WIDTH-1:0] d2;
	reg [KEY_WIDTH-1:0] d1_key;
	reg [KEY_WIDTH-1:0] d2_key;	
	begin
		d1_key = d1[KEY_WIDTH-1:0];
		d2_key = d2[KEY_WIDTH-1:0];

        cmp_lt = d1_key < d2_key;  //d2 is normal
    end
endfunction
            
            
function cmp_lte;        //return true if d1 key <= d2 key
	input [DATA_WIDTH-1:0] d1; 
	input [DATA_WIDTH-1:0] d2;
	reg [KEY_WIDTH-1:0] d1_key;
	reg [KEY_WIDTH-1:0] d2_key;	
	begin
		d1_key = d1[KEY_WIDTH-1:0];
		d2_key = d2[KEY_WIDTH-1:0];

        cmp_lte = d1_key <= d2_key;  //d2 is normal
    end
endfunction

endmodule