


module NN_NODE3(a,alpha,beta,a_out,CLK);

parameter N = 3;
parameter MEMSIZE = 6;

input CLK;
input [N-1:0] a,alpha,beta;

output a_out;

wire [N-1:0] zp;
wire z;

NN_InputWeight NN_in0 (.a(a[0]),.alpha(alpha[0]),.beta(beta[0]),.zp(zp[0]));
NN_InputWeight NN_in1 (.a(a[1]),.alpha(alpha[1]),.beta(beta[1]),.zp(zp[1]));
NN_InputWeight NN_in2 (.a(a[2]),.alpha(alpha[2]),.beta(beta[2]),.zp(zp[2]));

M_OR OR0(.EN(1'b1),.IN(zp),.OUT(z));
defparam OR0.N = N;

BURST BURST0(.IN(z),.OUT(a_out),.CLK(CLK));
defparam BURST0.MEMSIZE = MEMSIZE;

endmodule