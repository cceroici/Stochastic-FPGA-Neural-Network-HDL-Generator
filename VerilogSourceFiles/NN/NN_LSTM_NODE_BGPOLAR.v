
module NN_LSTM_NODE_BGPOLAR(
	a, a_last,
	a_out,
 	alpha_f_x, alpha_i_x, alpha_c_x, alpha_o_x, 
	alpha_f_r, alpha_i_r, alpha_c_r, alpha_o_r,
	beta_f, beta_i, beta_c, beta_o,
	SIGN_alpha_f_x, SIGN_alpha_i_x, SIGN_alpha_c_x, SIGN_alpha_o_x,
	SIGN_alpha_f_r, SIGN_alpha_i_r, SIGN_alpha_c_r, SIGN_alpha_o_r,
	SIGN_beta_f, SIGN_beta_i, SIGN_beta_c, SIGN_beta_o,
 	f, i, c, o, STATE, STATE_new_flat,
 	z_f, z_i, z_c, z_o,
 	SIGN_z_f, SIGN_z_i, SIGN_z_c, SIGN_z_o,
 	//INIT_STATE_c, INIT_STATE_a_STATE,
 	z_offset, rc,
 	CLK, INIT
);

parameter N = 4; // Number of nodes in previous layer
parameter NR = 4; // Number of nodes in current layer
parameter MEMSIZE = 3;

input CLK, INIT;
input [N-1:0] a;
input [NR-1:0] a_last;
input [3:0] z_offset;
input [4:0] rc;
//input INIT_STATE_c;
//input INIT_STATE_a_STATE;


// **** Node inputs ****
input [N-1:0] a;
input [NR-1:0] a_last;

// **** Node weights ****
input [N-1:0] alpha_f_x, alpha_i_x, alpha_c_x, alpha_o_x;
input [NR-1:0] alpha_f_r, alpha_i_r, alpha_c_r, alpha_o_r;
input beta_f, beta_i, beta_c, beta_o;

input [N-1:0] SIGN_alpha_f_x, SIGN_alpha_i_x, SIGN_alpha_c_x, SIGN_alpha_o_x;
input [NR-1:0] SIGN_alpha_f_r, SIGN_alpha_i_r, SIGN_alpha_c_r, SIGN_alpha_o_r;
input SIGN_beta_f, SIGN_beta_i, SIGN_beta_c, SIGN_beta_o;
// **********************

output reg a_out = 1'd0;
output wire f, i, c, o;
output wire z_f, z_i, z_c, z_o;
output wire SIGN_z_f, SIGN_z_i, SIGN_z_c, SIGN_z_o;
output wire STATE_new_flat;
output reg STATE = 1'b0;

wire z_f_temp, z_i_temp, z_c_temp, z_o_temp;
wire a_out_new, STATE_new;

// *** Forget
SS_ADDSUB ADDSUB_z_f(.CLK(CLK),.INIT(INIT),.IN({a&alpha_f_x,a_last&alpha_f_r,beta_f}),.SIGN({SIGN_alpha_f_x,SIGN_alpha_f_r,SIGN_beta_f}),.R_condition(rc[0]),.OUT(z_f_temp),.SIGN_out(SIGN_z_f));
defparam ADDSUB_z_f.N = N+NR+1;
defparam ADDSUB_z_f.DIFFCOUNTER_SIZE = 2;
defparam ADDSUB_z_f.DIFFCOUNT_LOWERLIM = 1;
NN_ZOFFSET NN_ZOFFSET_z_f(.CLK(CLK),.INIT(INIT),.z(z_f_temp),.SIGN_z(SIGN_z_f),.d(z_offset[0]),.zd(z_f));

NN_ReLU NN_ReLU_f(.z(z_f), .SIGN_z(SIGN_z_f), .a(f), .CLK(CLK), .INIT(INIT));
//NN_BGCluster BURST_f(.IN(z_f),.OUT(f),.INIT_STATE(INIT_STATE[0]),.CLK(CLK),.INIT(INIT));
//defparam BURST_f.MEMSIZE = MEMSIZE;

// *** Remember
SS_ADDSUB ADDSUB_z_i(.CLK(CLK),.INIT(INIT),.IN({a&alpha_i_x,a_last&alpha_i_r,beta_i}),.SIGN({SIGN_alpha_i_x,SIGN_alpha_i_r,SIGN_beta_i}),.R_condition(rc[1]),.OUT(z_i_temp),.SIGN_out(SIGN_z_i));
defparam ADDSUB_z_i.N = N+NR+1;
defparam ADDSUB_z_i.DIFFCOUNTER_SIZE = 2;
defparam ADDSUB_z_i.DIFFCOUNT_LOWERLIM = 1;
NN_ZOFFSET NN_ZOFFSET_z_i(.CLK(CLK),.INIT(INIT),.z(z_i_temp),.SIGN_z(SIGN_z_i),.d(z_offset[1]),.zd(z_i));

