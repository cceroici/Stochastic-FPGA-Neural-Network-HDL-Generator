// Chris Ceroici
// August 12 2013
//
// 8 bit LFSR
// Primitive Polynomial: 1+x2+x3+x4+x8
// use first 3 bits as output for 8 bit EM
module LFSR8(TRIG,RESET,OUT,SEED);

input TRIG, RESET; // TRIG: shift registers trigger
input [7:0] SEED;

output OUT;
 
wire [7:0] OUT;
//reg [8:0] D = {SEED[7:0],SEED[3]};
reg [8:0] D;
wire [2:0] Q;

//assign OUT[7:0] = {D[8:1]};
assign OUT[7:0] = {D[4:2],D[8],D[1],D[7:5]};

xor U1(Q[0],D[8],D[4]);
xor U2(Q[1],Q[0],D[3]);
xor U3(Q[2],Q[1],D[2]);

always @(posedge TRIG) begin
	D[8:0] <= #3 {D[7:0],Q[2]};
end

always @(posedge RESET) begin
	D <= SEED;
end

endmodule