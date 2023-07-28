// Chris Ceroici
// August 10, 2013
//
// Edge memory module with output address selection multiplexor
// N-bit shift register 
// Shift in new values from non-hold states and shift out old values
//
// ports: 
// RESET,Qi: reset register values to LLR
// SEL: output selection 
// OUT: output bit
module EM(TRIG,IN,OUT,SEL,FLAG);

parameter N = 8; // # of bits (must be >1)

input TRIG,IN;
input [3-1:0] SEL;   
input FLAG; 

output OUT;

wire [N-1:0] Temp;

DFFN DFFstart(.CLK(TRIG),.D(IN),.Q(Temp[0]));		// generate cascading D Flip Flops
DFFN DFFend (.CLK(TRIG),.D(Temp[N-2]),.Q(Temp[N-1]));
genvar n;
generate 
	for (n=0; n<(N-2);n=n+1) begin : SDFF1 
		DFFN D (.CLK(TRIG),.D(Temp[n]),.Q(Temp[n+1]));
	end
endgenerate

DEMUX DEMUX0(SEL,Temp,OUT);
defparam DEMUX0.NR = N;
defparam DEMUX0.NS = 3;


endmodule