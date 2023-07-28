// Chris Ceroici
// August 12 2013
//
// 16 bit LFSR
// Primitive Polynomial: 1+x2+x3+x5+x16
// 
module LFSR16_32Tap(TRIG,RESET,
	OUT0,OUT1,OUT2,OUT3,OUT4,OUT5,OUT6,OUT7,OUT8,OUT9,OUT10,OUT11,OUT12,OUT13,OUT14,OUT15,
	OUT16,OUT17,OUT18,OUT19,OUT20,OUT21,OUT22,OUT23,OUT24,OUT25,OUT26,OUT27,OUT28,OUT29,OUT30,OUT31,
	SEED);

input TRIG, RESET; // TRIG: shift registers trigger
input [15:0] SEED;

output [7:0] OUT0,OUT1,OUT2,OUT3,OUT4,OUT5,OUT6,OUT7,OUT8,OUT9,OUT10,OUT11,OUT12,OUT13,OUT14,OUT15,OUT16,OUT17,OUT18,OUT19,OUT20,OUT21,OUT22,OUT23,OUT24,OUT25,OUT26,OUT27,OUT28,OUT29,OUT30,OUT31;

reg [15:0] D = 16'd0 /* synthesis preserve */;
wire [2:0] Q /* synthesis keep */;

assign OUT0[7:0] = {D[7:0]};
assign OUT1[7:0] = {D[8:1]};
assign OUT2[7:0] = {D[9:2]};
assign OUT3[7:0] = {D[10:3]};
assign OUT4[7:0] = {D[11:4]};
assign OUT5[7:0] = {D[12:5]};
assign OUT6[7:0] = {D[13:6]};
assign OUT7[7:0] = {D[14:7]};
assign OUT8[7:0] = {D[15:8]};
assign OUT9[7:0] = {D[15:9],D[0]};
assign OUT10[7:0] = {D[15:10],D[1:0]};
assign OUT11[7:0] = {D[15:11],D[2:0]};
assign OUT12[7:0] = {D[15:12],D[3:0]};
assign OUT13[7:0] = {D[15:13],D[4:0]};
assign OUT14[7:0] = {D[15:14],D[5:0]};
assign OUT15[7:0] = {D[15],D[6:0]};

assign OUT16[7:0] = {D[1],D[3],D[5],D[7],D[9],D[11],D[13],D[15]};
assign OUT17[7:0] = {D[0],D[2],D[4],D[6],D[8],D[10],D[12],D[14]};
assign OUT18[7:0] = {D[0],D[3],D[4],D[7],D[8],D[11],D[12],D[15]};
assign OUT19[7:0] = {D[1],D[2],D[5],D[6],D[9],D[10],D[13],D[14]};
assign OUT20[7:0] = {D[15],D[2],D[5],D[14],D[9],D[6],D[13],D[2]};
assign OUT21[7:0] = {D[15],D[2],D[5],D[13],D[9],D[6],D[6],D[2]};
assign OUT22[7:0] = {D[3],D[15],D[7],D[2],D[9],D[2],D[3],D[0]};
assign OUT23[7:0] = {D[11],D[3],D[1],D[13],D[2],D[0],D[7],D[6]};
assign OUT24[7:0] = {D[7],D[11],D[12],D[4],D[1],D[10],D[2],D[14]};
assign OUT25[7:0] = {D[2],D[9],D[13],D[4],D[15],D[7],D[2],D[14]};
assign OUT26[7:0] = {D[14],D[2],D[1],D[4],D[6],D[0],D[12],D[11]};
assign OUT27[7:0] = {D[13],D[11],D[14],D[1],D[0],D[3],D[15],D[7]};
assign OUT28[7:0] = {D[0],D[10],D[7],D[9],D[6],D[3],D[5],D[4]};
assign OUT29[7:0] = {D[6],D[4],D[14],D[15],D[2],D[1],D[11],D[7]};
assign OUT30[7:0] = {D[0],D[2],D[11],D[1],D[9],D[14],D[11],D[3]};
assign OUT31[7:0] = {D[12],D[5],D[4],D[13],D[7],D[2],D[13],D[6]};


xor U1(Q[0],D[15],D[4]);
xor U2(Q[1],Q[0],D[2]);
xor U3(Q[2],Q[1],D[1]);

//assign Q[0] = D[15]^D[4];
//assign Q[1] = D[2]^Q[0];
//assign Q[2] = D[1]&Q[1];

wire [15:0] Dtemp;

/*
always @(posedge TRIG) begin
	D[15:0] <= #3 {D[14:0],Q[2]};
end
*/

always @(posedge TRIG or posedge RESET) begin
	if (RESET) D <= SEED;
	else D[15:0] <= #3 {D[14:0],Q[2]};
end

endmodule