// Temperal stochastic bit magnifier

module SCALE_N(IN,OUT,CLK);

parameter MemL = 16; // Memory size;

input IN,CLK;

output reg OUT;

reg [MemL-1:0] MEM = 1'b0; // Memory
reg [15:0] ZCounter = 1'b0; // Count zeros

wire [N-3:0] Temp;

or Ustart (Temp[0],G[0],G[1]);		// generate cascading XOR gates
assign OUT = (EN) ? Temp[N-3]&IN[N-1] : 1'b0;

genvar n;
generate 
	for (n=0; n<(MeEmL-3);n=n+1) begin : Cor1 
		or U (Temp[n+1],Temp[n],MEM[n+2]);
	end
endgenerate

always @(posedge CLK) begin
	if (IN) OUT = 1'b1;
	
	else OUT = M;
	
	
	M = IN;
end



endmodule
	
	