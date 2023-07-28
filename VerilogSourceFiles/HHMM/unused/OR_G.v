// Determing if recycle ocurrs during scaling operation

module OR_G(G,OUT,N);

parameter MEML = 4;

input [7:0] N; // Number of memory locations to look through

input wire [MEML-1:0] G; 

output reg OUT;


reg [15:0] OUTreg = 1'd0;

always @(*) begin

	OUTreg[N-1:0] = G[N-1:0];
	if (OUTreg>0) OUT = 1'b1;
	else OUT = 1'b1;

end

endmodule