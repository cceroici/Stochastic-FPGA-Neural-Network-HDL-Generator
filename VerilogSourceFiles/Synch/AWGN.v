// Generates AWGN with sigma = 9.27*SIG;
// Outputs two 16-bit independent GNs
//
// Chris Ceroici
// Oct 10, 2013

module AWGN_GEN(TRIG,r0,r1,G0,G1);

parameter NO = 8; 

input TRIG;
input [NO-1:0] r0,r1;   // LFSR output

output [NO-1:0] G0,G1;

wire [NO-1:0] t0,t1,t2,t3,t4;

LUT_AWGN_LOG LUT0(r0,t0);
LUT_AWGN_COS LUT1(r1,t1);
LUT_AWGN_SIN LUT2(r1,t2);
LUT_AWGN_LLR LUT3(t3,G0);
LUT_AWGN_LLR LUT4(t4,G1);

assign t3 = (t0[NO-1]^t1[NO-1]) ? {1'b1,t0[NO-2:0]*t1[NO-2:0]} : {1'b0,t0[NO-2:0]*t1[NO-2:0]};
assign t4 = (t0[NO-1]^t2[NO-1]) ? {1'b1,t0[NO-2:0]*t2[NO-2:0]} : {1'b0,t0[NO-2:0]*t2[NO-2:0]};

endmodule