
// Backpropagation Node

module NN_RNN_DNODE(delta,atj,atj_last,zk,dalpha,dgamma,dbeta,CLK,INIT);

parameter NB = 16; // Bit precision
parameter NN = 3; // Number of nodes in previous layer
parameter NR = 3; // Number of nodes in current layer
parameter MEMSIZE = 6;

input CLK,INIT;
input [NN-1:0] atj; // ouput of jth node for each node in previous layer
input [NR-1:0] atj_last; // ouput of jth node for current layer in previous time step
input zk; // input to kth output node
input delta;

output [NN-1:0] dalpha; // output gradients of alpha transition weight (dC/dalpha)
output [NR-1:0] dgamma;
output dbeta; // output gradients of alpha transition weight (dC/dbeta)

wire delta; // derivative of cost function wrt output state input z (dC/dzk)

genvar n;
generate 
	for (n=0; n<NN;n=n+1) begin : u_dalpha
		NN_GRADCALC GRADCALC (.delta(delta),.a(atj[n]),.dalpha(dalpha[n]));
	end
endgenerate

generate 
	for (n=0; n<NR;n=n+1) begin : u_dgamma
		NN_GRADCALC GRADCALC (.delta(delta),.a(atj_last[n]),.dalpha(dgamma[n]));
	end
endgenerate

assign dbeta = delta;

endmodule