


module NN_SMOOTHGRAD(CLK, CLK_TRAINING_flag, INIT, IN_SS, SIGN, OUT, OUT_INIT, RESISTANCE, EN);

parameter N = 8; // decimal output size


input CLK, CLK_TRAINING_flag, INIT, EN;
input IN_SS, SIGN;
input [N-1:0] OUT_INIT;
input [7:0] RESISTANCE;

output  reg [N-1:0] OUT = 0;


reg [N-1:0] MaxVal_reg = 1'd0-1'd1;
//reg [3-1:0] MaxVal_flag = 1'd0 - 1'd1;
reg [7:0] flag_pos = 1'b0;
reg [7:0] flag_neg = 1'b0;

always @(posedge INIT or posedge CLK) begin

 	if (INIT) OUT <= OUT_INIT;
	else if (~EN) OUT <= OUT;
	else if (!IN_SS) begin // input zero
		OUT <= OUT;
		flag_pos <= flag_pos;
		flag_neg <= flag_neg;
	end else if ((!SIGN)&(OUT!=MaxVal_reg)&(flag_pos==RESISTANCE)) begin  // Input Positive: positive flag at resistance -> increment parameter
		OUT <= OUT + IN_SS;
		flag_pos <= 1'b0;
		//flag_neg <= 1'b0;
	end else if ((!SIGN)&(OUT!=MaxVal_reg)&(flag_pos!=RESISTANCE)) begin // Input Positive: positive flag not at resistance -> increment positive flag
		OUT <= OUT;
		flag_pos <= flag_pos + 1'b1;
	end else if ((SIGN)&(OUT!=8'd0)&(flag_neg==RESISTANCE)) begin // Input Negative: negative flag at resistance -> decrement parameter
		OUT <= OUT - IN_SS;
		flag_neg <= 1'b0;
		//flag_pos <= 1'b0;
	end else if ((SIGN)&(OUT!=8'd0)&(flag_neg!=RESISTANCE)) begin // Input Negative: negative flag not at resistance -> increment negative flag
		OUT <= OUT;
		flag_neg <= flag_neg + 1'b1;
	end else OUT <= OUT; // Input Positive/Negative: parameter at upper/lower limit -> do nothing
	
end

endmodule