
// Chris Ceroici
// Add/Subtract NN weights and biases

module NN_ABS_ADDSUB(in,arg,rate,out,OPERATION,CLK);

parameter Nbase = 8;
parameter Narg = 16;
parameter BASE_SHIFT = 8;

input [Nbase-1:0] in;
input [Narg-1:0] arg;
input [7:0] rate; // maximum: d5 for 8bit input
input OPERATION; // 0 for addition, 1 for subtraction
input CLK;

output [Nbase-1:0] out;

wire VALID;
wire [Nbase-1:0] arg_scaled;
wire [Nbase-1:0] out_temp;
wire [Nbase-1:0] out_limit;


//assign arg_scaled = arg >> BASE_SHIFT+rate;
assign arg_scaled = arg[Nbase-1:0] >> rate;

assign VALID = ((~OPERATION)&((in+arg_scaled)>=in))||((OPERATION)&((in-arg_scaled)<=in));
assign out_temp = (OPERATION) ? (in-arg_scaled) : (in+arg_scaled);
assign out_limit = (~OPERATION) ? (8'd255) : (8'd0);
assign out = (VALID) ? (out_temp) : (out_limit);

endmodule
