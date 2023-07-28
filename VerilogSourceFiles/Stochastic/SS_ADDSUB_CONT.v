
// Chris Ceroici
// Perform stochastic arithmetic for N inputs
// requires SIGN input for each input argument

// Continuous buffer calculation

module SS_ADDSUB_CONT(CLK,INIT,IN,SIGN,OUT,SIGN_out);

parameter N = 6;
parameter DIFFCOUNTER_SIZE = 5;

input CLK,INIT;
input [N-1:0] IN,SIGN;

output OUT,SIGN_out;

wire INPUT_SUM_pos, INPUT_SUM_neg;
wire OUTPUT_pos, OUTPUT_neg, OUT;
wire COUNT_ACTIVE;

reg [DIFFCOUNTER_SIZE-1:0] DIFFCOUNT = 1'd0;
reg DIFFCOUNT_SIGN = 1'd0;

reg [DIFFCOUNTER_SIZE-1:0] DIFFCOUNT_LIMIT = 1'd0-1'd1;

wire [N-1:0] N_INPUTS_pos;
wire [N-1:0] N_INPUTS_neg;

wire [7:0] temp [2:0];

wire [N-1:0] INPUT_BUFFER_pos [DIFFCOUNTER_SIZE-1:0];
wire [N-1:0] INPUT_BUFFER_neg [DIFFCOUNTER_SIZE-1:0];
wire [N-1:0] BUFFERSUM_pos [7-1:0];
wire [N-1:0] BUFFERSUM_neg [7-1:0];

genvar n;
generate 
	for (n=0; n<(N-1);n=n+1) begin : BUFn
		BUFFER_REGISTER BRpos (.CLK(CLK),.INIT(INIT),.IN(IN[n]&(~SIGN[n])),.BUF(INPUT_BUFFER_pos[n][DIFFCOUNTER_SIZE-1:0]));
		BUFFER_REGISTER BRneg (.CLK(CLK),.INIT(INIT),.IN(IN[n]&(SIGN[n])),.BUF(INPUT_BUFFER_pos[n][DIFFCOUNTER_SIZE-1:0]));
		defparam BRpos.DIFFCOUNTER_SIZE = DIFFCOUNTER_SIZE;
		defparam BRneg.DIFFCOUNTER_SIZE = DIFFCOUNTER_SIZE;
		M_SIDEADD SIDEADDpos (.EN(1'b1),.IN(INPUT_BUFFER_pos[n][DIFFCOUNTER_SIZE-1:0]),.OUT(BUFFERSUM_pos[n][7:0]));
		M_SIDEADD SIDEADDneg (.EN(1'b1),.IN(INPUT_BUFFER_pos[n][DIFFCOUNTER_SIZE-1:0]),.OUT(BUFFERSUM_neg[n][7:0]));
		defparam SIDEADDpos.N_inputs = DIFFCOUNTER_SIZE;
		defparam SIDEADDpos.NB = 8;
		defparam SIDEADDneg.N_inputs = DIFFCOUNTER_SIZE;
		defparam SIDEADDneg.N_inputs = 8;
		
	end
endgenerate




endmodule