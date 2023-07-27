/* Generate top level module */

#include <Windows.h>
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <io.h>
#include <time.h>
#include <direct.h>
#include "genTOP.h"
#include "Verilog.h"
#include "NN_DataTypes.h"	
#include "genSEED.h"
#include "BitOps.h"
#include "genPARAMS.h"
#include "genFWDPROP.h"
#include "genBCKDPROP.h"
#include "genDECC.h"
#include "genCOSTMOD.h"
#include "genTRAINING.h"
#include "genUBlazeSim.h"
#include "genRNE.h"
#include "genSTOCH.h"
#include "genNETPAR_GRADIENT.h"

using namespace std;

void genTOP(sysConfig sConfig) {

	int negone = -1;

	ofstream outFile;
	outFile.open(sConfig.Dir + "TOPsim.v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Top level module for NN\n";
	outFile << "// Version 12.0\n\n";

	outFile << "module TOP();\n\n";

	// Parameter Definitions
	DeclareParameters(outFile, sConfig);

	// Net Definitions
	DeclareTopNets(outFile, sConfig, true);
	outFile << "reg CLK = 1'b0;\n\n";

	// Control Logic
	outFile << "always #10 CLK = ~CLK;\n\n";
	ControlLogic(outFile, sConfig);

	// ------------------------------- Random Number Engine
	genRNE_Inst(outFile, sConfig);

	// ------------------------------- Seed File
	genSEED_Inst(outFile, sConfig);

	// ------------------------------- Stochastic Bit Generator
	genSTOCH_Inst(outFile, sConfig);

	if (sConfig.TSloading == uBlazeRotation)
		genUBlazeSim_Inst(outFile, sConfig);

	// ------------------------------- Training
	genTRAINING_Inst(outFile, sConfig, true);

	// ------------------------------- Forward Propagation
	genFWDPROP_Inst(outFile, sConfig);

	// ------------------------------- Backwards Propagation
	genBCKPROP_Inst(outFile, sConfig);

	// ------------------------------- Cost Function
	genCOSTMOD_Inst(outFile, sConfig);

	// ------------------------------- Network Parallelization Calculation
	if (sConfig.usesTSParallelization())
		genNETPAR_GRADIENT_Inst(outFile, sConfig);

	// ------------------------------- Parameters Function
	genPARAMS_sim_Inst(outFile, sConfig);

	// ------------------------------- Stochastic to Decimal Converter
	genDECC_Inst(outFile, sConfig);

	outFile << "endmodule\n";
}

void genTOP_Quartus(sysConfig sConfig, string QuartusProjectName) {

	int negone = -1;

	ofstream outFile;
	string ext = ".v";
	outFile.open(sConfig.Dir + QuartusProjectName + ext);
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Top level module for NN\n";
	outFile << "// Version 9.0\n\n";

	outFile << "module " << QuartusProjectName << "(\n";
	outFile << "\tCLOCK_50,\n";
	outFile << "\tCLOCK2_50,\n";
	outFile << "\tCLOCK3_50,\n\n";
	outFile << "\t//////////// LED //////////\n";
	outFile << "\tLEDG,\n";
	outFile << "\tLEDR,\n";
	outFile << "\t//////////// KEY //////////\n";
	outFile << "\tKEY,\n";
	outFile << "\t//////////// EX_IO //////////\n";
	outFile << "\tEX_IO,\n";
	outFile << "\t////////// SEG7 /////////\n";
	outFile << "\tHEX0,\n";
	outFile << "\tHEX1,\n";
	outFile << "\tHEX2,\n";
	outFile << "\tHEX3,\n";
	outFile << "\tHEX4,\n";
	outFile << "\tHEX5,\n";
	outFile << "\tHEX6,\n";
	outFile << "\tHEX7,\n";
	outFile << "\t//////////// LCD //////////\n";
	outFile << "\tLCD_BLON,\n";
	outFile << "\tLCD_DATA,\n";
	outFile << "\tLCD_EN,\n";
	outFile << "\tLCD_ON,\n";
	outFile << "\tLCD_RS,\n";
	outFile << "\tLCD_RW,\n";
	outFile << "\t//////////// RS232 //////////\n";
	outFile << "\tUART_CTS,\n";
	outFile << "\tUART_RTS,\n";
	outFile << "\tUART_RXD,\n";
	outFile << "\tUART_TXD,\n";
	outFile << "\t//////////// Flash //////////\n";
	outFile << "\tFL_ADDR,\n";
	outFile << "\tFL_CE_N,\n";
	outFile << "\tFL_DQ,\n";
	outFile << "\tFL_OE_N,\n";
	outFile << "\tFL_RST_N,\n";
	outFile << "\tFL_RY,\n";
	outFile << "\tFL_WE_N,\n";
	outFile << "\tFL_WP_N,\n";
	outFile << "\teps, ERROR, N_Frames\n";
	outFile << ");\n\n";

	// Parameter Definitions
	DeclareParameters(outFile, sConfig);
	
	// Net Definitions
	// Quartus ports

	outFile << "input CLOCK_50;\n";
	outFile << "input CLOCK2_50;\n";
	outFile << "input CLOCK3_50;\n";


	outFile << "//////////// LED //////////\n";
	outFile << "output[8:0] LEDG;\n";
	outFile << "output[17:0] LEDR;\n";

	outFile << "//////////// EX_IO //////////\n";
	outFile << "inout[6:0] EX_IO;\n";

	outFile << "output [6:0] HEX0;\n";
	outFile << "output [6:0] HEX1;\n";
	outFile << "output [6:0] HEX2;\n";
	outFile << "output [6:0] HEX3;\n";
	outFile << "output [6:0] HEX4;\n";
	outFile << "output [6:0] HEX5;\n";
	outFile << "output [6:0] HEX6;\n";
	outFile << "output [6:0] HEX7;\n";

	outFile << "//////////// LCD //////////\n";
	outFile << "output LCD_BLON;\n";
	outFile << "inout [7:0]	LCD_DATA;\n";
	outFile << "output LCD_EN;\n";
	outFile << "output LCD_ON;\n";
	outFile << "output LCD_RS;\n";
	outFile << "output LCD_RW;\n";

	outFile << "//////////// RS232 //////////\n";
	outFile << "input UART_CTS;\n";
	outFile << "output UART_RTS;\n";
	outFile << "input UART_RXD;\n";
	outFile << "output UART_TXD;\n";

	outFile << "//////////// Flash //////////\n";
	outFile << "output [22:0] FL_ADDR;\n";
	outFile << "output FL_CE_N;\n";
	outFile << "inout [7:0] FL_DQ;\n";
	outFile << "output FL_OE_N;\n";
	outFile << "output FL_RST_N;\n";
	outFile << "input FL_RY;\n";
	outFile << "output FL_WE_N;\n";
	outFile << "output FL_WP_N;\n";
	outFile << "output KEY;\n";
	
	outFile << "\n";

	DeclareTopNets(outFile, sConfig, false);

	outFile << "wire CLK;\n\n";

	// Control Logic
	outFile << "assign CLK = CLOCK_50;\n\n";
	ControlLogic(outFile, sConfig);

	// ------------------------------- Nios Module
	NiosModule_Inst(outFile, sConfig);

	// ------------------------------- Random Number Engine
	genRNE_Inst(outFile, sConfig);

	// ------------------------------- Seed File
	genSEED_Inst(outFile, sConfig);

	// ------------------------------- Stochastic Bit Generator
	genSTOCH_Inst(outFile, sConfig);

	// ------------------------------- Training
	genTRAINING_Inst(outFile, sConfig, false);

	// ------------------------------- Forward Propagation
	genFWDPROP_Inst(outFile, sConfig);

	// ------------------------------- Backwards Propagation
	genBCKPROP_Inst(outFile, sConfig);

	// ------------------------------- Cost Function
	genCOSTMOD_Inst(outFile, sConfig);

	// ------------------------------- Parameters Function
	genPARAMS_sim_Inst(outFile, sConfig);

	// ------------------------------- Stochastic to Decimal Converter
	genDECC_Inst(outFile, sConfig);


	outFile << "endmodule\n";

}

void genTOP_uBlaze(sysConfig sConfig) {

	int negone = -1;

	ofstream outFile;
	string ext = ".v";
	outFile.open(sConfig.Dir + "TOP_uBlaze" + ext);
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Top level module for NN\n";
	outFile << "// Version 9.0\n\n";

	outFile << "module TOP_uBlaze(\n";
	outFile << "\tsysclk_n, sysclk_p, reset,\n";
	outFile << "\tuart_rx_out, uart_tx_in,\n";
	outFile << "\tsw,\n";
	outFile << "\tled,\n";
	outFile << "\tbtnc,\n";
	outFile << "\tja\n";
		/*
	outFile << "\tddr3_sdram_addr,\n";
	outFile << "\tddr3_sdram_ba,\n";
	outFile << "\tddr3_sdram_cas_n,\n";
	outFile << "\tddr3_sdram_ck_n,\n";
	outFile << "\tddr3_sdram_ck_p,\n";
	outFile << "\tddr3_sdram_cke,\n";
	outFile << "\tddr3_sdram_cs_n,\n";
	outFile << "\tddr3_sdram_dm,\n";
	outFile << "\tddr3_sdram_dq,\n";
	outFile << "\tddr3_sdram_dqs_n,\n";
	outFile << "\tddr3_sdram_dqs_p,\n";
	outFile << "\tddr3_sdram_odt,\n";
	outFile << "\tddr3_sdram_ras_n,\n";
	outFile << "\tddr3_sdram_reset_n,\n";
	outFile << "\tddr3_sdram_we_n\n";
		*/
	outFile << ");\n\n";

	// Parameter Definitions
	DeclareParameters(outFile, sConfig);

	// Net Definitions
	// Quartus ports

	outFile << "input wire sysclk_n, sysclk_p, reset;\n";
	outFile << "input uart_rx_out;\n";
	outFile << "output uart_tx_in;\n";
	outFile << "input[7:0] sw;\n";
	outFile << "inout[7:0] ja;\n";
	outFile << "input btnc;\n";
	outFile << "output[7:0] led;\n";
	/*
	outFile << "output [14:0]ddr3_sdram_addr;\n";
	outFile << "output [2:0]ddr3_sdram_ba;\n";
	outFile << "output ddr3_sdram_cas_n;\n";
	outFile << "output [0:0]ddr3_sdram_ck_n;\n";
	outFile << "output [0:0]ddr3_sdram_ck_p;\n";
	outFile << "output [0:0]ddr3_sdram_cke;\n";
	outFile << "output [0:0]ddr3_sdram_cs_n;\n";
	outFile << "output [3:0]ddr3_sdram_dm;\n";
	outFile << "inout [31:0]ddr3_sdram_dq;\n";
	outFile << "inout [3:0]ddr3_sdram_dqs_n;\n";
	outFile << "inout [3:0]ddr3_sdram_dqs_p;\n";
	outFile << "output [0:0]ddr3_sdram_odt;\n";
	outFile << "output ddr3_sdram_ras_n;\n";
	outFile << "output ddr3_sdram_reset_n;\n";
	outFile << "output ddr3_sdram_we_n;\n";
	*/

	outFile << "\n";

	DeclareTopNets(outFile, sConfig, false);
	outFile << "wire CLK;\n\n";

	// Control Logic
	ControlLogic(outFile, sConfig);

	// ------------------------------- Nios Module
	uBlazeModule_Inst(outFile, sConfig);

	// ------------------------------- Random Number Engine
	genRNE_Inst(outFile, sConfig);

	// ------------------------------- Seed File
	genSEED_Inst(outFile, sConfig);

	// ------------------------------- Stochastic Bit Generator
	genSTOCH_Inst(outFile, sConfig);

	// ------------------------------- Training
	genTRAINING_Inst(outFile, sConfig,false);

	// ------------------------------- Forward Propagation
	genFWDPROP_Inst(outFile, sConfig);

	// ------------------------------- Backwards Propagation
	genBCKPROP_Inst(outFile, sConfig);

	// ------------------------------- Cost Function
	genCOSTMOD_Inst(outFile, sConfig);

	// ------------------------------- Parameters Function
	genPARAMS_sim_Inst(outFile, sConfig);
	
	// ------------------------------- Stochastic to Decimal Converter
	genDECC_Inst(outFile, sConfig);


	outFile << "endmodule\n";

}

void DeclareTopNets(std::ostream &outFile, sysConfig sConfig, bool Simulation) {

	outFile << "// Control System Wires\n";
	outFile << "reg INIT = 1'b1;\n";
	outFile << "reg INIT_late = 1'b0;\n";
	outFile << "reg INIT_powerup = 1'b1;\n";
	outFile << "reg INIT_powerupflag = 1'b1;\n";
	outFile << "reg [" << 2 * sConfig.LFSR_S - 1 << ":0] SDcount = 32'd0;\n";
	if (Simulation&(sConfig.TSloading == HW)) outFile << "reg TrainingActive = 1'b1;\n"; else outFile << "wire TrainingActive;\n";
	outFile << "reg [15:0] INITcounter = 1'b0;\n";
	outFile << "reg [15:0] COUNTtotal = 1'b0;\n";
	outFile << "reg [15:0] FrameCounter = 1'b1;\n\n";
	outFile << "wire TRAINING_PRESET, TRAINING_PRESET2, CLK_TRAINING_flag;\n";
	outFile << "wire [7:0] TRAINING_SET_CURRENT;\n";
	if (Simulation) outFile << "wire [15:0] TRAINING_DURATION_cap;\n";

	outFile << "wire pre_FP_FRAME, FP_FRAME, pre_BP_FRAME, BP_FRAME, pre_FRAME_END, FrameEnd;\n";
	outFile << "wire FRAME_RESET_DELAYED, FRAME_RESET;\n";
	//if (sConfig.TSloading == HW)
	//	outFile << "wire [" << ceil(log2(sConfig.NTrainingSets)) - 1 << ":0] TRAINING_SET;\n";
	//else if (sConfig.TSloading == uBlazeRotation)
	if (!sConfig.usesTSParallelization()) outFile << "wire [7:0] TRAINING_SET;\n";
	else for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) outFile << "wire [7:0] TRAINING_SET_netPar" << ts << ";\n";
	if (Simulation) outFile << "reg [7:0] TRAINING_SET_OVR = 1'b0;\n"; else  outFile << "wire [7:0] TRAINING_SET_OVR;\n";
	if (Simulation) outFile << "reg [7:0] TS_OVR = 1'b0;\n";
	if (Simulation) outFile << "reg TS_OVR_EN = 1'b0;\n";
	if (Simulation) outFile << "reg TRAINING_SET_OVR_EN = 1'b0;\n"; else  outFile << "wire TRAINING_SET_OVR_EN;\n";
	if (Simulation) outFile << "reg [15:0] N_Frames = 1'b0;\n"; else outFile << "reg [15:0] N_Frames = 1'b0;\n";
	if (Simulation) outFile << "assign TRAINING_DURATION_cap = 16'd50;\n";
	if (Simulation) outFile << "reg [15:0] ReadWriteIndex = 1'd0;\n";
	if (Simulation) outFile << "wire [15:0] NodeOutputExp_data;\n";
	if (Simulation) outFile << "reg [7:0] WriteTrainingSet_index = 1'd0;\n";
	if (Simulation) outFile << "reg NodeOutputExp_clk = 1'b0;\n";
	if (Simulation) outFile << "wire [15:0] ParameterRead;\n";
	if (Simulation) outFile << "reg [15:0] DataWrite = 1'b0;\n";

	outFile << "reg [12:0] ERRROR_COUNTER = 1'd1;\n";
	outFile << "reg [15:0] ERROR_SUM = 1'd0;\n";
	if (Simulation) outFile << "reg [15:0] ERROR = 1'd0;\n"; else outFile << "reg [15:0] ERROR = 1'd0;\n";
	outFile << "\n";

	outFile << "\n// Random Numbers/Seeds\n";
	ofstream outFile_ext;
	outFile_ext.open(sConfig.Dir + "RN_SEED_Declarations.v");
	outFile << "`include \"RN_SEED_Declarations.v\"\n\t";
	DeclareNet(outFile_ext, "wire", "DP_in", sConfig.RandNums.short_d2s, "R", "");
	if (!sConfig.usesTSParallelization()) {
		DeclareNet(outFile_ext, "wire", "DP_in", sConfig.RandNums.short_ADDSUB_FP, "R", "_ADDSUB_FP");
		DeclareNet(outFile_ext, "wire", "DP_in", sConfig.RandNums.short_ADDSUB_BP, "R", "_ADDSUB_BP");
	}
	else
	{
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			DeclareNet(outFile_ext, "wire", "DP_in", sConfig.RandNums.short_ADDSUB_FP, "R", "_ADDSUB_FP_netPar" + Int2Str(ts));
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			DeclareNet(outFile_ext, "wire", "DP_in", sConfig.RandNums.short_ADDSUB_BP, "R", "_ADDSUB_BP_netPar" + Int2Str(ts));
	}
	if (!sConfig.usesTSParallelization()){
		//if (sConfig.IsRecurrent()) DeclareNet(outFile, "wire", "DP_in", sConfig.RandNums.short_ADDSUB_BPTT, "R", "_ADDSUB_BPTT");
		if (sConfig.IsRecurrent()) DeclareDistributionNets(outFile, "wire", sConfig.DP_in, sConfig.RandNums.short_ADDSUB_BPTT, sConfig.LFSR_S, "R", "_ADDSUB_BPTT");
		DeclareNet(outFile_ext, "wire", "DP_in", sConfig.RandNums.short_ADDSUB_error, "R", "_ADDSUB_error");
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			if (sConfig.IsRecurrent()) DeclareNet(outFile_ext, "wire", "DP_in", sConfig.RandNums.short_ADDSUB_BPTT, "R", "_ADDSUB_BPTT_netPar" + Int2Str(ts));
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			DeclareNet(outFile_ext, "wire", "DP_in", sConfig.RandNums.short_ADDSUB_error, "R", "_ADDSUB_error_netPar" + Int2Str(ts));
		DeclareNet(outFile_ext, "wire", "DP_in", sConfig.RandNums.short_ADDSUB_NETPAR, "R", "_ADDSUB_NETPAR");
	}
	DeclareNet(outFile_ext, "wire", "DP_out", sConfig.RandNums.long_d2s, "R_long", "");
	DeclareNet(outFile_ext, "wire", Int2Str(sConfig.LFSR_S), sConfig.RandNums.Seeds, "SD", "");
	if (!sConfig.usesTSParallelization()) outFile_ext << "wire [" << sConfig.RandNums.short_zeroCenter << "-1:0] zeroCenterSource;\n";
	else
	{
		for (int ts = 0; ts < sConfig.TrainingSetParallelization-1; ts++)
			outFile_ext << "wire [" << sConfig.RandNums.short_zeroCenter << "-1:0] zeroCenterSource_netPar" << ts << ";\n";
		outFile_ext << "wire [" << sConfig.RandNums.short_zeroCenter << "-1:0] zeroCenterSource_netPar" << sConfig.TrainingSetParallelization - 1 << ";\n";
	}
	outFile_ext.close();

	outFile << "\n// Decimal Network Outputs/inputs\n";
	outFile_ext.open(sConfig.Dir + "Decimal_IO_Declarations.v");
	outFile << "`include \"Decimal_IO_Declarations.v\"\n\t";
	NN_DeclareGenericDecimalNet(outFile_ext, sConfig, 0, 0, "aD", wire, "DP_in", TopLevel, singleLayer, sConfig.IsRecurrent());
	if (sConfig.usesTSParallelization())
	{
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[0]; i++) {
			outFile_ext << "wire [DP_in-1:0] ";
			for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++)
				outFile_ext << "aD_L0_" << i << "_netPar" << ts << ", ";
			outFile_ext << "aD_L0_" << i << "_netPar" << sConfig.TrainingSetParallelization - 1 << ";\n";
		}
	}
	NN_DeclareGenericDecimalNet(outFile_ext, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "aD", wire, "DP_out", TopLevel, singleLayer, sConfig.IsRecurrent());
	
	if (!sConfig.IsRecurrent())
		DeclareNet(outFile_ext, "wire", "DP_out", sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD", "");
	else
		for (int t = 0; t<sConfig.maxTimeSteps; t++)
			DeclareNet(outFile_ext, "wire", "DP_out", sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD_T" + Int2Str(t)+"_", "");
	if (sConfig.usesTSParallelization())
	{
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; i++) {
			outFile_ext << "wire [DP_out - 1:0] ";
			for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++)
				outFile_ext << "YD" << i << "_netPar" << ts << ", ";
			outFile_ext << "YD" << i << "_netPar" << sConfig.TrainingSetParallelization - 1 << ";\n";
		}
	}
	if (sConfig.optimizer == alternatingHardwareMask) {
		NN_DeclareParameterMaskNets(outFile_ext, sConfig, TopLevel);
	}


	outFile_ext.close();

	outFile << "\n// Stochastic Network Outputs/inputs\n";

	if (sConfig.usesTSParallelization()) for (int ts=0;ts<sConfig.TrainingSetParallelization; ts++) DeclareForwardPropagationNets(outFile, sConfig, Simulation, TopLevel, ts);
	else DeclareForwardPropagationNets(outFile, sConfig, Simulation, TopLevel);

	if (!sConfig.IsRecurrent()) {
		outFile << "wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << " - 1:0] ";
		if (sConfig.usesTSParallelization()) {
			for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++) outFile << "Y_netPar" << ts << ", ";
			outFile << "Y_netPar" << sConfig.TrainingSetParallelization - 1 << ";\n\n";
		}
		else outFile << "Y;\n\n";
	}
	else {
		outFile << "wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << " - 1:0] ";
		if (!sConfig.usesTSParallelization()) {
			outFile << StringSeries(sConfig.maxTimeSteps, "Y_T", "") << ";\n";
		}
		else {
			for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++) outFile << StringSeries(sConfig.maxTimeSteps, "Y_T", "_netPar" + Int2Str(ts)) << ", ";
			outFile << StringSeries(sConfig.maxTimeSteps, "Y_T", "_netPar" + Int2Str(sConfig.TrainingSetParallelization - 1)) << ";\n";
		}
	}

	if (!sConfig.usesTSParallelization()) DeclareSingleLayerNet(outFile, sConfig, "eps", TopLevel);
	else for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) DeclareSingleLayerNet(outFile, sConfig, "eps", TopLevel, ts);

	if (!sConfig.usesTSParallelization()) NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a_MEM_ACTIVE", wire, TopLevel, singleLayer, sConfig.IsRecurrent());
	else for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a_MEM_ACTIVE", wire, TopLevel, singleLayer, sConfig.IsRecurrent(), ts);

	outFile << "\n// Decimal Network Parameters\n";
	outFile_ext.open(sConfig.Dir + "DecimalParam_Declarations.v");
	outFile << "`include \"DecimalParam_Declarations.v\"\n\t";
	NN_DeclareDecimalWeightNets(outFile_ext, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, TopLevel);
	outFile_ext.close();

	outFile << "\n// Stochastic Network Parameters\n";
	//if (!sConfig.usesTSParallelization()) 
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, TopLevel);
	//else
	//	for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
	//		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, TopLevel,false, false, "_netPar" + Int2Str(ts));

	outFile << "\n// Network Parameter Polarity\n";
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, TopLevel, false, true);
	//if (sConfig.usesTSParallelization())
	//	for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
	//		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, TopLevel, false, true, "_netPar" + Int2Str(ts));

	outFile << "\n// Stochastic Backpropagation wires\n";
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, TopLevel, true, false);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, TopLevel, true, true);
	if (sConfig.usesTSParallelization()) {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, TopLevel, true, false, "_netPar" + Int2Str(ts));
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, TopLevel, true, true, "_netPar" + Int2Str(ts));
	}
	outFile << "\n";
	if (!sConfig.usesTSParallelization()) DeclareSingleLayerNet(outFile, sConfig, "SIGN_L" + Int2Str(sConfig.NNobj.OUTPUTLYR), TopLevel);
	else
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			DeclareSingleLayerNet(outFile, sConfig, "SIGN_L" + Int2Str(sConfig.NNobj.OUTPUTLYR) + "_netPar" + Int2Str(ts), TopLevel);

	outFile << "\n// Nios Data ports\n";
	if (Simulation) {
		outFile << "reg TrainingSetOutput_CLK = 1'd0;\n";
		outFile << "reg TrainingSetInput_CLK = 1'd0;\n";
		outFile << "reg [15:0] TrainingSetOutput_index = 1'd0;\n";
		outFile << "reg [15:0] TrainingSetInput_index = 1'd0;\n";
		outFile << "reg [15:0] TrainingSetOutput_data = 1'd0;\n";
		outFile << "reg [7:0] TrainingSetInput_data = 1'd0;\n";
		outFile << "wire ERROR_req;\n";
		outFile << "reg NNParamExp_CLK = 1'b0;\n";
		outFile << "reg [15:0] NNParamExp_index = 1'b0;\n";
		outFile << "\n";
	}
	else {
		outFile << "wire TrainingSetOutput_CLK;\n";
		outFile << "wire TrainingSetInput_CLK;\n";
		outFile << "wire [15:0] TrainingSetOutput_index = 1'b0;\n";
		outFile << "wire [15:0] TrainingSetInput_index = 1'b0;\n";
		outFile << "wire [15:0] TrainingSetOutput_data = 1'b0;\n";
		outFile << "wire [7:0] TrainingSetInput_data = 1'b0;\n";
		outFile << "wire ERROR_req;\n";
		outFile << "wire NNParamExp_CLK;\n";
		outFile << "wire [7:0] NNParamExp_data;\n";
		outFile << "wire [15:0] NNParamExp_index;\n";
		outFile << "wire errorTargetReached;\n";
		outFile << "assign errorTargetReached = (ERROR == 16'd0);\n";
		outFile << "\n";
	}

}

