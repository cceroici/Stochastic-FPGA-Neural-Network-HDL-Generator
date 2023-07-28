

// Simulated Nios System



module Nios_system_sim (
		clk_clk,                                    //                                  clk.clk
		lcd_16207_0_external_RS,                     //                 lcd_16207_0_external.RS
		lcd_16207_0_external_RW,                     //                                     .RW
		lcd_16207_0_external_data,                   //                                     .data
		lcd_16207_0_external_E,                      //                                     .E
		pio_offload_mutv_clk_export,                     //                     offload_data_clk.export
		pio_offload_mutv_dataout_export,                 //                 offload_data_dataout.export
		pio_offload_mutv_index_export,                   //                   offload_data_index.export
		pio_offload_pbest_clk_export,                     //                     offload_data_clk.export
		pio_offload_pbest_dataout_export,                 //                 offload_data_dataout.export
		pio_offload_pbest_index_export,                   //                   offload_data_index.export
		pio_fitbest_lower_exp_export,                //                pio_fitbest_lower_exp.export
		//pio_fitbest_req_export,                      //                      pio_fitbest_req.export
		pio_fitbest_upper_exp_export,                //                pio_fitbest_upper_exp.export
		pio_frameduration_export, // pio_framecounter_export.export
		pio_framedurationreached_export,
		pio_keepmut_exp_export,                      //                      pio_keepmut_exp.export
		pio_mutsel_exp_clk_export,                   //                   pio_mutsel_exp_clk.export
		pio_mutsel_exp_stream_export,                //                pio_mutsel_exp_stream.export
		pio_reset_export,                            //                            pio_reset.export
		uart_rs232_rxd,                              //                           uart_rs232.rxd
		uart_rs232_txd                               //                                     .txd
	);
	
	
	
parameter FrameLength = 10000;
parameter NC = 4;
	
	
input  wire        clk_clk;                                     //                                  clk.clk
output wire        lcd_16207_0_external_RS;                     //                 lcd_16207_0_external.RS
output wire        lcd_16207_0_external_RW;                     //                                     .RW
inout  wire [7:0]  lcd_16207_0_external_data;                   //                                     .data
output wire        lcd_16207_0_external_E;                      //                                     .E
output wire        pio_offload_mutv_clk_export;                     //                     offload_data_clk.export
output reg [7:0]  pio_offload_mutv_dataout_export  = 1'd1;                 //                 offload_data_dataout.export
output reg [7:0]  pio_offload_mutv_index_export;                   //                   offload_data_index.export
output wire        pio_offload_pbest_clk_export;                     //                     offload_data_clk.export
output reg [7:0]  pio_offload_pbest_dataout_export = 8'd100;                 //                 offload_data_dataout.export
output reg [7:0]  pio_offload_pbest_index_export;                   //                   offload_data_index.export
input  wire [7:0]  pio_fitbest_lower_exp_export;                //                pio_fitbest_lower_exp.export
//output wire        pio_fitbest_req_export;                      //                      pio_fitbest_req.export
input  wire [7:0]  pio_fitbest_upper_exp_export;                //                pio_fitbest_upper_exp.export
output  reg [15:0] pio_frameduration_export = 16'd5000; // pio_framecounter_external_connection.export
input wire pio_framedurationreached_export; // pio_framedurationreached.export
input  wire [NC-1:0]  pio_keepmut_exp_export;                      //                      pio_keepmut_exp.export
output wire        pio_mutsel_exp_clk_export;                   //                   pio_mutsel_exp_clk.export
input  wire        pio_mutsel_exp_stream_export;                //                pio_mutsel_exp_stream.export
output wire        pio_reset_export;                            //                            pio_reset.export
input  wire        uart_rs232_rxd;                              //                           uart_rs232.rxd
output wire        uart_rs232_txd;                              //                                     .txd
		
		
wire CLK;
reg [15:0] COUNT_int = 1'd0;
reg RESETval = 1'b1;
reg [NC-1:0] keepMut = 1'b0;
reg keepmut_clk = 1'b0;

assign lcd_16207_0_external_RS = 1'b0;
assign lcd_16207_0_external_RW = 1'b0;
assign lcd_16207_0_external_data = 1'b0;
assign lcd_16207_0_external_E = 1'b0;

assign uart_rs232_rxd = 1'b0;
assign uart_rs232_txd = 1'b0;


assign CLK = clk_clk;
assign pio_reset_export = RESETval;

assign pio_offload_mutv_clk_export = CLK;

assign pio_offload_pbest_clk_export = CLK;


always @(posedge CLK) begin 

	if (COUNT_int < 8'd5) begin // Initialize
		COUNT_int = COUNT_int + 1'b1;
		RESETval = 1'b1;
	end
	/*
	else if (pio_framecounter_external_connection_export < FrameLength) begin // Run frame
			RESETval = 1'b0;
			COUNT_int  = 8'd10;
	end
	else begin // Complete frame. Start initialization
			RESETval = 1'b1;
			COUNT_int = 1'd0;
	end
	*/
	if (pio_offload_mutv_index_export<8'd10) pio_offload_mutv_index_export = pio_offload_mutv_index_export + 1'b1;
	else pio_offload_mutv_index_export = 1'd0;
	
	pio_offload_pbest_index_export = pio_offload_mutv_index_export;
	
	
end

always @(posedge keepmut_clk) begin

		keepMut <= pio_keepmut_exp_export;

end



endmodule