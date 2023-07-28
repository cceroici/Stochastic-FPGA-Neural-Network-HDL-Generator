// Temperal stochastic bit magnifier

module MAG(IN,OUT,CLK);

input IN,CLK;

output reg OUT;

reg M = 1'b0;

always @(posedge CLK) begin
	if (IN) OUT = 1'b1;
	else OUT = M;
	M = IN;
end



endmodule
	
	