void DeclareParameters(std::ostream &outFile, sysConfig sConfig) {
	outFile << "parameter DP_in = " << sConfig.DP_in << "; // number of bits of precision for input decimal parameters\n";
	outFile << "parameter DP_out = " << sConfig.DP_out << "; // number of bits of precision for output decimal parameters\n";
	outFile << "parameter LFSR_S = " << sConfig.LFSR_S << "; // LFSR size\n";
	outFile << "parameter TRAINING_DURATION = 16; // Number of clock cycles per training set (2^N)\n\n";
}

void ControlLogic(std::ostream &outFile, sysConfig sConfig) {
	outFile << "assign pre_FP_FRAME = (FrameCounter == 8'd0);\n";
	outFile << "assign FP_FRAME = (FrameCounter == 8'd2);\n";
	outFile << "assign pre_BP_FRAME = (FrameCounter == 8'd0);\n";
	outFile << "assign BP_FRAME = (FrameCounter == 8'd2);\n";
	outFile << "assign FRAME_RESET_DELAYED = (FrameCounter == 8'd2);\n";
	outFile << "assign FRAME_RESET = (FrameCounter == 8'd0);\n";

	outFile << "assign pre_FRAME_END = (FrameCounter == 8'd0);\n";
	outFile << "assign FrameEnd = (FrameCounter == 8'd2);\n\n";
	
	outFile << "always @(posedge CLK) begin\n\t";
	outFile << "if (INITcounter > 8'd5) begin\n\t\t";
	outFile << "INITcounter <= INITcounter;\n\t\t";
	outFile << "INIT <= 1'b0;\n\t\t";
	outFile << "INIT_late <= 1'b0;\n\t\t";
	outFile << "FrameCounter <= FrameCounter + 1'b1;\n\t";
	outFile << "end else if (INITcounter > 8'd2) begin\n\t\t";
	outFile << "INITcounter <= INITcounter + 1'b1;\n\t\t";
	outFile << "INIT <= 1'b1;\n\t\t";
	outFile << "INIT_late <= 1'b1;\n\t\t";
	outFile << "FrameCounter <= FrameCounter;\n\t";
	outFile << "end else begin\n\t\t";
	outFile << "INITcounter <= INITcounter + 1'b1;\n\t\t";
	outFile << "INIT <= 1'b1;\n\t\t";
	outFile << "INIT_late <= 1'b0;\n\t\t";
	outFile << "FrameCounter <= FrameCounter;\n\t";
	outFile << "end\n\t";
	outFile << "COUNTtotal <= COUNTtotal + 1'b1;\n";
	outFile << "end\n\n";
	
	/*
	outFile << "always @(posedge CLK) begin\n\n\t";
	outFile << "SDcount = SDcount + 1'b1;\n\t";
	outFile << "if (INITcounter < 8'd5) begin\n\t\t";
	outFile << "INITcounter <= INITcounter + 1'b1;\n\t\t";
	outFile << "INIT <= 1'b1;\n\t\t";
	outFile << "FrameCounter <= FrameCounter;\n\t";
	outFile << "end\n\t";
	outFile << "else begin\n\t\t";
	outFile << "INITcounter <= INITcounter;\n\t\t";
	outFile << "INIT <= 1'b0;\n\t\t";
	outFile << "FrameCounter <= FrameCounter + 1'b1;\n\t";
	outFile << "end\n\t";
	outFile << "COUNTtotal <= COUNTtotal + 1'b1;\n";
	outFile << "end\n\n";
	*/

	outFile << "always @(posedge CLK) begin\n\t";
	outFile << "ERRROR_COUNTER <= ERRROR_COUNTER + 1'b1;\n";
	outFile << "end\n\n";

	outFile << "always @(posedge CLK) begin\n\t";
	outFile << "if ((ERRROR_COUNTER <= 1'd0)&(TrainingActive)) N_Frames <= N_Frames + 1'b1;\n";
	outFile << "end\n";
	outFile << "\n";

	outFile << "wire[7:0] ERROR_MOVAVG;\n\n";
	outFile << "SS_MOVAVG MOVAVG0(.IN(";
	if (!sConfig.usesTSParallelization()) {
		if (!sConfig.IsRecurrent()) {
			for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1; i++) outFile << "eps[" << i << "] | ";
			outFile << "eps[" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "]";
		}
		else
			for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; i++)
				for (int t = 0; t < sConfig.maxTimeSteps; t++) {
					if ((i == sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1) && (t == sConfig.maxTimeSteps - 1))
						outFile << "eps_T" << t << "[" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "]";
					else
						outFile << "eps_T" << t << "[" << i << "] | ";
				}
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent()) {
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; i++) {
					if ((ts == (sConfig.TrainingSetParallelization-1))&&(i == (sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1)))
						outFile << "eps_netPar" << ts << "[" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "]";
					else
						outFile << "eps_netPar" << ts << "[" << i << "] | ";
				}
			}
			else
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; i++)
					for (int t = 0; t < sConfig.maxTimeSteps; t++) {
						if ((i == sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1) && (t == sConfig.maxTimeSteps - 1) && (ts == (sConfig.TrainingSetParallelization - 1)))
							outFile << "eps" << "_T" << t << "_netPar" << ts << "[" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "]";
						else
							outFile << "eps" << "_T" << t << "_netPar" << ts << "[" << i << "] | ";
					}
		}
	}
	outFile << "), .AVG(ERROR_MOVAVG), .CLK(CLK), .INIT(INIT)); \n";
	outFile << "defparam MOVAVG0.W = 100;\n";
	outFile << "defparam MOVAVG0.N = 8;\n";
	outFile << "defparam MOVAVG0.F_sampling = 100;\n";
	outFile << "\n";

}

