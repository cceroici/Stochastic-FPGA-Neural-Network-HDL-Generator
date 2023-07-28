// Chris Ceroici
// August 10, 2013
//
// Parity Check node block

module PCN(CLK,Q,R,PC_sat);

parameter PCN_S = 6;

input CLK;
input [PCN_S-1:0] Q;

output [PCN_S-1:0] R;
output PC_sat; // parity check satisfied

wire [PCN_S-1:0] Temp;
wire [PCN_S-1:0] D;

xor Ustart(Temp[0],Q[0],Q[1]);		// XOR all inputs to get parity check bit
xor Uend (PC_sat,Q[PCN_S-1],Temp[PCN_S-3]);
genvar n;
generate 
	for (n=0; n<(PCN_S-3);n=n+1) begin : SXOR1 
		xor U(Temp[n+1],Q[n+2],Temp[n]);
	end
endgenerate

genvar l;                          // XOR individual bits again to get output bits
generate 
	for (l=0; l<PCN_S;l=l+1) begin : SFF2 
		xor U(D[l],PC_sat,Q[l]);
		DFFN FF(CLK,D[l],R[l]);
	end
endgenerate

endmodule