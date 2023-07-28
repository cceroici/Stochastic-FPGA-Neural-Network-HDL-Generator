// Chris Ceroici
// October 10, 2013
//
// 6 connection Parity Check node block with clockless design (no direct lines)

module PCN6(Q,R,PC_sat);

parameter PCN_S = 6;
parameter D_PCN = 1;
parameter G_D = 1;

input [PCN_S-1:0] Q;

output [PCN_S-1:0] R;
output PC_sat; // parity check satisfied

wire [PCN_S-3:0] Temp;
wire [PCN_S-1:0] R_temp;

xor #G_D Ustart(Temp[0],Q[0],Q[1]);		// XOR all inputs to get parity check bit
xor #G_D Uend (PC_sat,Q[PCN_S-1],Temp[PCN_S-3]);
genvar n;
generate 
	for (n=0; n<(PCN_S-3);n=n+1) begin : SXOR1 
		xor #G_D U(Temp[n+1],Q[n+2],Temp[n]);
	end
endgenerate

xor #G_D uA(Ra,Q[0],Q[1],Q[2]);
xor #G_D uB(Rb,Q[3],Q[4],Q[5]);

xor #G_D u0(R_temp[0],Rb,Q[1],Q[2]);
xor #G_D u1(R_temp[1],Rb,Q[0],Q[2]);
xor #G_D u2(R_temp[2],Rb,Q[0],Q[1]);

xor #G_D u3(R_temp[3],Ra,Q[4],Q[5]);
xor #G_D u4(R_temp[4],Ra,Q[3],Q[5]);
xor #G_D u5(R_temp[5],Ra,Q[3],Q[4]);

assign #D_PCN R = R_temp;

endmodule