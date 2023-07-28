// Chris Ceroici
// August 10, 2013
//
// Equality node block with d=6
// Consists of two d=3 subnodes and one d=2 subnode with a D Flip Flop
// d=3 subnodes each have an Internal Memory initialized to 0 and the the node has an Edge Memory initialized to the channel probabilities.

module EN3(CLK_D2S,INIT,R,c,Q,EM_SEL);

parameter EM_S = 8; // edge memory size
parameter D_EN = 1; // Gate delay

input CLK_D2S,INIT;
input [2:0] EM_SEL; // address selection bits from LFSR
input c; // channel probability
input [1:0] R; // signals from PCN

output Q; // output signal to PCN

wire tempA, tempB;
wire U; // hold or non-hold state indicator (hold: U=0, non-hold: U=1)
wire EM_update, U_pulse, U_pulsetemp /* synthesis keep */; 
wire EM_IN,EM_OUT; // EM input/output
wire EM_CLK; // initialization clock
wire Q_temp;

// ------------- Demultiplexors

DEMUX DEMUX0(INIT,{c,tempA},EM_IN); // EM initialization
defparam DEMUX0.NR = 2;
defparam DEMUX0.NS = 1;

DEMUX DEMUX1(U|INIT,{EM_IN,EM_OUT},Q_temp); // output of VN
defparam DEMUX1.NR = 2;
defparam DEMUX1.NS = 1;

DEMUX DEMUX2(INIT,{CLK_D2S,U_pulse},EM_update); // EM update controller
defparam DEMUX2.NR = 2;
defparam DEMUX2.NS = 1;

// ------------- VN Combinational logic

and u0(tempA,R[0],R[1],c);
and u1(tempB,~R[0],~R[1],~c);

or u2(U,tempA,tempB); // calculate hold or non-hold state

// ------------- EM and initializtion

not #1 u3(U_pulsetemp,U); // pulse generator
and u4(U_pulse,U,U_pulsetemp); 

//and u5(EM_CLK,CLK_D2S,INIT); // initialization clock
//or u6(EM_update,U_pulse,EM_CLK);

EM EM1(.TRIG(EM_update),.IN(EM_IN),.OUT(EM_OUT),.SEL(EM_SEL));

assign #D_EN Q = Q_temp;


endmodule
