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
#include "genNETPAR_GRADIENT.h"

using namespace std;

void genNETPAR_GRADIENT(sysConfig sConfig) {

	int negone = -1;

	ofstream outFile;
	outFile.open(sConfig.Dir + "NETPAR_GRADIENT.v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Parallel network training evaluator\n";
	outFile << "// Version 9.0\n\n";

	outFile << "module NETPAR_GRADIENT(\n\t";

	for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, true, false, "_netPar" + Int2Str(ts));
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, true, true, "_netPar" + Int2Str(ts));
	}
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, true, false);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, true, true);
	outFile << "rc,\n\t";
	outFile << "CLK, INIT\n);\n\n";

	for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsInput, true, false, "_netPar" + Int2Str(ts));
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsInput, true, true, "_netPar" + Int2Str(ts));
	}
	outFile << "\n";
	outFile << "input wire [" << sConfig.RandNums.short_ADDSUB_NETPAR*sConfig.DP_in << "-1:0] rc;\n\n";
	outFile << "input wire CLK, INIT;\n\n";
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsOutput, true, false);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsOutput, true, true);
	outFile << "\n";



	int rcIndex = 0;
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {

		if (sConfig.NNobj.LYRinfo[l].LT == FC) { // Fully Connected 
			for (int i = 0; i < sConfig.NNobj.LYRSIZE[l - 1]; i++) { // dalpha
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					outFile << "SS_ADDSUB ADDSUB_dalpha_L" << l << "_" << i << "_" << j << "(.CLK(CLK), .INIT(INIT), .IN({ ";
					for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++)
						outFile << "dalpha_L" << l << "_j_" << j << "_netPar" << ts << "[" << i << "], "; 
					outFile << "dalpha_L" << l << "_j_" << j << "_netPar" << sConfig.TrainingSetParallelization - 1 << "[" << i << "]";
					outFile << " }), .SIGN({ ";
					for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++)
						outFile << "SIGN_dalpha_L" << l << "_j_" << j << "_netPar" << ts << "[" << i << "], ";
					outFile << "SIGN_dalpha_L" << l << "_j_" << j << "_netPar" << sConfig.TrainingSetParallelization - 1 << "[" << i << "]";
					outFile << "}), .R_condition(rc[" << rcIndex++ << "]), .OUT(dalpha_L" << l << "_j_" << j << "[" << i << "]), .SIGN_out(SIGN_dalpha_L" << l << "_j_" << j << "[" << i << "]));\n";
				}
			}
			for (int i = 0; i < sConfig.NNobj.LYRSIZE[l - 1]; i++)
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
					outFile << "defparam ADDSUB_dalpha_L" << l << "_" << i << "_" << j << ".N = " << sConfig.TrainingSetParallelization << ";\n";
			for (int i = 0; i < sConfig.NNobj.LYRSIZE[l - 1]; i++)
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
					outFile << "defparam ADDSUB_dalpha_L" << l << "_" << i << "_" << j << ".DIFFCOUNTER_SIZE = " << 3 << ";\n";
			outFile << "\n";
			for (int i = 0; i < sConfig.NNobj.LYRSIZE[l]; i++) { // dbeta
				outFile << "SS_ADDSUB ADDSUB_dbeta_L" << l << "_" << i << "(.CLK(CLK), .INIT(INIT), .IN({ ";
				for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++)
					outFile << "dbeta_L" << l << "_netPar" << ts << "[" << i << "], ";
				outFile << "dbeta_L" << l << "_netPar" << sConfig.TrainingSetParallelization - 1 << "[" << i << "]";
				outFile << " }), .SIGN({ ";
				for (int ts = 0; ts < sConfig.TrainingSetParallelization - 1; ts++)
					outFile << "SIGN_dbeta_L" << l << "_netPar" << ts << "[" << i << "], ";
				outFile << "SIGN_dbeta_L" << l << "_netPar" << sConfig.TrainingSetParallelization - 1 << "[" << i << "]";
				outFile << "}), .R_condition(rc[" << rcIndex++ << "]), .OUT(dbeta_L" << l << "[" << i << "]), .SIGN_out(SIGN_dbeta_L" << l << "[" << i << "]));\n";
			}
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
					outFile << "defparam ADDSUB_dbeta_L" << l << "_" << j << ".N = " << sConfig.TrainingSetParallelization << ";\n";
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
				outFile << "defparam ADDSUB_dbeta_L" << l << "_" << j << ".DIFFCOUNTER_SIZE = " << 3 << ";\n";
			outFile << "\n";
		}


		outFile << "\n";
	}

	outFile << "\nendmodule\n";
}

void genNETPAR_GRADIENT_Inst(std::ostream &outFile, sysConfig sConfig) {
	int negone = -1;

	int Lindex = 0;
	outFile << "// Network parallelization gradient compounding:\n";
	outFile << "NETPAR_GRADIENT NETPAR_GRADIENT0(\n\t";
	for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, false, "_netPar" + Int2Str(ts), "_netPar" + Int2Str(ts));
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, true, "_netPar" + Int2Str(ts), "_netPar" + Int2Str(ts));
	}
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, false);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, true, true);
	outFile << ".rc({" << StringSeries(sConfig.RandNums.short_ADDSUB_NETPAR, "R", "_ADDSUB_NETPAR") << "}),\n\t";
	outFile << ".CLK(CLK), .INIT(INIT)\n);\n\n";	
}
