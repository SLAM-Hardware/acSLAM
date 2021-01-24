

////////////////////////////// adder tree
module adder_tree#(parameter NUM = 300, LEN = 16, LEVEL = $clog2(NUM) ) (
    input  clk,
    input  [NUM*LEN-1:0] in,
    output [LEN-1:0] sum
    );
    
    // 将输入长向量还原为矩阵形式。高位是右侧的C&S
    wire  [LEN-1:0] in_inner[NUM-1:0]; 
    reg   [LEN-1:0] sum_inner;
    
    genvar i;
    for ( i = 0; i < NUM ; i = i + 1)
    begin
        assign in_inner[i]=in[(i+1)*LEN-1:i*LEN];
    end
    
    
    // 计算各个级别的比较器数目，将各个级别的首地址记录在一个单独的变量之中
    // START_LEVELx             记录了LEVELx输入线的起始index。 
    // NUM_INPUT_LEVELx         记录LEVELx的输入线数目。
    // IS_ODD_NUM_INPUT_LEVELx  记录了LEVELx的输入线总数是否为奇数？
    // DIV2_NUM_LEVELx          为LEVELx输入线数目直接右移1位。（中间变量）
    // NUM_ADDER_LEVELx         记录了LEVELx的ADDER个数,同时也是LEVEL(x+1)的输入线数目。
         
    localparam   START_LEVEL0             =  0;
    localparam   NUM_INPUT_LEVEL0         =  NUM;
    localparam   IS_ODD_NUM_INPUT_LEVEL0  =  NUM_INPUT_LEVEL0[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL0    =  NUM_INPUT_LEVEL0>>1;
    localparam   NUM_ADDER_LEVEL0         =  (NUM_INPUT_LEVEL0==1)?0:DIV2_NUM_INPUT_LEVEL0 + IS_ODD_NUM_INPUT_LEVEL0;
    
    localparam   START_LEVEL1             =  START_LEVEL0 + NUM_INPUT_LEVEL0*LEN;
    localparam   NUM_INPUT_LEVEL1         =  NUM_ADDER_LEVEL0;
    localparam   IS_ODD_NUM_INPUT_LEVEL1  =  NUM_INPUT_LEVEL1[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL1    =  NUM_INPUT_LEVEL1>>1;
    localparam   NUM_ADDER_LEVEL1         =  (NUM_INPUT_LEVEL1==1)?0:DIV2_NUM_INPUT_LEVEL1 + IS_ODD_NUM_INPUT_LEVEL1;
    
    localparam   START_LEVEL2             =  START_LEVEL1 + NUM_INPUT_LEVEL1*(LEN+1);
    localparam   NUM_INPUT_LEVEL2         =  NUM_ADDER_LEVEL1;
    localparam   IS_ODD_NUM_INPUT_LEVEL2  =  NUM_INPUT_LEVEL2[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL2    =  NUM_INPUT_LEVEL2>>1;
    localparam   NUM_ADDER_LEVEL2         =  (NUM_INPUT_LEVEL2==1)?0:DIV2_NUM_INPUT_LEVEL2 + IS_ODD_NUM_INPUT_LEVEL2;
    
    localparam   START_LEVEL3             =  START_LEVEL2 + NUM_INPUT_LEVEL2;
    localparam   NUM_INPUT_LEVEL3         =  NUM_ADDER_LEVEL2;
    localparam   IS_ODD_NUM_INPUT_LEVEL3  =  NUM_INPUT_LEVEL3[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL3    =  NUM_INPUT_LEVEL3>>1;
    localparam   NUM_ADDER_LEVEL3         =  (NUM_INPUT_LEVEL3==1)?0:DIV2_NUM_INPUT_LEVEL3 + IS_ODD_NUM_INPUT_LEVEL3;
    
    localparam   START_LEVEL4             =  START_LEVEL3 + NUM_INPUT_LEVEL3;
    localparam   NUM_INPUT_LEVEL4         =  NUM_ADDER_LEVEL3;
    localparam   IS_ODD_NUM_INPUT_LEVEL4  =  NUM_INPUT_LEVEL4[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL4    =  NUM_INPUT_LEVEL4>>1;
    localparam   NUM_ADDER_LEVEL4         =  (NUM_INPUT_LEVEL4==1)?0:DIV2_NUM_INPUT_LEVEL4 + IS_ODD_NUM_INPUT_LEVEL4;
    
    localparam   START_LEVEL5             =  START_LEVEL4 + NUM_INPUT_LEVEL4;
    localparam   NUM_INPUT_LEVEL5         =  NUM_ADDER_LEVEL4;
    localparam   IS_ODD_NUM_INPUT_LEVEL5  =  NUM_INPUT_LEVEL5[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL5    =  NUM_INPUT_LEVEL5>>1;
    localparam   NUM_ADDER_LEVEL5         =  (NUM_INPUT_LEVEL5==1)?0:DIV2_NUM_INPUT_LEVEL5 + IS_ODD_NUM_INPUT_LEVEL5;
    
    localparam   START_LEVEL6             =  START_LEVEL5 + NUM_INPUT_LEVEL5;
    localparam   NUM_INPUT_LEVEL6         =  NUM_ADDER_LEVEL5;
    localparam   IS_ODD_NUM_INPUT_LEVEL6  =  NUM_INPUT_LEVEL6[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL6    =  NUM_INPUT_LEVEL6>>1;
    localparam   NUM_ADDER_LEVEL6         =  (NUM_INPUT_LEVEL6==1)?0:DIV2_NUM_INPUT_LEVEL6 + IS_ODD_NUM_INPUT_LEVEL6;
    
    localparam   START_LEVEL7             =  START_LEVEL6 + NUM_INPUT_LEVEL6;
    localparam   NUM_INPUT_LEVEL7         =  NUM_ADDER_LEVEL6;
    localparam   IS_ODD_NUM_INPUT_LEVEL7  =  NUM_INPUT_LEVEL7[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL7    =  NUM_INPUT_LEVEL7>>1;
    localparam   NUM_ADDER_LEVEL7         =  (NUM_INPUT_LEVEL7==1)?0:DIV2_NUM_INPUT_LEVEL7 + IS_ODD_NUM_INPUT_LEVEL7;
    
    localparam   START_LEVEL8             =  START_LEVEL7 + NUM_INPUT_LEVEL7;
    localparam   NUM_INPUT_LEVEL8         =  NUM_ADDER_LEVEL7;
    localparam   IS_ODD_NUM_INPUT_LEVEL8  =  NUM_INPUT_LEVEL8[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL8    =  NUM_INPUT_LEVEL8>>1;
    localparam   NUM_ADDER_LEVEL8         =  (NUM_INPUT_LEVEL8==1)?0:DIV2_NUM_INPUT_LEVEL8 + IS_ODD_NUM_INPUT_LEVEL8;
    
    localparam   START_LEVEL9             =  START_LEVEL8 + NUM_INPUT_LEVEL8;
    localparam   NUM_INPUT_LEVEL9         =  NUM_ADDER_LEVEL8;
    localparam   IS_ODD_NUM_INPUT_LEVEL9  =  NUM_INPUT_LEVEL9[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL9    =  NUM_INPUT_LEVEL9>>1;
    localparam   NUM_ADDER_LEVEL9         =  (NUM_INPUT_LEVEL9==1)?0:DIV2_NUM_INPUT_LEVEL9 + IS_ODD_NUM_INPUT_LEVEL9;
    
    localparam   START_LEVEL10            =  START_LEVEL9 + NUM_INPUT_LEVEL9;
        
    wire [LEN-1:0] wire_inner[START_LEVEL10-1:0]; //多加的1是最终的输出。
    
    for (i = 0; i < NUM_INPUT_LEVEL0; i = i + 1) 
    begin
       assign wire_inner[i] = in_inner[i];
    end
    
    genvar index_num;
    generate
        
    for (index_num = 0; index_num < NUM_ADDER_LEVEL0-IS_ODD_NUM_INPUT_LEVEL0; index_num = index_num + 1)
    begin
        adder#(.LEN(LEN)) level0(.a(wire_inner[START_LEVEL0 + 2*index_num]),.b(wire_inner[START_LEVEL0+2*index_num+1]),.out(wire_inner[START_LEVEL1+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL0 && (NUM_ADDER_LEVEL0>0))
        adder#(.LEN(LEN)) level0_last(.a(wire_inner[START_LEVEL0+2*(NUM_ADDER_LEVEL0-1)]),.b(0),.out(wire_inner[START_LEVEL1+NUM_ADDER_LEVEL0 - 1]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL1-IS_ODD_NUM_INPUT_LEVEL1; index_num = index_num + 1)
    begin
        adder#(.LEN(LEN)) level1(.a(wire_inner[START_LEVEL1+2*index_num]),.b(wire_inner[START_LEVEL1+2*index_num+1]),.out(wire_inner[START_LEVEL2+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL1 && (NUM_ADDER_LEVEL1>0))
        adder#(.LEN(LEN)) level1_last(.a(wire_inner[START_LEVEL1+2*(NUM_ADDER_LEVEL1-1)]),.b(0),.out(wire_inner[START_LEVEL2+NUM_ADDER_LEVEL1 - 1]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL2-IS_ODD_NUM_INPUT_LEVEL2; index_num = index_num + 1)
    begin
        adder#(.LEN(LEN)) level2(.a(wire_inner[START_LEVEL2+2*index_num]),.b(wire_inner[START_LEVEL2+2*index_num+1]),.out(wire_inner[START_LEVEL3+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL2 && (NUM_ADDER_LEVEL2>0))
        adder#(.LEN(LEN)) level2_last(.a(wire_inner[START_LEVEL2+2*(NUM_ADDER_LEVEL2-1)]),.b(0),.out(wire_inner[START_LEVEL3+NUM_ADDER_LEVEL2 - 1]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL3-IS_ODD_NUM_INPUT_LEVEL3; index_num = index_num + 1)
    begin
        adder#(.LEN(LEN)) level3(.a(wire_inner[START_LEVEL3+2*index_num]),.b(wire_inner[START_LEVEL3+2*index_num+1]),.out(wire_inner[START_LEVEL4+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL3 && (NUM_ADDER_LEVEL3>0))
        adder#(.LEN(LEN)) level3_last(.a(wire_inner[START_LEVEL3+2*(NUM_ADDER_LEVEL3-1)]),.b(0),.out(wire_inner[START_LEVEL4+NUM_ADDER_LEVEL3 - 1]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL4-IS_ODD_NUM_INPUT_LEVEL4; index_num = index_num + 1)
    begin
        adder#(.LEN(LEN)) level4(.a(wire_inner[START_LEVEL4+2*index_num]),.b(wire_inner[START_LEVEL4+2*index_num+1]),.out(wire_inner[START_LEVEL5+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL4 && (NUM_ADDER_LEVEL4>0))
        adder#(.LEN(LEN)) level4_last(.a(wire_inner[START_LEVEL4+2*(NUM_ADDER_LEVEL4-1)]),.b(0),.out(wire_inner[START_LEVEL5+NUM_ADDER_LEVEL4 - 1]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL5-IS_ODD_NUM_INPUT_LEVEL5; index_num = index_num + 1)
    begin
        adder#(.LEN(LEN)) level5(.a(wire_inner[START_LEVEL5+2*index_num]),.b(wire_inner[START_LEVEL5+2*index_num+1]),.out(wire_inner[START_LEVEL6+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL5 && (NUM_ADDER_LEVEL5>0))
        adder#(.LEN(LEN)) level5_last(.a(wire_inner[START_LEVEL5+2*(NUM_ADDER_LEVEL5-1)]),.b(0),.out(wire_inner[START_LEVEL6+NUM_ADDER_LEVEL5 - 1]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL6-IS_ODD_NUM_INPUT_LEVEL6; index_num = index_num + 1)
    begin
        adder#(.LEN(LEN)) level6(.a(wire_inner[START_LEVEL6+2*index_num]),.b(wire_inner[START_LEVEL6+2*index_num+1]),.out(wire_inner[START_LEVEL7+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL6 && (NUM_ADDER_LEVEL6>0))
        adder#(.LEN(LEN)) level6_last(.a(wire_inner[START_LEVEL6+2*(NUM_ADDER_LEVEL6-1)]),.b(0),.out(wire_inner[START_LEVEL7+NUM_ADDER_LEVEL6 - 1]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL7-IS_ODD_NUM_INPUT_LEVEL7; index_num = index_num + 1)
    begin
        adder#(.LEN(LEN)) level7(.a(wire_inner[START_LEVEL7+2*index_num]),.b(wire_inner[START_LEVEL7+2*index_num+1]),.out(wire_inner[START_LEVEL8+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL7 && (NUM_ADDER_LEVEL7>0))
        adder#(.LEN(LEN)) level7_last(.a(wire_inner[START_LEVEL7+2*(NUM_ADDER_LEVEL7-1)]),.b(0),.out(wire_inner[START_LEVEL8+NUM_ADDER_LEVEL7 - 1]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL8-IS_ODD_NUM_INPUT_LEVEL8; index_num = index_num + 1)
    begin
        adder#(.LEN(LEN)) level8(.a(wire_inner[START_LEVEL8+2*index_num]),.b(wire_inner[START_LEVEL8+2*index_num+1]),.out(wire_inner[START_LEVEL9+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL8 && (NUM_ADDER_LEVEL8>0))
        adder#(.LEN(LEN)) level8_last(.a(wire_inner[START_LEVEL8+2*(NUM_ADDER_LEVEL8-1)]),.b(0),.out(wire_inner[START_LEVEL9+NUM_ADDER_LEVEL8 - 1]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL9-IS_ODD_NUM_INPUT_LEVEL9; index_num = index_num + 1)
    begin
        adder#(.LEN(LEN)) level9(.a(wire_inner[START_LEVEL9+2*index_num]),.b(wire_inner[START_LEVEL9+2*index_num+1]),.out(wire_inner[START_LEVEL10+index_num]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL9 && (NUM_ADDER_LEVEL9>0))
        adder#(.LEN(LEN)) level9_last(.a(wire_inner[START_LEVEL9+2*(NUM_ADDER_LEVEL9-1)]),.b(0),.out(wire_inner[START_LEVEL10+NUM_ADDER_LEVEL9 - 1]));
    endgenerate
    
    always@*
    begin
        sum_inner = wire_inner[START_LEVEL10-1];
    end
    
    assign sum = sum_inner;
endmodule



////////////////////////////// adder tree
module adder_tree_var_bit#(parameter NUM = 300, LEN = 16, LEVEL = $clog2(NUM) ) (
    in,sum
    );
    
    // 将输入长向量还原为矩阵形式。高位是右侧的C&S
    input [NUM*LEN-1:0] in;
    wire  [LEN-1:0] in_inner[NUM-1:0]; 
    
    genvar i;
    for ( i = 0; i < NUM ; i = i + 1)
    begin
        assign in_inner[i]=in[(i+1)*LEN-1:i*LEN];
    end
    
    // 计算各个级别的比较器数目，将各个级别的首地址记录在一个单独的变量之中
    // START_LEVELx             记录了LEVELx输入线的起始bit index。 
    // NUM_INPUT_LEVELx         记录LEVELx的输入线数目。
    // IS_ODD_NUM_INPUT_LEVELx  记录了LEVELx的输入线总数是否为奇数？
    // DIV2_NUM_LEVELx          为LEVELx输入线数目直接右移1位。（中间变量）
    // NUM_ADDER_LEVELx         记录了LEVELx的ADDER个数,同时也是LEVEL(x+1)的输入线数目。
    
    localparam   START_LEVEL0             =  0;
    localparam   NUM_INPUT_LEVEL0         =  NUM;
    localparam   IS_ODD_NUM_INPUT_LEVEL0  =  NUM_INPUT_LEVEL0[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL0    =  NUM_INPUT_LEVEL0>>1;
    localparam   NUM_ADDER_LEVEL0         =  (NUM_INPUT_LEVEL0==1)?0:DIV2_NUM_INPUT_LEVEL0 + IS_ODD_NUM_INPUT_LEVEL0;
    
    localparam   START_LEVEL1             =  START_LEVEL0 + NUM_INPUT_LEVEL0*LEN;
    localparam   NUM_INPUT_LEVEL1         =  NUM_ADDER_LEVEL0;
    localparam   IS_ODD_NUM_INPUT_LEVEL1  =  NUM_INPUT_LEVEL1[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL1    =  NUM_INPUT_LEVEL1>>1;
    localparam   NUM_ADDER_LEVEL1         =  (NUM_INPUT_LEVEL1==1)?0:DIV2_NUM_INPUT_LEVEL1 + IS_ODD_NUM_INPUT_LEVEL1;
    
    localparam   START_LEVEL2             =  START_LEVEL1 + NUM_INPUT_LEVEL1*(LEN+1);
    localparam   NUM_INPUT_LEVEL2         =  NUM_ADDER_LEVEL1;
    localparam   IS_ODD_NUM_INPUT_LEVEL2  =  NUM_INPUT_LEVEL2[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL2    =  NUM_INPUT_LEVEL2>>1;
    localparam   NUM_ADDER_LEVEL2         =  (NUM_INPUT_LEVEL2==1)?0:DIV2_NUM_INPUT_LEVEL2 + IS_ODD_NUM_INPUT_LEVEL2;
    
    localparam   START_LEVEL3             =  START_LEVEL2 + NUM_INPUT_LEVEL2*(LEN+2);
    localparam   NUM_INPUT_LEVEL3         =  NUM_ADDER_LEVEL2;
    localparam   IS_ODD_NUM_INPUT_LEVEL3  =  NUM_INPUT_LEVEL3[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL3    =  NUM_INPUT_LEVEL3>>1;
    localparam   NUM_ADDER_LEVEL3         =  (NUM_INPUT_LEVEL3==1)?0:DIV2_NUM_INPUT_LEVEL3 + IS_ODD_NUM_INPUT_LEVEL3;
    
    localparam   START_LEVEL4             =  START_LEVEL3 + NUM_INPUT_LEVEL3*(LEN+3);
    localparam   NUM_INPUT_LEVEL4         =  NUM_ADDER_LEVEL3;
    localparam   IS_ODD_NUM_INPUT_LEVEL4  =  NUM_INPUT_LEVEL4[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL4    =  NUM_INPUT_LEVEL4>>1;
    localparam   NUM_ADDER_LEVEL4         =  (NUM_INPUT_LEVEL4==1)?0:DIV2_NUM_INPUT_LEVEL4 + IS_ODD_NUM_INPUT_LEVEL4;
    
    localparam   START_LEVEL5             =  START_LEVEL4 + NUM_INPUT_LEVEL4*(LEN+4);
    localparam   NUM_INPUT_LEVEL5         =  NUM_ADDER_LEVEL4;
    localparam   IS_ODD_NUM_INPUT_LEVEL5  =  NUM_INPUT_LEVEL5[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL5    =  NUM_INPUT_LEVEL5>>1;
    localparam   NUM_ADDER_LEVEL5         =  (NUM_INPUT_LEVEL5==1)?0:DIV2_NUM_INPUT_LEVEL5 + IS_ODD_NUM_INPUT_LEVEL5;
    
    localparam   START_LEVEL6             =  START_LEVEL5 + NUM_INPUT_LEVEL5*(LEN+5);
    localparam   NUM_INPUT_LEVEL6         =  NUM_ADDER_LEVEL5;
    localparam   IS_ODD_NUM_INPUT_LEVEL6  =  NUM_INPUT_LEVEL6[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL6    =  NUM_INPUT_LEVEL6>>1;
    localparam   NUM_ADDER_LEVEL6         =  (NUM_INPUT_LEVEL6==1)?0:DIV2_NUM_INPUT_LEVEL6 + IS_ODD_NUM_INPUT_LEVEL6;
    
    localparam   START_LEVEL7             =  START_LEVEL6 + NUM_INPUT_LEVEL6*(LEN+6);
    localparam   NUM_INPUT_LEVEL7         =  NUM_ADDER_LEVEL6;
    localparam   IS_ODD_NUM_INPUT_LEVEL7  =  NUM_INPUT_LEVEL7[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL7    =  NUM_INPUT_LEVEL7>>1;
    localparam   NUM_ADDER_LEVEL7         =  (NUM_INPUT_LEVEL7==1)?0:DIV2_NUM_INPUT_LEVEL7 + IS_ODD_NUM_INPUT_LEVEL7;
    
    localparam   START_LEVEL8             =  START_LEVEL7 + NUM_INPUT_LEVEL7*(LEN+7);
    localparam   NUM_INPUT_LEVEL8         =  NUM_ADDER_LEVEL7;
    localparam   IS_ODD_NUM_INPUT_LEVEL8  =  NUM_INPUT_LEVEL8[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL8    =  NUM_INPUT_LEVEL8>>1;
    localparam   NUM_ADDER_LEVEL8         =  (NUM_INPUT_LEVEL8==1)?0:DIV2_NUM_INPUT_LEVEL8 + IS_ODD_NUM_INPUT_LEVEL8;
    
    localparam   START_LEVEL9             =  START_LEVEL8 + NUM_INPUT_LEVEL8*(LEN+8);
    localparam   NUM_INPUT_LEVEL9         =  NUM_ADDER_LEVEL8;
    localparam   IS_ODD_NUM_INPUT_LEVEL9  =  NUM_INPUT_LEVEL9[0]==1?1:0;
    localparam   DIV2_NUM_INPUT_LEVEL9    =  NUM_INPUT_LEVEL9>>1;
    localparam   NUM_ADDER_LEVEL9         =  (NUM_INPUT_LEVEL9==1)?0:DIV2_NUM_INPUT_LEVEL9 + IS_ODD_NUM_INPUT_LEVEL9;
    
    localparam   LEN_LAST_LEVEL           =  LEN+LEVEL;      
    
    localparam   START_LEVEL10            =  START_LEVEL9  + NUM_INPUT_LEVEL9*(LEN+9);
    localparam   TOTAL_BIT                =  START_LEVEL10 + LEN_LAST_LEVEL;
    
    output [LEN_LAST_LEVEL-1:0] sum;
    reg    [LEN_LAST_LEVEL-1:0] sum_inner;
    wire   [START_LEVEL10-1:0 ] wire_inner; //多加的1是最终的输出。
    
    for (i = 0; i < NUM_INPUT_LEVEL0; i = i + 1) 
    begin
       assign wire_inner[(i+1)*LEN-1:i*LEN] = in_inner[i];
    end
    
    genvar index_num;
    generate
        
    for (index_num = 0; index_num < NUM_ADDER_LEVEL0-IS_ODD_NUM_INPUT_LEVEL0; index_num = index_num + 1)
    begin
        adder_var#(.LEN(LEN)) level0(.a(wire_inner[START_LEVEL0+(2*index_num+1)*LEN-1:START_LEVEL0+(2*index_num)*LEN]),.b(wire_inner[START_LEVEL0+(2*index_num+2)*LEN-1:START_LEVEL0+(2*index_num+1)*LEN]),.out(wire_inner[START_LEVEL1+(index_num+1)*(LEN+1)-1:START_LEVEL1+index_num*(LEN+1)]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL0 && (NUM_ADDER_LEVEL0>0))
        adder_var#(.LEN(LEN)) level0_last(.a(wire_inner[START_LEVEL0+(2*(NUM_ADDER_LEVEL0-1)+1)*LEN-1:START_LEVEL0+(2*(NUM_ADDER_LEVEL0-1))*LEN]),.b({LEN{1'b0}}),.out(wire_inner[START_LEVEL1+((NUM_ADDER_LEVEL0 - 1)+1)*(LEN+1)-1:START_LEVEL1+((NUM_ADDER_LEVEL0-1))*(LEN+1)]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL1-IS_ODD_NUM_INPUT_LEVEL1; index_num = index_num + 1)
    begin
        adder_var#(.LEN(LEN+1)) level1(.a(wire_inner[START_LEVEL1+(2*index_num+1)*(LEN+1)-1:START_LEVEL1+(2*index_num)*(LEN+1)]),.b(wire_inner[START_LEVEL1+(2*index_num+2)*(LEN+1)-1:START_LEVEL1+(2*index_num+1)*(LEN+1)]),.out(wire_inner[START_LEVEL2+(index_num+1)*(LEN+2)-1:START_LEVEL2+index_num*(LEN+2)]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL1 && (NUM_ADDER_LEVEL1>0))
        adder_var#(.LEN(LEN+1)) level1_last(.a(wire_inner[START_LEVEL1+(2*(NUM_ADDER_LEVEL1-1)+1)*(LEN+1)-1:START_LEVEL1+(2*(NUM_ADDER_LEVEL1-1))*(LEN+1)]),.b(0),.out(wire_inner[START_LEVEL2+((NUM_ADDER_LEVEL1 - 1)+1)*(LEN+2)-1:START_LEVEL2+((NUM_ADDER_LEVEL1-1))*(LEN+2)]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL2-IS_ODD_NUM_INPUT_LEVEL2; index_num = index_num + 1)
    begin
        adder_var#(.LEN(LEN+2)) level2(.a(wire_inner[START_LEVEL2+(2*index_num+1)*(LEN+2)-1:START_LEVEL2+(2*index_num)*(LEN+2)]),.b(wire_inner[START_LEVEL2+(2*index_num+2)*(LEN+2)-1:START_LEVEL2+(2*index_num+1)*(LEN+2)]),.out(wire_inner[START_LEVEL3+(index_num+1)*(LEN+3)-1:START_LEVEL3+index_num*(LEN+3)]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL2 && (NUM_ADDER_LEVEL2>0))
        adder_var#(.LEN(LEN+2)) level2_last(.a(wire_inner[START_LEVEL2+(2*(NUM_ADDER_LEVEL2-1)+1)*(LEN+2)-1:START_LEVEL2+(2*(NUM_ADDER_LEVEL2-1))*(LEN+2)]),.b(0),.out(wire_inner[START_LEVEL3+((NUM_ADDER_LEVEL2 - 1)+1)*(LEN+3)-1:START_LEVEL3+((NUM_ADDER_LEVEL2-1))*(LEN+3)]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL3-IS_ODD_NUM_INPUT_LEVEL3; index_num = index_num + 1)
    begin
        adder_var#(.LEN(LEN+3)) level3(.a(wire_inner[START_LEVEL3+(2*index_num+1)*(LEN+3)-1:START_LEVEL3+(2*index_num)*(LEN+3)]),.b(wire_inner[START_LEVEL3+(2*index_num+2)*(LEN+3)-1:START_LEVEL3+(2*index_num+1)*(LEN+3)]),.out(wire_inner[START_LEVEL4+(index_num+1)*(LEN+4)-1:START_LEVEL4+index_num*(LEN+4)]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL3 && (NUM_ADDER_LEVEL3>0))
        adder_var#(.LEN(LEN+3)) level3_last(.a(wire_inner[START_LEVEL3+(2*(NUM_ADDER_LEVEL3-1)+1)*(LEN+3)-1:START_LEVEL3+(2*(NUM_ADDER_LEVEL3-1))*(LEN+3)]),.b(0),.out(wire_inner[START_LEVEL4+((NUM_ADDER_LEVEL3 - 1)+1)*(LEN+4)-1:START_LEVEL4+((NUM_ADDER_LEVEL3-1))*(LEN+4)]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL4-IS_ODD_NUM_INPUT_LEVEL4; index_num = index_num + 1)
    begin
        adder_var#(.LEN(LEN+4)) level4(.a(wire_inner[START_LEVEL4+(2*index_num+1)*(LEN+4)-1:START_LEVEL4+(2*index_num)*(LEN+4)]),.b(wire_inner[START_LEVEL4+(2*index_num+2)*(LEN+4)-1:START_LEVEL4+(2*index_num+1)*(LEN+4)]),.out(wire_inner[START_LEVEL5+(index_num+1)*(LEN+5)-1:START_LEVEL5+index_num*(LEN+5)]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL4 && (NUM_ADDER_LEVEL4>0))
        adder_var#(.LEN(LEN+4)) level4_last(.a(wire_inner[START_LEVEL4+(2*(NUM_ADDER_LEVEL4-1)+1)*(LEN+4)-1:START_LEVEL4+(2*(NUM_ADDER_LEVEL4-1))*(LEN+4)]),.b(0),.out(wire_inner[START_LEVEL5+((NUM_ADDER_LEVEL4 - 1)+1)*(LEN+5)-1:START_LEVEL5+((NUM_ADDER_LEVEL4-1))*(LEN+5)]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL5-IS_ODD_NUM_INPUT_LEVEL5; index_num = index_num + 1)
    begin
        adder_var#(.LEN(LEN+5)) level5(.a(wire_inner[START_LEVEL5+(2*index_num+1)*(LEN+5)-1:START_LEVEL5+(2*index_num)*(LEN+5)]),.b(wire_inner[START_LEVEL5+(2*index_num+2)*(LEN+5)-1:START_LEVEL5+(2*index_num+1)*(LEN+5)]),.out(wire_inner[START_LEVEL6+(index_num+1)*(LEN+6)-1:START_LEVEL6+index_num*(LEN+6)]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL5 && (NUM_ADDER_LEVEL5>0))
        adder_var#(.LEN(LEN+5)) level5_last(.a(wire_inner[START_LEVEL5+(2*(NUM_ADDER_LEVEL5-1)+1)*(LEN+5)-1:START_LEVEL5+(2*(NUM_ADDER_LEVEL5-1))*(LEN+5)]),.b(0),.out(wire_inner[START_LEVEL6+((NUM_ADDER_LEVEL5 - 1)+1)*(LEN+6)-1:START_LEVEL6+((NUM_ADDER_LEVEL5-1))*(LEN+6)]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL6-IS_ODD_NUM_INPUT_LEVEL6; index_num = index_num + 1)
    begin
        adder_var#(.LEN(LEN+6)) level6(.a(wire_inner[START_LEVEL6+(2*index_num+1)*(LEN+6)-1:START_LEVEL6+(2*index_num)*(LEN+6)]),.b(wire_inner[START_LEVEL6+(2*index_num+2)*(LEN+6)-1:START_LEVEL6+(2*index_num+1)*(LEN+6)]),.out(wire_inner[START_LEVEL7+(index_num+1)*(LEN+7)-1:START_LEVEL7+index_num*(LEN+7)]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL6 && (NUM_ADDER_LEVEL6>0))
        adder_var#(.LEN(LEN+6)) level6_last(.a(wire_inner[START_LEVEL6+(2*(NUM_ADDER_LEVEL6-1)+1)*(LEN+6)-1:START_LEVEL6+(2*(NUM_ADDER_LEVEL6-1))*(LEN+6)]),.b(0),.out(wire_inner[START_LEVEL7+((NUM_ADDER_LEVEL6 - 1)+1)*(LEN+7)-1:START_LEVEL7+((NUM_ADDER_LEVEL6-1))*(LEN+7)]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL7-IS_ODD_NUM_INPUT_LEVEL7; index_num = index_num + 1)
    begin
        adder_var#(.LEN(LEN+7)) level7(.a(wire_inner[START_LEVEL7+(2*index_num+1)*(LEN+7)-1:START_LEVEL7+(2*index_num)*(LEN+7)]),.b(wire_inner[START_LEVEL7+(2*index_num+2)*(LEN+7)-1:START_LEVEL7+(2*index_num+1)*(LEN+7)]),.out(wire_inner[START_LEVEL8+(index_num+1)*(LEN+8)-1:START_LEVEL8+index_num*(LEN+8)]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL7 && (NUM_ADDER_LEVEL7>0))
        adder_var#(.LEN(LEN+7)) level7_last(.a(wire_inner[START_LEVEL7+(2*(NUM_ADDER_LEVEL7-1)+1)*(LEN+7)-1:START_LEVEL7+(2*(NUM_ADDER_LEVEL7-1))*(LEN+7)]),.b(0),.out(wire_inner[START_LEVEL8+((NUM_ADDER_LEVEL7 - 1)+1)*(LEN+8)-1:START_LEVEL8+((NUM_ADDER_LEVEL7-1))*(LEN+8)]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL8-IS_ODD_NUM_INPUT_LEVEL8; index_num = index_num + 1)
    begin
        adder_var#(.LEN(LEN+8)) level8(.a(wire_inner[START_LEVEL8+(2*index_num+1)*(LEN+8)-1:START_LEVEL8+(2*index_num)*(LEN+8)]),.b(wire_inner[START_LEVEL8+(2*index_num+2)*(LEN+8)-1:START_LEVEL8+(2*index_num+1)*(LEN+8)]),.out(wire_inner[START_LEVEL9+(index_num+1)*(LEN+9)-1:START_LEVEL9+index_num*(LEN+9)]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL8 && (NUM_ADDER_LEVEL8>0))
        adder_var#(.LEN(LEN+8)) level8_last(.a(wire_inner[START_LEVEL8+(2*(NUM_ADDER_LEVEL8-1)+1)*(LEN+8)-1:START_LEVEL8+(2*(NUM_ADDER_LEVEL8-1))*(LEN+8)]),.b(0),.out(wire_inner[START_LEVEL9+((NUM_ADDER_LEVEL8 - 1)+1)*(LEN+9)-1:START_LEVEL9+((NUM_ADDER_LEVEL8-1))*(LEN+9)]));
    
    for (index_num = 0; index_num < NUM_ADDER_LEVEL9-IS_ODD_NUM_INPUT_LEVEL9; index_num = index_num + 1)
    begin
        adder_var#(.LEN(LEN+9)) level9(.a(wire_inner[START_LEVEL9+(2*index_num+1)*(LEN+9)-1:START_LEVEL9+(2*index_num)*(LEN+9)]),.b(wire_inner[START_LEVEL9+(2*index_num+2)*(LEN+9)-1:START_LEVEL9+(2*index_num+1)*(LEN+9)]),.out(wire_inner[START_LEVEL10+(index_num+1)*(LEN+10)-1:START_LEVEL10+index_num*(LEN+10)]));
    end
    if(IS_ODD_NUM_INPUT_LEVEL9 && (NUM_ADDER_LEVEL9>0))
        adder_var#(.LEN(LEN+9)) level9_last(.a(wire_inner[START_LEVEL9+(2*(NUM_ADDER_LEVEL9-1)+1)*(LEN+9)-1:START_LEVEL9+(2*(NUM_ADDER_LEVEL9-1))*(LEN+9)]),.b(0),.out(wire_inner[START_LEVEL10+((NUM_ADDER_LEVEL9 - 1)+1)*(LEN+10)-1:START_LEVEL10+((NUM_ADDER_LEVEL9-1))*(LEN+10)]));
    
    endgenerate
    
    always@*
    begin
        sum_inner = wire_inner[START_LEVEL10-1:START_LEVEL10-LEN_LAST_LEVEL];
    end
    
    assign sum = sum_inner;
    
endmodule


module adder#(parameter LEN = 16)(
    input  [LEN-1:0] a,
    input  [LEN-1:0] b,
    output [LEN-1:0]  out
    );
    reg  [LEN-1:0]  out_inner;
    always@*
    begin
        out_inner = a + b;
    end
    
    assign out = out_inner;
endmodule

module adder_var#(parameter LEN = 16)(
    input  [LEN-1:0] a,
    input  [LEN-1:0] b,
    output [LEN:0]  out
    );
    
    reg  [LEN:0]  out_inner;
    
    always@*
    begin
        out_inner = a + b;
    end
    
    assign out = out_inner;
    
endmodule
