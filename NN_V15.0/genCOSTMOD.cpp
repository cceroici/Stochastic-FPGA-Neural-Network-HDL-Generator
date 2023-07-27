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
#include "genCOSTMOD.h"
#include "NN_DataTypes.h"
#include "Verilog.h"

using namespace std;

void genCOSTMOD(sysConfig sConfig){


	ofstream outFile;
	outFile.open(sConfig.Dir + "COSTMOD.v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Cost function calculation \n\n";
	int NProd = 0;
	int Nindex = 0;
	outFile << "module COSTMOD(\n\t";
	DeclareSingleLayerNet(outFile, sConfig, "Y", ModuleDeclarationPortList);
	DeclareSingleLayerNet(outFile, sConfig, "SIGN_L" + Int2Str(sConfig.NNobj.OUTPUTLYR), ModuleDeclarationPortList);
	DeclareSingleLayerNet(outFile, sConfig, "eps", ModuleDeclarationPortList);
	if (sConfig.IsRecurrent())
		outFile << StringSeries(sConfig.maxTimeSteps, "a_out_T", "") << ",\n\t";
	else
		outFile << "a_out,\n\t";
	outFile << StringSeries(sConfig.RandNums.short_ADDSUB_error, "R", "") << ",\n\t";
	outFile << "CLK, CLK_TRAINING_flag,  INIT\n);\n\n";

	outFile << "parameter DP_in = " << sConfig.DP_in << ";\n";
	outFile << "parameter DP_out = " << sConfig.DP_out << ";\n";
	outFile << "parameter SUPPRESS_TIME = 8;\n\n";

	

	// ---------------------------    port definitions
	outFile << "input wire CLK, CLK_TRAINING_flag, INIT;\n";
	DeclareSingleLayerNet(outFile, sConfig, "Y", ModuleDeclarationPortsInput);
	if (sConfig.IsRecurrent())
		outFile << "input wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << "-1:0]" << StringSeries(sConfig.maxTimeSteps, "a_out_T", "") << ";\n";
	else
		outFile << "input wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << "-1:0] a_out;\n";
	outFile << "input wire [" << sConfig.LFSR_S / 2 - 1 << ":0] " << StringSeries(sConfig.RandNums.short_ADDSUB_error, "R", "") << ";\n";
	DeclareSingleLayerNet(outFile, sConfig, "SIGN_L" + Int2Str(sConfig.NNobj.OUTPUTLYR), ModuleDeclarationPortsOutput);
	DeclareSingleLayerNet(outFile, sConfig, "eps", ModuleDeclarationPortsOutput);

	outFile << "\n";
	if (!sConfig.IsRecurrent())
		outFile << "wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << "-1:0] R_condition;\n";
	else 
		outFile << "wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]*sConfig.maxTimeSteps << "-1:0] R_condition;\n";
	DeclareSingleLayerNet(outFile, sConfig, "eps_temp", TopLevel);
	outFile << "reg [8-1:0] SUPPRESS_COUNTER = 1'd0;\n\n";

	// Cost Evaluation:
	outFile << "// Cost Evaluation:\n\n";

	outFile << "SS_RERANDOMIZER RERANDOMIZER0(.CLK(CLK), .INIT(INIT), .IN(R0[0]), .OUT(R_condition));\n";
	if (!sConfig.IsRecurrent())
		outFile << "defparam RERANDOMIZER0.N = " << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << ";\n\n";
	else 
		outFile << "defparam RERANDOMIZER0.N = " << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]*sConfig.maxTimeSteps << ";\n\n";


	if (!sConfig.IsRecurrent()) {
		if (sConfig.NNobj.CostFunction == MeanSquared) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[1 + sConfig.NNobj.N_HIDDENLYR]; j++) {
				outFile << "SS_ADDSUB2 ADDSUB" << j << "(.CLK(CLK),.INIT(INIT|CLK_TRAINING_flag),.IN({Y[" << j << "],a_out[" << j << "]}),.SIGN({1'b0,1'b1}),.OUT(eps_temp[" << j << "]),.R_condition(R_condition[" << j << "]),.SIGN_out(SIGN_L" << sConfig.NNobj.OUTPUTLYR << "[" << j << "]));\n";
			}
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[1 + sConfig.NNobj.N_HIDDENLYR]; j++) {
				outFile << "defparam ADDSUB" << j << ".N = 2;\n";
			}
			outFile << "\n";
		}
		else if (sConfig.NNobj.CostFunction == BurstDifference)
			for (int i = 0; i < sConfig.NNobj.LYRSIZE[1 + sConfig.NNobj.N_HIDDENLYR]; i++)
				outFile << "NN_COST_ENTROPY CE" << i << "(.CLK(CLK),.INIT(INIT|CLK_TRAINING_flag),.A(a_out[" << i << "]),.Y(Y[" << i << "]),.A_SIGN(1'b1),.Y_SIGN(1'b0),.R_condition(R_condition[" << i << "]),.OUT(eps_temp[" << i << "]),.SIGN_out(SIGN_L" << sConfig.NNobj.OUTPUTLYR << "[" << i << "]));\n";
	}
	else {
		int RconditionIndexCounter = 0;
		for (int t = 0; t < sConfig.maxTimeSteps; t++)
		{
			if (sConfig.NNobj.CostFunction == MeanSquared) {
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[1 + sConfig.NNobj.N_HIDDENLYR]; j++) {
					outFile << "SS_ADDSUB2 ADDSUB" << j << "_T" << t << "(.CLK(CLK),.INIT(INIT|CLK_TRAINING_flag),.IN({Y_T" << t << "[" << j << "],a_out_T" << t << "[" << j << "]}),.SIGN({1'b0,1'b1}),.OUT(eps_temp_T" << t << "[" << j << "]),.R_condition(R_condition[" << RconditionIndexCounter++ << "]),.SIGN_out(SIGN_L" << sConfig.NNobj.OUTPUTLYR << "_T" << t << "[" << j << "]));\n";
				}
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[1 + sConfig.NNobj.N_HIDDENLYR]; j++) {
					outFile << "defparam ADDSUB" << j << "_T" << t << ".N = 2;\n";
				}
				outFile << "\n";
			}
			else if (sConfig.NNobj.CostFunction == BurstDifference)
				for (int i = 0; i < sConfig.NNobj.LYRSIZE[1 + sConfig.NNobj.N_HIDDENLYR]; i++)
					outFile << "NN_COST_ENTROPY CE" << i << "_T" << t << "(.CLK(CLK),.INIT(INIT|CLK_TRAINING_flag),.A(a_out_T" << t << "[" << i << "]),.Y(Y_T" << t << "[" << i << "]),.A_SIGN(1'b1),.Y_SIGN(1'b0),.R_condition(R_condition[" << RconditionIndexCounter++ << "]),.OUT(eps_temp_T" << t << "[" << i << "]),.SIGN_out(SIGN_L" << sConfig.NNobj.OUTPUTLYR << "_T" << t << "[" << i << "]));\n";
			outFile << "\n";
		}
	}



	outFile << "\n";

	outFile << "always @(posedge CLK or posedge CLK_TRAINING_flag or posedge INIT) begin\n\t";
	outFile << "if (CLK_TRAINING_flag | INIT) SUPPRESS_COUNTER <= 1'b0;\n\t";
	outFile << "else if (SUPPRESS_COUNTER <= SUPPRESS_TIME) SUPPRESS_COUNTER <= SUPPRESS_COUNTER + 1'b1;\n\t";
	outFile << "else SUPPRESS_COUNTER <= SUPPRESS_COUNTER;\n";
	outFile << "end\n\n";

	if (!sConfig.IsRecurrent())
		outFile << "assign eps = (SUPPRESS_COUNTER>SUPPRESS_TIME) ? eps_temp : 3'b000;\n\n";
	else 
	{
		for (int t = 0; t < sConfig.maxTimeSteps; t++)
		{
			outFile << "assign eps_T" << t << " = (SUPPRESS_COUNTER>SUPPRESS_TIME) ? eps_temp_T" << t << " : 3'b000;\n";
		}
	}
	outFile << "\n";

	outFile << "endmodule\n";
	

}


