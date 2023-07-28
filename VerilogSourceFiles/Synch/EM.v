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
module EM(CLK,EN,IN,OUT,SEL);

parameter N = 8; // # of bits (must be >1)

input CLK,EN,IN;
input [N-1:0] SEL;    // IMPROVEMENT NEEDED ON INDEXING FOR SEL. ONLY NEED LOG2(N) BITS

output OUT;

wire [N-1:0] Temp;

DFFIM DFFstart(.CLK(CLK),.EN(EN),.D(IN),.Q(Temp[0]));		// generate cascading D Flip Flops
DFFIM DFFend (.CLK(CLK),.EN(EN),.D(Temp[N-2]),.Q(Temp[N-1]));
genvar n;
generate 
	for (n=0; n<(N-2);n=n+1) begin : SDFF1 
		DFFIM D (.CLK(CLK),.EN(EN),.D(Temp[n]),.Q(Temp[n+1]));
	end
endgenerate

MPX MPX1(SEL,Temp,OUT);
defparam MPX1.N = N;

endmodule