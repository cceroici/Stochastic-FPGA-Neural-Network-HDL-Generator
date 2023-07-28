module JKFF(J, K, CLK, Q, RESET);

input J, K, CLK, RESET;

output Q;

reg Q = 1'b0;

reg Qm= 1'b0;

always @(posedge CLK or posedge RESET) begin
	if (RESET) begin
		Q <= 1'b0;
		Qm <= 1'b0;
	end 
	else if(J == 1 && K == 0) Q <= 1;
	else if(J == 0 && K == 1) Q <= 0;
	else if(J == 1 && K == 1) Q <= ~Q;
	else Q <= Q;

	Qm <= Q;

end


endmodule