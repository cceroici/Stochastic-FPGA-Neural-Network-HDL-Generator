module JKFFmod1(J, K, CLK, Q, RESET);

input J, K, CLK, RESET;

output Q;

reg Q = 1'b0;

reg Qm= 1'b0;

always @(posedge CLK or posedge RESET) begin
	if (RESET) begin
		Q <= 1'b0;
		Qm <= 1'b0;
	end 
	else if(J == 1 && K == 0)  Qm <= 1;
	else if(J == 0 && K == 1) Qm <= 0;
	else if(J == 1 && K == 1) Qm <= 1;

	Q <= Qm;

end


endmodule