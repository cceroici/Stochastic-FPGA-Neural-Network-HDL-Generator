// Chris Ceroici
// August 27, 2013
//
// Up/Down counter for hard decision during LDPC decoding
//
// goes from -sat -> +sat
// increments when 1 received
// decrements when 0 received
// decision made based on sign bit:
// 0 sign bit -> output = +1
// 1 sign bit -> output = -1

module UDCOUNTER(CLK,INIT,BitIN,BitOUT);

parameter psat = 7;
parameter nsat = 7;
parameter CSize = 4;

input CLK,INIT,Bit;
integer i;

reg [CSize-1:0] Count;

output BitOUT;

assign BitOUT = Count[Csize-1];

always @(posedge INIT) for (i=0;i<CSize;i=i+1) Count[i] <= 1'b0;

always @(posedge CLK) begin
	if (Bit==0) begin
		if (Count!=4'b1001) begin
			Count <= Count - 1'b1;
		end
	end
	else if (Count!=4'b0111) begin
			Count <= Count +1'b1;
	end
end

endmodule