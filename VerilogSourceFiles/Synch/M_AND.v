// Chris Ceroici
// August 10, 2013
//
// Cascading and gates

module M_AND(IN,OUT);

parameter N = 3; // number of inputs

input [N-1:0] IN;

output OUT; 

wire [N-3:0] Temp;

and Ustart (Temp[0],IN[0],IN[1]);		// generate cascading XOR gates
and Uend (OUT,Temp[N-3],IN[N-1]);
genvar n;
generate 
	for (n=0; n<(N-3);n=n+1) begin : CAND1 
		and U (Temp[n+1],Temp[n],IN[n+2]);
	end
endgenerate

endmodule