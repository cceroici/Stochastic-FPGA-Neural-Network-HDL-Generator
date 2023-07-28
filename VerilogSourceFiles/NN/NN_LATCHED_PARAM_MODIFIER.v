


module NN_LATCHED_PARAM_MODIFIER(INIT, INIT_late, CLK, EN, TRIG, OUT, SIGN_OUT, MODIFIER, SIGN_MODIFIER, RESISTANCE_OUT, RESISTANCE_NEW, INITIAL_RESISTANCE, INITIAL_VALUE, INITIAL_SIGN);

parameter N = 8;
parameter N_RESISTANCE = 8;

input INIT, INIT_late, CLK, EN, TRIG;
input [N-1:0] MODIFIER;
input SIGN_MODIFIER;
input [N_RESISTANCE-1:0] RESISTANCE_NEW;
input [N-1:0] INITIAL_VALUE;
input INITIAL_SIGN;
input [N_RESISTANCE-1:0] INITIAL_RESISTANCE;

output reg [N-1:0] OUT;
output reg SIGN_OUT;
output reg [N_RESISTANCE-1:0] RESISTANCE_OUT;

//wire SIGNS_EQUAL;
//wire OVERFLOW_HIGH, OVERFLOW_LOW, SIGN_SWITCH;
//wire [N-1:0] maxVal;

//assign maxVal = 1'b0 - 1'b1;

//assign SIGNS_EQUAL = (SIGN_MODIFIER == SIGN_OUT);
//assign OVERFLOW_HIGH = (~SIGN_MODIFIER)&(~SIGN_OUT)&(MODIFIER>(maxVal-OUT));
//assign OVERFLOW_LOW = (SIGN_MODIFIER)&(SIGN_OUT)&(MODIFIER>(maxVal-OUT));
//assign SIGN_SWITCH = (~SIGNS_EQUAL) & (MODIFIER > OUT);


always @(posedge TRIG or posedge INIT_late or posedge INIT) begin
	if (INIT_late|INIT) begin
		OUT <= INITIAL_VALUE;
		SIGN_OUT <= INITIAL_SIGN;
		RESISTANCE_OUT <= INITIAL_RESISTANCE;
	end else if (!EN) begin
		OUT <= OUT;
		SIGN_OUT <= SIGN_OUT;
		RESISTANCE_OUT <= RESISTANCE_OUT;
	end else if (TRIG) begin
		OUT <= MODIFIER;
		SIGN_OUT <= SIGN_MODIFIER;
		RESISTANCE_OUT <= RESISTANCE_NEW;
	end
end

/*
always @(posedge TRIG or posedge INIT) begin
	if (INIT) begin
		OUT <= INITIAL_VALUE;
		SIGN_OUT <= INITIAL_SIGN;
	end else if (TRIG&(OVERFLOW_HIGH|OVERFLOW_LOW)) begin
		OUT <= maxVal;
		SIGN_OUT <= SIGN_OUT;
	end	else if (TRIG&SIGN_SWITCH) begin
		OUT <= maxVal- OUT;
		SIGN_OUT <= ~SIGN_OUT;
	end else if (SIGNS_EQUAL) begin
		OUT <= OUT + MODIFIER;
		SIGN_OUT <= ~SIGN_OUT;
	end else begin
		OUT <= OUT - MODIFIER;
		SIGN_OUT <= ~SIGN_OUT;
	end
end
*/
endmodule