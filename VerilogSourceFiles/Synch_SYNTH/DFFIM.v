// Chris Ceroici
// July 14 2013

// D Flip Flop with enable

module DFFIM(CLK,EN,D,Q);

input D;
input CLK,EN;
output Q;

reg hold=1'b0;
reg Dtemp=1'b0;

assign Q = Dtemp;

always @(posedge CLK) begin
	if (EN) hold <= D; 
	Dtemp <= hold;
end

endmodule
