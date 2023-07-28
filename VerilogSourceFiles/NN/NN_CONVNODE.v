
// Chris Ceroici
// 5/22/1017

// Convolution node for input window>2

module NN_CONVNODE(
	a,
	a_MEM_ACTIVE,
	d,
	alpha,
	beta,
	SIGN_alpha,
	SIGN_beta,
	z,zp,
	a_out,
	r,
	CLK, INIT
);

parameter N = 4; // Size of node window

input [N-1:0] a;
input a_MEM_ACTIVE;
input d;
input [N-1:0] alpha;
input beta;
input [N-1:0] SIGN_alpha;
input SIGN_beta;
input CLK, INIT;
input r;

output a_out,z,zp;

wire [N-1:0] zp_pos;
wire [N-1:0] zp_neg;
wire z_pos,z_neg,z_temp,SIGN_z;

//assign zp = ~SIGN_z;
//assign zp = 1'b1;
NN_BGPRIME BGPRIME_0(.z(z),.zp(zp),.CLK(CLK),.INIT(INIT));

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

//SS_ADDSUB2 ADDSUB2_0(.CLK(CLK),.INIT(INIT),.IN({z_pos,z_neg}),.SIGN({1'b0,1'b1}),.OUT(z_temp),.SIGN_out(SIGN_z));

SS_ADDSUB ADDSUB0(.CLK(CLK),.INIT(INIT),.IN({a&alpha,beta}),.SIGN({SIGN_alpha,SIGN_beta}),.R_condition(r),.OUT(z_temp),.SIGN_out(SIGN_z));
defparam ADDSUB0.N = N+1;
defparam ADDSUB0.DIFFCOUNTER_SIZE = 2;
defparam ADDSUB0.DIFFCOUNT_LOWERLIM = 1;

NN_ZOFFSET NN_ZOFFSET0(.CLK(CLK),.INIT(INIT),.z(z_temp),.SIGN_z(SIGN_z),.d(d),.zd(z));

NN_ReLU NN_ReLU0(.z(z), .SIGN_z(SIGN_z), .a(a_out), .CLK(CLK), .INIT(INIT));
//NN_BGCluster BURST0(.IN(z),.OUT(a_out),.INIT_STATE(a_MEM_ACTIVE),.CLK(CLK),.INIT(INIT));
//defparam BURST0.MEMSIZE = 3;


endmodule