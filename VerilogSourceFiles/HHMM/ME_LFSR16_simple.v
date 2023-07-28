// Chris Ceroici
// August 12 2013
//
// 16 bit LFSR
// Primitive Polynomial: 1+x2+x3+x5+x16
// 
module ME_LFSR16_simple(TRIG,RESET,OUT0,OUT1,OUT2,OUT3,SEED);

input TRIG, RESET; // TRIG: shift registers trigger
input [15:0] SEED;

output [3:0] OUT0,OUT1,OUT2,OUT3;


//wire [7:0] OUTtemp0,OUTtemp1,OUTtemp2,OUTtemp3;
reg [3:0] OUTtemp0 = 1'd0,OUTtemp1 = 1'd0,OUTtemp2 = 1'd0,OUTtemp3 = 1'd0;
wire [2:0] Q /* synthesis keep */;
wire [3:0] mut0,mut1,mut2,mut3;

reg [15:0] D = 16'd0 /* synthesis preserve */;

always @(posedge TRIG) begin
	OUTtemp0 = mut0;
	OUTtemp1 = mut1;
	OUTtemp2 = mut2;
	OUTtemp3 = mut3;
end

assign #1 OUT0 = (RESET) ? 8'd100 : OUTtemp0;
assign #1 OUT1 = (RESET) ? 8'd100 : OUTtemp1;
assign #1 OUT2 = (RESET) ? 8'd100 : OUTtemp2;
assign #1 OUT3 = (RESET) ? 8'd100 : OUTtemp3;



assign mut0[3:0] = {D[0],D[15],D[2],D[13]};
assign mut1[3:0] = {D[4],D[11],D[6],D[9]};
assign mut2[3:0] = {D[8],D[14],D[7],D[12]};
assign mut3[3:0] = {D[5],D[10],D[3],D[1]};

xor U1(Q[0],D[15],D[4]);
xor U2(Q[1],Q[0],D[2]);
xor U3(Q[2],Q[1],D[1]);

always @(posedge TRIG or posedge RESET) begin
	if (RESET) D <= SEED;
	else D[15:0] <= #3 {D[14:0],Q[2]};
end

endmodule