void QuartusPorts(std::ostream &outFile, sysConfig sConfig) {

	outFile << "\tCLOCK_50,\n";
	outFile << "\tCLOCK2_50,\n";
	outFile << "\tCLOCK3_50,\n\n";
	outFile << "\t//////////// LED //////////\n";
	outFile << "\tLEDG,\n";
	outFile << "\tLEDR,\n";
	outFile << "\t//////////// KEY //////////\n";
	outFile << "\tKEY,\n";
	outFile << "\t//////////// EX_IO //////////\n";
	outFile << "\tEX_IO,\n";
	outFile << "\t////////// SEG7 /////////\n";
	outFile << "\tHEX0,\n";
	outFile << "\tHEX1,\n";
	outFile << "\tHEX2,\n";
	outFile << "\tHEX3,\n";
	outFile << "\tHEX4,\n";
	outFile << "\tHEX5,\n";
	outFile << "\tHEX6,\n";
	outFile << "\tHEX7,\n";
	outFile << "\t//////////// LCD //////////\n";
	outFile << "\tLCD_BLON,\n";
	outFile << "\tLCD_DATA,\n";
	outFile << "\tLCD_EN,\n";
	outFile << "\tLCD_ON,\n";
	outFile << "\tLCD_RS,\n";
	outFile << "\tLCD_RW,\n";
	outFile << "\t//////////// RS232 //////////\n";
	outFile << "\tUART_CTS,\n";
	outFile << "\tUART_RTS,\n";
	outFile << "\tUART_RXD,\n";
	outFile << "\tUART_TXD,\n";
	outFile << "\t//////////// Flash //////////\n";
	outFile << "\tFL_ADDR,\n";
	outFile << "\tFL_CE_N,\n";
	outFile << "\tFL_DQ,\n";
	outFile << "\tFL_OE_N,\n";
	outFile << "\tFL_RST_N,\n";
	outFile << "\tFL_RY,\n";
	outFile << "\tFL_WE_N,\n";
	outFile << "\tFL_WP_N,\n";
	outFile << "\teps, ERROR\n";
	outFile << ");\n\n";

	outFile << "input CLOCK_50;\n";
	outFile << "input CLOCK2_50;\n";
	outFile << "input CLOCK3_50;\n";


	outFile << "//////////// LED //////////\n";
	outFile << "output[8:0] LEDG;\n";
	outFile << "output[17:0] LEDR;\n";

	outFile << "//////////// EX_IO //////////\n";
	outFile << "inout[6:0] EX_IO;\n";

	outFile << "output [6:0] HEX0;\n";
	outFile << "output [6:0] HEX1;\n";
	outFile << "output [6:0] HEX2;\n";
	outFile << "output [6:0] HEX3;\n";
	outFile << "output [6:0] HEX4;\n";
	outFile << "output [6:0] HEX5;\n";
	outFile << "output [6:0] HEX6;\n";
	outFile << "output [6:0] HEX7;\n";

	outFile << "//////////// LCD //////////\n";
	outFile << "output LCD_BLON;\n";
	outFile << "inout [7:0]	LCD_DATA;\n";
	outFile << "output LCD_EN;\n";
	outFile << "output LCD_ON;\n";
	outFile << "output LCD_RS;\n";
	outFile << "output LCD_RW;\n";

	outFile << "//////////// RS232 //////////\n";
	outFile << "input UART_CTS;\n";
	outFile << "output UART_RTS;\n";
	outFile << "input UART_RXD;\n";
	outFile << "output UART_TXD;\n";

	outFile << "//////////// Flash //////////\n";
	outFile << "output [22:0] FL_ADDR;\n";
	outFile << "output FL_CE_N;\n";
	outFile << "inout [7:0] FL_DQ;\n";
	outFile << "output FL_OE_N;\n";
	outFile << "output FL_RST_N;\n";
	outFile << "input FL_RY;\n";
	outFile << "output FL_WE_N;\n";
	outFile << "output FL_WP_N;\n";


	outFile << "output KEY;\n\n";
}

