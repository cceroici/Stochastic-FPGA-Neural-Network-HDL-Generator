
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
#include "Verilog.h"
#include "NN_DataTypes.h"	
#include "genPARAMS.h"

using namespace std;


void genPARAMS_sim(sysConfig sConfig, int& NparameterExports) {

	ofstream outFile;
	outFile.open(sConfig.Dir + "PARAMS_sim.v");
	outFile << "// Chris Ceroici \n\n";
	outFile << "module PARAMS_sim(\n\t";

	int RESISTANCE_precision = 6;

	int smoothGradSize = 1;

	bool useBuffering = false;

	NN_DeclareDecimalWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, false, true);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, true, false);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, true, true);

	outFile << "TrainingActive,\n\t";
	outFile << "NNParamExp_CLK, NNParamExp_index, NNParamExp_data, \n\t";
	outFile << "CLK, CLK_TRAINING_flag, TRAINING_CYCLE, BP_FRAME, FP_FRAME, UPDATE, INIT\n);\n\n";

	outFile << "parameter DP_in = " << sConfig.DP_in << ";\n";
	outFile << "parameter DP_out = " << sConfig.DP_out << ";\n";
	outFile << "parameter MAX_RATE_ALPHA = 6;\n";
	outFile << "parameter MAX_RATE_BETA = 5;\n";
	outFile << "parameter RESISTANCE_precision = " << RESISTANCE_precision << ";\n";
	outFile << "\n";


	// PORTS:

	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsInput, true, false);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsInput, true, true);
	outFile << "input wire TrainingActive;\n";
	outFile << "input wire NNParamExp_CLK;\n";
	outFile << "input wire [16-1:0] NNParamExp_index;\n";

	outFile << "\n";
	string bufferTag = "";
	NN_DeclareDecimalWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsOutput);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsOutput, false, true);

	outFile << "output reg [16 - 1:0] NNParamExp_data;\n";
	outFile << "\ninput wire CLK, CLK_TRAINING_flag, TRAINING_CYCLE, BP_FRAME, FP_FRAME, UPDATE, INIT;\n";
	outFile << "\n";

	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		outFile << "reg TrainingActive_L" << l << "_alpha = 1'b1;\n";
		if (sConfig.NNobj.LYRinfo[l].LT == RCC) outFile << "reg TrainingActive_L" << l << "_gamma = 1'b1;\n";
		outFile << "reg TrainingActive_L" << l << "_beta = 1'b1;\n";
	}
	outFile << "\n";

	/*
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		if (sConfig.IsConnectionType(l,FC_FC)) {
			outFile << "wire [" << sConfig.NNobj.LYRinfo[l].filters*sConfig.NNobj.LYRinfo[l].conv.Nconnections << "-1:0] TransitionChange_TRIG_alpha_k_L" << l << ";\n";
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			outFile << "wire [" << sConfig.NNobj.LYRinfo[l].filters*sConfig.NNobj.LYRinfo[l].conv.Nconnections << "-1:0] TransitionChange_TRIG_alpha_k_L" << l << ";\n";
		}

	}
	*/
	NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "TransitionChange_TRIG_alpha", wire, TopLevel, previousLayerWeight, false);
	if (sConfig.IsRecurrent()) 	NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "TransitionChange_TRIG_gamma", wire, TopLevel, recurrentLayer, false);
	NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "TransitionChange_TRIG_beta_k", wire, TopLevel, singleLayer, false, -1);

	outFile << "\nwire ";
	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++)
		outFile << "TransitionChange_TRIG_alpha_L" << l << ", ";
	outFile << "TransitionChange_TRIG_alpha_L" << sConfig.NNobj.OUTPUTLYR << ";\n";
	if (sConfig.IsRecurrent()) {
		outFile << "\nwire ";
		for (int l = 1; l < sConfig.NNobj.OUTPUTLYR-1; l++)
			outFile << "TransitionChange_TRIG_gamma_L" << l << ", ";
		outFile << "TransitionChange_TRIG_gamma_L" << sConfig.NNobj.OUTPUTLYR-1 << ";\n";
	}
	outFile << "wire ";
	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++)
		outFile << "TransitionChange_TRIG_beta_L" << l << ", ";
	outFile << "TransitionChange_TRIG_beta_L" << sConfig.NNobj.OUTPUTLYR << ";\n";
	outFile << "wire ";
	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++)
		outFile << "RESISTANCE_NOTATMAX_alpha_L" << l << ", ";
	outFile << "RESISTANCE_NOTATMAX_alpha_L" << sConfig.NNobj.OUTPUTLYR << ";\n";
	if (sConfig.IsRecurrent()) {
		outFile << "wire ";
		for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++)
			outFile << "RESISTANCE_NOTATMAX_gamma_L" << l << ", ";
		outFile << "RESISTANCE_NOTATMAX_gamma_L" << sConfig.NNobj.OUTPUTLYR << ";\n";
	}
	outFile << "wire ";
	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++)
		outFile << "RESISTANCE_NOTATMAX_beta_L" << l << ", ";
	outFile << "RESISTANCE_NOTATMAX_beta_L" << sConfig.NNobj.OUTPUTLYR << ";\n";

	outFile << "reg [RESISTANCE_precision-1:0] ";
	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++) {
		outFile << "RESISTANCE_alpha_L" << l << ", ";
		if (sConfig.NNobj.LYRinfo[l].LT == RCC) outFile << "RESISTANCE_gamma_L" << l << ", ";
		outFile << "RESISTANCE_beta_L" << l << ", ";
	}

	outFile << "RESISTANCE_alpha_L" << sConfig.NNobj.OUTPUTLYR << ", " << "RESISTANCE_beta_L" << sConfig.NNobj.OUTPUTLYR << ";\n";
	outFile << "wire [RESISTANCE_precision-1:0] ";
	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++) {
		outFile << "RESISTANCE_MAX_alpha_L" << l << ", ";
		if (sConfig.NNobj.LYRinfo[l].LT == RCC) outFile << "RESISTANCE_MAX_gamma_L" << l << ", ";
		outFile << "RESISTANCE_MAX_beta_L" << l << ", ";
	}
	outFile << "RESISTANCE_MAX_alpha_L" << sConfig.NNobj.OUTPUTLYR << ", ";
	if (sConfig.NNobj.LYRinfo[sConfig.NNobj.OUTPUTLYR].LT == RCC) outFile << "RESISTANCE_MAX_gamma_L" << sConfig.NNobj.OUTPUTLYR << ", ";
	outFile << "RESISTANCE_MAX_beta_L" << sConfig.NNobj.OUTPUTLYR << ";\n";
	outFile << "wire [8-1:0] ";
	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++) {
		outFile << "alphaD_L" << l << "_INIT, ";
		if (sConfig.NNobj.LYRinfo[l].LT == RCC) outFile << "gammaD_L" << l << "_INIT, ";
		outFile << "betaD_L" << l << "_INIT, ";
	}
	outFile << "alphaD_L" << sConfig.NNobj.OUTPUTLYR << "_INIT, ";
	outFile << "betaD_L" << sConfig.NNobj.OUTPUTLYR << "_INIT;\n\n";

	outFile << "wire ";
	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++) {
		outFile << "SIGN_alpha_L" << l << "_INIT, ";
		if (sConfig.NNobj.LYRinfo[l].LT == RCC) outFile << "SIGN_gamma_L" << l << "_INIT, ";
		outFile << "SIGN_beta_L" << l << "_INIT, ";
	}
	outFile << "SIGN_alpha_L" << sConfig.NNobj.OUTPUTLYR << "_INIT, ";
	outFile << "SIGN_beta_L" << sConfig.NNobj.OUTPUTLYR << "_INIT;\n\n";

	outFile << "assign RESISTANCE_MAX_alpha_L" << sConfig.NNobj.OUTPUTLYR << " = " << RESISTANCE_precision << "'d1;\n";
	outFile << "assign RESISTANCE_MAX_beta_L" << sConfig.NNobj.OUTPUTLYR << " = " << RESISTANCE_precision << "'d63;\n\n";

	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++) {
		outFile << "assign RESISTANCE_MAX_alpha_L" << l << " = " << RESISTANCE_precision << "'d1;\n";
		if (sConfig.NNobj.LYRinfo[l].LT == RCC) outFile << "assign RESISTANCE_MAX_gamma_L" << l << " = " << RESISTANCE_precision << "'d1;\n";
		outFile << "assign RESISTANCE_MAX_beta_L" << l << " = " << RESISTANCE_precision << "'d63;\n";
	}
	outFile << "\n";
	outFile << "assign alphaD_L" << sConfig.NNobj.OUTPUTLYR << "_INIT = 8'd0;\n";
	outFile << "assign betaD_L" << sConfig.NNobj.OUTPUTLYR << "_INIT = 8'd0;\n\n";

	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++) {
		if (sConfig.NNobj.LYRinfo[l].LT == FC)
			outFile << "assign alphaD_L" << l << "_INIT = 8'd0;\n";
		else if (sConfig.NNobj.LYRinfo[l].LT == CV)
			outFile << "assign alphaD_L" << l << "_INIT = 8'd0;\n";
		if (sConfig.NNobj.LYRinfo[l].LT == RCC) {
			outFile << "assign alphaD_L" << l << "_INIT = 8'd0;\n";
			outFile << "assign gammaD_L" << l << "_INIT = 8'd0;\n";
		}
		outFile << "assign betaD_L" << l << "_INIT = 8'd0;\n";
	}
	outFile << "\n";
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		if (sConfig.NNobj.LYRinfo[l].LT == FC)
			outFile << "assign SIGN_alpha_L" << l << "_INIT = 8'd0;\n";
		else if (sConfig.NNobj.LYRinfo[l].LT == CV)
			outFile << "assign SIGN_alpha_L" << l << "_INIT = 8'd0;\n";
		if (sConfig.NNobj.LYRinfo[l].LT == RCC) {
			outFile << "assign SIGN_alpha_L" << l << "_INIT = 8'd0;\n";
			outFile << "assign SIGN_gamma_L" << l << "_INIT = 8'd0;\n";
		}
		outFile << "assign SIGN_beta_L" << l << "_INIT = 8'd0;\n";
	}
	outFile << "\n";

	if (smoothGradSize > 1) {
		outFile << "reg[" << log2(smoothGradSize)-1 << ":0] regIndex = 1'd0;\n";
		outFile << "reg[3:0] regIndexCounter = 1'd0;\n";
		outFile << "always @(posedge CLK) begin\n\t";
		outFile << "if (INIT) begin\n\t\t";
		outFile << "regIndex <= 1'b0;\n\t\t";
		outFile << "regIndexCounter <= 1'b0;\n\t";
		outFile << "end else if (regIndexCounter == 1'd0) begin\n\t\t";
		outFile << "regIndex <= regIndex + 1'b1;\n\t\t";
		outFile << "regIndexCounter <= 1'b1;\n\t";
		outFile << "end else begin\n\t\t";
		outFile << "regIndex <= regIndex; \n\t\t";
		outFile << "regIndexCounter <= regIndexCounter + 1'b1;\n\t";
		outFile << "end\n";
		outFile << "end\n\n";
	}

	for (int l = (sConfig.NNobj.OUTPUTLYR); l > 0; l--) {
		if (sConfig.IsConnectionType(l,FC_FC)| sConfig.IsConnectionType(l, CV_FC)) {
			outFile << "assign RESISTANCE_NOTATMAX_alpha_L" << l << " = ~(RESISTANCE_alpha_L" << l << ">=RESISTANCE_MAX_alpha_L" << l << ");\n";
			outFile << "M_OR OR_alpha_L" << l << "(.EN(1'b1),.IN({";
			for (int k = 0; k < (sConfig.NNobj.LYRSIZE[l] - 1); k++)
				outFile << "TransitionChange_TRIG_alpha_L" << l << "_j_" << k << ", ";
			outFile << "TransitionChange_TRIG_alpha_L" << l << "_j_" << (sConfig.NNobj.LYRSIZE[l] - 1) << "}), .OUT(TransitionChange_TRIG_alpha_L" << l << "));\n";
			outFile << "defparam OR_alpha_L" << l << ".N = " << sConfig.NNobj.LYRSIZE[l] * sConfig.NNobj.LYRSIZE[l - 1] << ";\n";
			outFile << "always @(posedge INIT or posedge TransitionChange_TRIG_alpha_L" << l << ") begin\n\t";
			outFile << "if (INIT) RESISTANCE_alpha_L" << l << " <= 8'd1;\n\t";
			outFile << "else if (RESISTANCE_NOTATMAX_alpha_L" << l << ") RESISTANCE_alpha_L" << l << " <= RESISTANCE_alpha_L" << l << " + 1'b1;\n";
			outFile << "end\n\n";

			if (sConfig.NNobj.LYRinfo[l].LT == RCC) {
				outFile << "assign RESISTANCE_NOTATMAX_gamma_L" << l << " = ~(RESISTANCE_gamma_L" << l << ">=RESISTANCE_MAX_gamma_L" << l << ");\n";
				outFile << "M_OR OR_gamma_L" << l << "(.EN(1'b1),.IN({";
				for (int k = 0; k < (sConfig.NNobj.LYRSIZE[l]-1); k++)
					outFile << "TransitionChange_TRIG_gamma_L" << l << "_j_" << k << ", ";
				outFile << "TransitionChange_TRIG_gamma_L" << l << "_j_" << (sConfig.NNobj.LYRSIZE[l] - 1) << "}), .OUT(TransitionChange_TRIG_alpha_L" << l << "));\n";
				outFile << "defparam OR_gamma_L" << l << ".N = " << sConfig.NNobj.LYRSIZE[l] * sConfig.NNobj.LYRSIZE[l] << ";\n";
				outFile << "always @(posedge INIT or posedge TransitionChange_TRIG_gamma_L" << l << ") begin\n\t";
				outFile << "if (INIT) RESISTANCE_gamma_L" << l << " <= 8'd1;\n\t";
				outFile << "else if (RESISTANCE_NOTATMAX_gamma_L" << l << ") RESISTANCE_gamma_L" << l << " <= RESISTANCE_gamma_L" << l << " + 1'b1;\n";
				outFile << "end\n\n";
			}

			outFile << "assign RESISTANCE_NOTATMAX_beta_L" << l << " = ~(RESISTANCE_beta_L" << l << ">=RESISTANCE_MAX_beta_L" << l << ");\n";
			outFile << "M_OR OR_beta_L" << l << "(.EN(1'b1),.IN(";
			outFile << "TransitionChange_TRIG_beta_k_L" << l << "), .OUT(TransitionChange_TRIG_beta_L" << l << "));\n";
			outFile << "defparam OR_beta_L" << l << ".N = " << sConfig.NNobj.LYRSIZE[l] << ";\n";
			outFile << "always @(posedge INIT or posedge TransitionChange_TRIG_beta_L" << l << ") begin\n\t";
			outFile << "if (INIT) RESISTANCE_beta_L" << l << " <= 8'd" << pow(2,RESISTANCE_precision)-1  << ";\n\t";
			outFile << "else if (RESISTANCE_NOTATMAX_beta_L" << l << ") RESISTANCE_beta_L" << l << " <= RESISTANCE_beta_L" << l << " + 1'b1;\n";
			outFile << "end\n\n";
		}
		else if (sConfig.IsConnectionType(l, CV_FC)) {
			outFile << "assign RESISTANCE_NOTATMAX_alpha_L" << l << " = ~(RESISTANCE_alpha_L" << l << ">=RESISTANCE_MAX_alpha_L" << l << ");\n";
			outFile << "M_OR OR_alpha_L" << l << "(.EN(1'b1),.IN({";
			for (int f = 0; f < sConfig.NNobj.LYRinfo[l - 1].size; f++) {
				for (int k = 0; k < (sConfig.NNobj.LYRSIZE[l]); k++) {
					if ((f == sConfig.NNobj.LYRinfo[l - 1].size - 1)&(k == (sConfig.NNobj.LYRSIZE[l] - 1)))
						outFile << "TransitionChange_TRIG_alpha_L" << l << "_F" << f << "_j_" << k << "}), ";
					else
						outFile << "TransitionChange_TRIG_alpha_L" << l << "_F" << f << "_j_" << k << ", ";
				}
			}
			outFile << ".OUT(TransitionChange_TRIG_alpha_L" << l << "));\n";
			outFile << "defparam OR_alpha_L" << l << ".N = " << sConfig.NNobj.LYRSIZE[l] * sConfig.NNobj.LYRinfo[l - 1].size * sConfig.NNobj.LYRinfo[l - 1].conv.filterSize << ";\n";
			outFile << "always @(posedge INIT or posedge TransitionChange_TRIG_alpha_L" << l << ") begin\n\t";
			outFile << "if (INIT) RESISTANCE_alpha_L" << l << " <= 7'd1;\n\t";
			outFile << "else if (RESISTANCE_NOTATMAX_alpha_L" << l << ") RESISTANCE_alpha_L" << l << " <= RESISTANCE_alpha_L" << l << " + 1'b1;\n";
			outFile << "end\n\n";

			outFile << "assign RESISTANCE_NOTATMAX_beta_L" << l << " = ~(RESISTANCE_beta_L" << l << ">=RESISTANCE_MAX_beta_L" << l << ");\n";
			if (sConfig.NNobj.LYRSIZE[l] > 2) {
				outFile << "M_OR OR_beta_L" << l << "(.EN(1'b1),.IN(TransitionChange_TRIG_beta_k_L" << l << "), .OUT(TransitionChange_TRIG_beta_L" << l << "));\n";
				outFile << "defparam OR_beta_L" << l << ".N = " << sConfig.NNobj.LYRSIZE[l] << ";\n";
			}
			else {
				outFile << "assign TransitionChange_TRIG_beta_L" << l << " = TransitionChange_TRIG_beta_k_L" << l << "[0]|TransitionChange_TRIG_beta_k_L" << l << "[1];\n";
			}
			outFile << "always @(posedge INIT or posedge TransitionChange_TRIG_beta_L" << l << ") begin\n\t";
			outFile << "if (INIT) RESISTANCE_beta_L" << l << " <= 8'd100;\n\t";
			outFile << "else if (RESISTANCE_NOTATMAX_beta_L" << l << ") RESISTANCE_beta_L" << l << " <= RESISTANCE_beta_L" << l << " + 1'b1;\n";
			outFile << "end\n\n";
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			outFile << "assign RESISTANCE_NOTATMAX_alpha_L" << l << " = ~(RESISTANCE_alpha_L" << l << ">=RESISTANCE_MAX_alpha_L" << l << ");\n";
			outFile << "M_OR OR_alpha_L" << l << "(.EN(1'b1),.IN({";
			stringstream lineText;
			for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++)
				lineText << "TransitionChange_TRIG_alpha_L" << l << "_j_" << sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f) << ", ";
			lineText.seekp(-2, lineText.cur);
			lineText << "}), ";
			outFile << lineText.str() << ".OUT(TransitionChange_TRIG_alpha_L" << l << "));\n";
			outFile << "defparam OR_alpha_L" << l << ".N = " << sConfig.NNobj.LYRinfo[l].conv.filters * sConfig.NNobj.LYRinfo[l].conv.Nconnections << ";\n";
			outFile << "always @(posedge INIT or posedge TransitionChange_TRIG_alpha_L" << l << ") begin\n\t";
			outFile << "if (INIT) RESISTANCE_alpha_L" << l << " <= 8'd1;\n\t";
			outFile << "else if (RESISTANCE_NOTATMAX_alpha_L" << l << ") RESISTANCE_alpha_L" << l << " <= RESISTANCE_alpha_L" << l << " + 1'b1;\n";
			outFile << "end\n\n";

			outFile << "assign RESISTANCE_NOTATMAX_beta_L" << l << " = ~(RESISTANCE_beta_L" << l << ">=RESISTANCE_MAX_beta_L" << l << ");\n";
			if (sConfig.NNobj.LYRSIZE[l] > 2) {
				outFile << "M_OR OR_beta_L" << l << "(.EN(1'b1),.IN(TransitionChange_TRIG_beta_k_L" << l << "), .OUT(TransitionChange_TRIG_beta_L" << l << "));\n";
				outFile << "defparam OR_beta_L" << l << ".N = " << sConfig.NNobj.LYRSIZE[l] << ";\n";
			}
			else {
				outFile << "assign TransitionChange_TRIG_beta_L" << l << " = TransitionChange_TRIG_beta_k_L" << l << "[0]|TransitionChange_TRIG_beta_k_L" << l << "[1];\n";
			}
			outFile << "always @(posedge INIT or posedge TransitionChange_TRIG_beta_L" << l << ") begin\n\t";
			outFile << "if (INIT) RESISTANCE_beta_L" << l << " <= 8'd100;\n\t";
			outFile << "else if (RESISTANCE_NOTATMAX_beta_L" << l << ") RESISTANCE_beta_L" << l << " <= RESISTANCE_beta_L" << l << " + 1'b1;\n";
			outFile << "end\n\n";
		}
	}
	
	outFile << "\n";

	// *************************************************** SMOOTHGRAD 
	//genPARAMS_smoothGradBlockDeclaration(outFile, sConfig, smoothGradSize);
	
	for (int l = sConfig.NNobj.OUTPUTLYR; l > 0; l--) {
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[l - 1]; j++) {// alpha
			for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
				if (ConnectionExists(sConfig.NNobj, l, j, k)) {
					int connectionNumber = sConfig.NNobj.LYRinfo[l].GetFilterConnectionIndex(sConfig.NNobj.LYRSIZE[l - 1], k, j);
					outFile << "NN_SMOOTHGRAD_POLAR SMOOTHGRAD_a_L" << l << "_" << j << "_" << k << "(.CLK(CLK), .CLK_TRAINING_flag(CLK_TRAINING_flag), .INIT(INIT), .IN_SS(dalpha_L" << l << "_j_" << k << "[" << connectionNumber << "]), .SIGN(SIGN_dalpha_L" << l << "_j_" << k << "[" << connectionNumber << "]),  .SIGN_out(SIGN_alpha" << bufferTag << "_L" << l << "_j_" << k << "[" << connectionNumber << "]), .OUT(alphaD" << bufferTag << "_L" << l << "_" << j << "_" << k << "), ";
					if (!sConfig.randomInitialization) outFile << ".OUT_INIT(alphaD_L" << l << "_INIT), .SIGN_OUT_INIT(1'b0), .RESISTANCE(RESISTANCE_alpha_L" << l << "),";
					else {
						outFile << ".OUT_INIT(8'd" << floor((float)rand() / RAND_MAX * ((float)pow(2.0, 4.0)-1)) << "),";
						string randSign = "1'd0";
						if ((float)((rand() + 0.001) / RAND_MAX + 0.001) > 0.5)
							randSign = "1'd1";
						outFile << ".SIGN_OUT_INIT(" << randSign << "),";
					}
					outFile << " .RESISTANCE(RESISTANCE_alpha_L" << l << "), .TransitionChange_TRIG(TransitionChange_TRIG_alpha_L" << l << "_j_" << k << "[" << connectionNumber << "]), .EN(TrainingActive&TrainingActive_L" << l << "_alpha)); \n";
				}
			}
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[l - 1]; j++)
			for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++) {
				if (ConnectionExists(sConfig.NNobj, l, j, k)) {
					outFile << "defparam SMOOTHGRAD_a_L" << l << "_" << j << "_" << k << ".N = " << sConfig.DP_in << "; // decimal output size\n";
					outFile << "defparam SMOOTHGRAD_a_L" << l << "_" << j << "_" << k << ".N_RESISTANCE = RESISTANCE_precision;\n";
				}
			}

		if (sConfig.NNobj.LYRinfo[l].LT == RCC) {
			outFile << "\n";
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) 
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)  // gamma
					outFile << "NN_SMOOTHGRAD_POLAR SMOOTHGRAD_g_L" << l << "_" << j << "_" << k << "(.CLK(CLK), .CLK_TRAINING_flag(CLK_TRAINING_flag), .INIT(INIT), .IN_SS(dgamma_L" << l << "_j_" << k << "[" << j << "]), .SIGN(SIGN_dgamma_L" << l << "_j_" << k << "[" << j << "]),  .SIGN_out(SIGN_gamma" << bufferTag << "_L" << l << "_j_" << k << "[" << j << "]), .OUT(gammaD" << bufferTag << "_L" << l << "_" << j << "_" << k << "), .OUT_INIT(alphaD_L" << l << "_INIT), .SIGN_OUT_INIT(SIGN_gamma_L" << l << "_INIT), .RESISTANCE(RESISTANCE_gamma_L" << l << "), .TransitionChange_TRIG(TransitionChange_TRIG_gamma_L" << l << "_j_" << k << "[" << j << "] ), .EN(TrainingActive&TrainingActive_L" << l << "_gamma));\n";
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) 
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++) {
					outFile << "defparam SMOOTHGRAD_g_L" << l << "_" << j << "_" << k << ".N = " << sConfig.DP_in << "; // decimal output size\n";
					outFile << "defparam SMOOTHGRAD_g_L" << l << "_" << j << "_" << k << ".N_RESISTANCE = RESISTANCE_precision;\n";
				}
		}
		outFile << "\n";
		int Nbetas = sConfig.NNobj.LYRSIZE[l];
		if (sConfig.NNobj.LYRinfo[l].LT == CV) Nbetas = sConfig.NNobj.LYRinfo[l].conv.filters;
		for (int j = 0; j < Nbetas; j++)  // beta
			outFile << "NN_SMOOTHGRAD_POLAR SMOOTHGRAD_b_L" << l << "_" << j << "(.CLK(CLK), .CLK_TRAINING_flag(CLK_TRAINING_flag), .INIT(INIT), .IN_SS(dbeta_L" << l << "[" << j << "]), .SIGN(SIGN_dbeta_L" << l << "[" << j << "]), .SIGN_out(SIGN_beta" << bufferTag << "_L" << l << "[" << j << "]), .OUT(betaD" << bufferTag << "_L" << l << "_" << j << "), .OUT_INIT(betaD_L" << l << "_INIT), .SIGN_OUT_INIT(SIGN_beta_L" << l << "_INIT), .RESISTANCE(RESISTANCE_beta_L" << l << "), .TransitionChange_TRIG(TransitionChange_TRIG_beta_k_L" << l << "[" << j << "]), .EN(TrainingActive&TrainingActive_L" << l << "_beta));\n";
		for (int j = 0; j < Nbetas; j++) {
			outFile << "defparam SMOOTHGRAD_b_L" << l << "_" << j << ".N = " << sConfig.DP_in << "; // decimal output size\n";
			outFile << "defparam SMOOTHGRAD_b_L" << l << "_" << j << ".N_RESISTANCE = RESISTANCE_precision;\n";
		}
		outFile << "\n";
	}
	
	genPARAMS_parameterExportBlock(outFile, sConfig, NparameterExports); // generate parameter export block

	// ------------------ PARAMETER BUFFER
	outFile << "\nendmodule\n";

}


