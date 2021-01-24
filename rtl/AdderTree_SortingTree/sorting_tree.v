`timescale 1ns / 1ps

module max#(parameter NUM = 300, LEN = 16, LEVEL = $clog2(NUM) ) (
    input  [NUM*LEN-1:0] a,
    output [LEN-1:0] max
    );
    // 将输入长向量还原为矩阵形式。高位是右侧的C&S
    wire [LEN-1:0] a_inner[NUM-1:0]; 
    reg  [LEN-1:0] max_inner;
    genvar i;
    for ( i = 0; i < NUM ; i = i + 1)
    begin
        assign a_inner[i]=a[(i+1)*LEN-1:i*LEN];
    end
   
// 计算各个级别的比较器数目，将各个级别的首地址记录在一个单独的变量之中
//    reg  [LEVEL:0] LEVEL_START[LEVEL+1:0];
//    reg  [LEVEL:0] NUM_PER_LEVEL[LEVEL:0];
//    reg  [LEVEL:0] is_odd; //is_odd[k]记录第k级输入总数是否为奇数。
//    reg  [LEVEL:0] temp;
//    integer j;
//    initial 
//    begin
//        LEVEL_START[0]    = 0;
//        LEVEL_START[1]    = NUM;
//        is_odd[0]         = NUM[0]==1  ? 1:0;
//        NUM_PER_LEVEL[0]  = NUM;
//        for (j = 1; j < LEVEL + 1 ; j = j + 1)
//        begin
//            temp              = NUM_PER_LEVEL[j-1] >> 1;
//            NUM_PER_LEVEL[j]  =  temp + is_odd[j-1];
//            LEVEL_START[j+1]  = LEVEL_START[j] + NUM_PER_LEVEL[j];
//            is_odd[j]         = NUM_PER_LEVEL[j][0] == 1? 1:0;
//        end
//    end
     
    // 计算各个级别的比较器数目，将各个级别的首地址记录在一个单独的变量之中
    // START_LEVELx             记录了LEVELx输入线的起始index。 
    // NUM_INPUT_LEVELx         记录LEVELx的输入线数目。
    // IS_ODD_NUM_INPUT_LEVELx  记录了LEVELx的输入线总数是否为奇数？
    // DIV2_NUM_LEVELx          为LEVELx输入线数目直接右移1位。（中间变量）
    // NUM_CS_LEVELx            记录了LEVELx的C&S比较器个数,同时也是LEVEL(x+1)的输入线数目。
    
    localparam   START_LEVEL0             =  0;
    localparam   NUM_INPUT_LEVEL0         =  NUM;
    localparam   IS_ODD_NUM_INPUT_LEVEL0  =  NUM_INPUT_LEVEL0[0] == 1? 1:0;
    localparam   DIV2_NUM_INPUT_LEVEL0    =  NUM_INPUT_LEVEL0 >> 1;
    localparam   NUM_CS_LEVEL0            =  (NUM_INPUT_LEVEL0==1)?0:DIV2_NUM_INPUT_LEVEL0 + IS_ODD_NUM_INPUT_LEVEL0;
            
    localparam   START_LEVEL1             =  START_LEVEL0 + NUM_INPUT_LEVEL0;
    localparam   NUM_INPUT_LEVEL1         =  NUM_CS_LEVEL0;
    localparam   IS_ODD_NUM_INPUT_LEVEL1  =  NUM_INPUT_LEVEL1[0] == 1? 1:0;
    localparam   DIV2_NUM_INPUT_LEVEL1    =  NUM_INPUT_LEVEL1 >> 1;
    localparam   NUM_CS_LEVEL1            =  (NUM_INPUT_LEVEL1==1)?0:DIV2_NUM_INPUT_LEVEL1 + IS_ODD_NUM_INPUT_LEVEL1;
                
    localparam   START_LEVEL2             =  START_LEVEL1 + NUM_INPUT_LEVEL1;
    localparam   NUM_INPUT_LEVEL2         =  NUM_CS_LEVEL1;
    localparam   IS_ODD_NUM_INPUT_LEVEL2  =  NUM_INPUT_LEVEL2[0] == 1? 1:0;
    localparam   DIV2_NUM_INPUT_LEVEL2    =  NUM_INPUT_LEVEL2 >> 1;
    localparam   NUM_CS_LEVEL2            =  (NUM_INPUT_LEVEL2==1)?0:DIV2_NUM_INPUT_LEVEL2 + IS_ODD_NUM_INPUT_LEVEL2;
  
    localparam   START_LEVEL3             =  START_LEVEL2 + NUM_INPUT_LEVEL2;
    localparam   NUM_INPUT_LEVEL3         =  NUM_CS_LEVEL2;
    localparam   IS_ODD_NUM_INPUT_LEVEL3  =  NUM_INPUT_LEVEL3[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL3    =  NUM_INPUT_LEVEL3>>1;
    localparam   NUM_CS_LEVEL3            =  (NUM_INPUT_LEVEL3==1)?0:DIV2_NUM_INPUT_LEVEL3 + IS_ODD_NUM_INPUT_LEVEL3;
    
    localparam   START_LEVEL4             =  START_LEVEL3 + NUM_INPUT_LEVEL3;
    localparam   NUM_INPUT_LEVEL4         =  NUM_CS_LEVEL3;
    localparam   IS_ODD_NUM_INPUT_LEVEL4  =  NUM_INPUT_LEVEL4[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL4    =  NUM_INPUT_LEVEL4>>1;
    localparam   NUM_CS_LEVEL4            =  (NUM_INPUT_LEVEL4==1)?0:DIV2_NUM_INPUT_LEVEL4 + IS_ODD_NUM_INPUT_LEVEL4;
    
    localparam   START_LEVEL5             =  START_LEVEL4 + NUM_INPUT_LEVEL4;
    localparam   NUM_INPUT_LEVEL5         =  NUM_CS_LEVEL4;
    localparam   IS_ODD_NUM_INPUT_LEVEL5  =  NUM_INPUT_LEVEL5[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL5    =  NUM_INPUT_LEVEL5>>1;
    localparam   NUM_CS_LEVEL5            =  (NUM_INPUT_LEVEL5==1)?0:DIV2_NUM_INPUT_LEVEL5 + IS_ODD_NUM_INPUT_LEVEL5;
    
    localparam   START_LEVEL6             =  START_LEVEL5 + NUM_INPUT_LEVEL5;
    localparam   NUM_INPUT_LEVEL6         =  NUM_CS_LEVEL5;
    localparam   IS_ODD_NUM_INPUT_LEVEL6  =  NUM_INPUT_LEVEL6[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL6    =  NUM_INPUT_LEVEL6>>1;
    localparam   NUM_CS_LEVEL6            =  (NUM_INPUT_LEVEL6==1)?0:DIV2_NUM_INPUT_LEVEL6 + IS_ODD_NUM_INPUT_LEVEL6;
    
    localparam   START_LEVEL7             =  START_LEVEL6 + NUM_INPUT_LEVEL6;
    localparam   NUM_INPUT_LEVEL7         =  NUM_CS_LEVEL6;
    localparam   IS_ODD_NUM_INPUT_LEVEL7  =  NUM_INPUT_LEVEL7[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL7    =  NUM_INPUT_LEVEL7>>1;
    localparam   NUM_CS_LEVEL7            =  (NUM_INPUT_LEVEL7==1)?0:DIV2_NUM_INPUT_LEVEL7 + IS_ODD_NUM_INPUT_LEVEL7;
    
    localparam   START_LEVEL8             =  START_LEVEL7 + NUM_INPUT_LEVEL7;
    localparam   NUM_INPUT_LEVEL8         =  NUM_CS_LEVEL7;
    localparam   IS_ODD_NUM_INPUT_LEVEL8  =  NUM_INPUT_LEVEL8[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL8    =  NUM_INPUT_LEVEL8>>1;
    localparam   NUM_CS_LEVEL8            =  (NUM_INPUT_LEVEL8==1)?0:DIV2_NUM_INPUT_LEVEL8 + IS_ODD_NUM_INPUT_LEVEL8;
    
    localparam   START_LEVEL9             =  START_LEVEL8 + NUM_INPUT_LEVEL8;
    localparam   NUM_INPUT_LEVEL9         =  NUM_CS_LEVEL8;
    localparam   IS_ODD_NUM_INPUT_LEVEL9  =  NUM_INPUT_LEVEL9[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL9    =  NUM_INPUT_LEVEL9>>1;
    localparam   NUM_CS_LEVEL9            =  (NUM_INPUT_LEVEL9==1)?0:DIV2_NUM_INPUT_LEVEL9 + IS_ODD_NUM_INPUT_LEVEL9;
    localparam   START_LEVEL10            =  START_LEVEL9 + NUM_INPUT_LEVEL9;
    
    
    wire [LEN-1:0] wire_inner[START_LEVEL10-1:0]; //多加的1是最终的输出。
    
    for (i = 0; i < NUM_INPUT_LEVEL0; i = i + 1) 
    begin
       assign wire_inner[i] = a_inner[i];
    end
    
    genvar index_num;
    generate
    for (index_num = 0; index_num < NUM_CS_LEVEL0-IS_ODD_NUM_INPUT_LEVEL0; index_num = index_num + 1)
    begin
        compare_select#(.LEN(LEN)) level0(.a(wire_inner[START_LEVEL0+2*index_num]),.b(wire_inner[START_LEVEL0+2*index_num+1]),.out(wire_inner[START_LEVEL1+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL0 && (NUM_CS_LEVEL0>0))
        compare_select#(.LEN(LEN)) level0_last(.a(wire_inner[START_LEVEL0+2*(NUM_CS_LEVEL0-1)]),.b(wire_inner[START_LEVEL0+2*(NUM_CS_LEVEL0-1)]),.out(wire_inner[START_LEVEL1+NUM_CS_LEVEL0 - 1]));
    
    for (index_num = 0; index_num < NUM_CS_LEVEL1-IS_ODD_NUM_INPUT_LEVEL1; index_num = index_num + 1)
    begin
        compare_select#(.LEN(LEN)) level1(.a(wire_inner[START_LEVEL1+2*index_num]),.b(wire_inner[START_LEVEL1+2*index_num+1]),.out(wire_inner[START_LEVEL2+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL1 && (NUM_CS_LEVEL1>0))
        compare_select#(.LEN(LEN)) level1_last(.a(wire_inner[START_LEVEL1+2*(NUM_CS_LEVEL1-1)]),.b(wire_inner[START_LEVEL1+2*(NUM_CS_LEVEL1-1)]),.out(wire_inner[START_LEVEL2+NUM_CS_LEVEL1 - 1]));
    
    for (index_num = 0; index_num < NUM_CS_LEVEL2-IS_ODD_NUM_INPUT_LEVEL2; index_num = index_num + 1)
    begin
        compare_select#(.LEN(LEN)) level2(.a(wire_inner[START_LEVEL2+2*index_num]),.b(wire_inner[START_LEVEL2+2*index_num+1]),.out(wire_inner[START_LEVEL3+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL2 && (NUM_CS_LEVEL2>0))
        compare_select#(.LEN(LEN)) level2_last(.a(wire_inner[START_LEVEL2+2*(NUM_CS_LEVEL2-1)]),.b(wire_inner[START_LEVEL2+2*(NUM_CS_LEVEL2-1)]),.out(wire_inner[START_LEVEL3+NUM_CS_LEVEL2 - 1]));
    
    for (index_num = 0; index_num < NUM_CS_LEVEL3-IS_ODD_NUM_INPUT_LEVEL3; index_num = index_num + 1)
    begin
        compare_select#(.LEN(LEN)) level3(.a(wire_inner[START_LEVEL3+2*index_num]),.b(wire_inner[START_LEVEL3+2*index_num+1]),.out(wire_inner[START_LEVEL4+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL3 && (NUM_CS_LEVEL3>0))
        compare_select#(.LEN(LEN)) level3_last(.a(wire_inner[START_LEVEL3+2*(NUM_CS_LEVEL3-1)]),.b(wire_inner[START_LEVEL3+2*(NUM_CS_LEVEL3-1)]),.out(wire_inner[START_LEVEL4+NUM_CS_LEVEL3 - 1]));
    
    for (index_num = 0; index_num < NUM_CS_LEVEL4-IS_ODD_NUM_INPUT_LEVEL4; index_num = index_num + 1)
    begin
        compare_select#(.LEN(LEN)) level4(.a(wire_inner[START_LEVEL4+2*index_num]),.b(wire_inner[START_LEVEL4+2*index_num+1]),.out(wire_inner[START_LEVEL5+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL4 && (NUM_CS_LEVEL4>0))
        compare_select#(.LEN(LEN)) level4_last(.a(wire_inner[START_LEVEL4+2*(NUM_CS_LEVEL4-1)]),.b(wire_inner[START_LEVEL4+2*(NUM_CS_LEVEL4-1)]),.out(wire_inner[START_LEVEL5+NUM_CS_LEVEL4 - 1]));
    
    for (index_num = 0; index_num < NUM_CS_LEVEL5-IS_ODD_NUM_INPUT_LEVEL5; index_num = index_num + 1)
    begin
        compare_select#(.LEN(LEN)) level5(.a(wire_inner[START_LEVEL5+2*index_num]),.b(wire_inner[START_LEVEL5+2*index_num+1]),.out(wire_inner[START_LEVEL6+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL5 && (NUM_CS_LEVEL5>0))
        compare_select#(.LEN(LEN)) level5_last(.a(wire_inner[START_LEVEL5+2*(NUM_CS_LEVEL5-1)]),.b(wire_inner[START_LEVEL5+2*(NUM_CS_LEVEL5-1)]),.out(wire_inner[START_LEVEL6+NUM_CS_LEVEL5 - 1]));
    
    for (index_num = 0; index_num < NUM_CS_LEVEL6-IS_ODD_NUM_INPUT_LEVEL6; index_num = index_num + 1)
    begin
        compare_select#(.LEN(LEN)) level6(.a(wire_inner[START_LEVEL6+2*index_num]),.b(wire_inner[START_LEVEL6+2*index_num+1]),.out(wire_inner[START_LEVEL7+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL6 && (NUM_CS_LEVEL6>0))
        compare_select#(.LEN(LEN)) level6_last(.a(wire_inner[START_LEVEL6+2*(NUM_CS_LEVEL6-1)]),.b(wire_inner[START_LEVEL6+2*(NUM_CS_LEVEL6-1)]),.out(wire_inner[START_LEVEL7+NUM_CS_LEVEL6 - 1]));
    
    for (index_num = 0; index_num < NUM_CS_LEVEL7-IS_ODD_NUM_INPUT_LEVEL7; index_num = index_num + 1)
    begin
        compare_select#(.LEN(LEN)) level7(.a(wire_inner[START_LEVEL7+2*index_num]),.b(wire_inner[START_LEVEL7+2*index_num+1]),.out(wire_inner[START_LEVEL8+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL7 && (NUM_CS_LEVEL7>0))
        compare_select#(.LEN(LEN)) level7_last(.a(wire_inner[START_LEVEL7+2*(NUM_CS_LEVEL7-1)]),.b(wire_inner[START_LEVEL7+2*(NUM_CS_LEVEL7-1)]),.out(wire_inner[START_LEVEL8+NUM_CS_LEVEL7 - 1]));
    
    for (index_num = 0; index_num < NUM_CS_LEVEL8-IS_ODD_NUM_INPUT_LEVEL8; index_num = index_num + 1)
    begin
        compare_select#(.LEN(LEN)) level8(.a(wire_inner[START_LEVEL8+2*index_num]),.b(wire_inner[START_LEVEL8+2*index_num+1]),.out(wire_inner[START_LEVEL9+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL8 && (NUM_CS_LEVEL8>0))
        compare_select#(.LEN(LEN)) level8_last(.a(wire_inner[START_LEVEL8+2*(NUM_CS_LEVEL8-1)]),.b(wire_inner[START_LEVEL8+2*(NUM_CS_LEVEL8-1)]),.out(wire_inner[START_LEVEL9+NUM_CS_LEVEL8 - 1]));
    
    for (index_num = 0; index_num < NUM_CS_LEVEL9-IS_ODD_NUM_INPUT_LEVEL9; index_num = index_num + 1)
    begin
        compare_select#(.LEN(LEN)) level9(.a(wire_inner[START_LEVEL9+2*index_num]),.b(wire_inner[START_LEVEL9+2*index_num+1]),.out(wire_inner[START_LEVEL10+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL9 && (NUM_CS_LEVEL9>0))
        compare_select#(.LEN(LEN)) level9_last(.a(wire_inner[START_LEVEL9+2*(NUM_CS_LEVEL9-1)]),.b(wire_inner[START_LEVEL9+2*(NUM_CS_LEVEL9-1)]),.out(wire_inner[START_LEVEL10+NUM_CS_LEVEL9 - 1]));
    endgenerate
    
    always@*
    begin
        max_inner = wire_inner[START_LEVEL10-1];
    end
    
    assign max = max_inner;
endmodule

// a is the right input port & b is the left input port.
module compare_select#(parameter LEN = 16)(
    input  signed [LEN-1:0] a,
    input  signed [LEN-1:0] b,
    output signed [LEN-1:0] out
    );
    
    reg signed [LEN-1:0] out_inner;
    
    always @* begin
        if (a>b)
        begin
            out_inner = a;
        end
        else begin 
            out_inner = b;
        end
    end
    
    assign out = out_inner;
endmodule