void NiosModule_Inst(std::ostream &outFile, sysConfig sConfig) {


	outFile << "Nios_system(\n\t";
	outFile << ".clk_clk(CLOCK_50),                          //                       clk.clk\n\t";
	outFile << ".lcd_16207_0_external_RS(LCD_RS),          //      lcd_16207_0_external.RS\n\t";
	outFile << ".lcd_16207_0_external_RW(LCD_RW),          //                          .RW\n\t";
	outFile << ".lcd_16207_0_external_data(LCD_DATA),        //                          .data\n\t";
	outFile << ".lcd_16207_0_external_E(LCD_EN),           //                          .E\n\t";
	outFile << ".nios_upload_index_export(Nios_upload_index),         //         nios_upload_index.export\n\t";
	outFile << ".pio_error_data_export(ERROR),            //            pio_error_data.export\n\t";
	outFile << ".pio_error_req_export(ERROR_req),             //             pio_error_req.export\n\t";
	outFile << ".pio_errortargetreached_export(errorTargetReached),    //    pio_errortargetreached.export\n\t";
	outFile << ".pio_nios_upload_data_export(Nios_upload_data),      //      pio_nios_upload_data.export\n\t";
	outFile << ".pio_nnparamexp_clk_export(NNParamExp_CLK),        //        pio_nnparamexp_clk.export\n\t";
	outFile << ".pio_nnparamexp_data_export(NNParamExp_data),       //       pio_nnparamexp_data.export\n\t";
	outFile << ".pio_training_set_ovr_data_export(TRAINING_SET_OVR),\n\t";
	outFile << ".pio_training_set_ovr_en_export(TRAINING_SET_OVR_EN),\n\t";
	outFile << ".pio_nnparamexp_index_export(NNParamExp_index),      //      pio_nnparamexp_index.export\n\t";
	outFile << ".pio_trainingactive_export(TrainingActive),        //        pio_trainingactive.export\n\t";
	outFile << ".pio_trainingsetinput_clk_export(TrainingSetInput_CLK),  //  pio_trainingsetinput_clk.export\n\t";
	outFile << ".pio_trainingsetoutput_clk_export(TrainingSetOutput_CLK), // pio_trainingsetoutput_clk.export\n\t";
	outFile << ".uart_rs232_rxd(UART_RXD),                   //                uart_rs232.rxd\n\t";
	outFile << ".uart_rs232_txd(UART_TXD)                    //                          .txd\n";
	outFile << "); \n\n";
}

