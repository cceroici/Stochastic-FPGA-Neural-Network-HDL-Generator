// Chris Ceroici
// August 12 2013
//
// 16 bit LFSR
// Primitive Polynomial: 1+x2+x3+x5+x16
// 
module LFSR16(TRIG,RESET,OUT1,OUT2,SEED);

input TRIG, RESET; // TRIG: shift registers trigger
input [15:0] SEED;

output [7:0] OUT1,OUT2;
 
wire [7:0] OUT1, OUT2;
//reg [8:0] D = {SEED[7:0],SEED[3]};
reg [15:0] D;
wire [2:0] Q;

//assign OUT = D;
assign OUT1[7:0] = {D[0],D[15],D[2],D[13],D[4],D[11],D[6],D[9]};
assign OUT2[7:0] = {D[1],D[14],D[3],D[12],D[4],D[10],D[6],D[8]};



xor U1(Q[0],D[15],D[4]);
xor U2(Q[1],Q[0],D[2]);
xor U3(Q[2],Q[1],D[1]);

always @(posedge TRIG or posedge RESET) begin
	if (RESET) D <= SEED;
	else D <= {D[14:0],Q[2]};
end

endmodule