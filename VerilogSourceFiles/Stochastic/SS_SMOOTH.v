
module SS_SMOOTH #(parameter dist = 4) (CLK,INIT,IN,SIGN_IN,OUT,SIGN_OUT);

input CLK, INIT;
input IN, SIGN_IN;

output reg OUT;
output reg SIGN_OUT;

wire [dist-1:0] maxVal;

reg [dist-1:0] value_change_counter;
reg [dist-1:0] sign_change_counter;

assign maxVal = 1'b0-1'b1;

always @(posedge CLK or posedge INIT) begin
	if (INIT) begin 
		OUT <= 1'b0;
		value_change_counter <= 1'b0;
	end else if (IN == OUT) begin
		OUT <= OUT;
		value_change_counter <= 1'b0;
	end else if ((IN != OUT)&(value_change_counter==maxVal)) begin
		OUT <= IN;
		value_change_counter <= 1'b0;
	end else begin
		OUT <= OUT;
		value_change_counter <= value_change_counter + 1'b1;
	end
end

always @(posedge CLK or posedge INIT) begin
	if (INIT) begin 
		SIGN_OUT <= 1'b0;
		sign_change_counter <= 1'b0;
	end else if (SIGN_IN == SIGN_OUT) begin
		SIGN_OUT <= SIGN_OUT;
		sign_change_counter <= 1'b0;
	end else if ((SIGN_IN != SIGN_OUT)&(sign_change_counter==maxVal)) begin
		SIGN_OUT <= SIGN_IN;
		sign_change_counter <= 1'b0;
	end else begin
		SIGN_OUT <= SIGN_OUT;
		sign_change_counter <= sign_change_counter + 1'b1;
	end
end

endmodule
