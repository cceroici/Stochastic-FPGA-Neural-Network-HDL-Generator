// Chris Ceroici
// March 8, 2017
//
// Cascading XOR gates

module M_XOR(EN,IN,OUT);

parameter N = 3; // number of inputs

input [N-1:0] IN;
input EN;

output OUT; 

wire [N-3:0] Temp;

and Ustart (Temp[0],IN[0],IN[1]);		// generate cascading XOR gates
assign OUT = (EN) ? Temp[N-3]&IN[N-1] : 1'b0;
//and Uend (OUT,Temp[N-3],IN[N-1]);
genvar n;
generate 
	for (n=0; n<(N-3);n=n+1) begin : COR1 
		xor U (Temp[n+1],Temp[n],IN[n+2]);
	end
endgenerate

endmodule