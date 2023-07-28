
module NN_NODE_BGPOLAR_TRANS(a,alpha,beta,SIGN_alpha,SIGN_beta,z,a_out,d,INIT_STATE,r,CLK,INIT);

parameter N = 4;
parameter MEMSIZE = 3;

input CLK, INIT;
input [N-1:0] a,alpha;
input [N-1:0] SIGN_alpha;
input SIGN_beta;
input beta;
input d;
input INIT_STATE;
input r;

output a_out,z;

//wire [N-1:0] zp;
wire [N-1:0] zp_pos;
wire [N-1:0] zp_neg;
wire z_pos,z_neg,z_temp,SIGN_z;

genvar n;
generate 
	for (n=0; n<N;n=n+1) begin : NN0
		NN_InputWeight_POLAR NN (.a(a[n]),.alpha(alpha[n]),.SIGN_alpha(SIGN_alpha[n]), .zp_pos(zp_pos[n]), .zp_neg(zp_neg[n]));
	end
endgenerate

M_OR OR0_pos(.EN(1'b1),.IN({zp_pos,beta&(~SIGN_beta)}),.OUT(z_pos));
defparam OR0_pos.N = N+1;

M_OR OR0_neg(.EN(1'b1),.IN({zp_neg,beta&(SIGN_beta)}),.OUT(z_neg));
defparam OR0_neg.N = N+1;

SS_ADDSUB ADDSUB0(.CLK(CLK),.INIT(INIT),.IN({a&alpha,beta}),.SIGN({SIGN_alpha,SIGN_beta}),.R_condition(r),.OUT(z_temp),.SIGN_out(SIGN_z));
defparam ADDSUB0.N = N+1;
defparam ADDSUB0.DIFFCOUNTER_SIZE = 2;
defparam ADDSUB0.DIFFCOUNT_LOWERLIM = 1;

NN_ZOFFSET NN_ZOFFSET0(.CLK(CLK),.INIT(INIT),.z(z_temp),.SIGN_z(SIGN_z),.d(d),.zd(z));

NN_BGT BURSTT0(.IN(z),.OUT(a_out),.CLK(CLK),.INIT(INIT));
defparam BURSTT0.MEMSIZE = 6;
defparam BURSTT0.PULSE_DURATION = 80;
defparam BURSTT0.REFRACTORY_DURATION = 10;
defparam BURSTT0.COUNTER_SIZE = 8;

endmodule