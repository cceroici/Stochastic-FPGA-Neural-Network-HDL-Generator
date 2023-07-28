


module SS_MAGN_POLAR(CLK,INIT,IN,SIGN_in,OUT,SIGN_out, LIMIT);

parameter N = 8;
//parameter LIMIT = 5;

input CLK, INIT;
input IN, SIGN_in;
input [7:0] LIMIT;

output OUT;
output SIGN_out;

reg SIGN_out_reg;
reg OUTreg;
reg [N-1:0] COUNTER = 1'b0;

reg SIGN_mem = 1'b0;
reg MAG_ACTIVE = 1'b0;

assign OUT = (IN) ? IN : OUTreg;
assign SIGN_out = (IN) ? SIGN_in : SIGN_out_reg;

always @(posedge INIT or posedge CLK) begin
	if (INIT) begin
		MAG_ACTIVE = 1'b0;
		COUNTER <= 1'b0;
		OUTreg <= 1'b0;
		SIGN_out_reg <= SIGN_in;
		SIGN_mem <= 1'b0;
	end else if (IN) begin
		MAG_ACTIVE = 1'b1;
		COUNTER  <= 1'b0;
		OUTreg <= 1'b1;
		SIGN_out_reg <= SIGN_in;
		SIGN_mem <= SIGN_in;
	end else if ((MAG_ACTIVE)&(COUNTER<LIMIT)) begin
		MAG_ACTIVE <= MAG_ACTIVE;
		COUNTER <= COUNTER + 1'b1;
		OUTreg <= 1'b1;
		SIGN_out_reg <= SIGN_mem;
		SIGN_mem <= SIGN_mem;
	end else if (MAG_ACTIVE) begin
		MAG_ACTIVE <= 1'b0;
		COUNTER <= 1'b0;
		OUTreg <= 1'b0;
		SIGN_out_reg <= SIGN_in;
		SIGN_mem <= 1'b0;
	end else begin
		MAG_ACTIVE <= MAG_ACTIVE;
		COUNTER <= COUNTER;
		OUTreg <= 1'b0;
		SIGN_out_reg <= SIGN_in;
		SIGN_mem <= 1'b0;
	end
end

endmodule