// Calculate statistical mean of input stochastic sequence. Output N-bit mean at counter edge


module MEAN_POLAR(in,SIGN,out,SIGN_out,START,RESET,CLK,INIT,ENABLE,preRESET);

parameter N_input = 1;
parameter N = 8; // output mean precision
parameter N_Count = 8;

input CLK, SIGN, INIT, ENABLE;
input [N_input-1:0] in; // input stochastic sequence
input [N_Count-1:0] START; // output initialization
input RESET,preRESET; // Store new value and reset SUM

//output [N-1:0] outshift; // output (bit shifted)

//output reg [N_Count-1:0] out = 1'd0; // output sum (latched)
output reg [N_Count-1:0] out = 1'd0; // output sum (latched)
output reg SIGN_out = 1'b0;

reg [N_Count-1:0] SUM = 1'd0; // output sum
//reg RESETflag = 1'b1 /* synthesis keep */;

wire [N_Count-1:0] SUM_wire;

wire [N_Count -1:0] OVERFLOW;
assign OVERFLOW = 1'b0-1'b1;

always @(posedge CLK or posedge RESET or posedge preRESET or posedge INIT) begin
	if (INIT|(~ENABLE)) begin
		SUM <= 5'd0;
		SIGN_out <= 1'b0;
		out <= START;
	end	else if (RESET) begin
		SUM <= 5'd0;
		SIGN_out <= SIGN_out;
		out <= out;
	end	else if (preRESET) begin
		SUM <= SUM;
		SIGN_out <= SIGN_out;
		out <= SUM;
	end	else if ((~SIGN)&(~SIGN_out)&((SUM)<(OVERFLOW-in))) begin // Current input +v, current sum +v, sum not at overflow
		SUM<= SUM+in;
		SIGN_out <= SIGN_out;
		out <= out;
	end	else if ((SIGN)&(SIGN_out)&((SUM)<(OVERFLOW-in))) begin // Current input -v, current sum -v, sum not at overflow
		SUM<= SUM+in;
		SIGN_out <= SIGN_out;
		out <= out;
	end	else if ((SIGN)&(~SIGN_out)&(SUM>in)) begin // Current input -v, current sum +v, sum not at 0
		SUM<= SUM-in;
		SIGN_out <= SIGN_out;
		out <= out;
	end	else if ((~SIGN)&(SIGN_out)&(SUM>in)) begin // Current input +v, current sum -v, sum not at 0
		SUM<= SUM-in;
		SIGN_out <= SIGN_out;
		out <= out;
	end	else if ((SIGN)&(~SIGN_out)&(SUM<in)) begin // Current input -v, current sum +v, sum at 0
		SUM<= 1'b1;
		SIGN_out = 1'b1;
		out <= out;
	end	else if ((~SIGN)&(SIGN_out)&(SUM<in)) begin // Current input +v, current sum -v, sum at 0
		SUM<= 1'b1;
		SIGN_out = 1'b0;
		out <= out;
	end else begin
		SUM<= SUM;
	end
end


endmodule



