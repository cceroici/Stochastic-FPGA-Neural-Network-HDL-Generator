
module COUNTER2(CLK,in0,OUT,RESET);

parameter N = 14;


input CLK,in0,RESET;

output wire [N-1:0] OUT;

JK_FF JK_FF0(.J(1'b1),.K(1'b1),.CLK(in0&CLK),.Q(OUT[0]),.RESET(RESET));
JK_FF JK_FF1(.J(in1),.K(in1),.CLK(in0&CLK),.Q(OUT[1]),.RESET(RESET));
JK_FF JK_FF2(.J(in2),.K(in2),.CLK(in0&CLK),.Q(OUT[2]),.RESET(RESET));
JK_FF JK_FF3(.J(in3),.K(in3),.CLK(in0&CLK),.Q(OUT[3]),.RESET(RESET));
JK_FF JK_FF4(.J(in4),.K(in4),.CLK(in0&CLK),.Q(OUT[4]),.RESET(RESET));
JK_FF JK_FF5(.J(in5),.K(in5),.CLK(in0&CLK),.Q(OUT[5]),.RESET(RESET));
JK_FF JK_FF6(.J(in6),.K(in6),.CLK(in0&CLK),.Q(OUT[6]),.RESET(RESET));
JK_FF JK_FF7(.J(in7),.K(in7),.CLK(in0&CLK),.Q(OUT[7]),.RESET(RESET));
JK_FF JK_FF8(.J(in8),.K(in8),.CLK(in0&CLK),.Q(OUT[8]),.RESET(RESET));
JK_FF JK_FF9(.J(in9),.K(in9),.CLK(in0&CLK),.Q(OUT[9]),.RESET(RESET));
JK_FF JK_FF10(.J(in10),.K(in10),.CLK(in0&CLK),.Q(OUT[10]),.RESET(RESET));
JK_FF JK_FF11(.J(in11),.K(in11),.CLK(in0&CLK),.Q(OUT[11]),.RESET(RESET));
JK_FF JK_FF12(.J(in12),.K(in12),.CLK(in0&CLK),.Q(OUT[12]),.RESET(RESET));
JK_FF JK_FF13(.J(in13),.K(in13),.CLK(in0&CLK),.Q(OUT[13]),.RESET(RESET));

assign in1 = OUT[0];
assign in2 = in1&OUT[1];
assign in3 = in2&OUT[2];
assign in4 = in3&OUT[3];
assign in5 = in4&OUT[4];
assign in6 = in5&OUT[5];
assign in7 = in6&OUT[6];
assign in8 = in7&OUT[7];
assign in9 = in8&OUT[8];
assign in10 = in9&OUT[9];
assign in11 = in10&OUT[10];
assign in12 = in11&OUT[11];
assign in13 = in12&OUT[12];


endmodule



