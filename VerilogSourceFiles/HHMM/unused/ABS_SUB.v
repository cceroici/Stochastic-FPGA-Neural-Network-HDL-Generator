// Calculate the absolute difference between two numbers

module ABS_SUB(A,B,OUT);

parameter N = 12;

input [N-1:0] A,B;

output [N-1:0] OUT;


wire [N-1:0] OUTsign;

assign OUTsign = A-B;
assign OUT = (OUTsign[7]) ? (1'b0 - (A-B)) : (A-B);

endmodule