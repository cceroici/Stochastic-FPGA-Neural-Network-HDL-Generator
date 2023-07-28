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

wire tempA; // temporary signal (also output if regenerative)
wire tempB; // temporary signal
wire U,U_pulse,U_pulse_hold; // hold or non-hold state indicator (hold: U=0, non-hold: U=1)
wire [EM_S-1:0] EM_SEL;  // EM address
wire EM_IN,EM_OUT; // EM in/out

and u0(tempA,R[0],R[1],c);  // front stage (3 input) AND gates
and u1(tempB,~R[0],~R[1],~c);

or u2(U,tempA,tempB); // calculate U
not u3(U_pulse_hold,U);
and u4(U_pulse,U,U_pulse_hold);

assign EM_SEL [EM_S-1:0] = {5'b00000,SEL[3],SEL[5],SEL[1]};

assign EM_IN = (INIT) ? c : ((U&tempA[0])||((~U_B)&EM_OUT)); // initialization demux
assign Q = (U) ? EM_IN : EM_OUT; // output demux

endmodule
