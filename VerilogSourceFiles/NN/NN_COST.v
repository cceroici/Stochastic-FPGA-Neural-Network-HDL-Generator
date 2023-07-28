

// Chris Ceroici
// NN Cost calculation

module NN_COST(yk,ak,SIGN,epsD,CLK,INIT);

parameter NB = 16;

input CLK,INIT;

input [NB-1:0] yk; // triaining value for kth output node
input [NB-1:0] ak; // output of kth output node
//input [NB-1:0] R;

output SIGN; // sign of error for this output cost derivative to be applied to dalpha and dbeta
//output delta;
output [NB-1:0] epsD; // derivative of cost function wrt output (dC/dak)
//wire eps_ss;

NN_ABS_SUB SUB0(.A(ak),.B(yk),.OUT(epsD),.SIGN(SIGN));
defparam SUB0.NB = NB;

//DEC2STCH DS0(CLK,eps,R,eps_ss);
//defparam DS0.ND = NB;

//assign delta = eps_ss;

endmodule