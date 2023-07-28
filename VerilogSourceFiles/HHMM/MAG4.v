// Temperal stochastic bit magnifier

module MAG4(IN,OUT,CLK);

input IN,CLK;

output reg OUT;

reg [3:0] MEM = 1'b0;


always @(posedge CLK) begin
	if (IN) OUT = 1'b1;
	else if (MEM[0]) OUT = 1'b1;
	else if (MEM[1]) OUT = 1'b1;
	else if (MEM[2]) OUT = 1'b1;
	else OUT = MEM[3];
	MEM = {IN,MEM[3:1]};
end



endmodule
	
	