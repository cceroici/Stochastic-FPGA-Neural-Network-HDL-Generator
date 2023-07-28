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
module DFF(CLK,D,Q);

input D;
input CLK;
output Q;

reg hold=1'b0;
reg Dtemp=1'b0;

assign Q = Dtemp;

always @(posedge CLK) begin
	hold <= D; 
	#3 Dtemp <= hold;
end

endmodule
