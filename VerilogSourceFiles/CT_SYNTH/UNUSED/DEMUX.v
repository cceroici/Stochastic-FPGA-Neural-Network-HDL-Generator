// N bit multiplexor

module DEMUX(SEL,D,Q);

parameter NS = 3; // size of select signal
parameter NR = 8; // number of inputs
input [NS-1:0] SEL;
input [NR-1:0] D;
output Q;

assign Q = D[SEL];

endmodule