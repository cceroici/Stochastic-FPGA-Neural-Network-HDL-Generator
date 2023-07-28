
// With dynamic resistance

module NN_SMOOTHGRAD_POLAR_SIMPLE_4CHANNEL(CLK, CLK_TRAINING_flag, INIT,
	IN_SS, SIGN, regIndex,
	REG0, REG1, REG2, REG3, SIGN_out0, SIGN_out1, SIGN_out2, SIGN_out3,
	RESISTANCE,
	TransitionChange_TRIG, OUT_INIT, SIGN_OUT_INIT, EN
);

parameter N = 8; // decimal output size
parameter N_RESISTANCE = 9;

input CLK, CLK_TRAINING_flag, INIT, EN;
input [3:0] IN_SS, SIGN;
input [N-1:0] OUT_INIT;
input SIGN_OUT_INIT;
input [N_RESISTANCE-1:0] RESISTANCE;
input [1:0] regIndex;

//input [N_RESISTANCE -1:0] RESISTANCE_MAX;

output  reg [N-1:0] REG0 = 0;
output  reg [N-1:0] REG1 = 0;
output  reg [N-1:0] REG2 = 0;
output  reg [N-1:0] REG3 = 0;
output reg SIGN_out0 = 1'b0;
output reg SIGN_out1 = 1'b0;
output reg SIGN_out2 = 1'b0;
output reg SIGN_out3 = 1'b0;

output wire TransitionChange_TRIG;


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

/*
always @(posedge CLK) begin
	if (regIndex == 4'd0) begin
		currentReg <= REG0;
		currentSign <= SIGN_out0;
		currentInput_SS <= IN_SS[0];
		currentInputSign <= SIGN[0];
	end else if (regIndex == 4'd1) begin
		currentReg <= REG1;
		currentSign <= SIGN_out1;
		currentInput_SS <= IN_SS[1];
		currentInputSign <= SIGN[1];
	end else if (regIndex == 4'd2) begin
		currentReg <= REG2;
		currentSign <= SIGN_out2;
		currentInput_SS <= IN_SS[2];
		currentInputSign <= SIGN[2];
	end else begin
		currentReg <= REG3;
		currentSign <= SIGN_out3;
		currentInput_SS <= IN_SS[3];
		currentInputSign <= SIGN[3];
	end
end
*/


wire [7:0] currentReg_temp0, currentReg_temp1;
wire currentSign_temp0, currentSign_temp1;
wire currentInput_SS_temp0, currentInput_SS_temp1;
wire currentInputSign_temp0, currentInputSign_temp1;

assign currentSign_temp0 = (regIndex == 4'd1) ? SIGN_out1 : SIGN_out0;
assign currentSign_temp1 = (regIndex == 4'd2) ? SIGN_out2 : currentSign_temp0;
assign currentSign = (regIndex == 4'd3) ? SIGN_out3 : currentSign_temp1;

assign currentReg_temp0 = (regIndex == 4'd1) ? REG1 : REG0;
assign currentReg_temp1 = (regIndex == 4'd2) ? REG2 : currentReg_temp0;
assign currentReg = (regIndex == 4'd3) ? REG3 : currentReg_temp1;

assign currentInput_SS_temp0 = (regIndex == 4'd1) ? IN_SS[1] : IN_SS[0];
assign currentInput_SS_temp1 = (regIndex == 4'd2) ? IN_SS[2] : currentInput_SS_temp0;
assign currentInput_SS = (regIndex == 4'd3) ? IN_SS[3] : currentInput_SS_temp1;

assign currentInputSign_temp0 = (regIndex == 4'd1) ? SIGN[1] : SIGN[0];
assign currentInputSign_temp1 = (regIndex == 4'd2) ? SIGN[2] : currentInputSign_temp0;
assign currentInputSign = (regIndex == 4'd3) ? SIGN[3] : currentInputSign_temp1;



//assign currentReg = (regIndex) ? REG1 : REG0;
//assign currentSign = (regIndex) ? SIGN_out1 : SIGN_out0;
//assign currentInput_SS = (regIndex) ? IN_SS[1] : IN_SS[0];
//assign currentInputSign = (regIndex) ? SIGN[1] : SIGN[0];

assign atMaxVal = (currentReg == 8'd255);
assign atZeroVal = (currentReg == 8'd0);
assign signMatch = (currentInputSign == currentSign);

assign newVal_sum = (~atMaxVal) ? (currentReg + currentInput_SS) : currentReg;
assign newVal_diff = (~atZeroVal) ? (currentReg - currentInput_SS) : currentReg;
assign newVal = (signMatch) ? newVal_sum : newVal_diff;
assign newSign = (atZeroVal) ? (currentInputSign) : (currentSign);


always @(posedge CLK or posedge INIT or posedge EN) begin
	if (INIT) begin
		REG0 <= OUT_INIT;
		REG1 <= OUT_INIT;
		REG2 <= OUT_INIT;
		REG3 <= OUT_INIT;
		SIGN_out0 <= SIGN_OUT_INIT;
		SIGN_out1 <= SIGN_OUT_INIT;
		SIGN_out2 <= SIGN_OUT_INIT;
		SIGN_out3 <= SIGN_OUT_INIT;
	end else if (~EN) begin
		REG0 <= REG0;
		REG1 <= REG1;
		REG2 <= REG2;
		REG3 <= REG3;
		SIGN_out0 <= SIGN_out0;
		SIGN_out1 <= SIGN_out1;
		SIGN_out2 <= SIGN_out2;
		SIGN_out3 <= SIGN_out3;
	end else if (regIndex == 4'd0) begin
		REG0 <= newVal;
		SIGN_out0 <= newSign;
	end else if (regIndex == 4'd1) begin
		REG1 <= newVal;
		SIGN_out1 <= newSign;
	end else if (regIndex == 4'd2) begin
		REG2 <= newVal;
		SIGN_out2 <= newSign;
	end else begin
		REG3 <= newVal;
		SIGN_out3 <= newSign;
	end
end

endmodule