



module BUFFER_REGISTER(CLK,INIT,IN,BUF);

parameter N = 3; // buffer size

input CLK, INIT;
input IN;

output reg [N-1:0] BUF = 1'd0;



always @(posedge CLK or posedge INIT) begin
	if (INIT) BUF = 1'd0;
	else BUF <= {BUF[N-1:0],IN};
end

endmodule