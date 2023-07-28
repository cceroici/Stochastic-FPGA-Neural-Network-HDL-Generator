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
module DFFN(CLK,D,Q);

input D;
input CLK;
output Q;

reg Dtemp=1'b0;

assign Q = Dtemp;

always @(posedge CLK) begin
	#1 Dtemp <= D;
end

endmodule