NN_ReLU NN_ReLU_i(.z(z_i), .SIGN_z(SIGN_z_i), .a(i), .CLK(CLK), .INIT(INIT));
//NN_BGCluster BURST_i(.IN(z_i),.OUT(i),.INIT_STATE(INIT_STATE[1]),.CLK(CLK),.INIT(INIT));
//defparam BURST_i.MEMSIZE = MEMSIZE;

// *** Recall
SS_ADDSUB ADDSUB_z_o(.CLK(CLK),.INIT(INIT),.IN({a&alpha_o_x,a_last&alpha_o_r,beta_o}),.SIGN({SIGN_alpha_o_x,SIGN_alpha_o_r,SIGN_beta_o}),.R_condition(rc[2]),.OUT(z_o_temp),.SIGN_out(SIGN_z_o));
defparam ADDSUB_z_o.N = N+NR+1;
defparam ADDSUB_z_o.DIFFCOUNTER_SIZE = 2;
defparam ADDSUB_z_o.DIFFCOUNT_LOWERLIM = 1;
NN_ZOFFSET NN_ZOFFSET_z_o(.CLK(CLK),.INIT(INIT),.z(z_o_temp),.SIGN_z(SIGN_z_o),.d(z_offset[2]),.zd(z_o));

NN_ReLU NN_ReLU_o(.z(z_i), .SIGN_z(SIGN_z_o), .a(o), .CLK(CLK), .INIT(INIT));
//NN_BGCluster BURST_o(.IN(z_o),.OUT(o),.INIT_STATE(INIT_STATE[3]),.CLK(CLK),.INIT(INIT));
//defparam BURST_o.MEMSIZE = MEMSIZE;

// *** Store Memory
SS_ADDSUB ADDSUB_z_c(.CLK(CLK),.INIT(INIT),.IN({a&alpha_c_x,a_last&alpha_c_r,beta_c}),.SIGN({SIGN_alpha_c_x,SIGN_alpha_c_r,SIGN_beta_c}),.R_condition(rc[3]),.OUT(z_c_temp),.SIGN_out(SIGN_z_c));
defparam ADDSUB_z_c.N = N+NR+1;
defparam ADDSUB_z_c.DIFFCOUNTER_SIZE = 2;
defparam ADDSUB_z_c.DIFFCOUNT_LOWERLIM = 1;
NN_ZOFFSET NN_ZOFFSET_z_c(.CLK(CLK),.INIT(INIT),.z(z_c_temp),.SIGN_z(SIGN_z_c),.d(z_offset[3]),.zd(z_c));

NN_ReLU NN_ReLU_c(.z(z_c), .SIGN_z(SIGN_z_c), .a(c), .CLK(CLK), .INIT(INIT));
//NN_BGCluster BURST_c(.IN(z_c),.OUT(c),.INIT_STATE(INIT_STATE_c),.CLK(CLK),.INIT(INIT));
//defparam BURST_c.MEMSIZE = MEMSIZE;


always @(posedge CLK or posedge INIT) begin
	if (INIT) begin
		STATE <= 1'b0;
		a_out <= 1'b0;
	end
	else begin
		STATE <= STATE_new;
		a_out <= a_out_new;
	end
end

wire dummy;
SS_ADDSUB2 ADDSUB_STATE(.CLK(CLK),.INIT(INIT),.IN({f&STATE,i&c}),.SIGN({1'b0,1'b0}),.R_condition(rc[4]),.OUT(STATE_new),.SIGN_out(dummy));
defparam ADDSUB_STATE.DIFFCOUNTER_SIZE = 2;
defparam ADDSUB_STATE.DIFFCOUNT_MIN = 1;

NN_ReLU NN_ReLU_STATE(.z(STATE_new), .SIGN_z(dummy), .a(STATE_new_flat), .CLK(CLK), .INIT(INIT));
assign a_out_new = STATE_new_flat & o;
//NN_BGCluster BURST_a_out(.IN(a_out_new_STATE),.OUT(a_out_new_STATE),.INIT_STATE(INIT_STATE_a_STATE),.CLK(CLK),.INIT(INIT));
//defparam BURST_a_out.MEMSIZE = MEMSIZE;
//assign a_out_new = c&a_out_new_STATE;

//NN_ReLU NN_ReLU0(.z(z), .SIGN_z(SIGN_z), .a(a_out), .CLK(CLK), .INIT(INIT));
//BURST BURST0(.IN(z),.OUT(a_out),.CLK(CLK),.INIT_STATE(INIT_STATE),.INIT(INIT));
//NN_BGCluster BURST0(.IN(z),.OUT(a_out),.INIT_STATE(INIT_STATE),.CLK(CLK),.INIT(INIT));
//defparam BURST0.MEMSIZE = MEMSIZE;






endmodule