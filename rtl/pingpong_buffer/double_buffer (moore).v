`timescale 1ns / 1ps

module double_buffer_busy(
    input clk
    );
    
    wire [5:0] bram_write_addr;
    wire [7:0] bram_write_data;

    reg       start_write, start_read, addr_sel;
    wire      busy_write, busy_read;
    wire      busy_write_BRAM1, busy_write_BRAM2; 
    wire      busy_read_BRAM1, busy_read_BRAM2; 
    
    assign    busy_write_BRAM1 = (addr_sel)?0:busy_write;
    assign    busy_write_BRAM2 = (addr_sel)?busy_write:0;
    assign    busy_read_BRAM1  = (addr_sel)?busy_read:0;
    assign    busy_read_BRAM2  = (addr_sel)?0:busy_read;
    
    /////////////////////////////////////////////////// write buffer 
    write_buffer_busy1 write(
    .clk(clk),
    .start(start_write),
    .busy(busy_write),
    .data(bram_write_data),
    .addr(bram_write_addr)
    );
    
    //////////////////////////////////////////////////// control FSM
    reg    [2:0]  state;
    reg    [2:0]  next_state;
    localparam START_BRAM1_WRITE_BRAM2_READ = 3'b000;
    localparam BRAM1_WRITE_BRAM2_READ       = 3'b001;
    localparam START_BRAM2_WRITE_BRAM1_READ = 3'b011;
    localparam BRAM2_WRITE_BRAM1_READ       = 3'b010;
    localparam START_BRAM1_WRITE            = 3'b110;
    localparam BRAM1_WRITE                  = 3'b100;
    
    reg    bram49_wea1, bram49_wea2;
    
    initial begin
        addr_sel    <= 1'b0;
        bram49_wea1 <= 1'b1; 
        bram49_wea2 <= 1'b0;         
        start_write <= 1'b1;
        start_read  <= 1'b0;  
        
        state       = START_BRAM1_WRITE;
        next_state  = START_BRAM1_WRITE;
    end
    
    always@(posedge clk)
    begin
        state <= next_state;
    end
    
    always@(posedge clk)
    begin
        case(next_state)
            START_BRAM1_WRITE:
            begin
                addr_sel    <= 1'b0;
                bram49_wea1 <= 1'b1; 
                bram49_wea2 <= 1'b0;         
                start_write <= 1'b1;
                start_read  <= 1'b0;          
            end
            
            BRAM1_WRITE:
            begin
                addr_sel    <= 1'b0;
                bram49_wea1 <= 1'b1;
                bram49_wea2 <= 1'b0;
                start_read  <= 1'b0;
                start_write <= 1'b0;
            end
            
            START_BRAM2_WRITE_BRAM1_READ:
            begin
                addr_sel    <= 1'b1;
                bram49_wea1 <= 1'b0;
                bram49_wea2 <= 1'b1;
                start_read  <= 1'b1;
                start_write <= 1'b1;
            end
            
            BRAM2_WRITE_BRAM1_READ:
            begin
                addr_sel    <= 1'b1;
                bram49_wea1 <= 1'b0;
                bram49_wea2 <= 1'b1;
                start_read  <= 1'b0;
                start_write <= 1'b0;
            end           
             
            START_BRAM1_WRITE_BRAM2_READ:
            begin
                addr_sel    <= 1'b0;
                bram49_wea1 <= 1'b1;
                bram49_wea2 <= 1'b0;
                start_read  <= 1'b1;
                start_write <= 1'b1;
            end
            
            BRAM1_WRITE_BRAM2_READ:
            begin
                addr_sel    <= 1'b0;
                bram49_wea1 <= 1'b1;
                bram49_wea2 <= 1'b0;
                start_read  <= 1'b0;
                start_write <= 1'b0;
            end
        endcase
    end
    
    always@(*)
    begin
        case(state)
            START_BRAM1_WRITE: 
            begin
                next_state = BRAM1_WRITE;
            end
            
            BRAM1_WRITE: 
            begin
                next_state = (busy_write_BRAM1)?BRAM1_WRITE:START_BRAM2_WRITE_BRAM1_READ;
            end
             
            START_BRAM2_WRITE_BRAM1_READ: 
            begin    
                next_state  = BRAM2_WRITE_BRAM1_READ;
            end
            
            BRAM2_WRITE_BRAM1_READ: 
            begin    
                next_state  = (!busy_write_BRAM2 && !busy_read_BRAM1)?START_BRAM1_WRITE_BRAM2_READ:BRAM2_WRITE_BRAM1_READ;
            end   
                     
            START_BRAM1_WRITE_BRAM2_READ: 
            begin    
                next_state  = BRAM1_WRITE_BRAM2_READ;
            end
            
            BRAM1_WRITE_BRAM2_READ: 
            begin    
                next_state  = (!busy_write_BRAM1 && !busy_read_BRAM2)?START_BRAM2_WRITE_BRAM1_READ:BRAM1_WRITE_BRAM2_READ;
            end
        endcase
    end
    
    ////// BRAM 1 Instantiation
    wire   [5:0] bram49_addra1;
    wire   [5:0] addr_read;
    wire   [7:0] bram49_dina1;
    wire   [7:0] bram49_douta1;
    
    assign bram49_addra1 = (addr_sel)? addr_read:bram_write_addr;
    assign bram49_dina1  = bram_write_data; 
    
    BRAM bram1 (
        .BRAM_PORTA_0_clk(clk),             // input wire clka
        .BRAM_PORTA_0_we(bram49_wea1),      // input wire [0 : 0] wea
        .BRAM_PORTA_0_addr(bram49_addra1),  // input wire [5 : 0] addra
        .BRAM_PORTA_0_din(bram49_dina1),    // input wire [7 : 0] dina
        .BRAM_PORTA_0_dout(bram49_douta1)
    );    
    
    ////// BRAM 2 Instantiation
    wire   [5:0] bram49_addra2;
    wire   [7:0] bram49_dina2;
    wire   [7:0] bram49_douta2;
    
    assign bram49_addra2 = (addr_sel)? bram_write_addr:addr_read;
    assign bram49_dina2  = bram_write_data; 
    
    BRAM bram2 (
        .BRAM_PORTA_0_clk(clk),    // input wire clka
        .BRAM_PORTA_0_we(bram49_wea2),      // input wire [0 : 0] wea
        .BRAM_PORTA_0_addr(bram49_addra2),  // input wire [5 : 0] addra
        .BRAM_PORTA_0_din(bram49_dina2),    // input wire [7 : 0] dina
        .BRAM_PORTA_0_dout(bram49_douta2)
    );
    
    wire   [7:0] read_buffer_datain = (addr_sel)?bram49_douta2:bram49_douta1;
    ////// BRAM READ Instantiation   
    read_buffer_busy1 read(
    .clk(clk),
    .start(start_read),
    .busy(busy_read),
    .addr(addr_read),
    .data(read_buffer_datain)
    );
endmodule


///////////////////  write buffer
module write_buffer_busy1(
input clk,
input start,
output busy,
output [5:0] addr,
output [7:0] data
);

    ///////////////////////////// write part
    reg  busy_inner;
    reg  [5:0] addr_inner;
    reg  [7:0] data_inner;
    // use gray code to encode the state to reduce the bit change.
    reg  [3:0] state;
    reg  [3:0] next_state;
    localparam WAIT_START       = 4'b0000;
    localparam WRITE_ADDR0      = 4'b0001;
    localparam WRITE_ADDR1      = 4'b0011;
    localparam WRITE_ADDR2      = 4'b0010;
    localparam WRITE_ADDR3      = 4'b0110;
    localparam WRITE_ADDR4      = 4'b0111;
    localparam WRITE_ADDR5      = 4'b0101;
    localparam WRITE_ADDR6      = 4'b0100;
    localparam WRITE_ADDR7      = 4'b1100;
    localparam WRITE_ADDR8      = 4'b1101;
//    localparam WRITE_ADDR9      = 4'b1111;
//    localparam WRITE_ADDR10     = 4'b1110;
//    localparam WRITE_ADDR11     = 4'b1010;
//    localparam WRITE_ADDR12     = 4'b1011;
//    localparam WRITE_ADDR13     = 4'b1001;
//    localparam WRITE_ADDR14     = 4'b1000;

    initial begin
        state      = WAIT_START;
        next_state = WAIT_START;
        busy_inner = 0;
    end
    
    always@(posedge clk)
    begin
        case(next_state)
            WAIT_START: begin
                addr_inner <= 0;
                data_inner <= 0;
                busy_inner <= 0;
            end
            WRITE_ADDR0: begin
                addr_inner <= 0;
                data_inner <= 0;
                busy_inner <= 1;
            end
            WRITE_ADDR1: begin
                addr_inner <= 1;
                data_inner <= 1;
            end
            WRITE_ADDR2: begin
                addr_inner <= 2;
                data_inner <= 2;
            end
            WRITE_ADDR3: begin
                addr_inner <= 3;
                data_inner <= 3;
            end
            WRITE_ADDR4: begin
                addr_inner <= 4;
                data_inner <= 4;
            end
            WRITE_ADDR5: begin
                addr_inner <= 5;
                data_inner <= 5;
            end
            WRITE_ADDR6: begin
                addr_inner <= 6;
                data_inner <= 6;
            end
            WRITE_ADDR7: begin
                addr_inner <= 7;
                data_inner <= 7;   
            end
            default: begin
                addr_inner <= 0;
                data_inner <= 0;
            end
        endcase
    end
    
    always@(*)
    begin
        case(state)
            WAIT_START:  next_state = (start)?WRITE_ADDR0:WAIT_START;
            WRITE_ADDR0: next_state = WRITE_ADDR1;
            WRITE_ADDR1: next_state = WRITE_ADDR2;
            WRITE_ADDR2: next_state = WRITE_ADDR3;
            WRITE_ADDR3: next_state = WRITE_ADDR4;
            WRITE_ADDR4: next_state = WRITE_ADDR5;
            WRITE_ADDR5: next_state = WRITE_ADDR6;
            WRITE_ADDR6: next_state = WRITE_ADDR7;
            WRITE_ADDR7: next_state = WAIT_START;
            default :    next_state = WAIT_START;
        endcase
    end
    
    always@(posedge clk)
    begin
        state <= next_state;
    end
    
    assign data = data_inner;
    assign addr = addr_inner;
    assign busy = busy_inner;
    
endmodule


////////////////////////////////////////////// READ double buffer
module read_buffer_busy1(
input clk,
input start,
output busy,
output [5:0] addr,
input  [7:0] data
);

reg       busy_inner;
reg [2:0] state;
reg [2:0] next_state;
reg [5:0] addr_inner;
reg [5:0] addr_data;

localparam STANDBY          = 3'b000;
localparam PREPARE1         = 3'b001;
localparam PREPARE2         = 3'b011;
localparam READ             = 3'b010;
localparam READ_SECOND2END  = 3'b110;
localparam READ_LAST        = 3'b100;
reg  [7:0] data_inner[0:7];
reg  cnt;

initial
begin
    cnt = 0;
    state = STANDBY;
    next_state = STANDBY;
    busy_inner = 0;
end

always@(posedge clk)
begin
    state <= next_state;
end

// output 
always @(posedge clk)
begin
   case(next_state)
        STANDBY:
        begin
            addr_inner <= 0;
            addr_data  <= 0;
            busy_inner <= 0;
        end
        
        PREPARE1:
        begin
            addr_inner <= 1;   
            addr_data  <= 0;
            busy_inner <= 1;
        end
        
        PREPARE2:
        begin
            addr_inner <= 1;   
            addr_data  <= 0;
        end
            
        READ:
        begin
            addr_inner <= addr_inner + 1;
            addr_data  <= addr_data + 1;
            data_inner[addr_data] <= data;
        end   
            
        READ_SECOND2END:
        begin
            addr_inner <= 0;
            addr_data  <=  3'b111;
            data_inner[3'b110] <= data;
        end
                    
        READ_LAST:
        begin
            addr_inner <= 0;
            addr_data  <= 0;
            data_inner[3'b111] <= data;
            busy_inner <= 0;
        end
        
        default:  state <= STANDBY;
   endcase
end

// state trasient 
always@(*)
begin
    case(state)
        STANDBY:
        begin
            next_state = (start)?PREPARE1:STANDBY;
        end
        
        PREPARE1:
        begin
            next_state = PREPARE2;
        end
        
        PREPARE2:
        begin
            next_state = READ;
        end
        
        READ:
        begin
            next_state = (addr_inner == 3'b111)?READ_SECOND2END:READ;
        end    
             
        READ_SECOND2END:
        begin
            next_state = (addr_data == 3'b111)?READ_LAST:READ_SECOND2END;
        end 
        
        READ_LAST:
        begin
            next_state = (addr_data == 0)?STANDBY:READ_LAST;
        end
        default:  state <= STANDBY;
    endcase
end

assign addr = addr_inner;
assign busy = busy_inner;

endmodule