void uBlazeModule_Inst(std::ostream &outFile, sysConfig sConfig) {

	
	outFile << "wire NodeOutputExp_clk, WriteTS_Output_clk, WriteTS_Input_clk, ReadParams_clk, SetErrorTarget_clk, SetTrainingSetDuration_clk, SetAlphaEnable_clk, SetBetaEnable_clk;\n";
	outFile << "wire [15:0] DataWrite;\n";
	outFile << "wire ERROR_TARGET_REACHED;\n";
	outFile << "wire [15:0] ParameterRead;\n";
	outFile << "wire [15:0] NodeOutputExp_data;\n";
	outFile << "wire [15:0] ReadWriteIndex;\n";
	outFile << "wire [7:0] WriteTrainingSet_index;\n";
	outFile << "reg [15:0] NFramesRead = 1'b0;\n";
	outFile << "wire [15:0] ERROR_Read;\n";
	outFile << "reg [7:0] ERROR_TARGET = 8'd10;\n";
	outFile << "wire TS_OVR_EN;\n";
	outFile << "wire [7:0] TS_OVR;\n";
	outFile << "wire [7:0] TRAINING_SET_LIMIT;\n";
	outFile << "reg [15:0] TRAINING_DURATION_cap = 16'd50;\n\n";
	outFile << "assign led[7] = TrainingActive;\n";
	outFile << "assign led[6] = TS_OVR_EN;\n\n";


	outFile << "ila_0 ILA0(\n\t.clk(CLK),\n\t.probe0(ERROR_MOVAVG),\n\t.probe1(TRAINING_SET_CURRENT),\n\t";
	
	int probeNumber = 2;
	if (sConfig.IsRecurrent()) {
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; i++)
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				outFile << ".probe" << probeNumber++ << "(aD_L" << sConfig.NNobj.OUTPUTLYR << "_T" << t << "_" << i << "),\n\t";
	}
	else {
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; i++)
			outFile << ".probe" << probeNumber++ << "(aD_L" << sConfig.NNobj.OUTPUTLYR << "_" << i << "),\n\t";
	}
	outFile << ".probe" << probeNumber++ << "(TS_OVR),\n\t" << ".probe" << probeNumber++ << "(TS_OVR_EN)\n";
	outFile << ");\n\n";

	outFile << "uBlazeNew_wrapper uBlaze0(\n\t";
	outFile << ".clk(CLK),\n\t";
	outFile << ".ErrorTargetReached_tri_i(ERROR_TARGET_REACHED),\n\t";
	outFile << ".ReadData_tri_i(ParameterRead),\n\t";
	outFile << ".NodeOutputExp_tri_i(NodeOutputExp_data),\n\t";
	outFile << ".ReadError_tri_i(ERROR_MOVAVG),\n\t";
	outFile << ".ReadNFrames_tri_i(N_Frames),\n\t";
	outFile << ".TS_OVR_EN_tri_o(TS_OVR_EN),\n\t";
	outFile << ".TS_OVR_tri_o(TS_OVR),\n\t";
	outFile << ".TRAINING_SET_LIMIT_tri_o(TRAINING_SET_LIMIT),\n\t";
	outFile << ".TrainingActive_tri_o(TrainingActive),\n\t";
	outFile << ".gpio_TSindex_tri_o(WriteTrainingSet_index),\n\t";
	outFile << ".WriteData_tri_o(DataWrite),\n\t";
	outFile << ".ioCLK_tri_o({ NodeOutputExp_clk,WriteTS_Input_clk,WriteTS_Output_clk,ReadParams_clk,SetErrorTarget_clk,SetTrainingSetDuration_clk,SetAlphaEnable_clk,SetBetaEnable_clk }),\n\t";
	outFile << ".ioIndex_tri_o(ReadWriteIndex),\n\t";

	/*
	outFile << ".ddr3_sdram_addr(ddr3_sdram_addr),\n\t";
	outFile << ".ddr3_sdram_ba(ddr3_sdram_ba),\n\t";
	outFile << ".ddr3_sdram_cas_n(ddr3_sdram_cas_n),\n\t";
	outFile << ".ddr3_sdram_ck_n(ddr3_sdram_ck_n),\n\t";
	outFile << ".ddr3_sdram_ck_p(ddr3_sdram_ck_p),\n\t";
	outFile << ".ddr3_sdram_cke(ddr3_sdram_cke),\n\t";
	outFile << ".ddr3_sdram_cs_n(ddr3_sdram_cs_n),\n\t";
	outFile << ".ddr3_sdram_dm(ddr3_sdram_dm),\n\t";
	outFile << ".ddr3_sdram_dq(ddr3_sdram_dq),\n\t";
	outFile << ".ddr3_sdram_dqs_n(ddr3_sdram_dqs_n),\n\t";
	outFile << ".ddr3_sdram_dqs_p(ddr3_sdram_dqs_p),\n\t";
	outFile << ".ddr3_sdram_odt(ddr3_sdram_odt),\n\t";
	outFile << ".ddr3_sdram_ras_n(ddr3_sdram_ras_n),\n\t";
	outFile << ".ddr3_sdram_reset_n(ddr3_sdram_reset_n),\n\t";
	outFile << ".ddr3_sdram_we_n(ddr3_sdram_we_n),\n\t";
	*/

	
	outFile << ".ja_pin10_io(ja[7]), .ja_pin1_io(ja[0]), .ja_pin2_io(ja[1]), .ja_pin3_io(ja[2]), .ja_pin4_io(ja[3]), .ja_pin7_io(ja[4]), .ja_pin8_io(ja[5]), .ja_pin9_io(ja[6]),\n\t";
    outFile << ".sys_diff_clock_clk_n(sysclk_n),\n\t";
    outFile << ".sys_diff_clock_clk_p(sysclk_p),\n\t";
    outFile << ".reset(reset),\n\t";
    outFile << ".usb_uart_rxd(uart_rx_out),\n\t";
    outFile << ".usb_uart_txd(uart_tx_in)\n\t";
	outFile << "); \n\n";

	outFile << "always @(posedge SetErrorTarget_clk) ERROR_TARGET <= DataWrite;\n";
	outFile << "always @(posedge SetTrainingSetDuration_clk) TRAINING_DURATION_cap <= DataWrite;\n\n";
	outFile << "assign ERROR_TARGET_REACHED = (ERROR_MOVAVG < ERROR_TARGET)&TrainingActive;\n\n";

}

