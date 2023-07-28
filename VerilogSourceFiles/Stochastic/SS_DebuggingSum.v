


module SS_DebuggingSum(in,sum,CLK,INIT);



parameter N = 16; // Sum size


input in;
input CLK,INIT;

output reg [N-1:0] sum = 1'd0;



always @(posedge CLK or posedge INIT) begin

	if (INIT) sum <= 1'd0;
	else sum <= sum + in;
	
end



endmodule