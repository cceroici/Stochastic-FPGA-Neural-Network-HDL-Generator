	
// Chris Ceroici
// Smooth a signed stochastic stream with a smoothing coefficient NS
	
module SS_SMOOTHSIGNED(CLK,INIT,IN,SIGN,OUT);

parameter NS = 1;

input CLK, INIT;
input IN, SIGN;

output reg OUT = 1'b0; 

wire [NS-1:0] REGpos, REGneg;

reg [NS-1:0] ONSig = 1'd0 - 1'd1;

SHFTREGN SHFTREGN_pos(.CLK(CLK),.IN(IN&(~SIGN)),.REG(REGpos),.RESET(INIT));
defparam SHFTREGN_pos.N = NS;

SHFTREGN SHFTREGN_neg(.CLK(CLK),.IN(IN&SIGN),.REG(REGneg),.RESET(INIT));
defparam SHFTREGN_neg.N = NS;

always @(posedge INIT or posedge CLK) begin
	
	if (INIT) OUT <= 1'b0;
	else if ((REGpos==ONSig)|(REGneg==ONSig)) OUT <= 1'b1;
	else OUT <= 1'b0;

end 

endmodule