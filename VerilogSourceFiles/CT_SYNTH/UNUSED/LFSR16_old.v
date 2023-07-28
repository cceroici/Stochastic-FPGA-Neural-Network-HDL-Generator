// Chris Ceroici
// August 12 2013
//
// 16 bit LFSR
// Primitive Polynomial: 1+x2+x3+x5+x16
// 
module LFSR16_old(TRIG,RESET,OUT1,OUT2,SEED,INIT_powerup);

input TRIG, RESET, INIT_powerup; // TRIG: shift registers trigger
input [15:0] SEED;

output [7:0] OUT1,OUT2;
 
wire [7:0] OUT1, OUT2;
//reg [8:0] D = {SEED[7:0],SEED[3]};
reg [15:0] D;
wire [2:0] Q;
reg INIT_flag = 1'b0;

//assign OUT = D;
assign OUT1[7:0] = {D[0],D[15],D[2],D[13],D[4],D[11],D[6],D[9]};
assign OUT2[7:0] = {D[1],D[14],D[3],D[12],D[5],D[10],D[7],D[8]};
//assign OUT1[7:0] = D[15:8];
//assign OUT2[7:0] = D[7:0];

xor U1(Q[0],D[15],D[4]);
xor U2(Q[1],Q[0],D[2]);
xor U3(Q[2],Q[1],D[1]);

always @(posedge TRIG or posedge RESET&INIT_flag or posedge INIT_powerup) begin
	if (INIT_powerup) begin 
		D <= SEED[15:0];
		INIT_flag = 1'b1;
	end
	else if (RESET&INIT_flag) begin  // use flag to allow for operation during (frame) initialization phase
			D <= SEED[15:0];
			INIT_flag = 1'b0;
	end
	else begin 
		D[15:0] <= #1 {D[14:0],Q[2]};
		if (~RESET) INIT_flag <= 1'b1;
	end
end


endmodule