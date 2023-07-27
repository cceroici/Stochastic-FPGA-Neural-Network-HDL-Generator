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
#include "genBCKDPROP.h"
#include "NN_DataTypes.h"
#include "genFULLCONN.h"
#include "genCONV.h"
#include "Verilog.h"
#include "main.h"

using namespace std;

void genBCKDPROP(sysConfig sConfig){

	
	ofstream outFile;
	outFile.open(sConfig.Dir + "BCKDPROP.v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Neural Network Backward Propagation \n\n";
	int NProd = 0;
	int Nindex = 0;
	int negone = -1;
	outFile << "module BCKDPROP(\n\t";
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, false, true);
	DeclareForwardPropagationNets(outFile, sConfig, false, ModuleDeclarationPortList);
	DeclareSingleLayerNet(outFile, sConfig, "SIGN_L" + Int2Str(sConfig.NNobj.OUTPUTLYR), ModuleDeclarationPortList);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, true, false);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, true, true);
	DeclareSingleLayerNet(outFile, sConfig, "eps", ModuleDeclarationPortList);
	outFile << StringSeries(sConfig.RandNums.short_ADDSUB_BP, "R_BP", "") << ",\n\t";
	if (sConfig.IsRecurrent()) outFile << StringSeries(sConfig.RandNums.short_ADDSUB_BPTT, "R_BPTT", "") << ",\n\t";
	outFile << "CLK,  BP_FRAME, FP_FRAME, INIT\n);\n\n";

	outFile << "parameter MEMSIZE = 6;\n";
	outFile << "parameter DP_in = " << sConfig.DP_in << ";\n";
	outFile << "parameter DP_out = " << sConfig.DP_out << ";\n\n";
	

	// ---------------------------    port definitions
	outFile << "input wire CLK, BP_FRAME, FP_FRAME, INIT;\n";
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsInput);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsInput, false, true);
	DeclareForwardPropagationNets(outFile, sConfig, false, ModuleDeclarationPortsInput);
	DeclareSingleLayerNet(outFile, sConfig, "SIGN_L" + Int2Str(sConfig.NNobj.OUTPUTLYR), ModuleDeclarationPortsInput);
	DeclareSingleLayerNet(outFile, sConfig, "eps", ModuleDeclarationPortsInput);
	outFile << "input wire [" << sConfig.LFSR_S / 2 - 1 << ":0] " << StringSeries(sConfig.RandNums.short_ADDSUB_BP, "R_BP", "") << ";\n\n";
	if (sConfig.IsRecurrent()) outFile << "input wire [" << sConfig.LFSR_S / 2 - 1 << ":0] " << StringSeries(sConfig.RandNums.short_ADDSUB_BPTT, "R_BPTT", "") << ";\n\n";

	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsOutput, true, false);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsOutput, true, true);

	NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "delta", wire, TopLevel, singleLayer, sConfig.IsRecurrent(), -1);

	if (!sConfig.IsRecurrent()) {
		for (int l = (sConfig.NNobj.OUTPUTLYR); l > 0; l--) {
			if (sConfig.NNobj.LYRinfo[l].LT == CV) {
				outFile << "wire [" << sConfig.NNobj.LYRinfo[l].conv.Nconnections << "-1:0] ";
				stringstream netstring;
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					netstring << "dalpha_L" << l << "_n" << j << ", ";
				}
				TerminateLine(outFile, netstring.str(), -2, ";\n");
			}
			outFile << "wire [" << sConfig.NNobj.LYRSIZE[l] << "-1:0] dbeta_L" << l << "_n;\n";
		}
	}
	else {
		for (int l = (sConfig.NNobj.OUTPUTLYR); l > 0; l--) {
			if ((sConfig.NNobj.LYRinfo[l].LT == FC)| (sConfig.NNobj.LYRinfo[l].LT == RCC)) {
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
				{
					outFile << "wire [" << sConfig.NNobj.LYRSIZE[l - 1] << "-1:0] ";
					for (int i = 0; i < sConfig.NNobj.LYRSIZE[l] - 1; i++)
					{
						outFile << "dalpha_L" << l << "_T" << t << "_j_" << i << ", ";
					}
					outFile << "dalpha_L" << l << "_T" << t << "_j_" << sConfig.NNobj.LYRSIZE[l] - 1 << ";\n";
				}
				if (sConfig.NNobj.LYRinfo[l].LT == RCC) {
					for (int t = 0; t < sConfig.maxTimeSteps; t++)
					{
						outFile << "wire [" << sConfig.NNobj.LYRSIZE[l] << "-1:0] ";
						for (int i = 0; i < sConfig.NNobj.LYRSIZE[l] - 1; i++)
						{
							outFile << "dgamma_L" << l << "_T" << t << "_j_" << i << ", ";
						}
						outFile << "dgamma_L" << l << "_T" << t << "_j_" << sConfig.NNobj.LYRSIZE[l] - 1 << ";\n";
					}
				}
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					outFile << "wire [" << sConfig.NNobj.LYRSIZE[l] << "-1:0] " << "dbeta_L" << l << "_T" << t << ";\n";
			}
		}
		outFile << "\n";
		for (int l = (sConfig.NNobj.OUTPUTLYR); l > 0; l--) {
			if ((sConfig.NNobj.LYRinfo[l].LT == FC) | (sConfig.NNobj.LYRinfo[l].LT == RCC)) {
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
				{
					outFile << "wire [" << sConfig.NNobj.LYRSIZE[l - 1] << "-1:0] ";
					for (int i = 0; i < sConfig.NNobj.LYRSIZE[l] - 1; i++)
					{
						outFile << "SIGN_dalpha_L" << l << "_T" << t << "_j_" << i << ", ";
					}
					outFile << "SIGN_dalpha_L" << l << "_T" << t << "_j_" << sConfig.NNobj.LYRSIZE[l] - 1 << ";\n";
				}
				if (sConfig.NNobj.LYRinfo[l].LT == RCC) {
					for (int t = 0; t < sConfig.maxTimeSteps; t++)
					{
						outFile << "wire [" << sConfig.NNobj.LYRSIZE[l] << "-1:0] ";
						for (int i = 0; i < sConfig.NNobj.LYRSIZE[l] - 1; i++)
						{
							outFile << "SIGN_dgamma_L" << l << "_T" << t << "_j_" << i << ", ";
						}
						outFile << "SIGN_dgamma_L" << l << "_T" << t << "_j_" << sConfig.NNobj.LYRSIZE[l] - 1 << ";\n";
					}
				}
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					outFile << "wire [" << sConfig.NNobj.LYRSIZE[l] << "-1:0] " << "SIGN_dbeta_L" << l << "_T" << t << ";\n";
			}
		}
	}
	outFile << "\n";

	NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR-1, "SIGN", wire, TopLevel, singleLayer, sConfig.IsRecurrent());

	gradientCalculationBlocks(outFile, sConfig);

	// Backpropagation
	outFile << "// Backpropagation:\n\n";

	if (!sConfig.IsRecurrent()) {
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++) {
			outFile << "assign delta_L" << sConfig.NNobj.OUTPUTLYR << "[" << j << "] = eps[" << j << "] & (zp_L" << sConfig.NNobj.OUTPUTLYR << "[" << j << "]);\n";
		}
	}
	else {
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++) {
			for (int t=0; t<sConfig.maxTimeSteps; t++)
				outFile << "assign delta_L" << sConfig.NNobj.OUTPUTLYR << "_T" << t << "[" << j << "] = eps_T" << t << "[" << j << "] & (zp_L" << sConfig.NNobj.OUTPUTLYR << "_T" << t << "[" << j << "]);\n";
		}
	}
	outFile << "\n";


	int RNGindex = 0;
	for (int l = sConfig.NNobj.OUTPUTLYR-1; l > 0; l--) {
		if (!sConfig.IsRecurrent()) {
			if (sConfig.NNobj.LYRinfo[l].LT == FC)
				genFULLCONN_BP_block_Inst(outFile, sConfig, l, l, -1);
			else if (sConfig.NNobj.LYRinfo[l].LT == CV)
				genCONV_BP_block_Inst(outFile, sConfig, l, -1);
		}
		else
			for (int t = sConfig.maxTimeSteps-1; t >=0 ; t--)
				genFULLCONN_BP_block_Inst(outFile, sConfig, l, RNGindex++, t);
	}


	outFile << "\n";
	outFile << "endmodule\n";
	

}

