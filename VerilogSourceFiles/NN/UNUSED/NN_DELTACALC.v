

module NN_DELTACALC(eps,z,scale,delta,INIT,CLK);

parameter MEMSIZE = 6;

input INIT,CLK;
input eps,z,scale;
output delta;

wire da,da_temp; // da/dz

DBURST DBURST0(.IN(eps),.OUT(da_temp),.CLK(CLK),.INIT(INIT));
defparam DBURST0.MEMSIZE = MEMSIZE;

JK_FF JK0(.J(da_temp), .K(scale), .CLK(CLK), .Q(da), .RESET(INIT));

and a0(delta,da,z);

endmodule