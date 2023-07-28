

// Chris Ceroici
// Back Propogate Through Time Delta calculation


module NN_RNN_DELTABPTT_POLAR(CLK,INIT,R_condition,delta,delta_next,SIGN_delta,SIGN_delta_next,SIGN_alpha,alpha,SIGN_gamma, gamma, delta_out,SIGN_out,z,zp);

parameter N = 3; // Number of Nodes in next layer
parameter NR = 3; // Number of Nodes in current (recurrent) layer

input CLK,INIT;
input [N-1:0] delta, SIGN_delta, alpha, SIGN_alpha;
input [NR-1:0] delta_next, SIGN_delta_next, gamma, SIGN_gamma;
input z,zp;
input R_condition;

output delta_out;
output SIGN_out;

wire [N+NR-1:0] SIGN;

wire delta_out_temp;

wire zp; // z burst gate derivative w.r.t. input z

assign SIGN = {SIGN_delta^SIGN_alpha,SIGN_delta_next^SIGN_gamma};

SS_ADDSUB ADDSUB0(.CLK(CLK),.INIT(INIT),.IN({delta&alpha,delta_next&gamma}),.SIGN(SIGN),.R_condition(R_condition),.OUT(delta_out_temp),.SIGN_out(SIGN_out));
defparam ADDSUB0.N = N+NR;
defparam ADDSUB0.DIFFCOUNTER_SIZE = 1;
defparam ADDSUB0.DIFFCOUNT_LOWERLIM = 0;

//NN_BGPRIME BGPRIME0(.z(z),.zp(zp),.CLK(CLK),.INIT(INIT));

assign delta_out = delta_out_temp&zp;
//assign delta_out = delta_out_temp;

endmodule