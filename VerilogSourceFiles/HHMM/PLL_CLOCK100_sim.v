
// Simulated CLOCK PLL for 50 MHz -> 100 MHz

module PLL_CLOCK100_sim(
	areset,
	inclk0,
	c0
);

input areset;
input inclk0;

output reg c0 = 1'd0;

// --------------------- Clock ---------------------
always #15 c0 = ~c0; // Clock for D/S converter
// ------------------------------------------------




endmodule

