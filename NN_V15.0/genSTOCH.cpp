/* Chris Ceroici
Generates a Verilog File containing declarations for stochastic bit generators
*/

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <io.h>
#include "genSTOCH.h"
#include "BitOps.h"
#include "Verilog.h"

using namespace std;

void genSTOCH(sysConfig sConfig){


	ofstream outFile;
	outFile.open(sConfig.Dir + "STOCH.v");
	outFile<<"// Chris Ceroici\n"<<endl;
	outFile<<"// Stochastic bit generators \n\n";
	outFile << "module STOCH(\n\t";
	outFile << StringSeries(sConfig.RandNums.short_d2s, "PD", "") << ",\n\t";
	outFile << StringSeries(sConfig.RandNums.long_d2s, "PD_long", "") << ",\n\t";
	outFile << StringSeries(sConfig.RandNums.short_d2s, "R", "") << ",\n\t";
	outFile << StringSeries(sConfig.RandNums.long_d2s, "R_long", "") << ",\n\t";
	outFile << StringSeries(sConfig.RandNums.short_d2s, "P", "") << ",\n\t";
	outFile << StringSeries(sConfig.RandNums.long_d2s, "P_long", "") << ",\n\t";

	outFile << "CLK\n";
	outFile << ");\n\n";
	outFile<<"parameter DP_in = " << sConfig.DP_in << "; // Short Decimal precision \n\n";
	outFile << "parameter DP_out = " << sConfig.DP_out << "; // Long Decimal precision \n\n";

	outFile<<"input CLK; // Trigger\n";
	DeclareNet(outFile, "input wire", "DP_in", sConfig.RandNums.short_d2s, "PD", "");
	DeclareNet(outFile, "input wire", "DP_out", sConfig.RandNums.long_d2s, "PD_long", "");
	DeclareNet(outFile, "input wire", "DP_in", sConfig.RandNums.short_d2s, "R", "");
	DeclareNet(outFile, "input wire", "DP_out", sConfig.RandNums.long_d2s, "R_long", "");
	outFile << "\n";
	DeclareNet(outFile, "output wire", "1", sConfig.RandNums.short_d2s, "P", "");
	DeclareNet(outFile, "output wire", "1", sConfig.RandNums.long_d2s, "P_long", "");

	outFile << "\n\n";

	// Short D2S conversion:
	outFile << "// Short D2S Conversion:\n\n";

	for (int i = 0; i < sConfig.RandNums.short_d2s; i++)
		outFile << "DEC2STCH D2S" << i << "(.CLK(CLK), .D(PD" << i << "), .LFSR(R" << i << "), .S(P" << i << "),.INIT(1'b0));\n";

	outFile << "\n// Long D2S Conversion:\n\n";

	for (int i = 0; i < sConfig.RandNums.long_d2s; i++)
		outFile << "DEC2STCH D2S_long" << i << "(.CLK(CLK), .D(PD_long" << i << "), .LFSR(R_long" << i << "), .S(P_long" << i << "),.INIT(1'b0));\n";
	for (int i = 0; i < sConfig.RandNums.long_d2s; i++)
		outFile << "defparam D2S_long" << i << ".ND = DP_out;\n";

	
	outFile<<"\nendmodule";
	outFile.close();
}


