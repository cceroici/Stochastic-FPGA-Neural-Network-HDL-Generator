
module NN_NODE_POLAR(a,alpha,beta,SIGN_alpha,SIGN_beta,z,a_out,CLK,INIT);

parameter N = 4;
parameter MEMSIZE = 6;

input CLK, INIT;
input [N-1:0] a,alpha;
input [N-1:0] SIGN_alpha;
input SIGN_beta;
input beta;

output a_out,z;

wire [N-1:0] zp;
wire [N-1:0] zp_pos;
wire [N-1:0] zp_neg;
wire z_pos,z_pos_temp,z_neg,z;

genvar n;
generate 
	for (n=0; n<N;n=n+1) begin : NN0
		NN_InputWeight_POLAR NN (.a(a[n]),.alpha(alpha[n]),.SIGN_alpha(SIGN_alpha[n]), .zp_pos(zp_pos[n]),.zp_neg(zp_neg[n]));
	end
endgenerate

M_OR OR0_pos(.EN(1'b1),.IN({zp_pos,beta&(~SIGN_beta)}),.OUT(z_pos_temp));
defparam OR0_pos.N = N+1;

M_OR OR0_neg(.EN(1'b1),.IN({zp_neg,beta&(SIGN_beta)}),.OUT(z_neg));
defparam OR0_neg.N = N+1;

or OR1_pos(z_pos,z_pos_temp,d);

//M_AND AND0_neg(.EN(1'b1),.IN(zp_neg),.OUT(z_neg));
//defparam AND0_neg.N = N+1;

//assign zp = z_pos&z_neg;
 
//assign zbeta = (SIGN_beta) ? zp : zp|beta;

//assign z = (SIGN_beta) ? zbeta&(~beta) : zbeta;

assign z = z_pos&(~z_neg);

assign z = z_pos&(~z_neg);

//BURST BURST0(.IN(z),.OUT(a_out),.CLK(CLK),.INIT(INIT));
NN_BGCluster BURST0(.IN(z),.OUT(a_out),.CLK(CLK),.INIT(INIT));
defparam BURST0.MEMSIZE = MEMSIZE;

endmodule