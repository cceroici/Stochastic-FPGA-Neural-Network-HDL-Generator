
// Chris Ceroici
// Perform stochastic arithmetic for N inputs
// requires SIGN input for each input argument
// Continuous-sum Implementation

module SS_ADDSUB2_CONT(CLK,INIT,IN,SIGN,OUT,SIGN_out);

parameter N = 2;
parameter DIFFCOUNTER_SIZE = 3;
parameter DIFFCOUNT_UPPERLIM = 1;
parameter DIFFCOUNT_LOWERLIM = 0;

input CLK,INIT;
input [2-1:0] IN,SIGN;

output SIGN_out;
output reg OUT = 1'b0;

wire [3-1:0] N_INPUTS_pos;
wire [3-1:0] N_INPUTS_neg;

wire [7:0] BUFFERSUM0_pos, BUFFERSUM1_pos;
wire [7:0] BUFFERSUM0_neg, BUFFERSUM1_neg;
wire [7:0] BUFFERSUM_tot_pos, BUFFERSUM_tot_neg;
wire [7:0] NET_SUM;

reg [DIFFCOUNTER_SIZE-1:0] INPUT_BUFFER0_pos = 1'b0;
reg [DIFFCOUNTER_SIZE-1:0] INPUT_BUFFER1_pos = 1'b0;
reg [DIFFCOUNTER_SIZE-1:0] INPUT_BUFFER0_neg = 1'b0;
reg [DIFFCOUNTER_SIZE-1:0] INPUT_BUFFER1_neg = 1'b0;



assign N_INPUTS_pos = {1'b0,IN[0]&(~SIGN[0])} + {1'b0,IN[1]&(~SIGN[1])};
assign N_INPUTS_neg = {1'b0,IN[0]&(SIGN[0])} + {1'b0,IN[1]&(SIGN[1])};

always @(posedge CLK or posedge  INIT) begin
	if (INIT) begin
		INPUT_BUFFER0_pos <= 1'b0;
		INPUT_BUFFER1_pos <= 1'b0;
		INPUT_BUFFER0_neg <= 1'b0;
		INPUT_BUFFER1_neg <= 1'b0;
	end else begin
		INPUT_BUFFER0_pos <= {INPUT_BUFFER0_pos[DIFFCOUNTER_SIZE-1:0], IN[0]&(~SIGN[0])};
		INPUT_BUFFER1_pos <= {INPUT_BUFFER1_pos[DIFFCOUNTER_SIZE-1:0], IN[1]&(~SIGN[1])};
		INPUT_BUFFER0_neg <= {INPUT_BUFFER0_neg[DIFFCOUNTER_SIZE-1:0], IN[0]&(SIGN[0])};
		INPUT_BUFFER1_neg <= {INPUT_BUFFER1_neg[DIFFCOUNTER_SIZE-1:0], IN[1]&(SIGN[1])};
	end
end

M_SIDEADD SIDEADD_Buffer0_pos(.EN(1'b1),.IN(INPUT_BUFFER0_pos),.OUT(BUFFERSUM0_pos));
M_SIDEADD SIDEADD_Buffer1_pos(.EN(1'b1),.IN(INPUT_BUFFER1_pos),.OUT(BUFFERSUM1_pos));
defparam SIDEADD_Buffer0_pos.N_inputs = DIFFCOUNTER_SIZE;
defparam SIDEADD_Buffer1_pos.N_inputs = DIFFCOUNTER_SIZE;

M_SIDEADD SIDEADD_Buffer0_neg(.EN(1'b1),.IN(INPUT_BUFFER0_neg),.OUT(BUFFERSUM0_neg));
M_SIDEADD SIDEADD_Buffer1_neg(.EN(1'b1),.IN(INPUT_BUFFER1_neg),.OUT(BUFFERSUM1_neg));
defparam SIDEADD_Buffer0_neg.N_inputs = DIFFCOUNTER_SIZE;
defparam SIDEADD_Buffer1_neg.N_inputs = DIFFCOUNTER_SIZE;

assign BUFFERSUM_tot_pos = BUFFERSUM0_pos + BUFFERSUM1_pos;
assign BUFFERSUM_tot_neg = BUFFERSUM0_neg + BUFFERSUM1_neg;


assign SIGN_out = (BUFFERSUM_tot_neg>BUFFERSUM_tot_pos);

assign NET_SUM = (SIGN_out) ? (BUFFERSUM_tot_neg - BUFFERSUM_tot_pos) :  (BUFFERSUM_tot_pos-BUFFERSUM_tot_neg);

always @(posedge CLK or posedge  INIT) begin
	if (INIT) begin
		OUT <= 1'b0;
	end	else OUT <= NET_SUM>DIFFCOUNT_LOWERLIM;
end

endmodule