void genSTOCH_Inst(std::ostream &outFile, sysConfig sConfig) {
	int negone = -1;
	int Lindex = 0;

	outFile << "// Stochastic bit generator\n";
	outFile << "STOCH STOCH0(\n";
	ofstream outFile_ext;
	outFile_ext.open(sConfig.Dir + "STOCH_portDefinitions.v");
	outFile << "`include \"STOCH_portDefinitions.v\"\n\t";
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) { // alpha decimal
		if (sConfig.ConnectionType(l) == FC_FC) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l - 1]; j++) {
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
					outFile_ext << ".PD" << Lindex++ << "(" << "alphaD_L" << l << "_" << j << "_" << k << "), ";
			}
			if (sConfig.NNobj.LYRinfo[l].LT == RCC) { // Recurrent parameters
				outFile_ext << "\n\t";
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
						outFile_ext << ".PD" << Lindex++ << "(" << "gammaD_L" << l << "_" << j << "_" << k << "), ";
				}
			}
		}
		else if (sConfig.ConnectionType(l) == CV_FC){
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l - 1]; j++) {
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
					outFile_ext << ".PD" << Lindex++ << "(" << "alphaD_L" << l << "_" << j << "_" << k << "), ";
				outFile_ext << "\n\t";
			}
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l - 1]; j++) {
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
					if (ConnectionExists(sConfig.NNobj, l, j, k))
						outFile_ext << ".PD" << Lindex++ << "(" << "alphaD_L" << l << "_" << j << "_" << k << "), ";
			}
			outFile_ext << "\n\t";
		}
		outFile_ext << "\n\t";
	}

	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) { // beta decimal
		if ((sConfig.NNobj.LYRinfo[l].LT == FC)|(sConfig.NNobj.LYRinfo[l].LT == RCC)) {
			for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
				outFile_ext << ".PD" << Lindex++ << "(" << "betaD_L" << l << "_" << k << "), ";
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++)
				outFile_ext << ".PD" << Lindex++ << "(" << "betaD_L" << l << "_" << f << "), ";
		}
		outFile_ext << "\n\t";
	}

	if (!sConfig.usesTSParallelization()) {
		if (!sConfig.IsRecurrent())
			outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "PD", "", "aD_L0_", "", Lindex) << ",\n\t";
		else
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "PD", "", "aD_L0_T" + Int2Str(t) + "_", "", Lindex) << ",\n\t";
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent())
				outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "PD", "", "aD_L0_", "_netPar" + Int2Str(ts), Lindex) << ",\n\t";
			else
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "PD", "", "aD_L0_T" + Int2Str(t) + "_", "_netPar" + Int2Str(ts), Lindex) << ",\n\t";
		}
	}

	for (int i = 0; i < sConfig.RandNums.short_zeroCenter-1; i++) // zero center inputs
		outFile_ext << ".PD" << Lindex++ << "(8'd127), ";
	outFile_ext << ".PD" << Lindex++ << "(8'd127),\n\t";
	
	
	Lindex = 0;
	if (!sConfig.usesTSParallelization()) {
		if (!sConfig.IsRecurrent())
			outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "PD_long", "", "YD", "", Lindex) << ",\n\t";
		else
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "PD_long", "", "YD_T" + Int2Str(t) + "_", "", Lindex) << ",\n\t";
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent())
				outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "PD_long", "", "YD", "_netPar" + Int2Str(ts), Lindex) << ",\n\t";
			else
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "PD_long", "", "YD_T" + Int2Str(t) + "_", "_netPar" + Int2Str(ts), Lindex) << ",\n\t";
		}
	}
	Lindex = 0;
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) { // alpha stochastic
		if (sConfig.ConnectionType(l) == FC_FC) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l - 1]; j++) {
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
					outFile_ext << ".P" << Lindex++ << "(" << "alpha_L" << l << "_j_" << k << "[" << j << "]), ";
			}
			if (sConfig.NNobj.LYRinfo[l].LT == RCC) { 	// Recurrent parameters
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
						outFile_ext << ".P" << Lindex++ << "(" << "gamma_L" << l << "_j_" << k << "[" << j << "]), ";
				}
			}
		}
		else if (sConfig.ConnectionType(l) == CV_FC) {

			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l - 1]; j++) {
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
					outFile_ext << ".P" << Lindex++ << "(" << "alpha_L" << l << "_j_" << k << "[" << j << "]), ";
				outFile_ext << "\n\t";
			}
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			int CVinputIndex = 0;
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l - 1]; j++) {
				bool connectionPlaced = false;
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
					if (ConnectionExists(sConfig.NNobj, l, j, k)) {
						outFile_ext << ".P" << Lindex++ << "(" << "alpha_L" << l << "_j_" << k << "[" << CVinputIndex << "]), ";
						connectionPlaced = true;
					}
				if (connectionPlaced) CVinputIndex++;
			}
			outFile_ext << "\n\t";

		}
		outFile_ext << "\n\t";
	}

	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) { // beta stochastic
		if ((sConfig.NNobj.LYRinfo[l].LT == FC)| (sConfig.NNobj.LYRinfo[l].LT == RCC)) {
			for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++)
				outFile_ext << ".P" << Lindex++ << "(" << "beta_L" << l << "[" << k << "]), ";
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
				outFile_ext << ".P" << Lindex++ << "(" << "beta_L" << l << "[" << f << "]), ";
			}
		}
		outFile_ext << "\n\t";
	}
	if (!sConfig.usesTSParallelization()) {
		if (!sConfig.IsRecurrent()) outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "P", "", "a_L0[", "]", Lindex) << ",\n\t";
		else
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "P", "", "a_L0_T" + Int2Str(t) + "[", "]", Lindex) << ",\n\t";
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent()) outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "P", "", "a_L0_netPar" + Int2Str(ts) + "[", "]", Lindex) << ",\n\t";
			else
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[0], "P", "", "a_L0_T" + Int2Str(t) + "_netPar" + Int2Str(ts) + "[", "]", Lindex) << ",\n\t";
		}
	}
	if (!sConfig.usesTSParallelization()) outFile_ext << StringSeriesPort(sConfig.RandNums.short_zeroCenter, "P", "", "zeroCenterSource[", "]", Lindex) << ",\n\t";
	else 
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			outFile_ext << StringSeriesPort(sConfig.RandNums.short_zeroCenter/sConfig.TrainingSetParallelization, "P", "", "zeroCenterSource_netPar" + Int2Str(ts) + "[", "]", Lindex) << ",\n\t";

	Lindex = 0;
	if (!sConfig.usesTSParallelization()) {

		if (!sConfig.IsRecurrent())
			outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "P_long", "", "Y[", "]", Lindex) << ",\n\t";
		else
			for (int t = 0; t < sConfig.maxTimeSteps; t++)
				outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "P_long", "", "Y_T" + Int2Str(t) + "[", "]", Lindex) << ",\n\t";
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {

			if (!sConfig.IsRecurrent())
				outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "P_long", "", "Y_netPar" + Int2Str(ts) + "[", "]", Lindex) << ",\n\t";
			else
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					outFile_ext << StringSeriesPort(sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR], "P_long", "", "Y_T" + Int2Str(t) + "_netPar" + Int2Str(ts) + "[", "]", Lindex) << ",\n\t";
		}
	}
	
	Lindex = 0;
	outFile_ext << "\n\t" << StringSeriesPort(sConfig.RandNums.short_d2s, "R", "", "R", "", Lindex) << ",\n\t";
	Lindex = 0;
	outFile_ext << "\n\t" << StringSeriesPort(sConfig.RandNums.long_d2s, "R_long", "", "R_long", "", Lindex) << ",\n\t";
	outFile << ".CLK(CLK)\n";
	outFile << ");\n\n";
	outFile_ext.close();

}