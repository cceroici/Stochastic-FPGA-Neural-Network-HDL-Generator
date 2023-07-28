// Temperal stochastic bit magnifier

module SCALE_N(IN,OUT,G,CLK);

parameter MEML = 16; // Memory size;

input IN,CLK;
input [MEML-1:0] G;

output reg OUT;

reg [MEML-1:0] MEM = 1'b0; // Memory
reg [15:0] ZCounter = 1'b0; // Count zeros

OR_G OR_G1(.G(G),.OUT(CYC),.N(ZCounter));

/* wire [N-3:0] Temp;

or Ustart (Temp[0],G[0],G[1]);		// generate cascading XOR gates
assign OUT = (EN) ? Temp[N-3]&IN[N-1] : 1'b0;

genvar n;
generate 
	for (n=0; n<(MEML-3);n=n+1) begin : Cor1 
		or U (Temp[n+1],Temp[n],MEM[n+2]);
	end
endgenerate */

always @(posedge CLK) begin

	if (IN) OUT = 1'b1;
	else if (CYC) OUT = M;
	else zCounter = zCounter + 1'd1;

end



endmodule
	
	