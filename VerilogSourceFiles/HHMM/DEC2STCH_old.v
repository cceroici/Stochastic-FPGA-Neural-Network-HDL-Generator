// Chris Ceroici
// August 12 2013
//
// Converts a decimal probability into an 8-bit stochastic stream
// Input fraction must be scaled to x/256

module DEC2STCH(CLK,D,LFSR,S,INIT);

parameter ND = 8; 					// precision of decimal fraction
parameter LFSR_S = ND; 			// LFSR size

input [ND-1:0] D; 					// input probability scaled to x/256
input [LFSR_S-1:0] LFSR; // LFSR comparator value
input CLK,INIT; 

output S; 								// stochastic bit

assign S = (D>LFSR) ? 1'b1 : 1'b0;


//reg Comp = 1'b0; 								// comparator output


//assign S = Comp;



/*
always @(posedge CLK) begin
	Comp <= (D>LFSR) ? 1'b1 : 1'b0;
end	
*/

endmodule