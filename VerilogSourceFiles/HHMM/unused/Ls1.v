// Size 0 Level

module Ls1(CLK,INIT,BV,P0,Pe,S0,T);

parameter Del0 = 0;
parameter Del1 = 0;
parameter Del2 = 0;
parameter Del3 = 0;
parameter Del4 = 0;

input CLK,INIT;
input P0,Pe;
input [1:0] BV;

output reg S0 = 1'b0;
output reg T = 1'b0;

reg DEC = 1'b0;
reg S0mem = 1'b0;

wire P0,Pe /* synthesis keep */;


always @(posedge CLK) begin
	
	case (BV)
		0 : begin // sleep (no activation)
				S0 = 1'b0;
				T = 1'b0;
			end
		1 : begin // search state
				#Del0 DEC = (P0^Pe)&(~INIT)&(~T);
				#Del2 S0 = ((S0)&(~Pe))	|	((P0)&(S0|~Pe));	// looks for P0 = 1 and Pe = 0 while preserving S0
				#Del3 T  = (~P0)&(Pe);
			end
		2 : begin // sleep (sub-state active)
				S0 = 1'b0;
			end
		3 : begin // initialization
				#Del0 S0 = 1'b0;
				#Del1 T = 1'b0;
			end
	endcase
	
end

always @(posedge DEC) #Del4 S0mem=S0;
	
endmodule