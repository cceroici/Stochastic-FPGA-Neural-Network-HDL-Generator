
module NN_RNN_NODE_BGPOLAR(a, a_last, alpha, gamma, beta, SIGN_alpha, SIGN_gamma, SIGN_beta, z, a_out, d, INIT_STATE, r, CLK, INIT);

parameter N = 4; // Number of nodes in previous layer
parameter NR = 4; // Number of nodes in current layer
parameter MEMSIZE = 3;

input CLK, INIT;
input [N-1:0] a,alpha;
input [NR-1:0] a_last;
input [N-1:0] SIGN_alpha;
input SIGN_beta;
input [NR-1:0] gamma;
input [NR-1:0] SIGN_gamma;
input beta;
input d;
input INIT_STATE;
input r;

output a_out,z;


wire z_temp,SIGN_z;


SS_ADDSUB ADDSUB0(.CLK(CLK),.INIT(INIT),.IN({a&alpha,a_last&gamma,beta}),.SIGN({SIGN_alpha,SIGN_gamma,SIGN_beta}),.R_condition(r),.OUT(z_temp),.SIGN_out(SIGN_z));
defparam ADDSUB0.N = N+NR+1;
defparam ADDSUB0.DIFFCOUNTER_SIZE = 3;
defparam ADDSUB0.DIFFCOUNT_LOWERLIM = 1;

NN_ZOFFSET NN_ZOFFSET0(.CLK(CLK),.INIT(INIT),.z(z_temp),.SIGN_z(SIGN_z),.d(d),.zd(z));


/*
SS_ADDSUB ADDSUB0(.CLK(CLK),.INIT(INIT),.IN({a&alpha,a_last&gamma,beta,d}),.SIGN({SIGN_alpha,SIGN_gamma,SIGN_beta,1'b0}),.R_condition(r),.OUT(z),.SIGN_out(SIGN_z));
defparam ADDSUB0.N = N+NR+2;
defparam ADDSUB0.DIFFCOUNTER_SIZE = 4;
defparam ADDSUB0.DIFFCOUNT_LOWERLIM = 1;
*/

NN_ReLU NN_ReLU0(.z(z), .SIGN_z(SIGN_z), .a(a_out), .CLK(CLK), .INIT(INIT));
//BURST BURST0(.IN(z),.OUT(a_out),.CLK(CLK),.INIT_STATE(INIT_STATE),.INIT(INIT));
//NN_BGCluster BURST0(.IN(z),.OUT(a_out),.INIT_STATE(INIT_STATE),.CLK(CLK),.INIT(INIT));
//defparam BURST0.MEMSIZE = MEMSIZE;

endmodule