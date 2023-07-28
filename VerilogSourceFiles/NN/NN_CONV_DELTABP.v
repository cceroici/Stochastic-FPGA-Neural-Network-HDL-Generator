

// Chris Ceroici
// Back Propogate Delta calculation (convolutional layer)


module NN_CONV_DELTABP(CLK,INIT,delta,SIGN_delta,SIGN_alpha,alpha,delta_out,SIGN_out,zp,R_condition);

parameter N = 3; // Number of Node in next layer

input CLK,INIT;
input [N-1:0] delta, SIGN_delta, alpha, SIGN_alpha;
input zp;
input R_condition;

output delta_out;
output SIGN_out;

wire [N-1:0] SIGN;

wire delta_out_temp;

wire zp; // z burst gate derivative w.r.t. input z

assign SIGN = SIGN_delta^SIGN_alpha;


SS_ADDSUB ADDSUB0(.CLK(CLK),.INIT(INIT),.IN(delta&alpha),.SIGN(SIGN),.OUT(delta_out_temp),.SIGN_out(SIGN_out),.R_condition(R_condition));
defparam ADDSUB0.N = N;
defparam ADDSUB0.DIFFCOUNTER_SIZE = 1;
defparam ADDSUB0.DIFFCOUNT_LOWERLIM = 0;


//NN_BGPRIME BGPRIME0(.z(z),.zp(zp),.CLK(CLK),.INIT(INIT));

assign delta_out = delta_out_temp&zp;
//assign delta_out = delta_out_temp;

endmodule