void gradientCalculationBlocks(std::ostream &outFile, sysConfig sConfig) {

	// Gradient Evaluation:
	int Rcounter = 0;
	int Rindex = 0;
	if (!sConfig.IsRecurrent()) {
		for (int l = 1; l < sConfig.NNobj.OUTPUTLYR + 1; l++) {
			if (sConfig.NNobj.LYRinfo[l].LT == CV)
				outFile << "wire [" << (sConfig.NNobj.LYRinfo[l].conv.Nconnections + 1)*sConfig.NNobj.LYRinfo[l].conv.filters << "-1:0] R_BP_CV_ADDSUB_L" << l << ";\n";
		}
		outFile << "\n";
		for (int l = 1; l < sConfig.NNobj.OUTPUTLYR + 1; l++) {
			if (sConfig.NNobj.LYRinfo[l].LT == CV) {
				outFile << "SS_RERANDOMIZER RERANDOMIZER_CV_ADDSUB" << l - 1 << "(.CLK(CLK), .INIT(INIT), .IN(R_BP" << Rindex;
				outFile << "[" << Rcounter++ << "]), .OUT(R_BP_CV_ADDSUB_L" << l << "));\n";
				outFile << "defparam RERANDOMIZER_CV_ADDSUB" << l - 1 << ".N = " << (sConfig.NNobj.LYRinfo[l].conv.Nconnections + 1)*sConfig.NNobj.LYRinfo[l].conv.filters << ";\n";
			}
			if (Rcounter == sConfig.LFSR_S / 2)
			{
				Rcounter = 0;
				Rindex++;
			}
		}
	}
	else {
		for (int l = 1; l < sConfig.NNobj.OUTPUTLYR + 1; l++) {
			if (sConfig.NNobj.LYRinfo[l].LT == CV) {

			}
			else if (sConfig.NNobj.LYRinfo[l].LT == FC)
				outFile << "wire [" << sConfig.NNobj.LYRSIZE[l] * (sConfig.NNobj.LYRSIZE[l - 1] + 1) << "-1:0] R_BP_T_ADDSUB_L" << l << ";\n";
			else if (sConfig.NNobj.LYRinfo[l].LT == RCC)
				outFile << "wire [" << sConfig.NNobj.LYRSIZE[l] * (sConfig.NNobj.LYRSIZE[l - 1] + sConfig.NNobj.LYRSIZE[l] + 1) << "-1:0] R_BP_T_ADDSUB_L" << l << ";\n";
		}
		outFile << "\n";
		for (int l = 1; l < sConfig.NNobj.OUTPUTLYR + 1; l++) {
			if (sConfig.NNobj.LYRinfo[l].LT == CV) {

			}
			else if (sConfig.NNobj.LYRinfo[l].LT == FC) {
				outFile << "SS_RERANDOMIZER RERANDOMIZER_T_ADDSUB" << l - 1<< "(.CLK(CLK), .INIT(INIT), .IN(R_BPTT" << Rindex;
				outFile << "[" << Rcounter++ << "]), .OUT(R_BP_T_ADDSUB_L" << l << "));\n";
				outFile << "defparam RERANDOMIZER_T_ADDSUB" << l - 1 << ".N = " << sConfig.NNobj.LYRSIZE[l] * ( sConfig.NNobj.LYRSIZE[l - 1] + 1) << ";\n";
			}
			else if (sConfig.NNobj.LYRinfo[l].LT == RCC) {
				outFile << "SS_RERANDOMIZER RERANDOMIZER_T_ADDSUB" << l - 1 << "(.CLK(CLK), .INIT(INIT), .IN(R_BPTT" << Rindex;
				outFile << "[" << Rcounter++ << "]), .OUT(R_BP_T_ADDSUB_L" << l << "));\n";
				outFile << "defparam RERANDOMIZER_T_ADDSUB" << l - 1 << ".N = " << sConfig.NNobj.LYRSIZE[l] * ( sConfig.NNobj.LYRSIZE[l - 1] + sConfig.NNobj.LYRSIZE[l] + 1) << ";\n";
			}
			if (Rcounter == sConfig.LFSR_S / 2)
			{
				Rcounter = 0;
				Rindex++;
			}
		}
	}
	outFile << "\n";


	outFile << "// Backwards Evaluation:\n\n";
	if (!sConfig.IsRecurrent()) {
		for (int l = (sConfig.NNobj.OUTPUTLYR); l > 0; l--) {
			int R_CV_ADDSUB_Counter = 0;
			if (sConfig.NNobj.LYRinfo[l].LT == FC) {
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					outFile << "NN_DNODE DNODE_L" << l << "_" << j << "(.delta(delta_L" << l << "[" << j << "]), .atj(";
						outFile << "a_L" << l - 1 << "), ";
					outFile << ".zk(z_L" << l << "[" << j << "]), .dalpha(";
					outFile << "dalpha_L" << l << "_j_" << j << "), ";
					outFile << ".dbeta(dbeta_L" << l << "[" << j << "]), .CLK(CLK), .INIT(BP_FRAME)); " << "// layer " << l << " node " << j << "\n";
				}
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					outFile << "defparam DNODE_L" << l << "_" << j << ".NN = " << sConfig.NNobj.LYRSIZE[l - 1] << ";\n";
					outFile << "defparam DNODE_L" << l << "_" << j << ".MEMSIZE = MEMSIZE;\n";
				}
				outFile << "\n";
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					outFile << "assign SIGN_dalpha_L" << l << "_j_" << j << " = {";
					for (int k = 0; k < sConfig.NNobj.LYRSIZE[l - 1] - 1; k++)
						outFile << "SIGN_L" << l << "[" << j << "], ";
					outFile << "SIGN_L" << l << "[" << j << "]};\n";
				}
				outFile << "assign SIGN_dbeta_L" << l << " = SIGN_L" << l << ";\n";
				outFile << "\n";
			}
			else if (sConfig.ConnectionType(l) == FC_CV) {
				int CVnodeNumber = 0;
				for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
					for (int j = 0; j < sConfig.NNobj.LYRinfo[l].conv.filterSize; j++) {
						outFile << "NN_DNODE DNODE_L" << l << "_" << CVnodeNumber << "(.delta(delta_L" << l << "[" << CVnodeNumber << "]), .atj({";
						for (int k = (sConfig.NNobj.LYRinfo[l].conv.Nconnections - 1); k > 0; k--)
							outFile << "a_L" << l - 1 << "[" << sConfig.NNobj.LYRinfo[l].connections[j][k] << "],";
						outFile << "a_L" << l - 1 << "[" << sConfig.NNobj.LYRinfo[l].connections[j][0] << "]}), ";
						outFile << ".zk(z_L" << l << "[" << CVnodeNumber << "]), .dalpha(dalpha_L" << l << "_n" << CVnodeNumber << "), .dbeta(dbeta_L" << l << "_n[" << CVnodeNumber << "]), .CLK(CLK), .INIT(BP_FRAME)); \n";
						CVnodeNumber++;
					}
				}
				CVnodeNumber = 0;
				for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
					for (int j = 0; j < sConfig.NNobj.LYRinfo[l].conv.filterSize; j++) {
						outFile << "defparam DNODE_L" << l << "_" << CVnodeNumber << ".NN = " << sConfig.NNobj.LYRinfo[l].conv.Nconnections << ";\n";
						outFile << "defparam DNODE_L" << l << "_" << CVnodeNumber++ << ".MEMSIZE = MEMSIZE;\n";
					}
				}
				outFile << "\n"; CVnodeNumber = 0;
				for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
					for (int j = 0; j < sConfig.NNobj.LYRinfo[l].conv.Nconnections; j++) { 
						outFile << "SS_ADDSUB ADDSUB_L" << l << "_alpha_" << j << "_" << sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f) << "(.CLK(CLK),.INIT(INIT),.IN({";
						stringstream stringLine;
						for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
							if (NodeBelongsToFilter(sConfig.NNobj, l, k, f))
								stringLine << "dalpha_L" << l << "_n" << k << "[" << j << "], ";
						stringLine.seekp(-2, stringLine.cur);
						stringLine << "}), ";
						outFile << stringLine.str();
						outFile << ".R_condition(R_BP_CV_ADDSUB_L" << l << "[" << R_CV_ADDSUB_Counter++ << "]), ";
						outFile << ".SIGN(SIGN_L" << l << "[" << sConfig.NNobj.LYRinfo[l].conv.FilterEndNode(f) << ":" << sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f) << "]),";
						outFile << ".OUT(dalpha_L" << l << "_j_" << sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f) << "[" << j << "]), .SIGN_out(SIGN_dalpha_L" << l << "_j_" << sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f) << "[" << j << "])); \n";
						CVnodeNumber++;
					}
				}
				CVnodeNumber = 0;
				for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
					for (int j = 0; j < sConfig.NNobj.LYRinfo[l].conv.Nconnections; j++)
						outFile << "defparam ADDSUB_L" << l << "_alpha_" << j << "_" << sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f) << ".N = " << sConfig.NNobj.LYRinfo[l].conv.filterSize << ";\n";
				}
				outFile << "\n"; CVnodeNumber = 0;
				for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) { ///////////////// filterwise
					outFile << "SS_ADDSUB ADDSUB_L" << l << "_beta" << f << "(.CLK(CLK),.INIT(INIT),.IN({";
					stringstream stringLine;
					for (int j=0; j < sConfig.NNobj.LYRSIZE[l]; j++)
						if (NodeBelongsToFilter(sConfig.NNobj,l,j,f))
							stringLine << "dbeta_L" << l << "_n[" << j << "], ";
					stringLine.seekp(-2, stringLine.cur);
					stringLine << "}), ";
					outFile << stringLine.str(); stringLine.str("hi");
					outFile << ".R_condition(R_BP_CV_ADDSUB_L" << l << "[" << R_CV_ADDSUB_Counter++ << "]), ";
					outFile << ".SIGN({";
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
						if (NodeBelongsToFilter(sConfig.NNobj, l, j, f))
							stringLine << "SIGN_L" << l << "[" << j << "], ";
					stringLine.seekp(-2, stringLine.cur);
					stringLine << "}), ";
					outFile << stringLine.str();
					outFile << ".OUT(dbeta_L" << l << "[" << f << "]), .SIGN_out(SIGN_dbeta_L" << l << "[" << f << "])); \n";
				}
				CVnodeNumber = 0;
				for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) ///////////////// filterwise
					outFile << "defparam ADDSUB_L" << l << "_beta" << f << ".N = " << sConfig.NNobj.LYRinfo[l].conv.filterSize << ";\n";
				outFile << "\n";

			}
			else if (sConfig.ConnectionType(l) == CV_CV) {
				// ********************************** NEED TO ADD
				cout << "ERROR: INVALID LAYER CONFIGURATION (genBCKPROP\n)";
			}
		}
	}
	else {
		for (int l = (sConfig.NNobj.OUTPUTLYR); l > 0; l--) {
			outFile << "// -------------------------- Layer " << l << " ------------------------- //\n\n";
			// Fully Connected Layer (Recurrent network)
			if (sConfig.NNobj.LYRinfo[l].LT == FC) {
				Rindex = 0;
				for (int t = 0; t < sConfig.maxTimeSteps; t++) {

					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
						outFile << "NN_DNODE DNODE_L" << l << "_T" << t << "_" << j << "(.delta(delta_L" << l << "_T" << t << "[" << j << "]), .atj(";
						if (sConfig.NNobj.LYRinfo[l - 1].LT == CV) {
							outFile << "{";
							for (int f = 0; f < sConfig.NNobj.LYRinfo[l - 1].conv.filters - 1; f++)
								outFile << "a_L" << l - 1 << "_T" << t << "_F" << f << ", ";
							outFile << "a_L" << l - 1 << "_T" << t << "_F" << sConfig.NNobj.LYRinfo[l - 1].conv.filters - 1 << "}), ";
						}
						else 
							outFile << "a_L" << l - 1 << "_T" << t << "), ";
						outFile << ".zk(z_L" << l << "_T" << t << "[" << j << "]), .dalpha(";
						if (sConfig.NNobj.LYRinfo[l - 1].LT == CV) {
							outFile << "{";
							for (int f = 0; f < sConfig.NNobj.LYRinfo[l - 1].conv.filters - 1; f++)
								outFile << "dalpha_L" << l << "_T" << t << "_F" << f << "_j_" << j << ", ";
							outFile << "dalpha_L" << l << "_T" << t << "_F" << sConfig.NNobj.LYRinfo[l - 1].conv.filters - 1 << "_j_" << j << "}), ";
						}
						else
							outFile << "dalpha_L" << l << "_T" << t << "_j_" << j << "), ";
						outFile << ".dbeta(dbeta_L" << l << "_T" << t << "[" << j << "]), .CLK(CLK), .INIT(BP_FRAME)); " << "// layer " << l << " node " << j << "\n";
					}
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
						int size = 0;
						if ((sConfig.NNobj.LYRinfo[l - 1].LT == FC)|(sConfig.NNobj.LYRinfo[l - 1].LT == RCC))
							size = sConfig.NNobj.LYRSIZE[l - 1];
						else if (sConfig.NNobj.LYRinfo[l - 1].LT == CV)
							size = sConfig.NNobj.LYRinfo[l - 1].conv.filterSize*sConfig.NNobj.LYRinfo[l - 1].size;

						outFile << "defparam DNODE_L" << l << "_T"  << t << "_" << j << ".NN = " << size << ";\n";
						outFile << "defparam DNODE_L" << l << "_T"  << t << "_" << j << ".MEMSIZE = MEMSIZE;\n";
					}
					outFile << "\n";

					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
						if (sConfig.NNobj.LYRinfo[l - 1].LT == CV) {
							for (int f = 0; f < sConfig.NNobj.LYRinfo[l - 1].conv.filters; f++) {
								outFile << "assign SIGN_dalpha_L" << l << "_T" << t << "_F" << f << "_j_" << j << " = {";
								for (int k = 0; k < sConfig.NNobj.LYRinfo[l - 1].conv.filters - 1; k++)
									outFile << "SIGN_L" << l << "_T" << t << "[" << j << "], ";
								outFile << "SIGN_L" << l << "_T" << t << "[" << j << "]};\n";
							}
						}
						else {
							outFile << "assign SIGN_dalpha_L" << l << "_T" << t << "_j_" << j << " = {";
							for (int k = 0; k < sConfig.NNobj.LYRSIZE[l - 1] - 1; k++)
								outFile << "SIGN_L" << l << "_T" << t << "[" << j << "], ";
							outFile << "SIGN_L" << l << "_T" << t << "[" << j << "]};\n";
						}
					}
					outFile << "assign SIGN_dbeta_L" << l << "_T" << t << " = SIGN_L" << l << "_T" << t << ";\n";
					outFile << "\n";
				}
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[l - 1]; i++) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
						outFile << "SS_ADDSUB ADDSUB_dalpha_L" << l << "_" << i << "_" << j << "(.CLK(CLK), .INIT(INIT), .IN({";
						for (int t = 0; t < sConfig.maxTimeSteps-1; t++)
							outFile << "dalpha_L" << l << "_T" << t << "_j_" << j << "[" << i << "], ";
						outFile << "dalpha_L" << l << "_T" << sConfig.maxTimeSteps-1 << "_j_" << j << "[" << i << "]}), .SIGN({ ";
						for (int t = 0; t < sConfig.maxTimeSteps - 1; t++)
							outFile << "SIGN_dalpha_L" << l << "_T" << t << "_j_" << j << "[" << i << "], ";
						outFile << "SIGN_dalpha_L" << l << "_T" << sConfig.maxTimeSteps - 1 << "_j_" << j << "[" << i << "]}), ";
						outFile << ".R_condition(R_BP_T_ADDSUB_L" << l << "[" << Rindex++ << "]), .OUT(dalpha_L" << l << "_j_" << j << "[" << i << "]), .SIGN_out(SIGN_dalpha_L" << l << "_j_" << j << "[" << i<< "])); \n";
					}
				}
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[l - 1]; i++) 
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) 
						outFile << "defparam ADDSUB_dalpha_L" << l << "_" << i << "_" << j <<".N = " << sConfig.maxTimeSteps << ";\n";
				outFile << "\n";
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					outFile << "SS_ADDSUB ADDSUB_dbeta_L" << l << "_" << j << "(.CLK(CLK),.INIT(INIT),.IN({";
					for (int t = 0; t < sConfig.maxTimeSteps - 1; t++)
						outFile << "dbeta_L" << l << "_T" << t << "[" << j << "],";
					outFile << "dbeta_L" << l << "_T" << sConfig.maxTimeSteps - 1 << "[" << j << "]}), .SIGN({";
					for (int t = 0; t < sConfig.maxTimeSteps - 1; t++)
						outFile << "SIGN_dbeta_L" << l << "_T" << t << "[" << j << "], ";
					outFile << "SIGN_dbeta_L" << l << "_T" << sConfig.maxTimeSteps - 1 << "[" << j << "]}), ";
					outFile << ".R_condition(R_BP_T_ADDSUB_L" << l << "[" << Rindex++ << "]), .OUT(dbeta_L" << l << "[" << j <<"]), .SIGN_out(SIGN_dbeta_L" << l << "[" << j << "])); \n";
				}
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) 
					outFile << "defparam ADDSUB_dbeta_L" << l << "_" << j << ".N = " << sConfig.maxTimeSteps << ";\n";
				outFile << "\n";
			}
			// Recurrent Layer
			else if (sConfig.NNobj.LYRinfo[l].LT == RCC) {
				Rindex = 0;
				for (int t = 0; t < sConfig.maxTimeSteps; t++) {

					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
						outFile << "NN_RNN_DNODE DNODE_L" << l << "_T" << t << "_" << j << "(.delta(delta_L" << l << "_T" << t << "[" << j << "]), .atj(";
						if (sConfig.NNobj.LYRinfo[l - 1].LT == CV) {
							outFile << "{";
							for (int f = 0; f < sConfig.NNobj.LYRinfo[l - 1].conv.filters - 1; f++)
								outFile << "a_L" << l - 1 << "_T" << t << "_F" << f << ", ";
							outFile << "a_L" << l - 1 << "_T" << t << "_F" << sConfig.NNobj.LYRinfo[l - 1].conv.filters - 1 << "}), ";
						}
						else
							outFile << "a_L" << l - 1 << "_T" << t << "), ";
						outFile << ".atj_last(";
						if (sConfig.NNobj.LYRinfo[l - 1].LT == CV) {}
						else
							if (t>0) outFile << "a_L" << l << "_T" << t - 1 << "), "; else outFile << sConfig.NNobj.LYRSIZE[l] << "'d0" << "), ";
						outFile << ".zk(z_L" << l << "_T" << t << "[" << j << "]), .dalpha(";
						if (sConfig.NNobj.LYRinfo[l - 1].LT == CV) {
							outFile << "{";
							for (int f = 0; f < sConfig.NNobj.LYRinfo[l - 1].conv.filters - 1; f++)
								outFile << "dalpha_L" << l << "_T" << t << "_F" << f << "_j_" << j << ", ";
							outFile << "dalpha_L" << l << "_T" << t << "_F" << sConfig.NNobj.LYRinfo[l - 1].conv.filters - 1 << "_j_" << j << "}), ";
						}
						else
							outFile << "dalpha_L" << l << "_T" << t << "_j_" << j << "), .dgamma(";
						if (sConfig.NNobj.LYRinfo[l - 1].LT == CV) {
							outFile << "{";
							for (int f = 0; f < sConfig.NNobj.LYRinfo[l - 1].conv.filters - 1; f++)
								outFile << "dgamma_L" << l << "_T" << t << "_F" << f << "_j_" << j << ", ";
							outFile << "dgamma_L" << l << "_T" << t << "_F" << sConfig.NNobj.LYRinfo[l - 1].conv.filters - 1 << "_j_" << j << "}), ";
						}
						else
							outFile << "dgamma_L" << l << "_T" << t << "_j_" << j << "), ";
						outFile << ".dbeta(dbeta_L" << l << "_T" << t << "[" << j << "]), .CLK(CLK), .INIT(BP_FRAME)); " << "// layer " << l << " node " << j << "\n";
					}
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
						int size = 0;
						if (sConfig.NNobj.LYRinfo[l - 1].LT == FC)
							size = sConfig.NNobj.LYRSIZE[l - 1];
						else if (sConfig.NNobj.LYRinfo[l - 1].LT == CV)
							size = sConfig.NNobj.LYRinfo[l - 1].conv.filterSize*sConfig.NNobj.LYRinfo[l - 1].size;

						outFile << "defparam DNODE_L" << l << "_T" << t << "_" << j << ".NN = " << size << ";\n";
						outFile << "defparam DNODE_L" << l << "_T" << t << "_" << j << ".NR = " << sConfig.NNobj.LYRSIZE[l] << ";\n";
						outFile << "defparam DNODE_L" << l << "_T" << t << "_" << j << ".MEMSIZE = MEMSIZE;\n";
					}
					outFile << "\n";

					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
						if (sConfig.NNobj.LYRinfo[l - 1].LT == CV) {
							for (int f = 0; f < sConfig.NNobj.LYRinfo[l - 1].conv.filters; f++) {
								outFile << "assign SIGN_dalpha_L" << l << "_T" << t << "_F" << f << "_j_" << j << " = {";
								for (int k = 0; k < sConfig.NNobj.LYRinfo[l - 1].conv.filterSize - 1; k++)
									outFile << "SIGN_L" << l << "_T" << t << "[" << j << "], ";
								outFile << "SIGN_L" << l << "_T" << t << "[" << j << "]};\n";
							}
						}
						else {
							outFile << "assign SIGN_dalpha_L" << l << "_T" << t << "_j_" << j << " = {";
							for (int k = 0; k < sConfig.NNobj.LYRSIZE[l - 1] - 1; k++)
								outFile << "SIGN_L" << l << "_T" << t << "[" << j << "], ";
							outFile << "SIGN_L" << l << "_T" << t << "[" << j << "]};\n";
						}
					}
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
						outFile << "assign SIGN_dgamma_L" << l << "_T" << t << "_j_" << j << " = {";
						for (int k = 0; k < sConfig.NNobj.LYRSIZE[l] - 1; k++)
							outFile << "SIGN_L" << l << "_T" << t << "[" << j << "], ";
						outFile << "SIGN_L" << l << "_T" << t << "[" << j << "]};\n";
					}
					outFile << "assign SIGN_dbeta_L" << l << "_T" << t << " = SIGN_L" << l << "_T" << t << ";\n";
					outFile << "\n";
				}
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[l - 1]; i++) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
						outFile << "SS_ADDSUB ADDSUB_dalpha_L" << l << "_" << i << "_" << j << "(.CLK(CLK), .INIT(INIT), .IN({";
						for (int t = 0; t < sConfig.maxTimeSteps - 1; t++)
							outFile << "dalpha_L" << l << "_T" << t << "_j_" << j << "[" << i << "], ";
						outFile << "dalpha_L" << l << "_T" << sConfig.maxTimeSteps - 1 << "_j_" << j << "[" << i << "]}), .SIGN({ ";
						for (int t = 0; t < sConfig.maxTimeSteps - 1; t++)
							outFile << "SIGN_dalpha_L" << l << "_T" << t << "_j_" << j << "[" << i << "], ";
						outFile << "SIGN_dalpha_L" << l << "_T" << sConfig.maxTimeSteps - 1 << "_j_" << j << "[" << i << "]}), ";
						outFile << ".R_condition(R_BP_T_ADDSUB_L" << l << "[" << Rindex++ << "]), .OUT(dalpha_L" << l << "_j_" << j << "[" << i << "]), .SIGN_out(SIGN_dalpha_L" << l << "_j_" << j << "[" << i << "])); \n";
					}
				}
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[l - 1]; i++)
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
						outFile << "defparam ADDSUB_dalpha_L" << l << "_" << i << "_" << j << ".N = " << sConfig.maxTimeSteps << ";\n";
				outFile << "\n";
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[l]; i++) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
						outFile << "SS_ADDSUB ADDSUB_dgamma_L" << l << "_" << i << "_" << j << "(.CLK(CLK), .INIT(INIT), .IN({";
						for (int t = 0; t < sConfig.maxTimeSteps - 1; t++)
							outFile << "dgamma_L" << l << "_T" << t << "_j_" << j << "[" << i << "], ";
						outFile << "dgamma_L" << l << "_T" << sConfig.maxTimeSteps - 1 << "_j_" << j << "[" << i << "]}), .SIGN({ ";
						for (int t = 0; t < sConfig.maxTimeSteps - 1; t++)
							outFile << "SIGN_dgamma_L" << l << "_T" << t << "_j_" << j << "[" << i << "], ";
						outFile << "SIGN_dgamma_L" << l << "_T" << sConfig.maxTimeSteps - 1 << "_j_" << j << "[" << i << "]}), ";
						outFile << ".R_condition(R_BP_T_ADDSUB_L" << l << "[" << Rindex++ << "]), .OUT(dgamma_L" << l << "_j_" << j << "[" << i << "]), .SIGN_out(SIGN_dgamma_L" << l << "_j_" << j << "[" << i << "])); \n";
					}
				}
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[l]; i++)
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
						outFile << "defparam ADDSUB_dgamma_L" << l << "_" << i << "_" << j << ".N = " << sConfig.maxTimeSteps << ";\n";
				outFile << "\n";
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					outFile << "SS_ADDSUB ADDSUB_dbeta_L" << l << "_" << j << "(.CLK(CLK),.INIT(INIT),.IN({";
					for (int t = 0; t < sConfig.maxTimeSteps - 1; t++)
						outFile << "dbeta_L" << l << "_T" << t << "[" << j << "],";
					outFile << "dbeta_L" << l << "_T" << sConfig.maxTimeSteps - 1 << "[" << j << "]}), .SIGN({";
					for (int t = 0; t < sConfig.maxTimeSteps - 1; t++)
						outFile << "SIGN_dbeta_L" << l << "_T" << t << "[" << j << "], ";
					outFile << "SIGN_dbeta_L" << l << "_T" << sConfig.maxTimeSteps - 1 << "[" << j << "]}), ";
					outFile << ".R_condition(R_BP_T_ADDSUB_L" << l << "[" << Rindex++ << "]), .OUT(dbeta_L" << l << "[" << j << "]), .SIGN_out(SIGN_dbeta_L" << l << "[" << j << "])); \n";
				}
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
					outFile << "defparam ADDSUB_dbeta_L" << l << "_" << j << ".N = " << sConfig.maxTimeSteps << ";\n";
			}
		}
	}
	outFile << "\n";
}


