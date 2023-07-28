// Temperal stochastic bit magnifier

module MAG2(IN,OUT,CLK);

input IN,CLK;

output reg OUT;

reg M0 = 1'b0;
reg M1 = 1'b0;

always @(posedge CLK) begin
	if (IN) OUT = 1'b1;
	else if (M0) OUT = 1'b1;
	else OUT = M1;
	M1 = M0;
	M0 = IN;
end



endmodule
	
	