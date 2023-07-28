


module SS_MAGN(CLK,INIT,IN,OUT);

parameter N = 16;
parameter LIMIT = 5;

input CLK, INIT;
input IN;

output reg OUT;

reg [N-1:0] COUNTER = 1'b0;

reg MAG_ACTIVE = 1'b0;

always @(posedge INIT or posedge CLK) begin
	if (INIT) begin
		COUNTER <= 1'b0;
		OUT <= 1'b0;
	end else if (IN) begin
		MAG_ACTIVE = 1'b1;
		COUNTER  <= 1'b0;
		OUT <= 1'b1;
	end else if ((MAG_ACTIVE)&(COUNTER<LIMIT)) begin
		MAG_ACTIVE <= MAG_ACTIVE;
		COUNTER <= COUNTER + 1'b1;
		OUT <= 1'b1;
	end else if (MAG_ACTIVE) begin
		MAG_ACTIVE <= 1'b0;
		COUNTER <= 1'b0;
		OUT <= 1'b0;
	end else begin
		MAG_ACTIVE <= MAG_ACTIVE;
		COUNTER <= COUNTER;
		OUT <= 1'b0;
	end
end

endmodule