void genPARAMS_smoothGradBlockDeclaration(std::ostream &outFile, sysConfig sConfig, int smoothGradSize) {
	string moduleName = "NN_SMOOTHGRAD_POLAR";
	if (smoothGradSize>1)
		moduleName = "NN_SMOOTHGRAD_POLAR_SIMPLE_" + Int2Str(smoothGradSize) + "CHANNEL";

	for (int l = (sConfig.NNobj.OUTPUTLYR); l > 0; l--) {
		if ((sConfig.NNobj.LYRSIZE[l] % smoothGradSize) != 0)
			cout << "ERROR: Layer " << l << " size incompatable with smooth gradient size " << smoothGradSize << endl;

		for (int j = 0; j < (sConfig.NNobj.LYRSIZE[l - 1]); j += smoothGradSize) {
			for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++) {
				outFile << moduleName << " SMOOTHGRAD_a_L" << l << "_";
				for (int ni = j; ni < j + smoothGradSize - 1; ni++) outFile << ni << "o";
				outFile << j + smoothGradSize - 1 << "_";
				for (int ni = 0; ni < smoothGradSize - 1; ni++) outFile << k << "o";
				outFile << k;
				outFile << "(.CLK(CLK), .CLK_TRAINING_flag(CLK_TRAINING_flag), .INIT(INIT), .regIndex(regIndex), ";
				outFile << ".IN_SS({" << StringSeriesBound(j, j + smoothGradSize, "dalpha_L" + Int2Str(l) + "_j_" + Int2Str(k) + "[", "]", true) << "}), ";
				outFile << ".SIGN({" << StringSeriesBound(j, j + smoothGradSize, "SIGN_dalpha_L" + Int2Str(l) + "_j_" + Int2Str(k) + "[", "]", true) << "}), ";
				outFile << StringSeriesBoundPort(j, j + smoothGradSize, "SIGN_out", "", "SIGN_alpha_L" + Int2Str(l) + "_j_" + Int2Str(k) + "[", "]", true);
				outFile << StringSeriesBoundPort(j, j + smoothGradSize, "REG", "", "alphaD_L" + Int2Str(l) + "_", "_" + Int2Str(k), true);
				if (!sConfig.randomInitialization) outFile << ".OUT_INIT(alphaD_L" << l << "_INIT), .SIGN_OUT_INIT(1'b0), .RESISTANCE(RESISTANCE_alpha_L" << l << "),";
				else {
					outFile << ".OUT_INIT(8'd" << rand() / RAND_MAX * pow(2, 8) << "),";
					string randSign = "1'd0";
					if (rand() / RAND_MAX > 0.5)
						randSign = "1'd1";
					outFile << ".SIGN_OUT_INIT(1'b0), .RESISTANCE(" << randSign << "),";
				}
				outFile << ".TransitionChange_TRIG(TransitionChange_TRIG_alpha_L" << l << "_j_" << k << "[" << j << "] ), .EN(TrainingActive&TrainingActive_L" << l << "_alpha));\n";
			}
		}
		outFile << "\n";
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
			// beta
			outFile << "NN_SMOOTHGRAD_POLAR SMOOTHGRAD_b_L" << l << "_" << j << "(.CLK(CLK), .CLK_TRAINING_flag(CLK_TRAINING_flag), .INIT(INIT), .IN_SS(dbeta_L" << l << "[" << j << "]), .SIGN(SIGN_dbeta_L" << l << "[" << j << "]), .SIGN_out(SIGN_beta_L" << l << "[" << j << "]), .OUT(betaD_L" << l << "_" << j << "), .OUT_INIT(betaD_L" << l << "_INIT), .SIGN_OUT_INIT(1'b0), .RESISTANCE(RESISTANCE_beta_L" << l << "), .TransitionChange_TRIG(TransitionChange_TRIG_beta_k_L" << l << "[" << j << "]), .EN(TrainingActive&TrainingActive_L" << l << "_beta));\n";
		}
		outFile << "\n";
	}
}

