// N-bit shift register




module SHFTREGN #(parameter N) (CLK,IN,REG,RESET);

//parameter N = 2;

input CLK, RESET;
input IN;

output reg [N-1:0] REG = 1'd0;


//and Ustart (Temp[0],IN[0],IN[1]);		// generate cascading XOR gates
//and Uend (OUT,Temp[N-3],IN[N-1]);
//genvar n;
//generate 
//	for (n=0; n<(N-3);n=n+1) begin : CAND1 
//		and U (Temp[n+1],Temp[n],IN[n+2]);
//	end
//endgenerate


always @(posedge CLK) begin
	
	if (RESET) REG <= 1'd0;
	else	REG <= {REG[N-2:0],IN};

end




endmodule
