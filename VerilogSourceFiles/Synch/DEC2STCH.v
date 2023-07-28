// Chris Ceroici
// August 12 2013
//
// Converts a decimal probability into an 8-bit stochastic stream
// Input fraction must be scaled to x/256

module DEC2STCH(CLK,INIT,D,LFSR,S);

parameter ND = 8; 					// precision of decimal fraction

input [ND-1:0] D; 					// input probability scaled to x/256
input [ND-1:0] LFSR; // LFSR comparator value
input CLK,INIT; 

output S; 								// stochastic bit

reg Comp = 1'b0; 								// comparator output

assign S = Comp;

always @(posedge CLK) begin
	Comp <= (D>=LFSR) ? 1'b1 : 1'b0;
end	

endmodule