
// With dynamic resistance

module NN_SMOOTHGRAD_POLAR_STATIC_RESISTANCE(CLK, INIT, IN_SS, SIGN, OUT, SIGN_out, DIRECTION_CHANGE, OUT_INIT, SIGN_OUT_INIT, EN);

parameter N = 8; // decimal output size
parameter N_RESISTANCE = 9;

input CLK, INIT, EN;
input IN_SS, SIGN;
input [N-1:0] OUT_INIT;
input SIGN_OUT_INIT;

output  reg [N-1:0] OUT = 0;
output reg SIGN_out = 1'b0;
output wire DIRECTION_CHANGE;

reg [N-1:0] MaxVal_reg = 1'd0-1'd1;
reg [N_RESISTANCE-1:0] flag_counter = 1'b0;

reg Transition = 1'd0; // sign of current flag saturation (direction parameter has moved)
reg lastTransition = 1'd0; // sign of last flag saturation (direction parameter has moved)
reg Transition_TRIG = 1'b0;

reg DIRECTION_UP = 1'b0;
reg DIRECTION_DOWN = 1'b0;

wire OUT_ATMAX, OUT_ATMAX_neg, OUT_ATMAX_pos, OUT_ATZERO;
wire SIGNS_EQUAL;
wire flag_ATRESISTANCE;

assign OUT_ATMAX = (OUT==MaxVal_reg);
assign OUT_ATMAX_neg = (OUT==MaxVal_reg)&(SIGN_out==1'b1);
assign OUT_ATMAX_pos = (OUT==MaxVal_reg)&(SIGN_out==1'b0);
assign OUT_ATZERO = (OUT==1'b0);
assign SIGNS_EQUAL = (SIGN==SIGN_out);
//assign flag_ATRESISTANCE = (flag_counter>=RESISTANCE);
assign DIRECTION_CHANGE = (DIRECTION_UP&DIRECTION_DOWN);
//assign DIRECTION_CHANGE = (DIRECTION_UP|DIRECTION_DOWN);

always @(posedge INIT or posedge CLK) begin
	if (INIT) begin
		OUT <= OUT_INIT;
		SIGN_out <= SIGN_OUT_INIT;
		DIRECTION_UP <= 1'b0;
		DIRECTION_DOWN <= 1'b0;
	end else if (~EN) begin
		OUT <= OUT;
		SIGN_out <= SIGN_out;
		DIRECTION_UP <= DIRECTION_UP;
		DIRECTION_DOWN <= DIRECTION_DOWN;
	end else if ((!IN_SS)&DIRECTION_CHANGE) begin // input zero with reistance increment
		OUT <= OUT;
		SIGN_out <= SIGN_out;
		DIRECTION_UP <= 1'b0;
		DIRECTION_DOWN <= 1'b0;
	end else if (!IN_SS) begin // input zero
		OUT <= OUT;
		SIGN_out <= SIGN_out;
		DIRECTION_UP <= DIRECTION_UP;
		DIRECTION_DOWN <= DIRECTION_DOWN;
	end else if (OUT_ATZERO&SIGN) begin
		OUT <= 1'b1;
		SIGN_out <= 1'b1;
		DIRECTION_UP <= DIRECTION_UP;
		DIRECTION_DOWN <= 1'b1;
	end else if (OUT_ATZERO) begin
		OUT <= 1'b1;
		SIGN_out <= 1'b0;
		DIRECTION_UP <= 1'b1;
		DIRECTION_DOWN <= DIRECTION_DOWN;
	end else if (SIGN) begin // negative increment
		OUT <= OUT + ((SIGNS_EQUAL==1'b1)&(~OUT_ATMAX)) - (SIGNS_EQUAL==1'b0);
		SIGN_out <= SIGN_out;
		DIRECTION_UP <= DIRECTION_UP;
		DIRECTION_DOWN <= 1'b1;
	end else begin // positive increment
		OUT <= OUT + ((SIGNS_EQUAL==1'b1)&(~OUT_ATMAX)) - (SIGNS_EQUAL==1'b0);
		SIGN_out <= SIGN_out;
		DIRECTION_UP <= 1'b1;
		DIRECTION_DOWN <= DIRECTION_DOWN;
	end
end




/*
always @(posedge INIT or posedge CLK) begin
 	if (INIT) begin
		OUT <= OUT_INIT;
		SIGN_out <= SIGN_OUT_INIT;
		flag_counter <= 1'b0;
		DIRECTION_UP <= 1'b0;
		DIRECTION_DOWN <= 1'b0;
	end else if (~EN) begin
		OUT <= OUT;
		SIGN_out <= SIGN_out;
		flag_counter <= flag_counter;
		DIRECTION_UP <= DIRECTION_UP;
		DIRECTION_DOWN <= DIRECTION_DOWN;
	end else if (!IN_SS) begin // input zero
		OUT <= OUT;
		SIGN_out <= SIGN_out;
		flag_counter <= flag_counter;
		DIRECTION_UP <= DIRECTION_UP;
		DIRECTION_DOWN <= DIRECTION_DOWN;
	end else if (flag_ATRESISTANCE&OUT_ATZERO) begin
		OUT <= 1'b1;
		SIGN_out <= SIGN;
		flag_counter <= 1'b0;

	end else if (flag_ATRESISTANCE) begin
		OUT <= OUT + ((SIGNS_EQUAL==1'b1)&(~OUT_ATMAX)) - (SIGNS_EQUAL==1'b0);
		SIGN_out <= SIGN_out;
		flag_counter <= 1'b0;
	end else if (~flag_ATRESISTANCE) begin
		OUT <= OUT;
		SIGN_out <= SIGN_out;
		flag_counter <= flag_counter + 1'b1;
	end else begin
		OUT <= OUT; // Input Positive/Negative: parameter at upper/lower limit -> do nothing	
		SIGN_out <= SIGN_out;
	end
	
end
*/

endmodule