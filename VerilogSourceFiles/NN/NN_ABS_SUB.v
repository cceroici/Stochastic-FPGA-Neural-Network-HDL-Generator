// Calculate the absolute difference between two numbers
// Subtract: A-B
module NN_ABS_SUB(A,B,OUT,SIGN);

parameter NB = 16;

input [NB-1:0] A,B;

output [NB-1:0] OUT;
output SIGN;


assign SIGN = A>B;

assign OUT = (SIGN) ? (A-B) : (1'b0 - (A-B));


endmodule