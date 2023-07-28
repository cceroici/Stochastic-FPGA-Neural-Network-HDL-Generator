
// Chris Ceroici
// Perform stochastic arithmetic for N inputs
// requires SIGN input for each input argument

module SS_ADDSUB_CARRY(CLK,INIT,IN,SIGN,R_condition,OUT,SIGN_out);

parameter N = 6;
//parameter DIFFCOUNTER_SIZE = 5;
//parameter DIFFCOUNT_LOWERLIM = 4;
parameter DIFFCOUNTER_SIZE = 1;
parameter DIFFCOUNT_LOWERLIM = 0;

input CLK,INIT;
input [N-1:0] IN,SIGN;
input R_condition;

output reg OUT,SIGN_out;

wire INPUT_SUM_pos, INPUT_SUM_neg;
wire INPUT_CARRY0_pos, INPUT_CARRY0_neg;
wire INPUT_CARRY1_pos, INPUT_CARRY1_neg;
wire OUTPUT_pos, OUTPUT_neg, OUT_temp;

reg [DIFFCOUNTER_SIZE-1:0] DIFFCOUNT = 1'd0;
reg DIFFCOUNT_SIGN = 1'd0;

reg [DIFFCOUNTER_SIZE-1:0] DIFFCOUNT_LIMIT = 1'd0-1'd1;

wire [N-1:0] N_INPUTS_pos;
wire [N-1:0] N_INPUTS_neg;

M_SIDEADD #(.N_inputs(N), .NB_out(N)) SIDEADDpos(.EN(1'b1),.IN(IN&(~SIGN)),.OUT(N_INPUTS_pos));
M_SIDEADD #(.N_inputs(N), .NB_out(N)) SIDEADDneg(.EN(1'b1),.IN(IN&(SIGN)),.OUT(N_INPUTS_neg));

wire EQUAL, INPUT_SIGN;

assign EQUAL = (N_INPUTS_pos==N_INPUTS_neg);

assign INPUT_SIGN = (R_condition) ?  (N_INPUTS_neg>=N_INPUTS_pos) : (N_INPUTS_neg>N_INPUTS_pos);

always @(posedge CLK or posedge INIT) begin

	if (INIT) begin
		DIFFCOUNT_SIGN = 1'b0; 
		//DIFFCOUNT_SIGN = DIFFCOUNT_SIGN;
		DIFFCOUNT <= 1'b0;
	end else if (EQUAL) DIFFCOUNT <= DIFFCOUNT;
	else if ((!INPUT_SIGN)&(DIFFCOUNT_SIGN)&(DIFFCOUNT<N_INPUTS_pos)) begin // +ve input, currently -ve, cross: -ve -> +ve
		DIFFCOUNT_SIGN = 1'b0;
		DIFFCOUNT <= N_INPUTS_pos; 
	// NOTE: If DIFFCOUNT = 1, (DIFFCOUNT<(DIFFCOUNT_LIMIT-N_INPUTS_pos) will never be satisfied when only a single input is 1. 
	end else if ((!INPUT_SIGN)&(DIFFCOUNT<DIFFCOUNT_LIMIT)&(!DIFFCOUNT_SIGN)) begin // +ve input, currently +ve, stay as positive and increment
			DIFFCOUNT_SIGN <= DIFFCOUNT_SIGN;
			DIFFCOUNT <= DIFFCOUNT + 1'b1;
	end else if ((!INPUT_SIGN)&(!DIFFCOUNT_SIGN)) begin // +ve input, currently +ve, stay as positive and increment
			DIFFCOUNT_SIGN <= DIFFCOUNT_SIGN;
			DIFFCOUNT <= DIFFCOUNT_LIMIT;
	end else if ((!INPUT_SIGN)&(DIFFCOUNT_SIGN)) begin // +ve input, currently -ve, stay -ve and decrement
			DIFFCOUNT_SIGN <= DIFFCOUNT_SIGN;
			DIFFCOUNT <= DIFFCOUNT - N_INPUTS_pos; 
	end else if ((INPUT_SIGN)&(!DIFFCOUNT_SIGN)&(DIFFCOUNT<N_INPUTS_neg)) begin // -ve input, currently +ve, cross: +ve -> -ve
			DIFFCOUNT_SIGN = 1'b1;
			DIFFCOUNT <= N_INPUTS_neg; 
	end else if ((INPUT_SIGN)&(DIFFCOUNT<DIFFCOUNT_LIMIT)&(DIFFCOUNT_SIGN)) begin // -ve input, currently -ve, decrement
			DIFFCOUNT_SIGN <= DIFFCOUNT_SIGN;
			DIFFCOUNT <= DIFFCOUNT + 1'b1; 
	end else if ((INPUT_SIGN)&(DIFFCOUNT_SIGN)) begin // -ve input, currently -ve, decrement
			DIFFCOUNT_SIGN <= DIFFCOUNT_SIGN;
			DIFFCOUNT <= DIFFCOUNT_LIMIT; 
	end else if ((INPUT_SIGN)&(!DIFFCOUNT_SIGN)) begin // -ve input, currently +ve, stay +ve and decrement
			DIFFCOUNT_SIGN <= DIFFCOUNT_SIGN;
			DIFFCOUNT <= DIFFCOUNT - N_INPUTS_neg; 
	end else begin 
			DIFFCOUNT_SIGN <= DIFFCOUNT_SIGN;
			DIFFCOUNT <= DIFFCOUNT;
	end
