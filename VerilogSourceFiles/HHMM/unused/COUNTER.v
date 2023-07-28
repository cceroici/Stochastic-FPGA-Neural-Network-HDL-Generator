
module COUNTER(CLK,in0,OUT,RESET);

parameter N = 8;


input CLK,in0,RESET;

output wire [N-1:0] OUT;

JK_FF JK_FF0(.J(1'b1),.K(1'b1),.CLK(in0&CLK),.Q(OUT[0]),.RESET(RESET));

genvar n;
generate 
	for (n=0; n<N-1;n=n+1) begin : JKFFcount 
		JK_FF JKC (.J(1'b1),.K(1'b1),.CLK(~OUT[n]),.Q(OUT[n+1]),.RESET(RESET));
	end
endgenerate


endmodule



