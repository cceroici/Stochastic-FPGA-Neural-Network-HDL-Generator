

// Chris Ceroici
// Back Propogate Through Time Delta calculation


module NN_LSTM_DELTABPTT_POLAR(
	delta_forwardLayer, delta_STATE_next, delta_c_next, delta_i_next, delta_f_next, delta_o_next,
	SIGN_delta_forwardLayer, SIGN_delta_STATE_next, SIGN_delta_c_next, SIGN_delta_i_next, SIGN_delta_f_next, SIGN_delta_o_next,
	alpha, 
	alpha_f_r, alpha_i_r, alpha_c_r, alpha_o_r,
	SIGN_alpha, 
	SIGN_alpha_f_r, SIGN_alpha_i_r, SIGN_alpha_c_r, SIGN_alpha_o_r,
	//z_forwardLayer, zp_forwardLayer,
	f, i, c, o,
	f_next, STATE_last, STATE_flat,
	zp_f, zp_i, zp_o, zp_c,
	delta_f, delta_i, delta_c, delta_o,
	SIGN_delta_f, SIGN_delta_i, SIGN_delta_c, SIGN_delta_o,
	rc,
	CLK, INIT
);

parameter N = 3; // Number of Nodes in next layer
parameter NR = 3; // Number of Nodes in current (recurrent) layer

input CLK,INIT;
input [N-1:0] delta_forwardLayer, SIGN_delta_forwardLayer;
input delta_STATE_next, SIGN_delta_STATE_next;
input STATE_last, STATE_flat;
input f_next;
input [NR-1:0] delta_c_next, delta_i_next, delta_f_next, delta_o_next;
input [NR-1:0] SIGN_delta_c_next, SIGN_delta_i_next, SIGN_delta_f_next, SIGN_delta_o_next;
input zp_f, zp_i, zp_o, zp_c;
input [1:0] rc;

input [N-1:0] alpha;
input [NR-1:0] alpha_f_r, alpha_i_r, alpha_c_r, alpha_o_r;


input [N-1:0] SIGN_alpha;
input [NR-1:0] SIGN_alpha_f_r, SIGN_alpha_i_r, SIGN_alpha_c_r, SIGN_alpha_o_r;

input f, i, c, o;
input f_next;

output delta_f, delta_i, delta_c, delta_o;
output SIGN_delta_f, SIGN_delta_i, SIGN_delta_c, SIGN_delta_o;

wire d_delta_out_next, SIGN_d_delta_out_next;
wire delta_a, delta_STATE; // delta_a = dE/d(LSTM output)
wire SIGN_delta_a, SIGN_delta_STATE;

// Calculate next time step contribution:
wire [4*NR-1:0] SIGN_d_delta_out_next_inputs;
assign SIGN_d_delta_out_next_inputs = {{delta_c_next^alpha_c_r,delta_f_next^alpha_f_r,delta_i_next^alpha_i_r,delta_o_next^alpha_o_r}};
SS_ADDSUB ADDSUB_d_delta(.CLK(CLK),.INIT(INIT),.IN({delta_c_next&alpha_c_r,delta_f_next&alpha_f_r,delta_i_next&alpha_i_r,delta_o_next&alpha_o_r}),.SIGN(SIGN_delta_a_temp_inputs),.R_condition(rc[0]),.OUT(d_delta_out_next),.SIGN_out(SIGN_d_delta_out_next));
defparam ADDSUB_d_delta.N = 4*NR;
defparam ADDSUB_d_delta.DIFFCOUNTER_SIZE = 1;
defparam ADDSUB_d_delta.DIFFCOUNT_LOWERLIM = 0;


// Calculate gradient for node output
wire [N-1:0] SIGN_delta_a_inputs;
assign SIGN_delta_a_inputs = SIGN_delta_forwardLayer^SIGN_alpha;
SS_ADDSUB ADDSUB_delta_a(.CLK(CLK),.INIT(INIT),.IN({delta_forwardLayer&alpha,d_delta_out_next}),.SIGN({SIGN_delta_a_inputs,SIGN_d_delta_out_next}),.R_condition(rc[0]),.OUT(delta_a),.SIGN_out(SIGN_delta_a));
defparam ADDSUB_delta_a.N = N+1;
defparam ADDSUB_delta_a.DIFFCOUNTER_SIZE = 1;
defparam ADDSUB_delta_a.DIFFCOUNT_LOWERLIM = 0;


// Calculate gradient for state
wire [N-1:0] SIGN_delta_STATE_inputs;
assign SIGN_delta_STATE_inputs = {SIGN_delta_a,SIGN_delta_STATE_next};
SS_ADDSUB2 ADDSUB_delta_STATE(.CLK(CLK),.INIT(INIT),.IN({delta_a&o&zp_c,delta_STATE_next&f_next}),.SIGN(SIGN_delta_STATE_inputs),.R_condition(rc[1]),.OUT(delta_STATE),.SIGN_out(SIGN_delta_STATE));
defparam ADDSUB_delta_STATE.N = 2;
defparam ADDSUB_delta_STATE.DIFFCOUNTER_SIZE = 1;
defparam ADDSUB_delta_STATE.DIFFCOUNT_MIN = 0;

// Paramater input (z) gradients
assign delta_f = delta_STATE&STATE_last&zp_i;
assign delta_c = delta_STATE&i&zp_c;
assign delta_i = delta_STATE&c&zp_i;
assign delta_o = delta_a&STATE_flat&zp_o;

assign SIGN_delta_f = SIGN_delta_STATE;
assign SIGN_delta_c = SIGN_delta_STATE;
assign SIGN_delta_i = SIGN_delta_STATE;
assign SIGN_delta_o = SIGN_delta_a;



endmodule