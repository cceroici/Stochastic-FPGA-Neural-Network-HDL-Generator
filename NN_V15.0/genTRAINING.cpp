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
#include "genTRAINING.h"
#include "NN_DataTypes.h"
#include "Verilog.h"
#include "genSEED.h"
#include "BitOps.h"

using namespace std;

void genTRAINING(sysConfig sConfig) {
	ofstream outFile;
	outFile.open(sConfig.Dir + "TRAINING.v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Training set control \n\n";
	int NProd = 0;
	int Nindex = 0;
	int NTrainingSetBits = 8;
	//if (sConfig.TSloading == HW)
	//	NTrainingSetBits = ceil(log2(sConfig.NTrainingSets));

	outFile << "module TRAINING(\n\t";

	if (!sConfig.usesTSParallelization()) {
		NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a", wire, ModuleDeclarationPortList, singleLayer, sConfig.IsRecurrent(), -1);
		if (!sConfig.IsRecurrent())
			outFile << StringSeries(sConfig.NNobj.LYRSIZE[0], "aD_L0_", "") << ",\n\t";
		else
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				outFile << StringSeries(sConfig.NNobj.LYRSIZE[0], "aD_L0_T" + Int2Str(t) + "_", "") << ",\n\t";
		if (!sConfig.IsRecurrent())
			outFile << StringSeries(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD", "") << ",\n\t";
		else
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				outFile << StringSeries(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD_T" + Int2Str(t) + "_", "") << ",\n\t";

		NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a_MEM_ACTIVE", wire, ModuleDeclarationPortList, singleLayer, sConfig.IsRecurrent(), -1);

		outFile << "\n\t";
		outFile << "TRAINING_SET,TRAINING_SET_LIMIT,\n\t";
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a", wire, ModuleDeclarationPortList, singleLayer, sConfig.IsRecurrent(), ts);
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent())
				outFile << StringSeries(sConfig.NNobj.LYRSIZE[0], "aD_L0_", "_netPar" + Int2Str(ts)) << ",\n\t";
			else
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					outFile << StringSeries(sConfig.NNobj.LYRSIZE[0], "aD_L0_T" + Int2Str(t) + "_", "_netPar" + Int2Str(ts)) << ",\n\t";
		}
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent())
				outFile << StringSeries(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD", "_netPar" + Int2Str(ts)) << ",\n\t";
			else
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					outFile << StringSeries(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD_T" + Int2Str(t) + "_", "_netPar" + Int2Str(ts)) << ",\n\t";
		}
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a_MEM_ACTIVE", wire, ModuleDeclarationPortList, singleLayer, sConfig.IsRecurrent(), ts);
		outFile << "\n\t";
		for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++)
			outFile << "TRAINING_SET_netPar" << ts << ", ";
		outFile << "TRAINING_SET_netPar" << sConfig.TrainingSetParallelization - 1 << ",\n\t";
		outFile << "TRAINING_SET_LIMIT,\n\t";
	}
	outFile << "TrainingSetOutput_CLK, TrainingSetInput_CLK, TrainingSetOutput_index, TrainingSetInput_index,\n\t";
	outFile << "TrainingSetOutput_data, TrainingSetInput_data, WriteTrainingSet_index,\n\t";
	outFile << "TRAINING_SET_OVR, TRAINING_SET_OVR_EN,\n\t";
	outFile << "CLK, INIT, TRAINING_PRESET, TRAINING_PRESET2, CLK_TRAINING_flag, TRAINING_DURATION_cap,TRAINING_SET_CURRENT\n);\n\n";

	outFile << "parameter MAX_TS = " << sConfig.maxNTrainingSets << ";\n";
	outFile << "parameter DP_in = " << sConfig.DP_in << ";\n";
	outFile << "parameter DP_out = " << sConfig.DP_out << ";\n\n";

	// ---------------------------    port definitions
	if (!sConfig.usesTSParallelization())
		NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a", wire, ModuleDeclarationPortsInput, singleLayer, sConfig.IsRecurrent(), -1);
	else 
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a", wire, ModuleDeclarationPortsInput, singleLayer, sConfig.IsRecurrent(),ts);

	outFile << "input wire TrainingSetOutput_CLK, TrainingSetInput_CLK;\n";
	outFile << "input wire [" << NTrainingSetBits - 1 << ":0] TRAINING_SET_LIMIT;\n";
	outFile << "input wire [15:0] TrainingSetOutput_index, TrainingSetInput_index;\n";
	outFile << "input wire [15:0] TrainingSetOutput_data;\n";
	outFile << "input wire [15:0] TrainingSetInput_data;\n";
	outFile << "input wire [7:0] WriteTrainingSet_index;\n";
	outFile << "input wire [" << NTrainingSetBits - 1 << ":0] TRAINING_SET_OVR;\n";
	outFile << "input wire TRAINING_SET_OVR_EN;\n";
	outFile << "input wire [15:0] TRAINING_DURATION_cap;\n";
	outFile << "input wire CLK, INIT;\n";
	outFile << "\n";

	if (!sConfig.usesTSParallelization()) {
		NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a_MEM_ACTIVE", wire, ModuleDeclarationPortsOutput, singleLayer, sConfig.IsRecurrent(), -1);
		if (!sConfig.IsRecurrent()) {
			DeclareNet(outFile, "output reg", "DP_in", sConfig.NNobj.LYRSIZE[0], "aD_L0_", "");
			DeclareNet(outFile, "output reg", "DP_out", sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD", "");
		}
		else {
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				DeclareNet(outFile, "output reg", "DP_in", sConfig.NNobj.LYRSIZE[0], "aD_L0_T" + Int2Str(t) + "_", "");
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				DeclareNet(outFile, "output reg", "DP_out", sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD_T" + Int2Str(t) + "_", "");
		}
		if (sConfig.TSloading == HW)
			outFile << "output wire [" << NTrainingSetBits - 1 << ":0] TRAINING_SET;\n";
		else if (sConfig.TSloading == uBlazeRotation)
			outFile << "input wire [" << NTrainingSetBits - 1 << ":0] TRAINING_SET;\n";
		outFile << "output reg TRAINING_PRESET, TRAINING_PRESET2, CLK_TRAINING_flag;\n";
		outFile << "output reg[7:0] TRAINING_SET_CURRENT = 1'd0;\n";
		outFile << "\n";
		outFile << "reg[15:0] CLK_TRAINING_COUNT = 1'd0;\n";
		outFile << "reg CLK_TRAINING = 1'd0;\n";

		if (!sConfig.IsRecurrent()) {
			for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++)
				outFile << "(* ram_style = \"distributed\" *) reg [" << sConfig.NNobj.LYRSIZE[l] << "-1:0] a_MEM_L" << l << " [MAX_TS-1:0]; \n";
		}
		else
		{
			for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++)
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					outFile << "(* ram_style = \"distributed\" *) reg [" << sConfig.NNobj.LYRSIZE[l] << "-1:0] a_MEM_L" << l << "_T" << t << " [MAX_TS-1:0]; \n";
		}
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a_MEM_ACTIVE", wire, ModuleDeclarationPortsOutput, singleLayer, sConfig.IsRecurrent(), ts);
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent()) {
				DeclareNet(outFile, "output reg", "DP_in", sConfig.NNobj.LYRSIZE[0], "aD_L0_", "_netPar" + Int2Str(ts));
				DeclareNet(outFile, "output reg", "DP_out", sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD", "_netPar" + Int2Str(ts));
			}
			else {
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					DeclareNet(outFile, "output reg", "DP_in", sConfig.NNobj.LYRSIZE[0], "aD_L0_T" + Int2Str(t) + "_", "_netPar" + Int2Str(ts));
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					DeclareNet(outFile, "output reg", "DP_out", sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD_T" + Int2Str(t) + "_", "_netPar" + Int2Str(ts));
			}
		}
		if (sConfig.TSloading == HW)
			outFile << "output wire [" << NTrainingSetBits - 1 << ":0] ";
		else if (sConfig.TSloading == uBlazeRotation)
			outFile << "input wire [" << NTrainingSetBits - 1 << ":0] ";
		for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++)
			outFile << "TRAINING_SET_netPar" << ts << ", ";
		outFile << "TRAINING_SET_netPar" << sConfig.TrainingSetParallelization - 1 << ";\n";


		outFile << "output reg TRAINING_PRESET, TRAINING_PRESET2, CLK_TRAINING_flag; \n";
		outFile << "\n";
		outFile << "reg[15:0] CLK_TRAINING_COUNT = 1'd0;\n";
		outFile << "reg CLK_TRAINING = 1'd0;\n";
		outFile << "reg[7:0] ";
		for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++)
			outFile << "TRAINING_SET_CURRENT_netPar" << ts << ", ";
		outFile << "TRAINING_SET_CURRENT_netPar" << sConfig.TrainingSetParallelization - 1 << ";\n";
		//outFile << "wire [" << sConfig.NNobj.N_TOTALNODES << "-1:0] d;\n\n";
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent()) {
				for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++)
					outFile << "(* ram_style = \"distributed\" *) reg [" << sConfig.NNobj.LYRSIZE[l] << "-1:0] a_MEM_L" << l << "_netPar" << ts << " [MAX_TS-1:0]; \n";
			}
			else
			{
				for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++)
					for (int t = 0; t < sConfig.maxTimeSteps; t++)
						outFile << "(* ram_style = \"distributed\" *) reg [" << sConfig.NNobj.LYRSIZE[l] << "-1:0] a_MEM_L" << l << "_T" << t << "_netPar" << ts << " [MAX_TS-1:0]; \n";
			}
		}
	}
	outFile << "\n";

	// Declare Training Storage RAM

	if (!sConfig.IsRecurrent()) {
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[0]; i++)
			outFile << "(* ram_style = \"distributed\" *) reg [DP_in-1:0] TS_aD_L0_" << i << " [MAX_TS-1:0];\n";
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; i++)
			outFile << "(* ram_style = \"distributed\" *) reg [DP_out-1:0] TS_YD" << i << " [MAX_TS-1:0];\n";
		outFile << "\n";
	}
	else {
		for (int t = 0; t < sConfig.maxTimeSteps; t++) {
			for (int i = 0; i < sConfig.NNobj.LYRSIZE[0]; i++)
				outFile << "(* ram_style = \"distributed\" *) reg [DP_in-1:0] TS_aD_L0_T" << t << "_" << i << " [MAX_TS-1:0];\n";
			outFile << "\n";
		}
		for (int t = 0; t < sConfig.maxTimeSteps; t++) {
			for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; i++)
				outFile << "(* ram_style = \"distributed\" *) reg [DP_out-1:0] TS_YD_T" << t << "_" << i << " [MAX_TS-1:0];\n";
		}
	}


	if (sConfig.TSloading == HW)
	{

	}
	else if (sConfig.TSloading == uBlazeRotation)
	{
		DeclareNetEqual(outFile, "reg", "DP_in", sConfig.NNobj.LYRSIZE[0], "aD_L0_", "_buff", "1'b0");
		DeclareNetEqual(outFile, "reg", "DP_out", sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD", "_buff", "1'b0");
	}



	outFile << "//Training Set Rotation:\n\n";
	outFile << "always @(posedge CLK) begin\n\t";
	outFile << "if (INIT) begin\n\t\t";
	outFile << "CLK_TRAINING_flag <= 1'b0;\n\t\t";
	outFile << "CLK_TRAINING_COUNT <= 1'b0;\n\t";
	outFile << "end\n\t";
	outFile << "if (CLK_TRAINING_COUNT >= TRAINING_DURATION_cap) begin\n\t\t";
	outFile << "CLK_TRAINING_flag <= 1'b1;\n\t\t";
	outFile << "CLK_TRAINING_COUNT <= 1'd0;\n\t";
	outFile << "end else begin\n\t\t";
	outFile << "CLK_TRAINING_COUNT <= CLK_TRAINING_COUNT + 1'b1;\n\t\t";
	outFile << "CLK_TRAINING_flag <= 1'b0;\n\t";
	outFile << "end\n\t";
	outFile << "if (CLK_TRAINING_COUNT == (TRAINING_DURATION_cap-1'd1)) TRAINING_PRESET <= 1'b1;\n\t";
	outFile << "else TRAINING_PRESET <= 1'b0;\n\t";
	outFile << "if (CLK_TRAINING_COUNT == (TRAINING_DURATION_cap-4'd2)) TRAINING_PRESET2 <= 1'b1;\n\t"; // <<-------- changes 4'd5 -> 4'd2
	outFile << "else TRAINING_PRESET2 <= 1'b0;\n";
	outFile << "end\n\n";

	if (!sConfig.usesTSParallelization())
		outFile << "always @(posedge TRAINING_PRESET) TRAINING_SET_CURRENT <= TRAINING_SET;\n";
	for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
		outFile << "always @(posedge TRAINING_PRESET) TRAINING_SET_CURRENT_netPar" << ts << " <= TRAINING_SET_netPar" << ts << ";\n";
	outFile << "\n";
	if (sConfig.TSloading == HW)
	{
		if (!sConfig.usesTSParallelization()) {
			// Random Training Set order
			outFile << "wire [7:0] TRAINING_SET_temp;\n";
			outFile << "wire [7:0] dummy0;\n";
			outFile << "LFSR_LIMIT16 LFSR_Training(.TRIG(TRAINING_PRESET),.RESET(INIT),.LIMIT(TRAINING_SET_LIMIT),.OUT1(TRAINING_SET_temp),.OUT2(dummy0),";
			outFile << ".SEED(16'd" << roundval((double)rand() / RAND_MAX * pow(2.0, 16) - 1.0) << "));\n";
			outFile << "assign TRAINING_SET = (TRAINING_SET_OVR_EN) ? TRAINING_SET_OVR : TRAINING_SET_temp";
			outFile << ";\n";
		}
		else {
			for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
				outFile << "wire [7:0] TRAINING_SET_temp_netPar" << ts << ";\n";
				outFile << "wire [7:0] dummy" << ts << ";\n";
				outFile << "LFSR_LIMIT16 LFSR_Training_netPar" << ts << "(.TRIG(TRAINING_PRESET),.RESET(INIT),.LIMIT(TRAINING_SET_LIMIT),.OUT1(TRAINING_SET_temp_netPar" << ts << "),.OUT2(dummy" << ts << "),";
				outFile << ".SEED(16'd" << roundval((double)rand() / RAND_MAX * pow(2.0, 16) - 1.0) << "));\n";
				outFile << "assign TRAINING_SET_netPar" << ts << " = (TRAINING_SET_OVR_EN) ? TRAINING_SET_OVR : TRAINING_SET_temp_netPar" << ts;
				outFile << ";\n";
			}
		}
		outFile << "\n";
	}

	if (!sConfig.usesTSParallelization()) {
		if (!sConfig.IsRecurrent())
			for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++)
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[l]; i++)
					outFile << "assign a_MEM_ACTIVE_L" << l << "[" << i << "] = a_MEM_L" << l << "[TRAINING_SET_CURRENT][" << i << "];\n";
		else
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++)
					for (int i = 0; i < sConfig.NNobj.LYRSIZE[l]; i++)
						outFile << "assign a_MEM_ACTIVE_L" << l << "_T" << t << "[" << i << "] = a_MEM_L" << l << "_T" << t << "[TRAINING_SET_CURRENT][" << i << "];\n";
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent())
				for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++)
					for (int i = 0; i < sConfig.NNobj.LYRSIZE[l]; i++)
						outFile << "assign a_MEM_ACTIVE_L" << l << "_netPar" << ts << "[" << i << "] = a_MEM_L" << l << "_netPar" << ts << "[TRAINING_SET_CURRENT_netPar" << ts << "][" << i << "];\n";
			else
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++)
						for (int i = 0; i < sConfig.NNobj.LYRSIZE[l]; i++)
							outFile << "assign a_MEM_ACTIVE_L" << l << "_T" << t << "_netPar" << ts << "[" << i << "] = a_MEM_L" << l << "_T" << t << "_netPar" << ts << "[TRAINING_SET_CURRENT_netPar" << ts << "][" << i << "];\n";
		}
	}
	outFile << "\n";

	outFile << "integer k = 0;\n";
	outFile << "always @(posedge TRAINING_PRESET2 or posedge INIT) begin\n\t";
	outFile << "if (INIT) begin\n\t\t";
	outFile << "for (k = 0; k < MAX_TS; k = k + 1)\n\t\t";
	outFile << "begin\n\t\t\t";
	if (!sConfig.usesTSParallelization()) {
		if (!sConfig.IsRecurrent()) {
			for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR - 1; l++)
					outFile << "a_MEM_L" << l << "[k][" << sConfig.NNobj.LYRSIZE[l] - 1 << ":0] = 0;\n\t\t\t";
			outFile << "a_MEM_L" << sConfig.NNobj.OUTPUTLYR << "[k][" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << ":0] = 0;\n\t\t";
		}
		else for (int t = 0; t < sConfig.maxTimeSteps; t++)
			for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
				if ((t == (sConfig.maxTimeSteps - 1))&(l == (sConfig.NNobj.OUTPUTLYR)))
					outFile << "a_MEM_L" << l << "_T" << t << "[k][" << sConfig.NNobj.LYRSIZE[l] - 1 << ":0] = 0;\n\t\t";
				else
					outFile << "a_MEM_L" << l << "_T" << t << "[k][" << sConfig.NNobj.LYRSIZE[l] - 1 << ":0] = 0;\n\t\t\t";
			}
		outFile << "end\n\t";
		outFile << "end else begin\n\t\t";
		if (!sConfig.IsRecurrent()) {
			for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR - 1; l++) {
					outFile << "a_MEM_L" << l << "[TRAINING_SET_CURRENT][" << sConfig.NNobj.LYRSIZE[l] - 1 << ":0] = a_L" << l << "; \n\t\t";
			}
			outFile << "a_MEM_L" << sConfig.NNobj.OUTPUTLYR << "[TRAINING_SET_CURRENT][" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << ":0] = a_L" << sConfig.NNobj.OUTPUTLYR << "; \n\t";
		}
		else for (int t = 0; t < sConfig.maxTimeSteps; t++)
			for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
				if ((t == sConfig.maxTimeSteps - 1)&(l == sConfig.NNobj.OUTPUTLYR))
					outFile << "a_MEM_L" << sConfig.NNobj.OUTPUTLYR << "_T" << t << "[TRAINING_SET_CURRENT][" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << ":0] = a_L" << sConfig.NNobj.OUTPUTLYR << "_T" << t << "; \n\t";
				else
					outFile << "a_MEM_L" << l << "_T" << t << "[TRAINING_SET_CURRENT][" << sConfig.NNobj.LYRSIZE[l] - 1 << ":0] = a_L" << l << "_T" << t << "; \n\t\t";
			}
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent()) {
				for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR - 1; l++)
					outFile << "a_MEM_L" << l << "_netPar" << ts << "[k][" << sConfig.NNobj.LYRSIZE[l] - 1 << ":0] = 0;\n\t\t\t";
				outFile << "a_MEM_L" << sConfig.NNobj.OUTPUTLYR << "_netPar" << ts << "[k][" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << ":0] = 0;\n\t\t\t";
			}
			else for (int t = 0; t < sConfig.maxTimeSteps; t++)
				for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
					if ((t == (sConfig.maxTimeSteps - 1))&(l == (sConfig.NNobj.OUTPUTLYR)))
						outFile << "a_MEM_L" << l << "_T" << t << "_netPar" << ts << "[k][" << sConfig.NNobj.LYRSIZE[l] - 1 << ":0] = 0;\n\t\t";
					else
						outFile << "a_MEM_L" << l << "_T" << t << "_netPar" << ts << "[k][" << sConfig.NNobj.LYRSIZE[l] - 1 << ":0] = 0;\n\t\t\t";
				}
		}
		outFile << "\n\t\tend\n\t";
		outFile << "end else begin\n\t\t";
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent()) {
				for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR - 1; l++)
					outFile << "a_MEM_L" << l << "_netPar" << ts << "[TRAINING_SET_CURRENT_netPar" << ts << "][" << sConfig.NNobj.LYRSIZE[l] - 1 << ":0] = a_L" << l << "_netPar" << ts << "; \n\t\t";
				outFile << "a_MEM_L" << sConfig.NNobj.OUTPUTLYR << "_netPar" << ts << "[TRAINING_SET_CURRENT_netPar" << ts << "][" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << ":0] = a_L" << sConfig.NNobj.OUTPUTLYR << "_netPar" << ts << "; \n\t\t";
			}
			else for (int t = 0; t < sConfig.maxTimeSteps; t++)
				for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
					if ((t == sConfig.maxTimeSteps - 1)&(l == sConfig.NNobj.OUTPUTLYR))
						outFile << "a_MEM_L" << sConfig.NNobj.OUTPUTLYR << "_T" << t << "_netPar" << ts << "[TRAINING_SET_CURRENT_netPar" << ts << "][" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << ":0] = a_L" << sConfig.NNobj.OUTPUTLYR << "_T" << t << "_netPar" << ts << "; \n\t";
					else
						outFile << "a_MEM_L" << l << "_T" << t << "_netPar" << ts << "[TRAINING_SET_CURRENT_netPar" << ts << "][" << sConfig.NNobj.LYRSIZE[l] - 1 << ":0] = a_L" << l << "_T" << t << "_netPar" << ts << "; \n\t\t";
				}
		}
	}

	outFile << "\n\tend\n";
	outFile << "end\n\n";

	if (sConfig.TSloading == HW)
	{
		if (!sConfig.usesTSParallelization()) {
			outFile << "always @(posedge TRAINING_PRESET or posedge INIT) begin\n\t";
			outFile << "if (INIT) begin\n\t\t";
			if (!sConfig.IsRecurrent()) {
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++)
					outFile << "aD_L0_" << j << " <= TS_aD_L0_" << j << "[0][DP_in-1:0];\n\t\t";
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1; j++)
					outFile << "YD" << j << " <= TS_YD" << j << "[0][DP_out-1:0];\n\t\t";
				outFile << "YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << " <= TS_YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "[0][DP_out-1:0];\n";
				outFile << "\tend else begin\n\t\t";
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++)
					outFile << "aD_L0_" << j << " <= TS_aD_L0_" << j << "[TRAINING_SET][DP_in-1:0];\n\t\t";
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1; j++)
					outFile << "YD" << j << " <= TS_YD" << j << "[TRAINING_SET][DP_out-1:0];\n\t\t";
				outFile << "YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << " <= TS_YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "[TRAINING_SET][DP_out-1:0];\n";
			}
			else {
				for (int t = 0; t < sConfig.maxTimeSteps; t++) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++)
						outFile << "aD_L0_T" << t << "_" << j << " <= TS_aD_L0_T" << t << "_" << j << "[0][DP_in-1:0];\n\t\t";
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++) {
						if ((t == sConfig.maxTimeSteps - 1)&(j == sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1))
							outFile << "YD_T" << t << "_" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << " <= TS_YD_T" << t << "_" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "[0][DP_out-1:0];\n";
						else
							outFile << "YD_T" << t << "_" << j << " <= TS_YD_T" << t << "_" << j << "[0][DP_out-1:0];\n\t\t";
					}
				}
				outFile << "\tend else begin\n\t\t";
				for (int t = 0; t < sConfig.maxTimeSteps; t++) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++)
						outFile << "aD_L0_T" << t << "_" << j << " <= TS_aD_L0_T" << t << "_" << j << "[TRAINING_SET][DP_in-1:0];\n\t\t";
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++) {
						if ((t == sConfig.maxTimeSteps - 1)&(j == sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1))
							outFile << "YD_T" << t << "_" << j << " <= TS_YD_T" << t << "_" << j << "[TRAINING_SET][DP_out-1:0];\n";
						else
							outFile << "YD_T" << t << "_" << j << " <= TS_YD_T" << t << "_" << j << "[TRAINING_SET][DP_out-1:0];\n\t\t";
					}
				}
			}
			outFile << "\tend\n";
			outFile << "end\n\n";
		}
		else {
			for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
				outFile << "always @(posedge TRAINING_PRESET or posedge INIT) begin\n\t";
				outFile << "if (INIT) begin\n\t\t";
				if (!sConfig.IsRecurrent()) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++)
						outFile << "aD_L0_" << j << "_netPar" << ts << " <= TS_aD_L0_" << j << "[0][DP_in-1:0];\n\t\t";
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1; j++)
						outFile << "YD" << j << "_netPar" << ts << " <= TS_YD" << j << "[0][DP_out-1:0];\n\t\t";
					outFile << "YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "_netPar" << ts << " <= TS_YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "[0][DP_out-1:0];\n";
					outFile << "\tend else begin\n\t\t";
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++)
						outFile << "aD_L0_" << j << "_netPar" << ts << " <= TS_aD_L0_" << j << "[TRAINING_SET_netPar" << ts << "][DP_in-1:0];\n\t\t";
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1; j++)
						outFile << "YD" << j << "_netPar" << ts << " <= TS_YD" << j << "[TRAINING_SET_netPar" << ts << "][DP_out-1:0];\n\t\t";
					outFile << "YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "_netPar" << ts << " <= TS_YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "[TRAINING_SET_netPar" << ts << "][DP_out-1:0];\n";
				}
				else {
					for (int t = 0; t < sConfig.maxTimeSteps; t++) {
						for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++)
							outFile << "aD_L0_T" << t << "_" << j << "_netPar" << ts << " <= TS_aD_L0_T" << t << "_" << j << "[0][DP_in-1:0];\n\t\t";
						for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++) {
							if ((t == sConfig.maxTimeSteps - 1)&(j == sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1))
								outFile << "YD_T" << t << "_" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "_netPar" << ts << " <= TS_YD_T" << t << "_" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "[0][DP_out-1:0];\n";
							else
								outFile << "YD_T" << t << "_" << j << "_netPar" << ts << " <= TS_YD_T" << t << "_" << j << "[0][DP_out-1:0];\n\t\t";
						}
					}
					outFile << "\tend else begin\n\t\t";
					for (int t = 0; t < sConfig.maxTimeSteps; t++) {
						for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++)
							outFile << "aD_L0_T" << t << "_" << j << "_netPar" << ts << " <= TS_aD_L0_T" << t << "_" << j << "[TRAINING_SET_netPar" << ts << "][DP_in-1:0];\n\t\t";
						for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++) {
							if ((t == sConfig.maxTimeSteps - 1)&(j == sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1))
								outFile << "YD_T" << t << "_" << j << "_netPar" << ts << " <= TS_YD_T" << t << "_" << j << "[TRAINING_SET_netPar" << ts << "][DP_out-1:0];\n";
							else
								outFile << "YD_T" << t << "_" << j << "_netPar" << ts << " <= TS_YD_T" << t << "_" << j << "[TRAINING_SET_netPar" << ts << "][DP_out-1:0];\n\t\t";
						}
					}
				}
				outFile << "\tend\n";
				outFile << "end\n\n";
			}
		}


		// ------------ Training set initialization and uploading
		int index = 0;
		outFile << "always @(posedge INIT or posedge TrainingSetOutput_CLK) begin\n\t";
		outFile << "if (INIT) begin\n";
		if (!sConfig.IsRecurrent()) {
			for (int i = 0; i < sConfig.NTrainingSets; i++) {
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++) {
					if (j == i)
						outFile << "\t\tTS_YD" << j << "[" << i << "][DP_out-1:0] = 16'd" << pow(2, 16) - 1 << ";\n";
					else
						outFile << "\t\tTS_YD" << j << "[" << i << "][DP_out-1:0] = 16'd0; \n";
				}
				outFile << "\n";
			}
			outFile << "\tend\n\t";
			for (int n = 0; n < sConfig.NTrainingSets; n++) {
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++)
					outFile << "else if (TrainingSetOutput_index == 16'd" << index++ << ") TS_YD" << j << "[" << n << "][DP_out-1:0] = TrainingSetOutput_data;\n\t";
			}
		}
		else { // recurrent
			for (int n = 0; n < sConfig.NTrainingSets; n++) {
				for (int t = 0; t < sConfig.maxTimeSteps; t++) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++) {
						if (j == n)
							outFile << "\t\tTS_YD_T" << t << "_" << j << "[" << n << "][DP_out-1:0] = 16'd" << pow(2, 16) - 1 << ";\n";
						else
							outFile << "\t\tTS_YD_T" << t << "_" << j << "[" << n << "][DP_out-1:0] = 16'd0; \n";
					}
				}
				outFile << "\n";
			}
			outFile << "\tend\n\t";
			
			for (int t =0; t<sConfig.maxTimeSteps; t++)
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++)
					outFile << "else if (TrainingSetOutput_index == 16'd" << index++ << ") TS_YD_T" << t << "_" << j << "[WriteTrainingSet_index][DP_out-1:0] = TrainingSetOutput_data;\n\t";
		}

		outFile << "\nend\n\n";

		index = 0;
		outFile << "always @(posedge INIT or posedge TrainingSetInput_CLK) begin\n\t";
		outFile << "if (INIT) begin\n";
		if (!sConfig.IsRecurrent()) {
			for (int n = 0; n < sConfig.NTrainingSets; n++) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++) {
						if (j == n)
							outFile << "\t\tTS_aD_L0_" << j << "[" << n << "][DP_in-1:0] = 8'd255;\n";
						else
							outFile << "\t\tTS_aD_L0_" << j << "[" << n << "][DP_in-1:0] = 8'd0;\n";
					}
				outFile << "\n";
			}
			outFile << "\tend\n\t";
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++)
				outFile << "else if (TrainingSetInput_index == 16'd" << index++ << ") TS_aD_L0_" << j << "[WriteTrainingSet_index][DP_in-1:0] = TrainingSetInput_data;\n\t";
			
			outFile << "\nend\n";
		}
		else { // recurrent
			for (int n = 0; n < sConfig.NTrainingSets; n++) {
				for (int t = 0; t < sConfig.maxTimeSteps; t++) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++) {
						if (j == n)
							outFile << "\t\tTS_aD_L0_T" << t << "_" << j << "[" << n << "][DP_in-1:0] = 8'd255;\n";
						else
							outFile << "\t\tTS_aD_L0_T" << t << "_" << j << "[" << n << "][DP_in-1:0] = 8'd0;\n";
					}
					outFile << "\n";
				}
				outFile << "\n";
			}
			outFile << "\tend\n\t";
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++)
					outFile << "else if (TrainingSetInput_index == 16'd" << index++ << ") TS_aD_L0_T" << t << "_" << j << "[WriteTrainingSet_index][DP_in-1:0] = TrainingSetInput_data;\n\t";
			
			outFile << "\nend\n";
		}
	}
	else if (sConfig.TSloading == uBlazeRotation)
	{


		outFile << "always @(posedge TRAINING_PRESET or posedge INIT) begin\n\t";
		outFile << "if (INIT) begin\n\t\t";
		for (int i=0; i < sConfig.NNobj.LYRSIZE[0]; i++)
			outFile << "aD_L0_" << i << " <= 1'b0;\n\t\t";
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1; i++)
			outFile << "YD" << i << " <= 1'b0;\n\t\t";
		outFile << "YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << " <= 1'b0;\n\t";
		outFile << "end else begin\n\t\t";
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[0]; i++)
			outFile << "aD_L0_" << i << " <= aD_L0_" << i << "_buff;\n\t\t";
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1; i++)
			outFile << "YD" << i << " <= YD" << i << "_buff;\n\t\t";
		outFile << "YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << " <= YD" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] - 1 << "_buff;\n\t";
		outFile << "end\n";
		outFile << "end\n\n";

		outFile << "wire[15:0] TrainingSetOutput_data_temp;\n";
		outFile << "assign TrainingSetOutput_data_temp = (INIT) ? 1'b0 : TrainingSetOutput_data;\n";
		outFile << "wire[7:0] TrainingSetInput_data_temp;\n";
		outFile << "assign TrainingSetInput_data_temp = (INIT) ? 1'b0 : TrainingSetInput_data;\n\n";

		int index = 0;
		outFile << "always @(posedge TrainingSetOutput_CLK) begin\n\t";
		outFile << "case (TrainingSetOutput_index)\n\t\t";
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++)
			outFile << "16'd" << index++ << ": YD" << j << "_buff = TrainingSetOutput_data_temp;\n\t\t";
		
		outFile << " \n\tendcase\n";
		outFile << "end\n\n";

		index = 0;
		outFile << "always @(posedge TrainingSetInput_CLK) begin\n\t";
		outFile << "case (TrainingSetInput_index)\n\t\t";
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[0]; j++)
			outFile << "16'd" << index++ << ": aD_L0_" << j << "_buff = TrainingSetInput_data_temp;\n\t\t";
		
		outFile << " \n\tendcase\n";
		outFile << "end\n";
	}



	outFile << "\n";
	outFile << "endmodule\n";
	

}


