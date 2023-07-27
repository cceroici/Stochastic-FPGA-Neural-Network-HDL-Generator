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

using namespace std;

void genRNE(sysConfig sConfig){

	ofstream outFile;
	outFile.open(sConfig.Dir + "RNE.v");

	int RNperLFSR = 16;

	outFile << "// Chris Ceroici \n\n";
	outFile << "module RNE(\n\t";
	outFile << StringSeries(sConfig.RandNums.Seeds, "S", "") << ",\n\t";
	outFile << StringSeries(sConfig.RandNums.short_total, "R", "") << ",\n\t";
	if (sConfig.RandNums.long_d2s>0)
		outFile << StringSeries(sConfig.RandNums.long_d2s, "R_long", "") << ",\n\t";
	outFile << "CLK, INIT_powerup, SDcount\n";
	outFile<<"); \n\n";

	outFile << "parameter DP = " << sConfig.DP_in << "; // RN precision\n";
	outFile << "parameter DPlong =" << sConfig.DP_out << "; // RN alpha precision\n";
	outFile << "parameter LFSR_S = " << sConfig.LFSR_S << "; // LFSR size\n\n";
	outFile << "input CLK, INIT_powerup; // Initialization signal (new set)\n";
	outFile << "input [" << 2 * sConfig.LFSR_S - 1 << ":0] SDcount;\n";
	DeclareNet(outFile, "input wire", Int2Str(sConfig.LFSR_S), sConfig.RandNums.Seeds, "S", "");
	DeclareNet(outFile, "output", "DP", sConfig.RandNums.short_total, "R", "");
	
	DeclareNet(outFile, "output", "DPlong", sConfig.RandNums.long_total, "R_long", "");
	outFile << "\n";
	int dummyPorts = RNperLFSR - sConfig.RandNums.short_total%RNperLFSR;
	int actualNofRNs = dummyPorts + sConfig.RandNums.short_total;

	int * distributedRNorder = new int[actualNofRNs];

	int n = 0;
	for (int j = 0; j < RNperLFSR; j++) {
		for (int i = 0; i < actualNofRNs/RNperLFSR; i++)
			distributedRNorder[n++] = i*RNperLFSR + j;
	}
	
	for (int i = 0; i <= actualNofRNs / 2; i++)
	{
		if (i % 2 != 0) {
			int tempLo = distributedRNorder[i];
			int tempHi = distributedRNorder[actualNofRNs - i - 1];

			distributedRNorder[i] = tempHi;
			distributedRNorder[actualNofRNs - i - 1] = tempLo;
		}
	}

	if (dummyPorts != 0) {
		outFile << "wire [DP-1:0] ";
		for (int i = 0; i < dummyPorts-1; i++)
			outFile << "R" << sConfig.RandNums.short_total + i << ",";
		outFile << "R" << sConfig.RandNums.short_total + dummyPorts - 1 << ";\n\n";
	}

	// generate short RN LFSRs
	outFile << "// Short RN LFSRs\n";
	for (int i = 0; i <= sConfig.RandNums.short_total; i = i + RNperLFSR) {
		outFile << "cascLFSR_" << RNperLFSR  << "Tap xLFSR" << ceil(i/RNperLFSR) << "(.TRIG(CLK),.RESET(INIT_powerup),";
		for (int j = 0; j < RNperLFSR; j++)
			outFile << ".OUT" << j << "(R" << distributedRNorder[j+i] << "),";
		outFile << ".SEED(S" << ceil(i / 2) << "), .SDcount(SDcount)); \n";
	}
		int SeedCount = ceil((sConfig.RandNums.short_total - 1) / 2)+1;
	outFile << "\n// Long RN LFSRs\n";

	// generate long RN LFSRs
	for (int i = 0; i < sConfig.RandNums.long_d2s; i++)
		outFile << "cascLFSR xLFSR_L" << i << "(.TRIG(CLK),.RESET(INIT_powerup),.OUT1(R_long" << i << "[" << sConfig.DP_out / 2 - 1 << ":0]),.OUT2(R_long" << i << "[" << sConfig.DP_out - 1 <<":"<< sConfig.DP_out / 2 << "]),.SEED(S" << SeedCount++ << "),.SDcount(SDcount));\n";

	outFile << "\nendmodule\n";

}