void genPARAM_MASK_STORAGE(sysConfig sConfig) {

	ofstream outFile;
	outFile.open(sConfig.Dir + "PARAM_STORAGE.v");
	outFile << "// Chris Ceroici \n\n";
	outFile << "module PARAM_STORAGE(\n\t";

	NN_DeclareDecimalWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, false, true);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, true, false);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, true, true);
	NN_DeclareParameterMaskNets(outFile, sConfig, ModuleDeclarationPortList);
	outFile << "TrainingActive,\n\t";
	outFile << "NNParamExp_CLK, NNParamExp_index, NNParamExp_data, \n\t";
	outFile << "CLK, CLK_TRAINING_flag, CLK_TRAINING_MASK, TRAINING_CYCLE, UPDATE, INIT, INIT_late\n);\n\n";

	outFile << "parameter DP_in = " << sConfig.DP_in << ";\n";
	outFile << "parameter DP_out = " << sConfig.DP_out << ";\n";
	outFile << "parameter MAX_RATE_ALPHA = 6;\n";
	outFile << "parameter MAX_RATE_BETA = 5;\n";
	outFile << "parameter RESISTANCE_precision = " << sConfig.resistancePrecision << ";\n";
	outFile << "parameter N_TRAINING_MASKS = " << sConfig.parameterMask.Nmasks << ";\n";
	outFile << "\n";

	// PORTS:

	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsInput, true, false);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsInput, true, true);
	outFile << "input wire TrainingActive;\n";
	outFile << "input wire NNParamExp_CLK;\n";
	outFile << "input wire [16-1:0] NNParamExp_index;\n";
	outFile << "\n";
	string bufferTag = "";
	NN_DeclareDecimalWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsOutput);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsOutput, false, true);
	NN_DeclareParameterMaskNets(outFile, sConfig, ModuleDeclarationPortsOutput);
	outFile << "output reg [16 - 1:0] NNParamExp_data;\n";
	outFile << "output reg CLK_TRAINING_MASK = 1'd0;\n";
	outFile << "\ninput wire CLK, CLK_TRAINING_flag, TRAINING_CYCLE, UPDATE, INIT, INIT_late;\n";
	outFile << "\n";
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		outFile << "reg alpha_L" << l << "_trainingActive = 1'b1;\n";
	}
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		outFile << "reg beta_L" << l << "_trainingActive = 1'b1;\n";
	}
	outFile << "\n";
	NN_DeclareGenericDecimalNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "resistance_alpha", wire, Int2Str(sConfig.resistancePrecision), TopLevel, previousLayerWeight, sConfig.IsRecurrent());
	NN_DeclareGenericDecimalNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "resistance_beta", wire, Int2Str(sConfig.resistancePrecision),TopLevel, singleLayer, sConfig.IsRecurrent());
	outFile << "\n";
	outFile << "reg CLK_TRAINING_MASK_preReset = 1'b0;\n";
	outFile << "reg CLK_TRAINING_MASK_INITIALIZER = 1'b0;\n";
	outFile << "reg CLK_TRAINING_MASK_UPDATE_PARAM = 1'b0;\n";
	outFile << "reg[7:0] TRAINING_MASK_COUNTER = 1'b0;\n";
	outFile << "reg[7:0] TRAINING_MASK_DURATION = 8'd100;\n";
	outFile << "reg[7:0] CURRENT_TRAINING_MASK = 1'd0;\n";
	outFile << "wire[7:0] NEXT_TRAINING_MASK;\n";
	outFile << "wire [" << sConfig.parameterMask.Nmasks << "-1:0] UPDATE_TM;\n";
	outFile << "reg switchUpdate = 1'b0;\n";
	outFile << "wire [N_TRAINING_MASKS - 1:0] " << StringSeries(sConfig.parameterMask.maskSize, "TM_paramSet", "", false) << ";\n";
	outFile << "wire [N_TRAINING_MASKS - 1:0] " << StringSeries(sConfig.parameterMask.maskSize, "TM_SIGN_paramSet", "", false) << ";\n";

	outFile << "\nassign NEXT_TRAINING_MASK = (CURRENT_TRAINING_MASK<8'd" << sConfig.parameterMask.Nmasks-1 << ") ? (CURRENT_TRAINING_MASK + 1'b1) : 1'd0;\n\n";

	outFile << "always @(posedge CLK or posedge INIT) begin\n\t";
	outFile << "if (INIT) begin\n\t\t";
	outFile << "TRAINING_MASK_COUNTER <= 1'b0;\n\t\t";
	outFile << "CLK_TRAINING_MASK_preReset <= 1'b0;\n\t\tCLK_TRAINING_MASK_INITIALIZER <= 1'b1;\n\t\tCLK_TRAINING_MASK_UPDATE_PARAM <= 1'b0;\n\t\tCLK_TRAINING_MASK <= 1'b1;\n\t\t";
	outFile << "switchUpdate <= 1'b0;\n\t";
	outFile << "end else if (TRAINING_MASK_COUNTER == (TRAINING_MASK_DURATION - 5)) begin \n\t\t";
	outFile << "TRAINING_MASK_COUNTER <= TRAINING_MASK_COUNTER + 1'b1;\n\t\t";
	outFile << "CLK_TRAINING_MASK_preReset <= 1'b0;\n\t\tCLK_TRAINING_MASK_INITIALIZER <= 1'b0;\n\t\tCLK_TRAINING_MASK_UPDATE_PARAM <= 1'b0;\n\t\tCLK_TRAINING_MASK <= 1'b0;\n\t\t";
	outFile << "switchUpdate <= 1'b1;\n\t";
	outFile << "end else if (TRAINING_MASK_COUNTER == (TRAINING_MASK_DURATION - 3)) begin\n\t\t";
	outFile << "TRAINING_MASK_COUNTER <= TRAINING_MASK_COUNTER + 1'b1;\n\t\t";
	outFile << "CLK_TRAINING_MASK_preReset <= 1'b1;\n\t\tCLK_TRAINING_MASK_INITIALIZER <= 1'b0;\n\t\tCLK_TRAINING_MASK_UPDATE_PARAM <= 1'b0;\n\t\tCLK_TRAINING_MASK <= 1'b0;\n\t\t";
	outFile << "switchUpdate <= 1'b0;\n\t";
	outFile << "end else if (TRAINING_MASK_COUNTER == (TRAINING_MASK_DURATION - 4)) begin\n\t\t";
	outFile << "TRAINING_MASK_COUNTER <= TRAINING_MASK_COUNTER + 1'b1;\n\t\t";
	outFile << "CLK_TRAINING_MASK_preReset <= 1'b0;\n\t\tCLK_TRAINING_MASK_INITIALIZER <= 1'b0;\n\t\tCLK_TRAINING_MASK_UPDATE_PARAM <= 1'b1;\n\t\tCLK_TRAINING_MASK <= 1'b0;\n\t\t";
	outFile << "switchUpdate <= 1'b0;\n\t";
	outFile << "end else if (TRAINING_MASK_COUNTER == (TRAINING_MASK_DURATION - 1)) begin\n\t\t";
	outFile << "TRAINING_MASK_COUNTER <= TRAINING_MASK_COUNTER + 1'b1;\n\t\t";
	outFile << "CLK_TRAINING_MASK_preReset <= 1'b0;\n\t\tCLK_TRAINING_MASK_INITIALIZER <= 1'b1;\n\t\tCLK_TRAINING_MASK_UPDATE_PARAM <= 1'b0;\n\t\tCLK_TRAINING_MASK <= 1'b0;\n\t\t";
	outFile << "switchUpdate <= 1'b0;\n\t";
	outFile << "end else if (TRAINING_MASK_COUNTER == (TRAINING_MASK_DURATION)) begin\n\t\t";
	outFile << "TRAINING_MASK_COUNTER <= 1'd0;\n\t\t";
	outFile << "CLK_TRAINING_MASK_preReset <= 1'b0;\n\t\tCLK_TRAINING_MASK_INITIALIZER <= 1'b0;\n\t\tCLK_TRAINING_MASK_UPDATE_PARAM <= 1'b0;\n\t\tCLK_TRAINING_MASK <= 1'b1;\n\t\t";
	outFile << "switchUpdate <= 1'b0;\n\t";
	outFile << "end else begin\n\t\t";
	outFile << "TRAINING_MASK_COUNTER <= TRAINING_MASK_COUNTER + 1'b1;\n\t\t";
	outFile << "CLK_TRAINING_MASK_preReset <= 1'b0;\n\t\tCLK_TRAINING_MASK_INITIALIZER <= 1'b0;\n\t\tCLK_TRAINING_MASK_UPDATE_PARAM <= 1'b0;\n\t\tCLK_TRAINING_MASK <= 1'b0;\n\t\t";
	outFile << "switchUpdate <= 1'b0;\n\t";
	outFile << "end\n";
	outFile << "end\n\n";

	outFile << "always @(posedge CLK_TRAINING_MASK or posedge INIT) begin\n\t";
	outFile << "if (INIT) begin\n\t\t";
	outFile << "CURRENT_TRAINING_MASK <= 1'd0;\n\t";
	outFile << "end\n\t";
	outFile << "else if (CURRENT_TRAINING_MASK < (N_TRAINING_MASKS - 1'b1)) begin\n\t\t";
	outFile << "CURRENT_TRAINING_MASK <= CURRENT_TRAINING_MASK + 1'd1;\n\t";
	outFile << "end else begin\n\t\t";
	outFile << "CURRENT_TRAINING_MASK <= 1'd0;\n\t";
	outFile << "end\n";
	outFile << "end\n\n";

	for (int i = 0; i < sConfig.parameterMask.Nmasks; i++)
		outFile << "assign UPDATE_TM[" << i << "] = ((CURRENT_TRAINING_MASK == 8'd" << i << ")&(switchUpdate));\n";

	int alphaN = 0;
	int betaN = 0;

	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[l - 1]; i++) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
				outFile << "NN_LATCHED_PARAM_MODIFIER #(.N_RESISTANCE(RESISTANCE_precision)) LM_alphaD_L" << l << "_" << i << "_" << j << "(.INIT(INIT), .INIT_late(INIT_late), .CLK(CLK), .EN(TrainingActive&alpha_L" << l << "_trainingActive), ";
				outFile << ".TRIG(UPDATE_TM[" << sConfig.parameterMask.alpha_mask[alphaN].set << "]), .OUT(alphaD_L" << l << "_" << i << "_" << j << "), .SIGN_OUT(SIGN_alpha_L" << l<< "_j_" << j << "[" << i << "]), .MODIFIER(gradD" << sConfig.parameterMask.alpha_mask[alphaN].mapping << "), .SIGN_MODIFIER(SIGN_gradD" << sConfig.parameterMask.alpha_mask[alphaN].mapping << "), ";
				outFile << ".RESISTANCE_OUT(resistance_alpha_L" << l << "_" << i << "_" << j << "), .RESISTANCE_NEW(resistance" << sConfig.parameterMask.alpha_mask[alphaN].mapping << "), .INITIAL_RESISTANCE(" << sConfig.resistancePrecision << "'d0), ";
				outFile << ".INITIAL_VALUE(8'd" << floor((float)rand() / RAND_MAX * ((float)pow(2.0, 4.0) - 1)) << "),";
				string randSign = "1'd0";
				if ((float)((rand() + 0.001) / RAND_MAX + 0.001) > 0.5)
					randSign = "1'd1";
				outFile << ".INITIAL_SIGN(" << randSign << "));\n";
				alphaN++;
			}
		}
		outFile << "\n";
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
			outFile << "NN_LATCHED_PARAM_MODIFIER #(.N_RESISTANCE(RESISTANCE_precision)) LM_betaD_L" << l << "_" << j << "(.INIT(INIT), .INIT_late(INIT_late), .CLK(CLK), .EN(TrainingActive&beta_L" << l << "_trainingActive), ";
			outFile << ".TRIG(UPDATE_TM[" << sConfig.parameterMask.beta_mask[betaN].set << "]), .OUT(betaD_L" << l << "_" << j << "), .SIGN_OUT(SIGN_beta_L" << l << "[" << j << "]), .MODIFIER(gradD" << sConfig.parameterMask.beta_mask[betaN].mapping << "), .SIGN_MODIFIER(SIGN_gradD" << sConfig.parameterMask.beta_mask[betaN].mapping << "),";
			outFile << ".RESISTANCE_OUT(resistance_beta_L" << l << "_" << j << "), .RESISTANCE_NEW(resistance" << sConfig.parameterMask.beta_mask[betaN].mapping << "), .INITIAL_RESISTANCE(" << sConfig.resistancePrecision << "'d40), ";
			outFile << ".INITIAL_VALUE(8'd" << floor((float)rand() / RAND_MAX * ((float)pow(2.0, 4.0) - 1)) << "),";
			string randSign = "1'd0";
			if ((float)((rand() + 0.001) / RAND_MAX + 0.001) > 0.5)
				randSign = "1'd1";
			outFile << ".INITIAL_SIGN(" << randSign << "));\n";
			betaN++;
		}
		outFile << "\n";
	}

	for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
		outFile << "assign TM_paramSet" << map << " = { ";
		stringstream outString;
		for (int set = sConfig.parameterMask.Nmasks-1; set >= 0; set--) {
			bool assignedParam = false;
			for (int i_a = 0; i_a < sConfig.parameterMask.Nalpha; i_a++) {
				if ((sConfig.parameterMask.alpha_mask[i_a].set == set) && (sConfig.parameterMask.alpha_mask[i_a].mapping == map))
				{
					outString << "dalpha_L" << sConfig.parameterMask.alpha_mask[i_a].layer << "_j_" << sConfig.parameterMask.alpha_mask[i_a].node << "[" << sConfig.parameterMask.alpha_mask[i_a].node_prev << "], ";
					assignedParam = true;
					break;
				}
			}
			for (int i_b = 0; i_b < sConfig.parameterMask.Nbeta; i_b++) {
				if ((sConfig.parameterMask.beta_mask[i_b].set == set) && (sConfig.parameterMask.beta_mask[i_b].mapping == map))
				{
					outString << "dbeta_L" << sConfig.parameterMask.beta_mask[i_b].layer << "[" << sConfig.parameterMask.beta_mask[i_b].node << "], ";
					assignedParam = true;
					break;
				}
			}
			if (!assignedParam) {
				outString << "1'b0, ";
			}
		}
		//outFile << "dalpha_L1_j_7[3], dbeta_L2[2], dbeta_L2[1], dbeta_L2[0]";
		outString.seekp(-2, outString.cur);
		outString << "};\n";
		outFile << outString.str();
	}
	outFile << "\n";
	for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
		outFile << "assign TM_SIGN_paramSet" << map << " = { ";
		stringstream outString;
		for (int set = sConfig.parameterMask.Nmasks - 1; set >= 0; set--) {
			bool assignedParam = false;
			for (int i_a = 0; i_a < sConfig.parameterMask.Nalpha; i_a++) {
				if ((sConfig.parameterMask.alpha_mask[i_a].set == set) && (sConfig.parameterMask.alpha_mask[i_a].mapping == map))
				{
					outString << "SIGN_dalpha_L" << sConfig.parameterMask.alpha_mask[i_a].layer << "_j_" << sConfig.parameterMask.alpha_mask[i_a].node << "[" << sConfig.parameterMask.alpha_mask[i_a].node_prev << "], ";
					assignedParam = true;
					break;
				}
			}
			for (int i_b = 0; i_b < sConfig.parameterMask.Nbeta; i_b++) {
				if ((sConfig.parameterMask.beta_mask[i_b].set == set) && (sConfig.parameterMask.beta_mask[i_b].mapping == map))
				{
					outString << "SIGN_dbeta_L" << sConfig.parameterMask.beta_mask[i_b].layer << "[" << sConfig.parameterMask.beta_mask[i_b].node << "], ";
					assignedParam = true;
					break;
				}
			}
			if (!assignedParam) {
				outString << "1'b0, ";
			}
		}
		//outFile << "dalpha_L1_j_7[3], dbeta_L2[2], dbeta_L2[1], dbeta_L2[0]";
		outString.seekp(-2, outString.cur);
		outString << "};\n";
		outFile << outString.str();
	}
	outFile << "\n";
	for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
		outFile << "assign grad" << map << " = TM_paramSet" << map << "[CURRENT_TRAINING_MASK];\n";
	}
	outFile << "\n";
	for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
		outFile << "assign SIGN_grad" << map << " = TM_SIGN_paramSet" << map << "[CURRENT_TRAINING_MASK];\n";
	}
	outFile << "\n";
	outFile << "always @(posedge CLK_TRAINING_MASK_INITIALIZER or posedge INIT_late) begin\n\t";
	outFile << "if (INIT_late) begin\n\t\t";
	if (true) {
		stringstream outString;
		for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
			outString << "paramD" << map << " <= ";
			bool assignedParam = false;
			for (int i_a = 0; i_a < sConfig.parameterMask.Nalpha; i_a++) {
				if ((sConfig.parameterMask.alpha_mask[i_a].set == 0) && (sConfig.parameterMask.alpha_mask[i_a].mapping == map))
				{
					outString << "alphaD_L" << sConfig.parameterMask.alpha_mask[i_a].layer << "_" << sConfig.parameterMask.alpha_mask[i_a].node_prev << "_" << sConfig.parameterMask.alpha_mask[i_a].node << ";";
					assignedParam = true;
					break;
				}
			}
			for (int i_b = 0; i_b < sConfig.parameterMask.Nbeta; i_b++) {
				if ((sConfig.parameterMask.beta_mask[i_b].set == 0) && (sConfig.parameterMask.beta_mask[i_b].mapping == map))
				{
					outString << "betaD_L" << sConfig.parameterMask.beta_mask[i_b].layer << "_" << sConfig.parameterMask.beta_mask[i_b].node << ";";
					assignedParam = true;
					break;
				}
			}
			if (!assignedParam) {
				outString << "1'b0;";
			}
			outString << "\n\t\t";
		}
		for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
			outString << "SIGN_param" << map << " <= ";
			bool assignedParam = false;
			for (int i_a = 0; i_a < sConfig.parameterMask.Nalpha; i_a++) {
				if ((sConfig.parameterMask.alpha_mask[i_a].set == 0) && (sConfig.parameterMask.alpha_mask[i_a].mapping == map))
				{
					outString << "SIGN_alpha_L" << sConfig.parameterMask.alpha_mask[i_a].layer << "_j_" << sConfig.parameterMask.alpha_mask[i_a].node << "[" << sConfig.parameterMask.alpha_mask[i_a].node_prev << "];";
					assignedParam = true;
					break;
				}
			}
			for (int i_b = 0; i_b < sConfig.parameterMask.Nbeta; i_b++) {
				if ((sConfig.parameterMask.beta_mask[i_b].set == 0) && (sConfig.parameterMask.beta_mask[i_b].mapping == map))
				{
					outString << "SIGN_beta_L" << sConfig.parameterMask.beta_mask[i_b].layer << "[" << sConfig.parameterMask.beta_mask[i_b].node << "];";
					assignedParam = true;
					break;
				}
			}
			if (!assignedParam) {
				outString << "1'b0;";
			}
			outString << "\n\t\t";
		}
		// resistance
		for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
			outString << "resistance_INIT" << map << " <= ";
			bool assignedParam = false;
			for (int i_a = 0; i_a < sConfig.parameterMask.Nalpha; i_a++) {
				if ((sConfig.parameterMask.alpha_mask[i_a].set == 0) && (sConfig.parameterMask.alpha_mask[i_a].mapping == map))
				{
					outString << "resistance_alpha_L" << sConfig.parameterMask.alpha_mask[i_a].layer << "_" << sConfig.parameterMask.alpha_mask[i_a].node_prev << "_" << sConfig.parameterMask.alpha_mask[i_a].node << ";";
					assignedParam = true;
					break;
				}
			}
			for (int i_b = 0; i_b < sConfig.parameterMask.Nbeta; i_b++) {
				if ((sConfig.parameterMask.beta_mask[i_b].set == 0) && (sConfig.parameterMask.beta_mask[i_b].mapping == map))
				{
					outString << "resistance_beta_L" << sConfig.parameterMask.beta_mask[i_b].layer << "_" << sConfig.parameterMask.beta_mask[i_b].node << ";";
					assignedParam = true;
					break;
				}
			}
			if (!assignedParam) {
				outString << "1'b0;";
			}
			outString << "\n\t\t";
		}
		outString.seekp(-2, outString.cur);
		outString << "\n\t";
		outFile << outString.str();
	}
	for (int set=0; set<sConfig.parameterMask.Nmasks; set++)
	{
		outFile << "end else if (NEXT_TRAINING_MASK == 8'd" << set << ") begin\n\t\t";
		stringstream outString;
		// paramD
		for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
			outString << "paramD" << map << " <= ";
			bool assignedParam = false;
			for (int i_a = 0; i_a < sConfig.parameterMask.Nalpha; i_a++) {
				if ((sConfig.parameterMask.alpha_mask[i_a].set == set) && (sConfig.parameterMask.alpha_mask[i_a].mapping == map))
				{
					outString << "alphaD_L" << sConfig.parameterMask.alpha_mask[i_a].layer << "_" << sConfig.parameterMask.alpha_mask[i_a].node_prev << "_" << sConfig.parameterMask.alpha_mask[i_a].node << ";";
					assignedParam = true;
					break;
				}
			}
			for (int i_b = 0; i_b < sConfig.parameterMask.Nbeta; i_b++) {
				if ((sConfig.parameterMask.beta_mask[i_b].set == set) && (sConfig.parameterMask.beta_mask[i_b].mapping == map))
				{
					outString << "betaD_L" << sConfig.parameterMask.beta_mask[i_b].layer << "_" << sConfig.parameterMask.beta_mask[i_b].node << ";";
					assignedParam = true;
					break;
				}
			}
			if (!assignedParam) {
				outString << "1'b0;";
			}
			outString << "\n\t\t";
		}
		// SIGN_param
		for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
			outString << "SIGN_param" << map << " <= ";
			bool assignedParam = false;
			for (int i_a = 0; i_a < sConfig.parameterMask.Nalpha; i_a++) {
				if ((sConfig.parameterMask.alpha_mask[i_a].set == set) && (sConfig.parameterMask.alpha_mask[i_a].mapping == map))
				{
					outString << "SIGN_alpha_L" << sConfig.parameterMask.alpha_mask[i_a].layer << "_j_" << sConfig.parameterMask.alpha_mask[i_a].node << "[" << sConfig.parameterMask.alpha_mask[i_a].node_prev << "];";
					assignedParam = true;
					break;
				}
			}
			for (int i_b = 0; i_b < sConfig.parameterMask.Nbeta; i_b++) {
				if ((sConfig.parameterMask.beta_mask[i_b].set == set) && (sConfig.parameterMask.beta_mask[i_b].mapping == map))
				{
					outString << "SIGN_beta_L" << sConfig.parameterMask.beta_mask[i_b].layer << "[" << sConfig.parameterMask.beta_mask[i_b].node << "];";
					assignedParam = true;
					break;
				}
			}
			if (!assignedParam) {
				outString << "1'b0;";
			}
			outString << "\n\t\t";
		}
		// resistance
		for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
			outString << "resistance_INIT" << map << " <= ";
			bool assignedParam = false;
			for (int i_a = 0; i_a < sConfig.parameterMask.Nalpha; i_a++) {
				if ((sConfig.parameterMask.alpha_mask[i_a].set == set) && (sConfig.parameterMask.alpha_mask[i_a].mapping == map))
				{
					outString << "resistance_alpha_L" << sConfig.parameterMask.alpha_mask[i_a].layer << "_" << sConfig.parameterMask.alpha_mask[i_a].node_prev << "_" << sConfig.parameterMask.alpha_mask[i_a].node << ";";
					assignedParam = true;
					break;
				}
			}
			for (int i_b = 0; i_b < sConfig.parameterMask.Nbeta; i_b++) {
				if ((sConfig.parameterMask.beta_mask[i_b].set == set) && (sConfig.parameterMask.beta_mask[i_b].mapping == map))
				{
					outString << "resistance_beta_L" << sConfig.parameterMask.beta_mask[i_b].layer << "_" << sConfig.parameterMask.beta_mask[i_b].node << ";";
					assignedParam = true;
					break;
				}
			}
			if (!assignedParam) {
				outString << "1'b0;";
			}
			outString << "\n\t\t";
		}
		outString.seekp(-2, outString.cur);
		outFile << outString.str() << "\n\t";
	}	
	if (true) {
		outFile << "end else begin\n\t\t";
		stringstream outString;
		for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
			outString << "paramD" << map << " <= ";
			bool assignedParam = false;
			for (int i_a = 0; i_a < sConfig.parameterMask.Nalpha; i_a++) {
				if ((sConfig.parameterMask.alpha_mask[i_a].set == 0) && (sConfig.parameterMask.alpha_mask[i_a].mapping == map))
				{
					outString << "alphaD_L" << sConfig.parameterMask.alpha_mask[i_a].layer << "_" << sConfig.parameterMask.alpha_mask[i_a].node_prev << "_" << sConfig.parameterMask.alpha_mask[i_a].node << ";";
					assignedParam = true;
					break;
				}
			}
			for (int i_b = 0; i_b < sConfig.parameterMask.Nbeta; i_b++) {
				if ((sConfig.parameterMask.beta_mask[i_b].set == 0) && (sConfig.parameterMask.beta_mask[i_b].mapping == map))
				{
					outString << "betaD_L" << sConfig.parameterMask.beta_mask[i_b].layer << "_" << sConfig.parameterMask.beta_mask[i_b].node << ";";
					assignedParam = true;
					break;
				}
			}
			if (!assignedParam) {
				outString << "1'b0;";
			}
			outString << "\n\t\t";
		}
		for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
			outString << "SIGN_param" << map << " <= ";
			bool assignedParam = false;
			for (int i_a = 0; i_a < sConfig.parameterMask.Nalpha; i_a++) {
				if ((sConfig.parameterMask.alpha_mask[i_a].set == 0) && (sConfig.parameterMask.alpha_mask[i_a].mapping == map))
				{
					outString << "SIGN_alpha_L" << sConfig.parameterMask.alpha_mask[i_a].layer << "_j_" << sConfig.parameterMask.alpha_mask[i_a].node << "[" << sConfig.parameterMask.alpha_mask[i_a].node_prev << "];";
					assignedParam = true;
					break;
				}
			}
			for (int i_b = 0; i_b < sConfig.parameterMask.Nbeta; i_b++) {
				if ((sConfig.parameterMask.beta_mask[i_b].set == 0) && (sConfig.parameterMask.beta_mask[i_b].mapping == map))
				{
					outString << "SIGN_beta_L" << sConfig.parameterMask.beta_mask[i_b].layer << "[" << sConfig.parameterMask.beta_mask[i_b].node << "];";
					assignedParam = true;
					break;
				}
			}
			if (!assignedParam) {
				outString << "1'b0;";
			}
			outString << "\n\t\t";
		}
		// resistance
		for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
			outString << "resistance_INIT" << map << " <= ";
			bool assignedParam = false;
			for (int i_a = 0; i_a < sConfig.parameterMask.Nalpha; i_a++) {
				if ((sConfig.parameterMask.alpha_mask[i_a].set == 0) && (sConfig.parameterMask.alpha_mask[i_a].mapping == map))
				{
					outString << "resistance_alpha_L" << sConfig.parameterMask.alpha_mask[i_a].layer << "_" << sConfig.parameterMask.alpha_mask[i_a].node_prev << "_" << sConfig.parameterMask.alpha_mask[i_a].node << ";";
					assignedParam = true;
					break;
				}
			}
			for (int i_b = 0; i_b < sConfig.parameterMask.Nbeta; i_b++) {
				if ((sConfig.parameterMask.beta_mask[i_b].set == 0) && (sConfig.parameterMask.beta_mask[i_b].mapping == map))
				{
					outString << "resistance_beta_L" << sConfig.parameterMask.beta_mask[i_b].layer << "_" << sConfig.parameterMask.beta_mask[i_b].node << ";";
					assignedParam = true;
					break;
				}
			}
			if (!assignedParam) {
				outString << "1'b0;";
			}
			outString << "\n\t\t";
		}
		outString.seekp(-2, outString.cur);
		outString << "\tend\n\t";
		outFile << outString.str() << endl;
	}
	outFile << "end\n\n";


	outFile << "\nendmodule";
	outFile.close();
}