void genTRAINING_Inst(std::ostream &outFile, sysConfig sConfig, bool Simulation){

	int negone = -1;
	int Lindex = 0;

	bool netpar = false;
	if (sConfig.usesTSParallelization())
		netpar = true;

	outFile << "TRAINING TRAINING0(\n";
	ofstream outFile_ext;
	outFile_ext.open(sConfig.Dir + "TRAINING_portDefinitions.v");
	outFile << "`include \"TRAINING_portDefinitions.v\"\n\t";
	if (!netpar) {
		NN_DeclareGenericNet(outFile_ext, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a", wire, ModuleInstantiationPorts, singleLayer, sConfig.IsRecurrent(), -1);
		if (!sConfig.IsRecurrent())
			outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "aD_L0_", "", "aD_L0_", "", negone) << "\n\t";
		else
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "aD_L0_T" + Int2Str(t) + "_", "", "aD_L0_T" + Int2Str(t) + "_", "", negone) << "\n\t";

		if (!sConfig.IsRecurrent())
			outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD", "", "YD", "", negone) << "\n\t";
		else
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD_T" + Int2Str(t) + "_", "", "YD_T" + Int2Str(t) + "_", "", negone) << "\n\t";
		NN_DeclareGenericNet(outFile_ext, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a_MEM_ACTIVE", wire, ModuleInstantiationPorts, singleLayer, sConfig.IsRecurrent(), - 1);
		outFile_ext << "\n\t";
		outFile_ext << ".TRAINING_SET(TRAINING_SET),\n\t";
	}
	else {
		for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++)
		{
			for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++)
				outFile_ext << ".a_L" << l << "_netPar" << ts << "(a_L" << l << "_netPar" << ts << "), ";
			outFile_ext << ".a_L" << l << "_netPar" << sConfig.TrainingSetParallelization - 1 << "(a_L" << l << "_netPar" << sConfig.TrainingSetParallelization - 1 << "),\n\t";
		}
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent())
				outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "aD_L0_", "_netPar" + Int2Str(ts), "aD_L0_", "_netPar" + Int2Str(ts), negone) << "\n\t";
			else
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "aD_L0_T" + Int2Str(t) + "_", "_netPar" + Int2Str(ts), "aD_L0_T" + Int2Str(t) + "_", "_netPar" + Int2Str(ts), negone) << "\n\t";
		}
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent())
				outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD", "_netPar" + Int2Str(ts), "YD", "_netPar" + Int2Str(ts), negone) << "\n\t";
			else
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "YD_T" + Int2Str(t) + "_", "_netPar" + Int2Str(ts), "YD_T" + Int2Str(t) + "_", "_netPar" + Int2Str(ts), negone) << "\n\t";
		}

		for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++)
		{
			for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++) {
				outFile_ext << ".a_MEM_ACTIVE_L" << l << "_netPar" << ts << "(a_MEM_ACTIVE_L" << l << "_netPar" << ts << "), ";
			}
			outFile_ext << ".a_MEM_ACTIVE_L" << l << "_netPar" << sConfig.TrainingSetParallelization - 1 << "(a_MEM_ACTIVE_L" << l << "_netPar" << sConfig.TrainingSetParallelization - 1 << "),\n\t";
		}
		outFile_ext << "\n\t";
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) 
			outFile_ext << ".TRAINING_SET_netPar" << ts << "(TRAINING_SET_netPar" << ts << "),\n\t";
	}
	outFile_ext.close();
	outFile << ".TrainingSetOutput_CLK(WriteTS_Output_clk), .TrainingSetInput_CLK(WriteTS_Input_clk),\n\t";
	outFile << ".TrainingSetOutput_index(ReadWriteIndex), .TrainingSetInput_index(ReadWriteIndex),\n\t";
	outFile << ".TrainingSetOutput_data(DataWrite), .TrainingSetInput_data(DataWrite),\n\t";
	if (sConfig.TSloading == HW) {
		if (Simulation)
			outFile << ".TRAINING_SET_LIMIT(8'd" << sConfig.NTrainingSets << "), ";
		else
			outFile << ".TRAINING_SET_LIMIT(TRAINING_SET_LIMIT), ";
		outFile << ".TRAINING_SET_OVR(TS_OVR), .TRAINING_SET_OVR_EN(TS_OVR_EN), .WriteTrainingSet_index(WriteTrainingSet_index),\n\t";
	}
	// [" << ceil(log2(sConfig.NTrainingSets)) - 1 << ":0]
	else if (sConfig.TSloading == uBlazeRotation)
		outFile << ".TRAINING_SET_OVR(TS_OVR), .TRAINING_SET_OVR_EN(TS_OVR_EN),\n\t";
	outFile << ".CLK(CLK), .INIT(INIT), .TRAINING_PRESET(TRAINING_PRESET), .TRAINING_PRESET2(TRAINING_PRESET2), .CLK_TRAINING_flag(CLK_TRAINING_flag), .TRAINING_DURATION_cap(TRAINING_DURATION_cap), .TRAINING_SET_CURRENT(TRAINING_SET_CURRENT)\n);\n\n";

}