

// Chris Ceroici
// Back Propogate Delta calculation


module NN_DELTABP(CLK,INIT,delta,SIGN,alpha,delta_out,SIGN_out);

parameter N = 3; // Number of Node in next layer

input CLK,INIT;
input [N-1:0] delta, SIGN, alpha;

output delta_out;
output SIGN_out;



SS_ADDSUB ADDSUB0(.CLK(CLK),.INIT(INIT),.IN(delta&alpha),.SIGN(SIGN),.OUT(delta_out),.SIGN_out(SIGN_out));
defparam ADDSUB0.N = N;
//defparam ADDSUB0.POLARITY_COUNT_SIZE = 8;




endmodule