void genBCKPROP_Inst(std::ostream &outFile, sysConfig sConfig) {
	int negone = -1;
	if (!sConfig.usesTSParallelization()) {
		int Lindex = 0;
		outFile << "// Back Propagation Module:\n";
		outFile << "BCKDPROP BCKPROP0(\n\t";
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts);
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, false, true);
		DeclareForwardPropagationNets(outFile, sConfig, false, ModuleInstantiationPorts);
		DeclareSingleLayerNet(outFile, sConfig, "SIGN_L" + Int2Str(sConfig.NNobj.OUTPUTLYR), ModuleInstantiationPorts);
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, false);
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, true);
		DeclareSingleLayerNet(outFile, sConfig, "eps", ModuleInstantiationPorts);
		outFile << StringSeriesPort(sConfig.RandNums.short_ADDSUB_BP, "R_BP", "", "R", "_ADDSUB_BP", negone) << "\n\t";
		if (sConfig.IsRecurrent()) outFile << StringSeriesPort(sConfig.RandNums.short_ADDSUB_BPTT, "R_BPTT", "", "R", "_ADDSUB_BPTT", negone) << "\n\t";
		outFile << ".CLK(CLK), .BP_FRAME(INIT), .FP_FRAME(INIT), .INIT(INIT)\n);\n\n";
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			int Lindex = 0;
			outFile << "// Back Propagation Module " << ts << " :\n";
			outFile << "BCKDPROP BCKPROP" << ts << "(\n\t";
			NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts);
			NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, false, true);
			DeclareForwardPropagationNets(outFile, sConfig, false, ModuleInstantiationPorts, ts);
			DeclareSingleLayerNet(outFile, sConfig, "SIGN_L" + Int2Str(sConfig.NNobj.OUTPUTLYR), ModuleInstantiationPorts, ts);
			NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, false, "_netPar" + Int2Str(ts), "");
			NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, true, "_netPar" + Int2Str(ts), "");
			DeclareSingleLayerNet(outFile, sConfig, "eps", ModuleInstantiationPorts, ts);
			outFile << StringSeriesPort(sConfig.RandNums.short_ADDSUB_BP, "R_BP", "", "R", "_ADDSUB_BP_netPar" + Int2Str(ts), negone) << "\n\t";
			if (sConfig.IsRecurrent()) outFile << StringSeriesPort(sConfig.RandNums.short_ADDSUB_BPTT, "R_BPTT", "", "R", "_ADDSUB_BPTT", negone) << "\n\t";
			outFile << ".CLK(CLK), .BP_FRAME(INIT), .FP_FRAME(INIT), .INIT(INIT)\n);\n\n";
		}
	}
	
}