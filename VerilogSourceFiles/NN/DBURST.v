// Chris Ceroici

// Burst Gate Derivative Operator

module DBURST(IN,OUT,CLK,INIT);

parameter MEMSIZE = 6;
//parameter MEMSIZE_TARGET = 3;

input IN;
input CLK;
input INIT;

output reg OUT;

//reg [MEMSIZE-1:0] MEM = 1'b0;
//wire [MEMSIZE-1:0] MEMweight;

reg [3-1:0] MEMCOUNTER = 1'b1;

//M_SIDEADD MEMADD0(.EN(1'b1),.IN(MEM),.OUT(MEMweight));
//defparam MEMADD0.N = MEMSIZE;

wire MEMCount_hiCond;
wire MEMCount_loCond;
reg [5:0] HiLevel = 6'd32;
reg [5:0] LoLevel = 6'd0;
assign MEMCount_hiCond  = (MEMCOUNTER<HiLevel);
assign MEMCount_loCond  = (MEMCOUNTER>LoLevel);


always @(posedge CLK) begin

	if (INIT) MEMCOUNTER <= 1'b1;
	else if (!IN&(MEMCount_loCond)) MEMCOUNTER <= MEMCOUNTER - 1'b1;
	else if (IN&(MEMCount_hiCond)) MEMCOUNTER <= MEMCOUNTER + 1'b1;
	else MEMCOUNTER <= MEMCOUNTER;
	
	if (MEMCOUNTER == 3'd4) OUT <= 1'b1;
	else OUT <= 1'b0;
	//MEM <= {MEM[MEMSIZE-2:0],IN};
end






endmodule