void genRNE_Inst(std::ostream &outFile, sysConfig sConfig) {
	int negone = -1;
	int Lindex = 0;
	outFile << "\n//	Random number engine\n";
	outFile << "RNE RNE0(\n";
	ofstream outFile_ext;
	outFile_ext.open(sConfig.Dir + "RNE_portDefinitions.v");
	outFile << "`include \"RNE_portDefinitions.v\"\n\t";
	outFile_ext << StringSeriesPort(sConfig.RandNums.Seeds, "S", "", "SD", "", negone) << "\n\t";
	Lindex = 0;
	outFile_ext << StringSeriesPort(sConfig.RandNums.short_d2s, "R", "", "R", "", Lindex) << ",\n\t";
	if (!sConfig.usesTSParallelization()) {
		outFile_ext << StringSeriesPort(sConfig.RandNums.short_ADDSUB_FP, "R", "", "R", "_ADDSUB_FP", Lindex) << ",\n\t";
		outFile_ext << StringSeriesPort(sConfig.RandNums.short_ADDSUB_BP, "R", "", "R", "_ADDSUB_BP", Lindex) << ",\n\t";
		if (sConfig.RandNums.short_ADDSUB_BP_CV>0) outFile_ext << StringSeriesPort(sConfig.RandNums.short_ADDSUB_BP_CV, "R", "", "R", "_ADDSUB_BP_CV", Lindex) << ",\n\t";
		if (sConfig.IsRecurrent()) outFile_ext << StringSeriesPort(sConfig.RandNums.short_ADDSUB_BPTT, "R", "", "R", "_ADDSUB_BPTT", Lindex) << ",\n\t";
		outFile_ext << StringSeriesPort(sConfig.RandNums.short_ADDSUB_error, "R", "", "R", "_ADDSUB_error", Lindex) << ",\n\t";
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			outFile_ext << StringSeriesPort(sConfig.RandNums.short_ADDSUB_FP/ sConfig.TrainingSetParallelization, "R", "", "R", "_ADDSUB_FP_netPar" + Int2Str(ts), Lindex) << ",\n\t";
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			outFile_ext << StringSeriesPort(sConfig.RandNums.short_ADDSUB_BP/ sConfig.TrainingSetParallelization, "R", "", "R", "_ADDSUB_BP_netPar" + Int2Str(ts), Lindex) << ",\n\t";
		if (sConfig.IsRecurrent()) for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++)
			outFile_ext << StringSeriesPort(sConfig.RandNums.short_ADDSUB_BPTT, "R", "", "R", "_ADDSUB_BPTT_netPar" + Int2Str(ts), Lindex) << ",\n\t";
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if ((sConfig.RandNums.short_ADDSUB_error / sConfig.TrainingSetParallelization)>1)
				outFile_ext << StringSeriesPort(sConfig.RandNums.short_ADDSUB_error / sConfig.TrainingSetParallelization, "R", "", "R", "_ADDSUB_error_netPar" + Int2Str(ts), Lindex) << ",\n\t";
			else
				outFile_ext << ".R" << Lindex++ << "(R0_ADDSUB_error_netPar" << ts << "),\n\t";
		}
		outFile_ext << StringSeriesPort(sConfig.RandNums.short_ADDSUB_NETPAR, "R", "", "R", "_ADDSUB_NETPAR", Lindex) << ",\n\t";
	}
	Lindex = 0;
	outFile_ext << StringSeriesPort(sConfig.RandNums.long_d2s, "R_long", "", "R_long", "", Lindex) << ",\n\t";
	Lindex = 0;

	outFile << ".CLK(CLK),.INIT_powerup(INIT),.SDcount(SDcount)\n";
	outFile << "); \n\n";

}