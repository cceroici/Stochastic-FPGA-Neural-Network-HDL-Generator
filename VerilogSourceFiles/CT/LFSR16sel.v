// Chris Ceroici
// August 12 2013
//
// 16 bit LFSR
// Primitive Polynomial: 1+x2+x3+x5+x16
// 
module LFSR16sel(TRIG,RESET,OUT,SEED);

input TRIG, RESET; // TRIG: shift registers trigger
input [15:0] SEED;

output wire [14:0] OUT;

//reg [8:0] D = {SEED[7:0],SEED[3]};
reg [15:0] D;
wire [2:0] Q;

//assign OUT = D;
assign OUT[14:0] = {D[0],D[15],D[1],D[8],D[2],D[7],D[13],D[3],D[12],D[11],D[5],D[10],D[6],D[14],D[9]};
xor U1(Q[0],D[15],D[4]);
xor U2(Q[1],Q[0],D[2]);
xor U3(Q[2],Q[1],D[1]);


always @(posedge TRIG or posedge RESET) begin
	if (RESET) D <= SEED[15:0];
	else D[15:0] <= #3 {D[14:0],Q[2]};
end

endmodule