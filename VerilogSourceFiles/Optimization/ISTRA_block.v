


module ISTRA_block #(parameter N_set = 100) (CLK,INIT,x_in,x_offset_last,SIGN_x_in,SIGN_x_offset_last,HtH,SIGN_HtH,B,SIGN_B,lambda,l,x_out,SIGN_x_out, Rc);

input CLK, INIT;
input [N_set-1:0] x_in, SIGN_x_in;
input x_offset_last, SIGN_x_offset_last;
input [N_set-1:0] HtH, SIGN_HtH;
input B, SIGN_B;
input lambda,l;
input [1:0] Rc;

output x_out;
output SIGN_x_out;

wire [N_set-1:0] d;
wire [N_set-1:0] SIGN_d;
wire q;
wire SIGN_q;
wire y;
wire SIGN_y;

genvar n;
generate 
    for (n=0;n<N_set;n=n+1) begin : calc_d
        M_AND #(.N(2)) AND (.EN(1'b1),.IN({x_in[n],HtH[n]}),.OUT(d[n]));
        assign SIGN_d[n] = SIGN_x_in[n] ^ SIGN_HtH[n];
    end
endgenerate



SS_ADDSUB_CARRY_N #(.N(N_set), .N_CARRY(4), .DIFFCOUNTER_SIZE(2), .DIFFCOUNT_LOWERLIM(1)) ADDSUB_q(.CLK(CLK), .INIT(INIT), .IN(d), .SIGN(SIGN_d), .R_condition(Rc[0]), .OUT(q), .SIGN_out(SIGN_q));

SS_ADDSUB_CARRY_N #(.N(3), .N_CARRY(2), .DIFFCOUNTER_SIZE(2), .DIFFCOUNT_LOWERLIM(1)) ADDSUB_y(.CLK(CLK), .INIT(INIT), .IN({x_offset_last,B&l,q&l}), .SIGN({SIGN_x_offset_last,SIGN_B,SIGN_q}), .R_condition(Rc[1]), .OUT(y), .SIGN_out(SIGN_y));


assign x_out = y&(~lambda);

assign SIGN_x_out = SIGN_y;


endmodule