void genPARAM_TRAINING_MASK(sysConfig sConfig) {

	ofstream outFile;
	outFile.open(sConfig.Dir + "PARAM_TRAINING_MASK.v");
	outFile << "// Chris Ceroici \n\n";
	outFile << "module PARAM_TRAINING_MASK(\n\t";
	NN_DeclareParameterMaskNets(outFile, sConfig, ModuleDeclarationPortList);

	outFile << "TrainingActive,\n\t";
	outFile << "CLK, CLK_TRAINING_flag, TRAINING_CYCLE, UPDATE, INIT\n";
	outFile << ");\n\n";

	outFile << "parameter DP_in = 8;\n";
	outFile << "parameter DP_out = 16;\n";
	outFile << "parameter MAX_RATE_ALPHA = 6;\n";
	outFile << "parameter MAX_RATE_BETA = 5;\n";
	outFile << "parameter RESISTANCE_precision = 6;\n";
	outFile << "parameter RESISTANCE_MAX = 100;\n\n";

	NN_DeclareParameterMaskNets(outFile, sConfig, ModuleDeclarationPortsInput);
	outFile << "\ninput wire TrainingActive;\n";
	outFile << "input wire CLK, CLK_TRAINING_flag, TRAINING_CYCLE, INIT;\n";
	outFile << "input wire UPDATE;\n\n";

	DeclareNet(outFile, "reg", "1", sConfig.parameterMask.maskSize, "CLK_regu_param", "");
	DeclareNet(outFile, "wire", "1", sConfig.parameterMask.maskSize, "CLK_param", "");
	DeclareNet(outFile, "wire", "1", sConfig.parameterMask.maskSize, "DC", "");
	DeclareNet(outFile, "reg", "8", sConfig.parameterMask.maskSize, "clock_counter", "");

	outFile << "wire [RESISTANCE_precision-1:0] GLOBAL_RESISTANCE;\n";
	outFile << "assign GLOBAL_RESISTANCE = 6'd0;\n\n";

	for (int map = 0; map < sConfig.parameterMask.maskSize; map++)
		outFile << "assign CLK_param" << map << " = ((resistance" << map << " > 1'b0)&(!INIT)) ? CLK_regu_param" << map << " : CLK;\n";
	outFile << "\n";

	for (int map = 0; map < sConfig.parameterMask.maskSize; map++)
	{
		outFile << "always @(posedge CLK or posedge INIT or posedge UPDATE) begin\n\t";
		outFile << "if (INIT|UPDATE) begin\n\t\t";
		outFile << "clock_counter" << map << " <= 1'b0;\n\t\t";
		outFile << "CLK_regu_param" << map << " <= 1'b0;\n\t";
		outFile << "end else if (clock_counter" << map << " == resistance" << map << ") begin\n\t\t";
		outFile << "clock_counter" << map << " <= 1'b0;\n\t\t";
		outFile << "CLK_regu_param" << map << " <= 1'b1;\n\t";
		outFile << "end else begin\n\t\t";
		outFile << "clock_counter" << map << " <= clock_counter" << map << " + 1'b1;\n\t\t";
		outFile << "CLK_regu_param" << map << " <= 1'b0;\n\t";
		outFile << "end\n";
		outFile << "end\n\n";
	}

	for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
		outFile << "NN_SMOOTHGRAD_POLAR_STATIC_RESISTANCE SG" << map << "(.CLK(CLK_param" << map << "), .INIT(UPDATE), .IN_SS(grad" << map << "), .SIGN(SIGN_grad" << map << "), .OUT(gradD" << map << "), .SIGN_out(SIGN_gradD" << map << "), .DIRECTION_CHANGE(DC" << map << "), .OUT_INIT(paramD" << map << "), .SIGN_OUT_INIT(SIGN_param" << map << "), .EN(TrainingActive));\n";
	}
	for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
		outFile << "defparam SG" << map << ".N = 8; // decimal output size\n";
	}
	for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
		outFile << "defparam SG" << map << ".N_RESISTANCE = RESISTANCE_precision;\n";
	}
	outFile << "\n";
	for (int map = 0; map < sConfig.parameterMask.maskSize; map++) {
		outFile << "always @(posedge UPDATE or posedge DC" << map << " or posedge (CLK&INIT)) begin\n\t";
		outFile << "if (UPDATE|INIT) resistance" << map << " <= resistance_INIT" << map << ";\n\t";
		outFile << "else if (DC" << map << ") resistance" << map << " <= resistance" << map << " + (resistance" << map << "<RESISTANCE_MAX);\n";
		outFile << "end\n\n";
	}
	outFile << "\nendmodule";
	outFile.close();
}


