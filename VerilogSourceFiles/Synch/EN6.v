// Chris Ceroici
// August 10, 2013
//
// Equality node block with d=6
// Consists of two d=3 subnodes and one d=2 subnode with a D Flip Flop
// d=3 subnodes each have an Internal Memory initialized to 0 and the the node has an Edge Memory initialized to the channel probabilities.

module EN6(CLK,INIT,R,c,Q,SEL);

parameter EM_S = 8; // edge memory size
parameter IM_S = 2; // internal memory size
parameter LFSR_S = 8; // LFSR size

input CLK,INIT;
input [LFSR_S-1:0] SEL; // address selection bits from LFSR
input c; // channel probability
input [4:0] R; // signals from PCN

output Q; // output signal to PCN

wire [1:0] tempA;
wire [1:0] tempB;
wire [1:0] tempC;
wire D;  // input to D Flip-Flop
wire U_A,U_B,U_C; // hold or non-hold state indicator (hold: U=0, non-hold: U=1)
wire U_Cp; // for initialization
wire RP_A,RP_B; // inputs to last stage (d=2)
wire IM_SEL_A,IMSEL_B;  // IM addresses
wire [EM_S-1:0] EM_SEL;  // EM address
wire IM_OUT_A,IM_OUT_B,EM_OUT; // IM and EM outputs
wire EM_IN; // input to EM

assign EM_SEL [EM_S-1:0] = {SEL[0:EM_S]};
assign IM_SEL_A = SEL[EM_S+1];
assign IM_SEL_B = SEL[EM_S+2];

reg [2:0] DM = 3'b000; // demultiplexors

// ------------- top d=3 subnode
and u1(tempA[0],R[0],R[1],R[2]);
and u2(tempA[1],~R[0],~R[1],~R[2]);
or u3(U_Ap,tempA[0],tempA[1]);

assign RP_A = DM[0];
 
always @(posedge CLK) begin
	if (U_A==1) DM[0] = tempA[0];
	else      DM[0] = IM_OUT_A;
end

// ------------- bottom d=3 subnode
and u4(tempB[0],R[3],R[4],c);
and u5(tempB[1],~R[3],~R[4],~c);
or u6(U_B,tempB[0],tempB[1]);

assign RP_B = DM[1];

always @(posedge CLK) begin
	if (U_B==1) DM[1] = tempB[0];
	else        DM[1] = IM_OUT_B;
end

// ------------- d=2 subnode
and u7(tempC[0],RP_A,RP_B);
and u8(tempC[1],~RP_A,~RP_B);
or u9(U_Cp,tempC[0],tempC[1]);
or u10(U_C,U_Cp,INIT);
dff ff1(CLK,D,Q);
assign D = DM[2];

always @(posedge CLK) begin
	if (U_C==1) DM[2] = tempC[0];
	else        DM[2] = EM_OUT;
end

// ------------- Internal Memories
EM IM_A(.CLK(CLK),.IN(tempA[0]),.OUT(IM_OUT_A),.SEL(IM_SEL_A));
defparam IM_A.N = IM_S;
EM IM_B(.CLK(CLK),.IN(tempB[0]),.OUT(IM_OUT_B),.SEL(IM_SEL_B));
defparam IM_B.N = IM_S;

// ------------- Edge Memories 
assign EM = (INIT) ? c : tempC[0];
EM EM1(.CLK(CLK),.IN(),.OUT(EM_OUT),.SEL(EM_SEL));
defparam EM1.N = EM_S;

endmodule
