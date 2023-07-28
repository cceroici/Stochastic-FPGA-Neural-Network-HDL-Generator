

module BITADD2(IN0,IN1,OUT);

parameter N = 1;

input [N-1:0] IN0;
input IN1;

output wire [N-1:0] OUT;

assign OUT = IN0 + IN1;

endmodule
