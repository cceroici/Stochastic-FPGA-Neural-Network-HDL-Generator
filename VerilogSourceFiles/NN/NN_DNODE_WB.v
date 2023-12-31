
// Backpropagation Node

module NN_DNODE_WB(delta,atj,zk,dalpha,dbeta,CLK,INIT);

parameter NB = 16;
parameter NN = 3;
parameter MEMSIZE = 6;

input CLK,INIT;
input [NN-1:0] atj; // ouput of jth node for each node in previous layer
input zk; // input to kth output node
input delta;

output [NN-1:0] dalpha; // output gradients of alpha transition weight (dC/dalpha)
output dbeta; // output gradients of alpha transition weight (dC/dbeta)

wire delta; // derivative of cost function wrt output state input z (dC/dzk)

genvar n;
generate 
	for (n=0; n<NN;n=n+1) begin : u
		NN_GRADCALC GRADCALC (.delta(delta),.a(atj[n]),.dalpha(dalpha[n]));
	end
endgenerate

wire a_sum;
M_OR OR0(.EN(1'b1),.IN(atj),.OUT(a_sum));
defparam OR0.N = NN;

assign dbeta = delta&a_sum;

endmodule