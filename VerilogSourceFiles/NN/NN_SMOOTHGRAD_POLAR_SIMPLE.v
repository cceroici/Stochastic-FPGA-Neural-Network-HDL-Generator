
// With dynamic resistance

module NN_SMOOTHGRAD_POLAR_SIMPLE(CLK, CLK_TRAINING_flag, INIT, IN_SS, SIGN, OUT, SIGN_out, RESISTANCE, TransitionChange_TRIG, OUT_INIT, SIGN_OUT_INIT, EN);

parameter N = 8; // decimal output size
parameter N_RESISTANCE = 9;

input CLK, CLK_TRAINING_flag, INIT, EN;
input IN_SS, SIGN;
input [N-1:0] OUT_INIT;
input SIGN_OUT_INIT;
input [N_RESISTANCE-1:0] RESISTANCE;
//input [N_RESISTANCE -1:0] RESISTANCE_MAX;

output  reg [N-1:0] OUT = 0;
output reg SIGN_out = 1'b0;

output wire TransitionChange_TRIG;


assign TransitionChange_TRIG = 1'b0;

wire [7:0] newVal;
wire newSign;
wire [7:0] newVal_sum;
wire [7:0] newVal_diff;
wire atZeroVal;
wire signMatch;

assign atMaxVal = (OUT == 8'd255);
assign atZeroVal = (OUT == 8'd0);
assign signMatch = (SIGN == SIGN_out);

assign newVal_sum = (~atMaxVal) ? (OUT + IN_SS) : OUT;
assign newVal_diff = (~atZeroVal) ? (OUT - IN_SS) : OUT;
assign newVal = (signMatch) ? newVal_sum : newVal_diff;
assign newSign = (atZeroVal) ? (SIGN) : (SIGN_out);

always @(posedge CLK or posedge INIT) begin
	if (INIT) begin
		OUT <= OUT_INIT;
		SIGN_out <= SIGN_OUT_INIT;
	end else begin
		OUT <= newVal;
		SIGN_out <= newSign;
	end
end

endmodule