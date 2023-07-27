/* Generate module which contains declarations for children models */

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
#include "genUBlazeSim.h"
#include "NN_DataTypes.h"
#include "Verilog.h"
#include "genSEED.h"
#include "BitOps.h"

using namespace std;

void genUBlazeSim(sysConfig sConfig){
	ofstream outFile;
	outFile.open(sConfig.Dir + "uBlaze_sim.v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Simulated microblaze module \n\n";
	int NProd = 0;
	int Nindex = 0;
	
	outFile << "module uBlaze_sim(\n\t";
	outFile << "CLK,\n\t";
	outFile << "INIT,\n\t";
	outFile << "TRAINING_PRESET, TRAINING_PRESET2,\n\t";
	outFile << "CLK_TRAINING_flag,\n\t";
	outFile << "TrainingActive,\n\t";
	outFile << "TRAINING_SET,\n\t";
	outFile << "TrainingSetOutput_CLK, TrainingSetInput_CLK,\n\t";
	outFile << "TrainingSetOutput_index, TrainingSetInput_index,\n\t";
	outFile << "TrainingSetOutput_data, TrainingSetInput_data\n";
	outFile << ");\n\n";

	outFile << "parameter N_TRAINING_SETS = " << sConfig.NTrainingSetsRAM << ";\n";
	outFile << "parameter N_NETWORK_INPUTS = " << sConfig.NNobj.LYRSIZE[0] << ";\n";
	outFile << "parameter N_NETWORK_OUTPUTS = " << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << ";\n";
	outFile << "parameter DP_in = " << sConfig.DP_in << ";\n";
	outFile << "parameter DP_out = " << sConfig.DP_out << ";\n\n";

	outFile << "input CLK, INIT;\n";
	outFile << "input TRAINING_PRESET, TRAINING_PRESET2, CLK_TRAINING_flag;\n\n";
	outFile << "output wire [7:0] TRAINING_SET; \n";
	outFile << "output reg TrainingActive = 1'b0;\n";
	outFile << "output wire TrainingSetOutput_CLK, TrainingSetInput_CLK;\n";
	outFile << "output reg [15:0] TrainingSetOutput_index = 1'b0;\n";
	outFile << "output reg [15:0] TrainingSetInput_index = 1'b0;\n";
	outFile << "output reg [15:0] TrainingSetOutput_data; \n";
	outFile << "output reg [7:0] TrainingSetInput_data; \n";

	outFile << "reg OutputsComplete = 1'b0;\n\n";
	outFile << "reg InputsComplete = 1'b0;\n";
	outFile << "reg LOAD_TRAINING_BUFFER = 1'b1;\n\n";

	// System inputs:
	outFile << "// ********************************** System inputs:\n";

	for (int i = 0; i < sConfig.NTrainingSetsRAM; i++) {
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++) {
			if (j == i)
				outFile << "reg [DP_in-1:0] aD_L0_" << j << "_set" << i << " = 8'd255;\n";
			else
				outFile << "reg [DP_in-1:0] aD_L0_" << j << "_set" << i << " = 8'd0;\n";
		}
		outFile << "\n";
	}
	outFile << "\n";
	for (int i = 0; i < sConfig.NTrainingSetsRAM; i++) { // System outputs:
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++) {
			if (j == i)
				outFile << "reg [DP_out-1:0] YD" << j << "_set" << i << " = 16'd" << pow(2, 16) - 1 << ";\n";
			else
				outFile << "reg [DP_out-1:0] YD" << j << "_set" << i << " = 16'd0; \n";
		}
		outFile << "\n";
	}
	outFile << "// ********************************************** \n\n";

	outFile << "reg[15:0] TrainingSetOutput_index_internal = 1'b0;\n";
	outFile << "reg[15:0] TrainingSetInput_index_internal = 1'b0;\n\n";

	int NTrainingSetBits = ceil(log2(sConfig.NTrainingSetsRAM));
	int difference = sConfig.NTrainingSetsRAM - pow(2.0, floor(log2(sConfig.NTrainingSetsRAM)));
	if (difference == 1)
		difference = 2;
	outFile << "wire [" << floor(log2(sConfig.NTrainingSetsRAM)) - 1 << ":0] TRAINING_SET_temp;\n";
	outFile << "wire [" << 8 - floor(log2(sConfig.NTrainingSetsRAM)) - 1 << ":0] dummy0;\n";
	if (difference != 0)
		outFile << "wire [" << 8 - ceil(log2(difference)) - 1 << ":0] dummy1;\n\n";
	else
		outFile << "wire [" << 8 - 1 << ":0] dummy1;\n\n";

	outFile << "LFSR16 LFSR_Training(.TRIG(TRAINING_PRESET),.RESET(INIT),.OUT1({TRAINING_SET_temp, dummy0}),";
	if (difference != 0)
		outFile << ".OUT2({ TRAINING_SET_cap, dummy1 }), ";
	else
		outFile << ".OUT2(dummy1), ";
	outFile << ".SEED(16'd" << roundval((double)rand() / RAND_MAX * pow(2.0, 16) - 1.0) << "));\n";
	if (difference != 0)
		outFile << "assign TRAINING_SET = {" << ceil(log2(sConfig.NTrainingSets)) - floor(log2(sConfig.NTrainingSets)) << "'d0,TRAINING_SET_temp} + {" << ceil(log2(sConfig.NTrainingSets)) - ceil(log2(difference)) << "'d0,TRAINING_SET_cap}";
	else
		outFile << "assign TRAINING_SET = TRAINING_SET_temp";
	outFile << ";\n\n";

	outFile << "assign TrainingSetOutput_CLK = CLK&LOAD_TRAINING_BUFFER&(!INIT);\n";
	outFile << "assign TrainingSetInput_CLK = CLK&LOAD_TRAINING_BUFFER&(!INIT); \n";
	outFile << "assign TRAINING_LOAD_COMPLETE = InputsComplete&OutputsComplete; \n";
	outFile << "always @(posedge INIT or posedge TRAINING_LOAD_COMPLETE) begin\n";
	outFile << "if (INIT) TrainingActive <= 1'b0;\n";
	outFile << "else if (TRAINING_LOAD_COMPLETE) TrainingActive <= 1'b1;\n";
	outFile << "end\n\n";

	outFile << "always @(posedge TrainingSetOutput_CLK or posedge TRAINING_PRESET2) begin\n\t";
	outFile << "if (TRAINING_PRESET2) TrainingSetOutput_index_internal <= 1'b0;\n\t";
	outFile << "else if (TrainingSetOutput_index_internal == N_NETWORK_OUTPUTS) begin\n\t\t";
	outFile << "TrainingSetOutput_index_internal <= TrainingSetOutput_index_internal; \n\t\t";
	outFile << "OutputsComplete <= 1'b1;\n\t";
	outFile << "end else begin\n\t\t";
	outFile << "TrainingSetOutput_index_internal <= TrainingSetOutput_index_internal + 1'b1;\n\t\t";
	outFile << "OutputsComplete <= 1'b0;\n\t";
	outFile << "end\n";
	outFile << "end\n\n";

	outFile << "always @(posedge TrainingSetInput_CLK or posedge TRAINING_PRESET2 ) begin\n\t";
	outFile << "if (TRAINING_PRESET2) TrainingSetInput_index_internal <= 1'b0;\n\t";
	outFile << "else if (TrainingSetInput_index_internal == N_NETWORK_INPUTS) begin\n\t\t";
	outFile << "TrainingSetInput_index_internal <= TrainingSetInput_index_internal;\n\t\t";
	outFile << "InputsComplete <= 1'b1;\n\t";
	outFile << "end else begin\n\t\t";
	outFile << "TrainingSetInput_index_internal <= TrainingSetInput_index_internal + 1'b1;\n\t\t";
	outFile << "InputsComplete <= 1'b0;\n\t";
	outFile << "end\n";
	outFile << "end\n\n";

	outFile << "always @(negedge TrainingSetOutput_CLK&(~INIT) or posedge TRAINING_PRESET&(~INIT)) begin\n";
	outFile << "\tif (TRAINING_SET == 8'd" << 0 << ") begin\n\t\t";
	for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1; i++)
		outFile << "if (TrainingSetOutput_index_internal==16'd" << i << ") TrainingSetOutput_data <= YD" << i << "_set" << 0 << ";\n\t\t";
	outFile << "if (TrainingSetOutput_index_internal==16'd" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << ") TrainingSetOutput_data <= YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "_set" << 0 << ";\n";
	for (int n = 1; n < sConfig.NTrainingSetsRAM; n++) {
		outFile << "\tend else if (TRAINING_SET == 8'd" << n << ") begin\n\t\t";
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1; i++)
			outFile << "if (TrainingSetOutput_index_internal==16'd" << i << ") TrainingSetOutput_data <= YD" << i << "_set" << n << ";\n\t\t";
		outFile << "if (TrainingSetOutput_index_internal==16'd" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << ") TrainingSetOutput_data <= YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "_set" << n << ";\n";
	}
	outFile << "\tend\n\t";
	outFile << "if (TRAINING_PRESET) TrainingSetOutput_index <= 1'b0;\n\t";
	outFile << "else if (TrainingSetOutput_index == N_NETWORK_OUTPUTS) begin\n\t\t";
	outFile << "TrainingSetOutput_index <= TrainingSetOutput_index; \n\t\t";
	outFile << "OutputsComplete <= 1'b1;\n\t";
	outFile << "end else begin\n\t\t";
	outFile << "TrainingSetOutput_index <= TrainingSetOutput_index + 1'b1;\n\t\t";
	outFile << "OutputsComplete <= 1'b0;\n\t";
	outFile << "end\n";
	outFile << "end\n\n";

	outFile << "always @(negedge TrainingSetInput_CLK&(~INIT) or posedge TRAINING_PRESET&(~INIT)) begin\n";
	outFile << "\tif (TRAINING_SET == 8'd" << 0 << ") begin\n\t\t";
	for (int i = 0; i < sConfig.NNobj.LYRSIZE[0] - 1; i++)
		outFile << "if (TrainingSetInput_index_internal==16'd" << i << ") TrainingSetInput_data <= aD_L0_" << i << "_set" << 0 << ";\n\t\t";
	outFile << "if (TrainingSetInput_index_internal==16'd" <<sConfig.NNobj.LYRSIZE[0] - 1 << ") TrainingSetInput_data <= aD_L0_" << sConfig.NNobj.LYRSIZE[0] - 1 << "_set" << 0 << ";\n";
	for (int n = 1; n < sConfig.NTrainingSetsRAM; n++) {
		outFile << "\tend else if (TRAINING_SET == 8'd" << n << ") begin\n\t\t";
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[0] - 1; i++)
			outFile << "if (TrainingSetInput_index_internal==16'd" << i << ") TrainingSetInput_data <= aD_L0_" << i << "_set" << n << ";\n\t\t";
		outFile << "if (TrainingSetInput_index_internal==16'd" << sConfig.NNobj.LYRSIZE[0] - 1 << ") TrainingSetInput_data <= aD_L0_" << sConfig.NNobj.LYRSIZE[0] - 1 << "_set" << n << ";\n";
	}
	outFile << "\tend\n\t";
	outFile << "if (TRAINING_PRESET) TrainingSetInput_index <= 1'b0;\n\t";
	outFile << "else if (TrainingSetInput_index == N_NETWORK_INPUTS) begin\n\t\t";
	outFile << "TrainingSetInput_index <= TrainingSetInput_index; \n\t\t";
	outFile << "InputsComplete <= 1'b1;\n\t";
	outFile << "end else begin\n\t\t";
	outFile << "TrainingSetInput_index <= TrainingSetInput_index + 1'b1;\n\t\t";
	outFile << "InputsComplete <= 1'b0;\n\t";
	outFile << "end\n";
	outFile << "end\n\n";

	outFile << "\nendmodule";
	

}


void genUBlazeSim_Inst(std::ostream &outFile, sysConfig sConfig){

	int Lindex = 0;
	int negone = -1;
	outFile << "uBlaze_sim uBlaze0(\n\t";
	outFile << ".CLK(CLK),\n\t";
	outFile << ".INIT(INIT),\n\t";
	outFile << ".TRAINING_PRESET(TRAINING_PRESET), .TRAINING_PRESET2(TRAINING_PRESET2),\n\t";
	outFile << ".CLK_TRAINING_flag(CLK_TRAINING_flag),\n\t";
	outFile << ".TrainingActive(TrainingActive),\n\t";
	outFile << ".TRAINING_SET(TRAINING_SET),\n\t";
	outFile << ".TrainingSetOutput_CLK(TrainingSetOutput_CLK), .TrainingSetInput_CLK(TrainingSetInput_CLK),\n\t";
	outFile << ".TrainingSetOutput_index(TrainingSetOutput_index), .TrainingSetInput_index(TrainingSetInput_index),\n\t";
	outFile << ".TrainingSetOutput_data(TrainingSetOutput_data), .TrainingSetInput_data(TrainingSetInput_data)\n";
	outFile << ");\n\n";

}