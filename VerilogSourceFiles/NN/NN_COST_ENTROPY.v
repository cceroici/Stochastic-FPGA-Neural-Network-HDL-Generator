
// Cost function calculation 

module NN_COST_ENTROPY(
	CLK,INIT,A,Y,A_SIGN,Y_SIGN,R_condition,OUT,SIGN_out
);

parameter DIFFCOUNTER_SIZE = 2;
parameter DIFFCOUNT_MIN = 1;

input CLK,INIT;
input A, A_SIGN, Y, Y_SIGN;
input R_condition;

output OUT;
output SIGN_out;

wire INPUT_SUM_pos, INPUT_SUM_neg;
wire OUTPUT_pos, OUTPUT_neg;
wire COUNT_ACTIVE;

reg [DIFFCOUNTER_SIZE-1:0] DIFFCOUNT = 1'd0;
reg DIFFCOUNT_SIGN = 1'd0;

reg [DIFFCOUNTER_SIZE-1:0] DIFFCOUNT_LIMIT = 1'd0-1'd1;
wire OUT_temp;

wire [1:0] N_INPUTS_pos;
wire [1:0] N_INPUTS_neg;

assign N_INPUTS_pos = {1'b0,A&(~A_SIGN)} + {1'b0,Y&(~Y_SIGN)};
assign N_INPUTS_neg = {1'b0,A&(A_SIGN)} + {1'b0,Y&(Y_SIGN)};

wire EQUAL, INPUT_SIGN;

assign EQUAL = (N_INPUTS_pos==N_INPUTS_neg);

assign INPUT_SIGN = (R_condition) ?  (N_INPUTS_neg>=N_INPUTS_pos) : (N_INPUTS_neg>N_INPUTS_pos);

always @(posedge CLK) begin

	if (INIT) begin
		DIFFCOUNT_SIGN = DIFFCOUNT_SIGN;
		DIFFCOUNT <= 1'b0;
	end else if (EQUAL) DIFFCOUNT <= DIFFCOUNT;
	else if ((!INPUT_SIGN)&(DIFFCOUNT_SIGN)&(DIFFCOUNT<N_INPUTS_pos)) begin // +ve input, currently -ve, cross: -ve -> +ve
		DIFFCOUNT_SIGN = 1'b0;
		DIFFCOUNT <= N_INPUTS_pos; 
	end else if ((!INPUT_SIGN)&(DIFFCOUNT<(DIFFCOUNT_LIMIT-N_INPUTS_pos))&(!DIFFCOUNT_SIGN)) begin // +ve input, currently +ve, stay as positive and increment
			DIFFCOUNT <= DIFFCOUNT + N_INPUTS_pos; 
	end else if ((!INPUT_SIGN)&(DIFFCOUNT_SIGN)) begin // +ve input, currently -ve, stay -ve and decrement
			DIFFCOUNT <= DIFFCOUNT - N_INPUTS_pos; 
	end else if ((INPUT_SIGN)&(!DIFFCOUNT_SIGN)&(DIFFCOUNT<N_INPUTS_neg)) begin // -ve input, currently +ve, cross: +ve -> -ve
		DIFFCOUNT_SIGN = 1'b1;
		DIFFCOUNT <= N_INPUTS_neg; 
	end else if ((INPUT_SIGN)&(DIFFCOUNT<(DIFFCOUNT_LIMIT-N_INPUTS_neg))&(DIFFCOUNT_SIGN)) begin // -ve input, currently -ve, stay as negative and increment
			DIFFCOUNT <= DIFFCOUNT + N_INPUTS_neg; 
	end else if ((INPUT_SIGN)&(!DIFFCOUNT_SIGN)) begin // -ve input, currently +ve, stay +ve and decrement
			DIFFCOUNT <= DIFFCOUNT - N_INPUTS_neg; 
	end else DIFFCOUNT <= DIFFCOUNT;
	
end


assign SIGN_out = DIFFCOUNT_SIGN;

assign INPUT_SUM_pos = (A&(~A_SIGN))|(Y&(~Y_SIGN));
assign INPUT_SUM_neg = (A&(A_SIGN))|(Y&(Y_SIGN));

assign OUTPUT_pos = INPUT_SUM_pos&(~INPUT_SUM_neg);
assign OUTPUT_neg = INPUT_SUM_neg&(~INPUT_SUM_pos);

//assign OUTPUT_pos = INPUT_SUM_pos&(~INPUT_SUM_neg)&(DIFFCOUNT>DIFFCOUNT_MIN);
//assign OUTPUT_neg = INPUT_SUM_neg&(~INPUT_SUM_pos)&(DIFFCOUNT>DIFFCOUNT_MIN);
wire IN;
assign OUT_temp = (SIGN_out) ? (OUTPUT_neg) : (OUTPUT_pos);
assign IN = (DIFFCOUNT > (DIFFCOUNT_MIN)) ? OUT_temp : 1'b0;

/*
always @(posedge CLK or posedge INIT) begin
	if (INIT) begin
		// reset
		OUT_temp <= 1'b0;
	end
	else if ((!A)&(!Y)) OUT_temp <= 1'b0;
	else if ((A)&(!Y)) OUT_temp <= OUT_temp;
	else if ((!A)&(Y)) OUT_temp <= OUT_temp;
	else if ((A)&(Y)) OUT_temp <= 1'b1;
end
assign OUT = OUT_temp;
*/
parameter MEMSIZE = 4;
reg MODE = 1'b0;
reg [MEMSIZE-1:0] MEM = 1'b0;

always @(posedge INIT or posedge CLK) begin
	if (INIT) begin 
		MEM <= 1'b0;
	end
	else begin 
		MEM <= {MEM[MEMSIZE-2:0],IN};
	end
end

M_AND AND0(.EN(1'b1),.IN(MEM),.OUT(OUT));
defparam AND0.N = MEMSIZE;

//wire [15:0] diff_dec;
//MEAN MEAN0(.in(IN),.out(diff_dec),.START(16'd0),.RESET(INIT),.CLK(CLK),.INIT(INIT),.ENABLE(1'b1),.preRESET(1'b0));
//defparam MEAN0.N_Count = 16;

endmodule