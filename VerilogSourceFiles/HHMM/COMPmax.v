// Two input comparator, outputs maximum (OUT) and maximum address (RESULT)

module COMPmax(IN0,IN1,OUT,RESULT);

parameter ND = 16;

input [ND-1:0] IN0,IN1;

output [ND-1:0] OUT;
output RESULT;

assign OUT = (IN0>=IN1) ? (IN0) : (IN1);
assign RESULT = IN0<IN1;

endmodule