void genPARAMS_parameterExportBlock(std::ostream &outFile, sysConfig sConfig, int& NparameterExports) {
	int index = 0;
	int blockSize = 16; // size of parameter export register
	outFile << "always @(posedge NNParamExp_CLK) begin\n\t";
	outFile << "case (NNParamExp_index)\n\t\t";
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		if (true) {
			// Alpha export
			for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++) {
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l - 1]; j++) {
					if (ConnectionExists(sConfig.NNobj, l, j, k))
						outFile << "16'd" << index++ << ": NNParamExp_data = alphaD_L" << l << "_" << j << "_" << k << ";\n\t\t";
				}
			}
			outFile << "\n\t\t";
			for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++) {
				// SIGN Alpha export
				if (ConnectionExists(sConfig.NNobj, l, 0, k)) {
					int SignBusSize = sConfig.NNobj.LYRSIZE[l - 1];
					if (sConfig.NNobj.LYRinfo[l].LT == CV)
						SignBusSize = sConfig.NNobj.LYRinfo[l].conv.Nconnections;
					if (SignBusSize <= blockSize)
						outFile << "16'd" << index++ << ": NNParamExp_data = SIGN_alpha_L" << l << "_j_" << k << ";\n\t\t";
					else {
						int N_required_bytes = ceil((double)SignBusSize / blockSize);
						int startLoc = SignBusSize;
						int endLoc = SignBusSize - blockSize;
						for (int n = 0; n < (N_required_bytes - 1); n++) {
							outFile << "16'd" << index++ << ": NNParamExp_data = SIGN_alpha_L" << l << "_j_" << k << "[" << startLoc - 1 << ":" << endLoc << "];\n\t\t";
							startLoc -= blockSize;
							endLoc -= blockSize;
						}
						if ((SignBusSize % blockSize) == 0)
							outFile << "16'd" << index++ << ": NNParamExp_data = SIGN_alpha_L" << l << "_j_" << k << "[" << startLoc - 1 << ":" << endLoc << "];\n\t\t";
						else
							outFile << "16'd" << index++ << ": NNParamExp_data = {" << blockSize - SignBusSize % blockSize << "'d0 ,SIGN_alpha_L" << l << "_j_" << k << "[" << SignBusSize % blockSize - 1 << ":0]};\n\t\t";
					}
				}
			}
			if (sConfig.NNobj.LYRinfo[l].LT == RCC) {
				outFile << "\n\t\t";
				// Gamma Export
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
						outFile << "16'd" << index++ << ": NNParamExp_data = gammaD_L" << l << "_" << j << "_" << k << ";\n\t\t";
					}
				}
				outFile << "\n\t\t";
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++) {
					// Sign Gamma Export
					if (sConfig.NNobj.LYRSIZE[l] <= blockSize)
						outFile << "16'd" << index++ << ": NNParamExp_data = SIGN_gamma_L" << l << "_j_" << k << ";\n\t\t";
					else {
						int N_required_bytes = ceil((double)sConfig.NNobj.LYRSIZE[l] / blockSize);
						int startLoc = sConfig.NNobj.LYRSIZE[l];
						int endLoc = sConfig.NNobj.LYRSIZE[l] - blockSize;
						for (int n = 0; n < (N_required_bytes - 1); n++) {
							outFile << "16'd" << index++ << ": NNParamExp_data = SIGN_gamma_L" << l << "_j_" << k << "[" << startLoc - 1 << ":" << endLoc << "];\n\t\t";
							startLoc -= blockSize;
							endLoc -= blockSize;
						}
						if ((sConfig.NNobj.LYRSIZE[l - 1] % blockSize) == 0)
							outFile << "16'd" << index++ << ": NNParamExp_data = SIGN_gamma_L" << l << "_j_" << k << "[" << startLoc - 1 << ":" << endLoc << "];\n\t\t";
						else
							outFile << "16'd" << index++ << ": NNParamExp_data = {" << blockSize - sConfig.NNobj.LYRSIZE[l] % blockSize << "'d0 ,SIGN_gamma_L" << l << "_j_" << k << "[" << sConfig.NNobj.LYRSIZE[l] % blockSize - 1 << ":0]};\n\t\t";
					}

				}
			}
			outFile << "\n\t\t";
			int betaSize = sConfig.NNobj.LYRSIZE[l];
			if (sConfig.NNobj.LYRinfo[l].LT == CV)
				betaSize = sConfig.NNobj.LYRinfo[l].conv.filters;
			for (int k = 0; k < betaSize; k++) {
				outFile << "16'd" << index++ << ": NNParamExp_data = betaD_L" << l << "_" << k << ";\n\t\t";
			}
			outFile << "\n\t\t";
			if (betaSize <= blockSize)
				outFile << "16'd" << index++ << ": NNParamExp_data = SIGN_beta_L" << l << ";\n\t\t";
			else {
				int N_required_bytes = ceil((double)betaSize / blockSize);
				int startLoc = sConfig.NNobj.LYRSIZE[l];
				int endLoc = betaSize - blockSize;
				for (int n = 0; n < (N_required_bytes - 1); n++) {
					outFile << "16'd" << index++ << ": NNParamExp_data = SIGN_beta_L" << l << "[" << startLoc - 1 << ":" << endLoc << "];\n\t\t";
					startLoc -= blockSize;
					endLoc -= blockSize;
				}
				if ((betaSize % blockSize) == 0)
					outFile << "16'd" << index++ << ": NNParamExp_data = SIGN_beta_L" << l << "[" << startLoc - 1 << ":" << endLoc << "];\n\t\t";
				else
					outFile << "16'd" << index++ << ": NNParamExp_data = {" << blockSize - betaSize % blockSize << "'d0 , SIGN_beta_L" << l << "[" << betaSize % blockSize - 1 << ":0]};\n\t\t";
			}
			outFile << "\n\t\t";
		}
		else if (sConfig.IsConnectionType(l, CV_CV)) {
			// ********************************* NEED TO IMPLEMENT
			cout << "ERROR: INVALID LAYER CONFIGURATION (genPARAMS_sim.cpp)\n";
		}
	}
	outFile << "default: NNParamExp_data = 1'b0;\n\t";
	outFile << "\n\tendcase\n";
	outFile << "end\n\n";

	NparameterExports = index;
}



