// Chris Ceroici
// July 14 2013

// D Flip Flop
/*
       |------------|
	   |		    Q |------>
---->| D            |
       |   CLK ~Q |------>
       |-----^-----|
               |
*/
module DFFV(CLK,D,Q);

parameter N=8; // size

input [N-1:0] D;
input CLK;
output [N-1:0] Q;

reg [N-1:0] Dtemp;

assign Q = Dtemp;

always @(posedge CLK) begin
	Dtemp <= D;
end

endmodule
