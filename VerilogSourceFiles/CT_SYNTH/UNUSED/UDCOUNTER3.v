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

module UDCOUNTER3(CLK,INIT,BitIN,BitOUT);

parameter psat = 7;
parameter nsat = 7;
parameter Csize = 4;

input CLK,INIT;
input [2:0] BitIN;

integer i;

reg [Csize-1:0] Count0,Count1,Count2;

output BitOUT;

assign BitOUT = (Count0[Csize-1]&&Count1[Csize-1]||Count1[Csize-1]&&Count2[Csize-1]||Count0[Csize-1]&&Count2[Csize-1]) ? 1'b0 : 1'b1;

always @(posedge INIT) begin
	for (i=0;i<Csize;i=i+1) begin
		Count0[i] <= 1'b0;
		Count1[i] <= 1'b0;
		Count2[i] <= 1'b0;
	
	end
end
	
always @(posedge CLK) begin
	if (BitIN[0]==0) begin
		if (Count0!=4'b1001) begin
			Count0 <= Count0 - 1'b1;
		end
	end
	else if (Count0!=4'b0111) begin
			Count0 <= Count0 +1'b1;
	end
end

always @(posedge CLK) begin
	if (BitIN[1]==0) begin
		if (Count1!=4'b1001) begin
			Count1 <= Count1 - 1'b1;
		end
	end
	else if (Count1!=4'b0111) begin
			Count1 <= Count1 +1'b1;
	end
end

always @(posedge CLK) begin
	if (BitIN[2]==0) begin
		if (Count2!=4'b1001) begin
			Count2 <= Count2 - 1'b1;
		end
	end
	else if (Count2!=4'b0111) begin
			Count2 <= Count2+1'b1;
	end
end

endmodule