void genCOSTMOD_Inst(std::ostream &outFile, sysConfig sConfig) {
	int negone = -1;
	if (!sConfig.usesTSParallelization()) {
		int Lindex = 0;
		outFile << "// Cost Function Module:\n";
		outFile << "COSTMOD COSTMOD0(\n\t";
		DeclareSingleLayerNet(outFile, sConfig, "Y", ModuleInstantiationPorts);
		if (sConfig.IsRecurrent())
			outFile << StringSeriesPort(sConfig.maxTimeSteps, "a_out_T", "", "a_L" + Int2Str(sConfig.NNobj.OUTPUTLYR) + "_T", "", negone) << "\n\t";
		else
			outFile << ".a_out(" << "a_L" << sConfig.NNobj.OUTPUTLYR << "),\n\t";
		outFile << StringSeriesPort(sConfig.RandNums.short_ADDSUB_error, "R", "", "R", "_ADDSUB_error", negone) << "\n\t";
		DeclareSingleLayerNet(outFile, sConfig, "SIGN_L" + Int2Str(sConfig.NNobj.OUTPUTLYR), ModuleInstantiationPorts);
		DeclareSingleLayerNet(outFile, sConfig, "eps", ModuleInstantiationPorts);
		outFile << ".CLK(CLK), .CLK_TRAINING_flag(TRAINING_PRESET2), .INIT(INIT|FP_FRAME)\n);\n\n";
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			int Lindex = 0;
			outFile << "// Cost Function Module " << ts << ":\n";
			outFile << "COSTMOD COSTMOD" << ts << "(\n\t";
			DeclareSingleLayerNet(outFile, sConfig, "Y", ModuleInstantiationPorts, ts);
			if (sConfig.IsRecurrent())
				outFile << StringSeriesPort(sConfig.maxTimeSteps, "a_out_T", "", "a_L" + Int2Str(sConfig.NNobj.OUTPUTLYR) + "_T", "", negone) << "\n\t";
			else {
				outFile << ".a_out(" << "a_L" << sConfig.NNobj.OUTPUTLYR << "_netPar" << ts << "),\n\t";
			}
			outFile << StringSeriesPort(sConfig.RandNums.short_ADDSUB_error, "R", "", "R", "_ADDSUB_error_netPar" + Int2Str(ts), negone) << "\n\t";
			DeclareSingleLayerNet(outFile, sConfig, "SIGN_L" + Int2Str(sConfig.NNobj.OUTPUTLYR), ModuleInstantiationPorts, ts);
			DeclareSingleLayerNet(outFile, sConfig, "eps", ModuleInstantiationPorts, ts);
			outFile << ".CLK(CLK), .CLK_TRAINING_flag(TRAINING_PRESET2), .INIT(INIT|FP_FRAME)\n);\n\n";
		}
	}
}