
module SOFTPLUS(z, SIGN_z, a, CLK, INIT);

parameter MEMSIZE = 3;

input z, SIGN_z;
//input INIT_STATE;
input CLK, INIT;

output a;

reg MODE = 1'b1; // output mode

reg INIT_STATE = 1'b0;

reg [MEMSIZE-1:0] MEM = 1'b0;

wire MEMsig1,MEMsig0;

M_AND AND0(.EN(1'b1),.IN(~MEM),.OUT(MEMsig0));
defparam AND0.N = MEMSIZE;
//and u0(MEMsig0,~MEM[0],~MEM[1]);

M_AND AND1(.EN(1'b1),.IN(MEM),.OUT(MEMsig1));
defparam AND1.N = MEMSIZE;

assign a = (MODE) ? z : 1'b0;

always @(posedge INIT or posedge CLK) begin
	if (INIT) begin 
		MODE <= INIT_STATE;
		MEM <= INIT_STATE;
	end
	else if ((MODE==1'b1)&(z==1'b1)) begin // Mode 1: '1' input, no change
		MODE <= MODE;
		MEM <= {MEM[MEMSIZE-2:0],z};
	end 
	else if ((MODE==1'b0)&(z==1'b0)) begin // Mode 0: '0' input, no change
		MODE <= MODE;
		MEM <= {MEM[MEMSIZE-2:0],z};
	end
	else if ((MODE==1'b1)&(z==1'b0)&(MEMsig0==1'b0)) begin // Mode 1: no mode switch
		MODE <= MODE;
		MEM <= {MEM[MEMSIZE-2:0],z};
	end
	else if ((MODE==1'b0)&(z==1'b1)&(MEMsig1==1'b0)) begin // Mode 0: no mode switch
		MODE <= MODE;
		MEM <= {MEM[MEMSIZE-2:0],z};
	end
	else if ((MODE==1'b1)&(z==1'b0)&(MEMsig0==1'b1)) begin // Mode 1 -> Mode 0
		MODE <= 1'b0;
		MEM <= {MEM[MEMSIZE-2:0],z};
	end
	else if ((MODE==1'b0)&(z==1'b1)&(MEMsig1==1'b1)) begin // Mode 0 -> Mode 1
		MODE <= 1'b1;
		MEM <= {MEM[MEMSIZE-2:0],z};
	end

	
end






endmodule