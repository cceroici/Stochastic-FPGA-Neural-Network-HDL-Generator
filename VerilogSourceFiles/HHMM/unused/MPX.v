// N bit multiplexor

module MPX(EN,SEL,D,Q);

parameter NS = 2; // number of states other than "END"

input EN;
input [NS-1:0] SEL;
input [NS-1:0] D;

output Q;

assign Q = (EN) ? D[(~SEL[0])&SEL[1]] : 1'b0;
	
endmodule