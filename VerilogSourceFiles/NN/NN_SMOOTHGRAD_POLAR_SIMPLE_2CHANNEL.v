
// With dynamic resistance

module NN_SMOOTHGRAD_POLAR_SIMPLE_2CHANNEL(CLK, CLK_TRAINING_flag, INIT, regIndex, IN_SS, SIGN, REG0, REG1, SIGN_out0, SIGN_out1, RESISTANCE, TransitionChange_TRIG, OUT_INIT, SIGN_OUT_INIT, EN);

parameter N = 8; // decimal output size
parameter N_RESISTANCE = 9;

input CLK, CLK_TRAINING_flag, INIT, EN;
input [1:0] IN_SS, SIGN;
input [N-1:0] OUT_INIT;
input SIGN_OUT_INIT;
input [N_RESISTANCE-1:0] RESISTANCE;
input regIndex;
//input [N_RESISTANCE -1:0] RESISTANCE_MAX;

output  reg [N-1:0] REG0 = 0;
output  reg [N-1:0] REG1 = 0;
output reg SIGN_out0 = 1'b0;
output reg SIGN_out1 = 1'b0;

output wire TransitionChange_TRIG;

//reg [0:0] regIndex = 1'd0;

assign TransitionChange_TRIG = 1'b0;

wire [7:0] currentReg;
wire currentSign;
wire currentInput_SS;
wire currentInputSign;
wire [7:0] newVal;
wire newSign;
wire [7:0] newVal_sum;
wire [7:0] newVal_diff;

wire atZeroVal;
wire signMatch;

assign currentReg = (regIndex) ? REG1 : REG0;
assign currentSign = (regIndex) ? SIGN_out1 : SIGN_out0;
assign currentInput_SS = (regIndex) ? IN_SS[1] : IN_SS[0];
assign currentInputSign = (regIndex) ? SIGN[1] : SIGN[0];

assign atMaxVal = (currentReg == 8'd255);
assign atZeroVal = (currentReg == 8'd0);
assign signMatch = (currentInputSign == currentSign);

assign newVal_sum = (~atMaxVal) ? (currentReg + currentInput_SS) : currentReg;
assign newVal_diff = (~atZeroVal) ? (currentReg - currentInput_SS) : currentReg;
assign newVal = (signMatch) ? newVal_sum : newVal_diff;
assign newSign = (atZeroVal) ? (currentInputSign) : (currentSign);

//always @(posedge CLK) begin
//	regIndex <= regIndex + 1'b1;
//end

always @(posedge CLK or posedge INIT or posedge EN) begin
	if (INIT) begin
		REG0 <= OUT_INIT;
		REG1 <= OUT_INIT;
		SIGN_out0 <= SIGN_OUT_INIT;
		SIGN_out1 <= SIGN_OUT_INIT;
	end else if (~EN) begin
		REG0 <= REG0;
		REG1 <= REG1;
		SIGN_out0 <= SIGN_out0;
		SIGN_out1 <= SIGN_out1;
	end else if (regIndex == 1'd0) begin
		REG0 <= newVal;
		SIGN_out0 <= newSign;
	end else if (regIndex == 1'd1) begin
		REG1 <= newVal;
		SIGN_out1 <= newSign;
	end
end

endmodule