void genPARAMS_sim_Inst(std::ostream &outFile, sysConfig sConfig) {
	if (sConfig.optimizer == fullHardware) {
		int Lindex = 0;
		int negone = -1;
		outFile << "// Parameters Module:\n";
		outFile << "PARAMS_sim PARAMS0(\n";
		ofstream outFile_ext;
		outFile_ext.open(sConfig.Dir + "PARAMS_portDefinitions.v");
		outFile << "`include \"PARAMS_portDefinitions.v\"\n\t";
		NN_DeclareDecimalWeightNets(outFile_ext, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts);
		NN_DeclareWeightNets(outFile_ext, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, false, true);
		NN_DeclareWeightNets(outFile_ext, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, false);
		NN_DeclareWeightNets(outFile_ext, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, true);
		outFile << ".TrainingActive(TrainingActive),\n\t";
		outFile << ".NNParamExp_CLK(ReadParams_clk), .NNParamExp_index(ReadWriteIndex), .NNParamExp_data(ParameterRead), \n\t";
		outFile << ".CLK(CLK), .CLK_TRAINING_flag(CLK_TRAINING_flag), .TRAINING_CYCLE(CLK_TRAINING_flag), .BP_FRAME(BP_FRAME), .FP_FRAME(FP_FRAME), .UPDATE(pre_BP_FRAME), .INIT(INIT)\n);\n\n";
		outFile_ext.close();
	}
	else if (sConfig.optimizer == alternatingHardwareMask){
		genPARAM_MASK_STORAGE_Inst(outFile, sConfig);
		genPARAM_TRAINING_MASK_Inst(outFile, sConfig);
	}
}

