// Chris Ceroici

// Burst Gate Operator

module BURST2(IN,OUT,CLK,INIT);

parameter MEMSIZE = 2;

input IN;
input CLK, INIT;

output reg OUT;

reg MODE = 1'b1; // output mode

reg [MEMSIZE-1:0] MEM = 1'b0;

wire MEMsig1,MEMsig0;

and AND0(MEMsig0,~MEM[0],~MEM[1]);

and AND1(MEMsig1,MEM[0],MEM[1]);

always @(posedge CLK) begin
	if (INIT) begin
		OUT <= 1'b0;
		MODE <= 1'b0;
		MEM <= 1'b0;
	end
	else if ((MODE==1'b1)&(IN==1'b1)) begin // Mode 1: par
		OUT <= 1'b1;
		MODE <= MODE;
		MEM <= {MEM[MEMSIZE-2:0],IN};
	end 
	else if ((MODE==1'b0)&(IN==1'b0)) begin // Mode 0: par
		OUT <= 1'b0;
		MODE <= MODE;
		MEM <= {MEM[MEMSIZE-2:0],IN};
	end
	else if ((MODE==1'b1)&(IN==1'b0)&(MEMsig0==1'b0)) begin // Mode 1: no mode switch
		OUT <= 1'b1;
		MODE <= MODE;
		MEM <= {MEM[MEMSIZE-2:0],IN};
	end
	else if ((MODE==1'b0)&(IN==1'b1)&(MEMsig1==1'b0)) begin // Mode 0: no mode switch
		OUT <= 1'b0;
		MODE <= MODE;
		MEM <= {MEM[MEMSIZE-2:0],IN};
	end
	else if ((MODE==1'b1)&(IN==1'b0)&(MEMsig0==1'b1)) begin // Mode 1 -> Mode 0
		OUT <= 1'b0;
		MODE <= 1'b0;
		MEM <= {MEM[MEMSIZE-2:0],IN};
	end
	else if ((MODE==1'b0)&(IN==1'b1)&(MEMsig1==1'b1)) begin // Mode 0 -> Mode 1
		OUT <= 1'b1;
		MODE <= 1'b1;
		MEM <= {MEM[MEMSIZE-2:0],IN};
	end

	
end






endmodule