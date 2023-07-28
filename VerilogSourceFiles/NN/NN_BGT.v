// Chris Ceroici

// Spike Burst Gate Operator

module NN_BGT(IN,OUT,CLK,INIT);

//parameter THRESHOLD_LEVEL = 4;
parameter PULSE_DURATION = 10;
parameter REFRACTORY_DURATION = 10;
parameter MEMSIZE = 8;
parameter COUNTER_SIZE = 8;


input IN;
input CLK, INIT;

output reg OUT;

reg MODE = 1'b1; // output mode
reg REFRACTORY_PERIOD = 1'b0;

reg [MEMSIZE-1:0] MEM = 1'b0;
reg [COUNTER_SIZE-1:0] COUNTER = 1'd0;
reg [COUNTER_SIZE-1:0] REFRACTORY_COUNTER = 1'd0;

wire COUNTER_ATMAX;
wire REFRACTORY_COUNTER_ATMAX;

reg [7:0] MEM_COUNTER = 1'b0;
wire [7:0] MEM_COUNTER_MAXVAL;
wire MEM_COUNTER_ATMAX, MEM_COUNTER_ATMIN;
reg [7:0] THRESHOLD_LEVEL = 8'd250;

assign MEM_COUNTER_MAXVAL = 1'b0 - 1'b1;
assign MEM_COUNTER_ATMAX = (MEM_COUNTER == MEM_COUNTER_MAXVAL);
assign MEM_COUNTER_ATMIN = (MEM_COUNTER == 1'b0);

assign COUNTER_ATMAX = (COUNTER >= PULSE_DURATION);
assign REFRACTORY_COUNTER_ATMAX = (REFRACTORY_COUNTER >= REFRACTORY_DURATION);

M_AND AND0(.EN(1'b1),.IN(MEM),.OUT(THRESHOLD_REACHED));
defparam AND0.N = MEMSIZE;

//assign THRESHOLD_REACHED = (MEM_COUNTER > THRESHOLD_LEVEL);

always @(posedge INIT or posedge CLK) begin
	if (INIT) begin 
		COUNTER <= 1'b0;
		REFRACTORY_COUNTER <= 1'b0;
		OUT <= 1'b0;
		REFRACTORY_PERIOD <= 1'b0;
	end
	else if (REFRACTORY_COUNTER_ATMAX) begin
		OUT <= 1'b0;
		REFRACTORY_COUNTER <= 1'b0;
		REFRACTORY_PERIOD <= 1'b0;
		COUNTER <= 1'b0;
	end
	else if (REFRACTORY_PERIOD) begin
		OUT <= 1'b0;
		REFRACTORY_COUNTER <= REFRACTORY_COUNTER + 1'b1;
		COUNTER <= 1'b0;
	end
	else if (COUNTER_ATMAX) begin
		COUNTER <= 1'b0;
		OUT <= 1'b0;
		REFRACTORY_PERIOD <= 1'b1;
		REFRACTORY_COUNTER <= 1'b0;
	end
	else if (OUT) begin
		COUNTER <= COUNTER + 1'b1;
	end
	else if (THRESHOLD_REACHED) begin
		OUT <= 1'b1;
		COUNTER <= 1'b0;
	end
	else begin
		OUT <= 1'b0;
	end
end

always @(posedge INIT or posedge CLK) begin
	if (INIT) begin 
		MEM <= 1'd0;
	end
	else begin
		MEM <= {MEM[MEMSIZE-2:0],IN};
	end
end
/*
always @(posedge INIT or posedge CLK) begin
	if (INIT) begin 
		MEM_COUNTER <= 1'd0;
	end
	else if (IN & (!MEM_COUNTER_ATMAX) & (!THRESHOLD_REACHED)) begin
		MEM_COUNTER <= MEM_COUNTER + 1'b1;
	end
	else if (!IN & (!MEM_COUNTER_ATMIN)) begin
		MEM_COUNTER <= MEM_COUNTER - 1'b1;
	end
	else MEM_COUNTER <= MEM_COUNTER;
end

*/
endmodule