end





//assign SIGN_out = DIFFCOUNT_SIGN;

M_OR_CARRY1 #(.N(N)) OR_pos(.EN(1'b1),.IN(IN&(~SIGN)),.OUT(INPUT_SUM_pos),.CARRY0(INPUT_CARRY0_pos),.CARRY1(INPUT_CARRY1_pos));
M_OR_CARRY1 #(.N(N)) OR_neg(.EN(1'b1),.IN(IN&(SIGN)),.OUT(INPUT_SUM_neg),.CARRY0(INPUT_CARRY0_neg),.CARRY1(INPUT_CARRY1_neg));

assign OUTPUT_pos = INPUT_SUM_pos&(~INPUT_SUM_neg);
assign OUTPUT_neg = INPUT_SUM_neg&(~INPUT_SUM_pos);

assign OUT_temp = (SIGN_out) ? (OUTPUT_neg) : (OUTPUT_pos);

//assign OUT  = (DIFFCOUNT > (DIFFCOUNT_LOWERLIM)) ? OUT_temp : 1'b0;

always @(posedge CLK or posedge INIT) begin
	if (INIT) begin
		OUT <= 1'b0;
		SIGN_out <= 1'b0;
	end	else if (INPUT_CARRY1_pos&INPUT_CARRY1_neg) begin
		OUT <= 1'b0;
		SIGN_out <= 1'b0;
	end else if (INPUT_CARRY1_pos&(~INPUT_CARRY1_neg)) begin
		OUT <= 1'b1;
		SIGN_out <= 1'b0;
	end else if ((~INPUT_CARRY1_pos)&INPUT_CARRY1_neg) begin
		OUT <= 1'b1;
		SIGN_out <= 1'b1;
	end else if (INPUT_CARRY0_pos&INPUT_CARRY0_neg) begin
		OUT <= 1'b0;
		SIGN_out <= 1'b0;
	end else if (INPUT_CARRY0_pos&(~INPUT_CARRY0_neg)) begin
		OUT <= 1'b1;
		SIGN_out <= 1'b0;
	end else if ((~INPUT_CARRY0_pos)&INPUT_CARRY0_neg) begin
		OUT <= 1'b1;
		SIGN_out <= 1'b1;
	end else if (DIFFCOUNT > (DIFFCOUNT_LOWERLIM)) begin
		OUT <= OUT_temp;
		SIGN_out <= DIFFCOUNT_SIGN;
	end else begin
		OUT <= 1'b0;
		SIGN_out <= SIGN_out;
	end
end

endmodule