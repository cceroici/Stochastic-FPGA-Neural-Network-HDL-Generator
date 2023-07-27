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
#include "genFWDPROP.h"
#include "genFULLCONN.h"
#include "genCONV.h"
#include "NN_DataTypes.h"
#include "Verilog.h"
#include "genSEED.h"


using namespace std;

void genFWDPROP(sysConfig sConfig){

	ofstream outFile;
	outFile.open(sConfig.Dir + "FWDPROP.v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Neural Network Forward Propagation \n\n";
	int NProd = 0;
	int Nindex = 0;
	int negone = -1;
	outFile << "module FWDPROP(\n\t";
	//outFile << NN_StringSeries2(sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, "alpha_L", "_j_", "", "\n\t", 1);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortList, false, true);


	outFile << StringSeries(sConfig.RandNums.short_ADDSUB_FP, "R_FP", "") << ",\n\t";
	outFile << "zeroCenterSource,\n\t";
	DeclareForwardPropagationNets(outFile, sConfig, false, ModuleDeclarationPortList);
	NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a_MEM_ACTIVE", wire, ModuleDeclarationPortList, singleLayer, sConfig.IsRecurrent());
	outFile << "CLK, INIT, CLK_TRAINING_flag\n);\n\n";

	outFile << "parameter MEMSIZE = 3;\n";
	outFile << "parameter DP_in = " << sConfig.DP_in << ";\n";
	outFile << "parameter DP_out = " << sConfig.DP_out << ";\n\n";
	

	// ---------------------------    port definitions
	outFile << "input wire CLK, INIT, CLK_TRAINING_flag;\n";
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsInput);
	NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleDeclarationPortsInput, false, true);
	DeclareForwardPropagationNets(outFile, sConfig, false, ModuleDeclarationPortsOutput);
	NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a_MEM_ACTIVE", wire, ModuleDeclarationPortsInput, singleLayer, sConfig.IsRecurrent());
	outFile << "input wire [" << sConfig.LFSR_S / 2 - 1 << ":0] " << StringSeries(sConfig.RandNums.short_ADDSUB_FP, "R_FP", "") << ";\n";
	outFile << "input wire [" << sConfig.RandNums.short_zeroCenter << "-1:0] zeroCenterSource;\n";
	outFile << "\n";

	outFile << "wire [" << sConfig.RandNums.short_ADDSUB_FP - 1 << ":0] R_FP_source;\n\n";

	outFile << "\n";
	if (!sConfig.IsRecurrent()) {
		for (int l = 1; l < sConfig.NNobj.OUTPUTLYR + 1; l++) {
			outFile << "DEC2STCH DSRFP" << l - 1 << "(.CLK(CLK), .INIT(INIT), .D(8'd127),.LFSR(R_FP" << l - 1 << "),.S(R_FP_source[" << l - 1 << "]));\n";
		}
	}
	else {
		for (int l = 1; l < (sConfig.NNobj.OUTPUTLYR)*sConfig.maxTimeSteps + 1; l++) {
			outFile << "DEC2STCH DSRFP" << l - 1 << "(.CLK(CLK), .INIT(INIT), .D(8'd127),.LFSR(R_FP" << l - 1 << "),.S(R_FP_source[" << l - 1 << "]));\n";
		}
	}
	outFile << "\n";
	int RNGindex = 0;
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		if (sConfig.ConnectionType(l) == FC_CV) {
			genCONV_FWD_block_Inst(outFile, sConfig, l, RNGindex++, 0);
		}
		else if ((sConfig.NNobj.LYRinfo[l].LT == FC)|(sConfig.NNobj.LYRinfo[l].LT == RCC)) {
			if (!sConfig.IsRecurrent())
				genFULLCONN_FWD_block_Inst(outFile, sConfig, l, l - 1, -1);
			else
				for (int t = 0; t < sConfig.maxTimeSteps; t++)
					genFULLCONN_FWD_block_Inst(outFile, sConfig, l, RNGindex++, t);
		}
	}
	
	outFile << "\n";

	outFile << "endmodule\n";
	
	outFile.close();
}

void genFWDPROP_Inst(std::ostream &outFile, sysConfig sConfig) {
	int negone = -1;
	if (!sConfig.usesTSParallelization()) {
		int Lindex = 0;
		outFile << "// Forward Propagation Module:\n";
		outFile << "FWDPROP FWDPROP0(\n\t";
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts);
		NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, false, true);
		DeclareForwardPropagationNets(outFile, sConfig, false, ModuleInstantiationPorts);
		NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a_MEM_ACTIVE", wire, ModuleInstantiationPorts, singleLayer, sConfig.IsRecurrent());
		outFile << "\n\t" << StringSeriesPort(sConfig.RandNums.short_ADDSUB_FP, "R_FP", "", "R", "_ADDSUB_FP", negone) << "\n\t";
		outFile << ".zeroCenterSource(zeroCenterSource),\n\t";
		outFile << ".CLK(CLK),  .INIT(INIT), .CLK_TRAINING_flag(CLK_TRAINING_flag)\n);\n\n";
	}
	else
	{
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			int Lindex = 0;
			outFile << "// Forward Propagation Module " << ts << ":\n";
			outFile << "FWDPROP FWDPROP" << ts << "(\n\t";
			NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts);
			NN_DeclareWeightNets(outFile, sConfig, sConfig.NNobj.LYRSIZE, 1, sConfig.NNobj.OUTPUTLYR, wire, ModuleInstantiationPorts, false, true);
			DeclareForwardPropagationNets(outFile, sConfig, false, ModuleInstantiationPorts, ts);
			NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a_MEM_ACTIVE", wire, ModuleInstantiationPorts, singleLayer, sConfig.IsRecurrent(), ts);
			outFile << "\n\t" << StringSeriesPort(sConfig.RandNums.short_ADDSUB_FP, "R_FP", "", "R", "_ADDSUB_FP_netPar" + Int2Str(ts), negone) << "\n\t";
			outFile << ".zeroCenterSource(zeroCenterSource_netPar" << ts << "),\n\t";
			outFile << ".CLK(CLK),  .INIT(INIT), .CLK_TRAINING_flag(CLK_TRAINING_flag)\n);\n\n";
		}
	}
}