void genPARAM_MASK_STORAGE_Inst(std::ostream &outFile, sysConfig sConfig) {
	int Lindex = 0;
	int negone = -1;
	outFile << "// Parameters Module:\n";
	outFile << "PARAM_STORAGE PARAM_STORAGE0(\n";
	ofstream outFile_ext;
	outFile_ext.open(sConfig.Dir + "PARAM_STORAGE_portDefinitions.v");
	outFile << "`include \"PARAM_STORAGE_portDefinitions.v\"\n\t";
	NN_DeclareDecimalWeightNets(outFile_ext, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts);
	NN_DeclareWeightNets(outFile_ext, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, false, true);
	NN_DeclareWeightNets(outFile_ext, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, false);
	NN_DeclareWeightNets(outFile_ext, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, true);
	NN_DeclareParameterMaskNets(outFile_ext, sConfig, ModuleInstantiationPorts);
	outFile << ".TrainingActive(TrainingActive),\n\t";
	outFile << ".NNParamExp_CLK(ReadParams_clk), .NNParamExp_index(ReadWriteIndex), .NNParamExp_data(ParameterRead), \n\t";
	outFile << ".CLK(CLK), .CLK_TRAINING_flag(CLK_TRAINING_flag), .CLK_TRAINING_MASK(CLK_TRAINING_MASK), .TRAINING_CYCLE(CLK_TRAINING_flag), .UPDATE(pre_BP_FRAME), .INIT(INIT), .INIT_late(INIT_late)\n);\n\n";
	outFile_ext.close();
}

void genPARAM_TRAINING_MASK_Inst(std::ostream &outFile, sysConfig sConfig) {
	int Lindex = 0;
	int negone = -1;
	outFile << "// Parameter Mask Modification Module:\n";
	outFile << "PARAM_TRAINING_MASK PARAM_TRAINING_MASK0(\n";
	ofstream outFile_ext;
	outFile_ext.open(sConfig.Dir + "PARAM_TRAINING_MASK_portDefinitions.v");
	outFile << "`include \"PARAM_TRAINING_MASK_portDefinitions.v\"\n\t";
	NN_DeclareParameterMaskNets(outFile_ext, sConfig, ModuleInstantiationPorts);
	outFile << ".TrainingActive(TrainingActive),\n\t";
	outFile << ".CLK(CLK), .CLK_TRAINING_flag(CLK_TRAINING_flag), .TRAINING_CYCLE(CLK_TRAINING_flag), .UPDATE(CLK_TRAINING_MASK), .INIT(INIT)\n);\n\n";
	outFile_ext.close();
}