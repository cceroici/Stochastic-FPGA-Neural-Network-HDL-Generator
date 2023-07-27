/* Generate RNE */

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
#include "genRNE.h"
#include "BitOps.h"
#include "genDECC.h"

using namespace std;

void genDECC(sysConfig sConfig){

	ofstream outFile;
	outFile.open(sConfig.Dir + "DECC.v");
	outFile << "// Chris Ceroici \n\n";
	outFile << "module DECC(\n\t";

	NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a", wire, ModuleDeclarationPortList, singleLayer, sConfig.IsRecurrent());
	NN_DeclareGenericDecimalNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "aD", wire, "DP_out", ModuleDeclarationPortList, singleLayer, sConfig.IsRecurrent());

	outFile << "\tNodeOutputExp_clk, NodeOutputExp_index, NodeOutputExp_data,";
	outFile << "\n\tCLK, BP_FRAME, pre_BP_FRAME, FP_FRAME, pre_FP_FRAME, INIT\n);\n\n";

	outFile << "parameter DP_in = " << sConfig.DP_in << ";\n";
	outFile << "parameter DP_out = " << sConfig.DP_out << ";\n";
	outFile << "\n";

	// INPUT PORTS:
	NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a", wire, ModuleDeclarationPortsInput, singleLayer, sConfig.IsRecurrent());

	outFile << "input NodeOutputExp_clk;\n";
	outFile << "input [15:0] NodeOutputExp_index;\n";
	outFile << "input wire CLK, BP_FRAME, pre_BP_FRAME, FP_FRAME, pre_FP_FRAME, INIT;\n";
	outFile << "\n// Outputs:\n"; // Outputs:
	outFile << "output reg [15:0] NodeOutputExp_data;\n\n";

	NN_DeclareGenericDecimalNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "aD", wire, "DP_out", ModuleDeclarationPortsOutput, singleLayer, sConfig.IsRecurrent());

	if (!sConfig.IsRecurrent()) {
		for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
			if (sConfig.NNobj.LYRinfo[l].LT == FC) {
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					outFile << "MEAN MEANa_L" << l << "_" << j << "(.in(a_L" << l << "[" << j << "]), .out(aD_L" << l << "_" << j << "), .START(16'd0),.RESET(FP_FRAME),.CLK(CLK),.INIT(INIT),.ENABLE(1'b1), .preRESET(pre_FP_FRAME));\n";
				}
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					outFile << "defparam MEANa_L" << l << "_" << j << ".N_Count = DP_out;\n";
				}
			}
			outFile << "\n";
		}
	}
	else for (int t=0; t< sConfig.maxTimeSteps; t++) {
		for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
			if ((sConfig.NNobj.LYRinfo[l].LT == FC)|(sConfig.NNobj.LYRinfo[l].LT == RCC)) {
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					outFile << "MEAN MEANa_L" << l << "_T" << t << "_" << j << "(.in(a_L" << l << "_T" << t << "[" << j << "]), .out(aD_L" << l << "_T" << t << "_" << j << "), .START(16'd0),.RESET(FP_FRAME),.CLK(CLK),.INIT(INIT),.ENABLE(1'b1), .preRESET(pre_FP_FRAME));\n";
				}
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					outFile << "defparam MEANa_L" << l << "_T" << t << "_" << j << ".N_Count = DP_out;\n";
				}
			}
			else if (sConfig.NNobj.LYRinfo[l].LT == CV) {

			}
			outFile << "\n";
		}
		outFile << "\n";
	}

	genDECC_NodeOutputExport(outFile, sConfig);

	outFile << "\nendmodule\n";


	outFile.close();
}

void genDECC_NodeOutputExport(std::ostream &outFile, sysConfig sConfig) {
	outFile << "always @(posedge NodeOutputExp_clk) begin\n\t";
	outFile << "case (NodeOutputExp_index)\n\t\t";
	int index = 0;
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		if (sConfig.ConnectionType(l) == FC_FC) {
			if (!sConfig.IsRecurrent())
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[l]; i++)
					outFile << "16'd" << index++ << ": NodeOutputExp_data = aD_L" << l << "_" << i << ";\n\t\t";
			else 
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[l]; i++)
					for (int t = 0; t < sConfig.maxTimeSteps; t++)
						outFile << "16'd" << index++ << ": NodeOutputExp_data = aD_L" << l << "_T" << t << "_" << i << ";\n\t\t";
		}
	}
	outFile << "default: NodeOutputExp_data = 1'b0;\n\t";
	outFile << "\n\tendcase\n";
	outFile << "end\n\n";
}


void genDECC_Inst(std::ostream &outFile, sysConfig sConfig){
	int Lindex = 0;
	int negone = -1;
	outFile << "// Stochastic to Decimal Module:\n";
	outFile << "DECC DECC0(\n\t";

	string netPar = "";
	int netParInt = -1;
	if (sConfig.usesTSParallelization()) {
		netPar = "_nePar0";
		netParInt = 1;
	}

	NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a", wire, ModuleInstantiationPorts, singleLayer, sConfig.IsRecurrent(),netParInt);
	NN_DeclareGenericDecimalNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "aD", wire, "DP_out", ModuleInstantiationPorts, singleLayer, sConfig.IsRecurrent());
	outFile << "\n\t.NodeOutputExp_clk(NodeOutputExp_clk),.NodeOutputExp_index(ReadWriteIndex),.NodeOutputExp_data(NodeOutputExp_data),";
	outFile << "\n\t.CLK(CLK), .BP_FRAME(BP_FRAME), .pre_BP_FRAME(pre_BP_FRAME), .FP_FRAME(FRAME_RESET_DELAYED), .pre_FP_FRAME(FRAME_RESET), .INIT(INIT)\n);\n\n";
}