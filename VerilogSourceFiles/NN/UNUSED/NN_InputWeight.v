


module NN_InputWeight(a,alpha,zp);


input a,alpha;

output zp;

wire zp_temp;

assign zp = a&alpha;


endmodule