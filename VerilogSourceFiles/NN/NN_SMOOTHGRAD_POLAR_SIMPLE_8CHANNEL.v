
// With dynamic resistance

module NN_SMOOTHGRAD_POLAR_SIMPLE_8CHANNEL(CLK, CLK_TRAINING_flag, INIT,
	IN_SS, SIGN, regIndex,
	REG0, REG1, REG2, REG3, REG4, REG5, REG6, REG7, SIGN_out0, SIGN_out1, SIGN_out2, SIGN_out3, SIGN_out4, SIGN_out5, SIGN_out6, SIGN_out7,
	RESISTANCE,
	TransitionChange_TRIG, OUT_INIT, SIGN_OUT_INIT, EN
);

parameter N_RESISTANCE = 9;
parameter N = 8; // decimal output size

input CLK, CLK_TRAINING_flag, INIT, EN;
input [7:0] IN_SS, SIGN;
input [N-1:0] OUT_INIT;
input SIGN_OUT_INIT;
input [2:0] regIndex;
input [N_RESISTANCE-1:0] RESISTANCE;


output reg [N-1:0] REG0 = 0;
output reg [N-1:0] REG1 = 0;
output reg [N-1:0] REG2 = 0;
output reg [N-1:0] REG3 = 0;
output reg [N-1:0] REG4 = 0;
output reg [N-1:0] REG5 = 0;
output reg [N-1:0] REG6 = 0;
output reg [N-1:0] REG7 = 0;
output reg SIGN_out0 = 1'b0;
output reg SIGN_out1 = 1'b0;
output reg SIGN_out2 = 1'b0;
output reg SIGN_out3 = 1'b0;
output reg SIGN_out4 = 1'b0;
output reg SIGN_out5 = 1'b0;
output reg SIGN_out6 = 1'b0;
output reg SIGN_out7 = 1'b0;

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

wire [7:0] currentReg_temp0, currentReg_temp1, currentReg_temp2, currentReg_temp3, currentReg_temp4, currentReg_temp5;
wire currentSign_temp0, currentSign_temp1, currentSign_temp2, currentSign_temp3, currentSign_temp4, currentSign_temp5;
wire currentInput_SS_temp0, currentInput_SS_temp1;
wire currentInputSign_temp0, currentInputSign_temp1;

assign currentSign_temp0 = (regIndex == 4'd1) ? SIGN_out1 : SIGN_out0;
assign currentSign_temp1 = (regIndex == 4'd2) ? SIGN_out2 : currentSign_temp0;
assign currentSign_temp2 = (regIndex == 4'd3) ? SIGN_out3 : currentSign_temp1;
assign currentSign_temp3 = (regIndex == 4'd4) ? SIGN_out4 : currentSign_temp2;
assign currentSign_temp4 = (regIndex == 4'd5) ? SIGN_out5 : currentSign_temp3;
assign currentSign_temp5 = (regIndex == 4'd6) ? SIGN_out6 : currentSign_temp4;
assign currentSign 		 = (regIndex == 4'd7) ? SIGN_out7 : currentSign_temp5;

assign currentReg_temp0 = (regIndex == 4'd1) ? REG1 : REG0;
assign currentReg_temp1 = (regIndex == 4'd2) ? REG2 : currentReg_temp0;
assign currentReg_temp2 = (regIndex == 4'd3) ? REG3 : currentReg_temp1;
assign currentReg_temp3 = (regIndex == 4'd4) ? REG4 : currentReg_temp2;
assign currentReg_temp4 = (regIndex == 4'd5) ? REG5 : currentReg_temp3;
assign currentReg_temp5 = (regIndex == 4'd6) ? REG6 : currentReg_temp4;
assign currentReg 		= (regIndex == 4'd7) ? REG7 : currentReg_temp5;


MPX MPX_Input_SS(.EN(1'b1),.SEL(regIndex),.D(IN_SS),.Q(currentInput_SS));
defparam MPX_Input_SS.N = 8; // number of inputs
defparam MPX_Input_SS.Nsel = 3; // number of selection bits

MPX MPX_Input_Sign(.EN(1'b1),.SEL(regIndex),.D(SIGN),.Q(currentInputSign));
defparam MPX_Input_Sign.N = 8; // number of inputs
defparam MPX_Input_Sign.Nsel = 3; // number of selection bits

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
	end else if (regIndex == 4'd3) begin
		REG3 <= newVal;
		SIGN_out3 <= newSign;
	end else if (regIndex == 4'd4) begin
		REG4 <= newVal;
		SIGN_out4 <= newSign;
	end else if (regIndex == 4'd5) begin
		REG5 <= newVal;
		SIGN_out5 <= newSign;
	end else if (regIndex == 4'd6) begin
		REG6 <= newVal;
		SIGN_out6 <= newSign;
	end else begin
		REG7 <= newVal;
		SIGN_out7 <= newSign;
	end
end

endmodule