


module NN_NODE_SOFTMAX(a,alpha,beta,z,z_norm,a_out,CLK,INIT);

parameter N = 3;
parameter MEMSIZE = 6;

input CLK, INIT;
input [N-1:0] a,alpha;
input beta,z_norm;

output a_out,z;

wire [N-1:0] zp;
wire z;

genvar n;
generate 
	for (n=0; n<N;n=n+1) begin : NN0
		NN_InputWeight NN (.a(a[n]),.alpha(alpha[n]),.zp(zp[n]));
	end
endgenerate

M_OR OR0(.EN(1'b1),.IN(zp|beta),.OUT(z));
defparam OR0.N = N;

//BURST BURST0(.IN(z_norm),.OUT(a_out),.CLK(CLK),.INIT(INIT));
//defparam BURST0.MEMSIZE = MEMSIZE;

assign a_out = z_norm;

endmodule