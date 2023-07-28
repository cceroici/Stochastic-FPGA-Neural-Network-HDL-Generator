// Chris Ceroici
// August 10, 2013
//
// Equality node block with d=6
// Consists of two d=3 subnodes and one d=2 subnode with a D Flip Flop
// d=3 subnodes each have an Internal Memory initialized to 0 and the the node has an Edge Memory initialized to the channel probabilities.

module EN3(CLK,INIT,R,c,Q,SEL);

parameter EM_S = 8; // edge memory size
parameter IM_S = 1; // internal memory size
parameter LFSR_S = 8; // LFSR size

input CLK,INIT;
input [LFSR_S-1:0] SEL; // address selection bits from LFSR
input c; // channel probability
input [1:0] R; // signals from PCN

output Q; // output signal to PCN

wire [1:0] tempA;
wire [1:0] tempB;
wire D;  // input to D Flip-Flop
wire U_A,U_B; // hold or non-hold state indicator (hold: U=0, non-hold: U=1)
wire U_B_Update;
wire Rtemp; // signal to second subnode
wire [EM_S-1:0] EM_SEL;  // EM address
wire IM_OUT,EM_OUT; // IM and EM outputs
wire EM_IN; // input to EM

assign EM_SEL [EM_S-1:0] = {5'b00000,SEL[3],SEL[5],SEL[1]};

reg [1:0] DM = 2'b00; // demultiplexors


// ------------- first d=2 subnode
and #(1:2:3,1:2:3) u0(tempA[0],R[0],c);
and #(1:2:3,1:2:3) u1(tempA[1],~R[0],~c);
or #(1:2:3,1:2:3) u2(U_A,tempA[0],tempA[1]);

DFFIM DIM1(.CLK(CLK),.EN(U_A),.D(tempA[0]),.Q(IM_OUT)); // IM with U_A = ENABLE

assign Rtemp = DM[0]; 
always @(posedge CLK) begin
	if (U_A==1) DM[0] = tempA[0];
	else        DM[0] = IM_OUT;
end

// ------------- second d=2 subnode
and #(1:2:3,1:2:3) u3(tempB[0],R[1],Rtemp);
and #(1:2:3,1:2:3) u4(tempB[1],~R[1],~Rtemp);
or #(1:2:3,1:2:3) u5(U_B,tempB[0],tempB[1]);

DFF FF1(CLK,D,Q);

assign D = DM[1];
or #(1:2:3,1:2:3) u6(U_B_DM,INIT,U_B);
always @(posedge CLK) begin
	if (U_B_DM==1) DM[1] = EM_IN; 
	else           DM[1] = EM_OUT;
end

// ------------- Edge Memories 
assign EM_IN = (INIT) ? c : tempB[0];
EM EM1(.CLK(CLK),.EN(U_B_DM),.IN(EM_IN),.OUT(EM_OUT),.SEL(EM_SEL));
defparam EM1.N = EM_S;

endmodule
