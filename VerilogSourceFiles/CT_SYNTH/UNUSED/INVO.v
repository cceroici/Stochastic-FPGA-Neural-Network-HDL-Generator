// Chris Ceroici
// Nov. 7 2013
// 
// Inverting oscillator with variable number of gates


module INVO(CLK_ext,INIT,CLKout);

parameter N = 10; // Number of delay gates
parameter INV_D = 5; // inverter delay 
parameter CLK_T = 5; // Simulation clock period (/2)

input INIT;
input CLK_ext; // external clock for measurement of oscillator frequency

//output reg CLKout = 1'b0;
output CLKout;

//always #CLK_T CLKout = ~CLKout;


wire [N-1:0] Temp /* synthesis keep */;

wire A /* synthesis keep */;
assign CLKout = A;
assign #INV_D Temp[0] = ~A;

genvar n;
generate 
	for (n=0; n<(N-1);n=n+1) begin : CNOT1 
		assign #INV_D Temp[n+1] = ~Temp[n];
	end
endgenerate

assign #INV_D A = (INIT) ? 1'b1 : ~Temp[N-1];
//assign #INV_D A = ~Temp[N-1];


/* // Frequency measurement:
output reg [31:0] Acount = 32'd0;
output reg [31:0] CLKcount = 32'd0;
always @(posedge CLK_ext) begin
	count = count +1'b1;
	if (count==8'd5) INIT = 1'b0;
	if (~INIT) CLKcount = CLKcount + 1'b1;
end
always @(posedge A) begin 
	if (~INIT) Acount = Acount +1'b1;
end
*/
	
endmodule
