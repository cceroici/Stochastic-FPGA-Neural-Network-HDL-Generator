

// Moving average acumulator with decay factor

module SS_ACCUM #(parameter N = 16, decay_time = 4) (IN,AVG,INITIAL_AVG,CLK,INIT);

input IN;
input CLK, INIT;
input wire [N-1:0] INITIAL_AVG;

output reg [N-1:0] AVG = 1'd0-1'd1;

reg [7:0] sampleCOUNTER = 8'd0;

wire decay_time_reached;
wire at_max, at_min;
wire [N-1:0] max_val;


assign max_val = (1'b0-1'b1);
assign at_max = (AVG == max_val);
assign at_min = (AVG == 1'b0);
assign decay_time_reached = (sampleCOUNTER == decay_time);

always @(posedge CLK or posedge INIT) begin
	if (INIT) begin
		sampleCOUNTER <= 1'b0;
		AVG <= INITIAL_AVG;
	end else if (IN&(!at_max)) begin // input high, increment
		AVG <= AVG + 1'b1;
		sampleCOUNTER <= 1'b0;
	end else if (IN) begin // input high, at max
		AVG <= AVG;
		sampleCOUNTER <= 1'b0;
	end else if (decay_time_reached&(!at_min)) begin // decay reached, decrement
		AVG <= AVG - 1'b1;
		sampleCOUNTER <= 1'b0;
	end else if (decay_time_reached) begin // decay reached, at zero
		AVG <= AVG;
		sampleCOUNTER <= sampleCOUNTER;
	end else begin
		AVG <= AVG;
		sampleCOUNTER <= sampleCOUNTER + 1'b1;
	end
end



endmodule



