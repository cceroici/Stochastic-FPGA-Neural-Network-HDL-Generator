// N bit multiplexor

module MPX(SEL,D,Q);

parameter N = 8; // number of inputs
input [N-1:0] SEL;
input [N-1:0] D;
output Q;

assign Q = D[SEL];

endmodule