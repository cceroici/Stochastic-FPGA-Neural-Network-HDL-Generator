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
#include "BitOps.h"
#include "genFULLCONN.h"
#include "NN_DataTypes.h"
#include "Verilog.h"

using namespace std;

void genFULLCONN_FWD_block(sysConfig sConfig, int L)
{
	ofstream outFile;
	outFile.open(sConfig.Dir + "FULLCONNBLOCK_L" + Int2Str(L) + ".v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Fully Connected Layer " << Int2Str(L) << " \n\n";

	outFile << "module FULLCONNBLOCK_L" << L << "(\n\t";
	outFile << "a_input,\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << "a_last,\n\t";
	outFile << "a_MEM_ACTIVE,\n\t";
	outFile << StringSeries(sConfig.NNobj.LYRSIZE[L], "alpha_j_", "") << ",\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << StringSeries(sConfig.NNobj.LYRSIZE[L], "gamma_j_", "") << ",\n\t";
	outFile << "beta,\n\t";
	outFile << StringSeries(sConfig.NNobj.LYRSIZE[L], "SIGN_alpha_j_", "") << ",\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << StringSeries(sConfig.NNobj.LYRSIZE[L], "SIGN_gamma_j_", "") << ",\n\t";
	outFile << "SIGN_beta,\n\t";
	outFile << "z,\n\t";
	outFile << "zp,\n\t";
	outFile << "a_out,\n\t";
	outFile << "R_FPsource, zeroCenterSource,\n\t";
	outFile << "CLK, INIT, CLK_TRAINING_flag\n);\n\n";

	// ---------------------------    port definitions

	outFile << "parameter MEMSIZE = 3;\n";
	outFile << "parameter DP_in = 8;\n";
	outFile << "parameter DP_out = 16;\n\n";

	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L-1] << " - 1:0] a_input;\n";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << " - 1:0] a_last;\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] a_MEM_ACTIVE;\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L-1] << "-1:0] " << StringSeries(sConfig.NNobj.LYRSIZE[L], "alpha_j_", "") << ";\n";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] " << StringSeries(sConfig.NNobj.LYRSIZE[L], "gamma_j_", "") << ";\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] beta;\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L-1] << "-1:0] " << StringSeries(sConfig.NNobj.LYRSIZE[L], "SIGN_alpha_j_", "") << ";\n";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] " << StringSeries(sConfig.NNobj.LYRSIZE[L], "SIGN_gamma_j_", "") << ";\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] SIGN_beta;\n";
	outFile << "input wire R_FPsource, zeroCenterSource;\n";
	outFile << "input wire CLK, INIT, CLK_TRAINING_flag;\n";
	outFile << "\n";

	outFile << "output wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] z;\n";
	outFile << "output wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] zp;\n";
	outFile << "output wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] a_out;\n";
	outFile << "\n";

	outFile << "wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] zeroCenter, R_FP;\n";
	outFile << "SS_RERANDOMIZER RERANDOMIZER_zC(.CLK(CLK), .INIT(INIT), .IN(zeroCenterSource), .OUT(zeroCenter));\n";
	outFile << "defparam RERANDOMIZER_zC.N = " << sConfig.NNobj.LYRSIZE[L] << "; \n";
	outFile << "SS_RERANDOMIZER RERANDOMIZER_FP(.CLK(CLK), .INIT(INIT), .IN(R_FPsource), .OUT(R_FP));\n";
	outFile << "defparam RERANDOMIZER_FP.N = " << sConfig.NNobj.LYRSIZE[L] << "; \n\n";

	if (sConfig.NNobj.LYRinfo[L].LT == FC) {
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[L]; i++) {
			outFile << "NN_NODE_BGPOLAR NODE_" << i << "(.a(a_input), .alpha(alpha_j_" << i << "), .beta(beta[" << i << "]), .SIGN_alpha(SIGN_alpha_j_" << i << "), .SIGN_beta(SIGN_beta[" << i << "]), .z(z[" << i << "]), .d(zeroCenter[" << i << "]), .r(R_FP[" << i << "]), .a_out(a_out[" << i << "]), .INIT_STATE(a_MEM_ACTIVE[" << i << "]), .CLK(CLK), .INIT(INIT | CLK_TRAINING_flag)); // layer " << L << " node " << i << "\n";
		}
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[L]; i++) {
			outFile << "defparam NODE_" << i << ".N = " << sConfig.NNobj.LYRSIZE[L - 1] << ";\n";
			outFile << "defparam NODE_" << i << ".MEMSIZE = MEMSIZE;\n";
		}
	}
	else if (sConfig.NNobj.LYRinfo[L].LT == RCC) {
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[L]; i++) {
			outFile << "NN_RNN_NODE_BGPOLAR NODE_" << i << "(.a(a_input), .a_last(a_last), .alpha(alpha_j_" << i << "), .gamma(gamma_j_" << i << "), .beta(beta[" << i << "]), .SIGN_alpha(SIGN_alpha_j_" << i << "), .SIGN_gamma(SIGN_gamma_j_" << i << "), .SIGN_beta(SIGN_beta[" << i << "]), .z(z[" << i << "]), .d(zeroCenter[" << i << "]), .r(R_FP[" << i << "]), .a_out(a_out[" << i << "]), .INIT_STATE(a_MEM_ACTIVE[" << i << "]), .CLK(CLK), .INIT(INIT | CLK_TRAINING_flag)); // layer " << L << " node " << i << "\n";
		}
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[L]; i++) {
			outFile << "defparam NODE_" << i << ".N = " << sConfig.NNobj.LYRSIZE[L - 1] << ";\n";
			outFile << "defparam NODE_" << i << ".NR = " << sConfig.NNobj.LYRSIZE[L] << ";\n";
			outFile << "defparam NODE_" << i << ".MEMSIZE = MEMSIZE;\n";
		}
	}
	outFile << "\n";

	for (int i = 0; i < sConfig.NNobj.LYRSIZE[L]; i++) {
		outFile << "NN_BGPRIME BGPRIME_" << i << "(.z(z[" << i << "]), .zp(zp[" << i << "]), .CLK(CLK), .INIT(INIT));\n";
	}
	outFile << "\n";


	outFile << "endmodule\n";
		
}

void genFULLCONN_BP_block(sysConfig sConfig, int L) {

	ofstream outFile;
	outFile.open(sConfig.Dir + "FULLCONNBLOCKBP_L" + Int2Str(L) + ".v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Fully Connected Backpropagation Layer " << Int2Str(L) << " \n\n";

	outFile << "module FULLCONNBLOCKBP_L" << L << "(\n\t";
	outFile << StringSeries(sConfig.NNobj.LYRSIZE[L + 1], "alpha_j_", "") << ",\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << StringSeries(sConfig.NNobj.LYRSIZE[L], "gamma_j_", "") << ",\n\t";
	outFile << StringSeries(sConfig.NNobj.LYRSIZE[L + 1], "SIGN_alpha_j_", "") << ",\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << StringSeries(sConfig.NNobj.LYRSIZE[L], "SIGN_gamma_j_", "") << ",\n\t";
	outFile << "z,\n\t";
	outFile << "zp,\n\t";
	outFile << "delta_in,\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << "delta_next,\n\t";
	outFile << "SIGN_delta_in,\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << "SIGN_delta_next,\n\t";
	outFile << "delta_out,\n\t";
	outFile << "SIGN_delta_out,\n\t";
	outFile << "R_BPsource,\n\t";
	outFile << "CLK, INIT\n);\n\n";

	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] " << StringSeries(sConfig.NNobj.LYRSIZE[L + 1], "alpha_j_", "") << ";\n";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] " << StringSeries(sConfig.NNobj.LYRSIZE[L], "gamma_j_", "") << ";\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] " << StringSeries(sConfig.NNobj.LYRSIZE[L + 1], "SIGN_alpha_j_", "") << ";\n";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] " << StringSeries(sConfig.NNobj.LYRSIZE[L], "SIGN_gamma_j_", "") << ";\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] z;\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] zp;\n";

	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L + 1] << "-1:0] " << "delta_in;\n";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] " << "delta_next;\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L + 1] << "-1:0] " << "SIGN_delta_in;\n";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << "input wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] " << "SIGN_delta_next;\n";
	outFile << "input wire R_BPsource;\n";
	outFile << "input wire CLK, INIT;\n";
	outFile << "\n";

	outFile << "output wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] delta_out;\n";
	outFile << "output wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] SIGN_delta_out;\n";
	outFile << "\n";
	outFile << "wire [" << sConfig.NNobj.LYRSIZE[L] << "-1:0] R_BP;\n\n";
	outFile << "SS_RERANDOMIZER RERANDOMIZER0(.CLK(CLK), .INIT(INIT), .IN(R_BPsource), .OUT(R_BP));\n";
	outFile << "defparam RERANDOMIZER0.N = " << sConfig.NNobj.LYRSIZE[L] << ";\n\n";

	if (sConfig.NNobj.LYRinfo[L].LT == FC) {
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[L]; i++) {
			outFile << "NN_DELTABP_POLAR DELTABP_" << i << "(.CLK(CLK),.INIT(INIT),.R_condition(R_BP[" << i << "]),.z(z[" << i << "]),.zp(zp[" << i << "]),.delta(delta_in),.SIGN_delta(SIGN_delta_in), .alpha({";
			for (int j = (sConfig.NNobj.LYRSIZE[L + 1] - 1); j > 0; j--)
				outFile << "alpha_j_" << j << "[" << i << "], ";
			outFile << "alpha_j_0[" << i << "]}), .SIGN_alpha({";
			for (int j = (sConfig.NNobj.LYRSIZE[L + 1] - 1); j > 0; j--)
				outFile << "SIGN_alpha_j_" << j << "[" << i << "], ";
			outFile << "SIGN_alpha_j_0[" << i << "]}), ";
			outFile << ".delta_out(delta_out[" << i << "]), .SIGN_out(SIGN_delta_out[" << i << "]));\n";
		}
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[L]; i++)
			outFile << "defparam DELTABP_" << i << ".N = " << sConfig.NNobj.LYRSIZE[L + 1] << ";\n";
	}
	else if (sConfig.NNobj.LYRinfo[L].LT == RCC) {
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[L]; i++) {
			outFile << "NN_RNN_DELTABPTT_POLAR DELTABP_" << i << "(.CLK(CLK),.INIT(INIT),.R_condition(R_BP[" << i << "]),.z(z[" << i << "]),.zp(zp[" << i << "]),.delta(delta_in),.delta_next(delta_next),.SIGN_delta(SIGN_delta_in),.SIGN_delta_next(SIGN_delta_next), .alpha({";
			for (int j = (sConfig.NNobj.LYRSIZE[L + 1] - 1); j > 0; j--)
				outFile << "alpha_j_" << j << "[" << i << "], ";
			outFile << "alpha_j_0[" << i << "]}), .gamma({";
			for (int j = (sConfig.NNobj.LYRSIZE[L] - 1); j > 0; j--)
				outFile << "gamma_j_" << j << "[" << i << "], ";
			outFile << "gamma_j_0[" << i << "]}), .SIGN_alpha({";
			for (int j = (sConfig.NNobj.LYRSIZE[L + 1] - 1); j > 0; j--)
				outFile << "SIGN_alpha_j_" << j << "[" << i << "], ";
			outFile << "SIGN_alpha_j_0[" << i << "]}), .SIGN_gamma({";
			for (int j = (sConfig.NNobj.LYRSIZE[L] - 1); j > 0; j--)
				outFile << "SIGN_gamma_j_" << j << "[" << i << "], ";
			outFile << "SIGN_gamma_j_0[" << i << "]}), .delta_out(delta_out[" << i << "]), .SIGN_out(SIGN_delta_out[" << i << "]));\n";
		}
		for (int i = 0; i < sConfig.NNobj.LYRSIZE[L]; i++) {
			outFile << "defparam DELTABP_" << i << ".N = " << sConfig.NNobj.LYRSIZE[L + 1] << ";\n";
			outFile << "defparam DELTABP_" << i << ".NR = " << sConfig.NNobj.LYRSIZE[L] << ";\n";
		}
	}
	
	
	outFile << "\nendmodule";
	outFile.close();

	//for (int f = 0; f < sConfig.NNobj.LYRSIZE[L]; f++)
		//genCONV_BP_filter(sConfig.NNobj, sConfig.Dir, DP_in, DP_out, LFSR_S, FRAMESIZE, RandNums, LayerInfo, f, Connections);



}


void genFULLCONN_FWD_block_Inst(std::ostream &outFile, sysConfig sConfig, int L, int RNGindex, int T) {
	int negone = -1;
	string blockLabel = "0";
	string portTimeLabel = "";
	if (T >= 0) {
		blockLabel = "T" + Int2Str(T);
		portTimeLabel = "_T" + Int2Str(T);
	}
	outFile << "FULLCONNBLOCK_L" << L << " FULLCONNBLOCK_L" << L << "_" << blockLabel << "(\n\t";
	outFile << ".a_input(a_L" << L - 1 << portTimeLabel << "),\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == FC) {
		outFile << ".a_MEM_ACTIVE(a_MEM_ACTIVE_L" << L << portTimeLabel << "),\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "alpha_j_", "", "alpha_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
		outFile << ".beta(beta_L" << L << "),\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "SIGN_alpha_j_", "", "SIGN_alpha_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
		outFile << ".SIGN_beta(SIGN_beta_L" << L << "),\n\t";
		outFile << ".z(z_L" << L << portTimeLabel << "),\n\t";
		outFile << ".zp(zp_L" << L << portTimeLabel << "),\n\t";
		outFile << ".a_out(a_L" << L << portTimeLabel << "),\n\t";
		outFile << ".R_FPsource(R_FP_source[" << RNGindex << "]), .zeroCenterSource(zeroCenterSource[" << RNGindex << "]),\n\t";
		outFile << ".CLK(CLK), .INIT(INIT), .CLK_TRAINING_flag(CLK_TRAINING_flag)\n);\n\n";
	}
	else if (sConfig.NNobj.LYRinfo[L].LT == RCC) {
		if (T > 0) outFile << ".a_last(a_L" << L << "_T" << T - 1 << "),\n\t"; else outFile << ".a_last(" << sConfig.NNobj.LYRSIZE[L] << "'b0),\n\t";
		outFile << ".a_MEM_ACTIVE(a_MEM_ACTIVE_L" << L << portTimeLabel << "),\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "alpha_j_", "", "alpha_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "gamma_j_", "", "gamma_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
		outFile << ".beta(beta_L" << L << "),\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "SIGN_alpha_j_", "", "SIGN_alpha_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "SIGN_gamma_j_", "", "SIGN_gamma_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
		outFile << ".SIGN_beta(SIGN_beta_L" << L << "),\n\t";
		outFile << ".z(z_L" << L << portTimeLabel << "),\n\t";
		outFile << ".zp(zp_L" << L << portTimeLabel << "),\n\t";
		outFile << ".a_out(a_L" << L << portTimeLabel << "),\n\t";
		outFile << ".R_FPsource(R_FP_source[" << RNGindex << "]), .zeroCenterSource(zeroCenterSource[" << RNGindex << "]),\n\t";
		outFile << ".CLK(CLK), .INIT(INIT), .CLK_TRAINING_flag(CLK_TRAINING_flag)\n);\n\n";
	}
	else if (sConfig.NNobj.LYRinfo[L].LT == LSTM) {
		if (T > 0) outFile << ".a_last(a_L" << L << "_T" << T - 1 << "),\n\t"; else outFile << ".a_last(" << sConfig.NNobj.LYRSIZE[L] << "'b0),\n\t";
		//outFile << ".a_MEM_ACTIVE(a_MEM_ACTIVE_L" << L << portTimeLabel << "),\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "alpha_j_", "", "alpha_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "gamma_j_", "", "gamma_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
		outFile << ".beta(beta_L" << L << "),\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "SIGN_alpha_j_", "", "SIGN_alpha_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "SIGN_gamma_j_", "", "SIGN_gamma_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
		outFile << ".SIGN_beta(SIGN_beta_L" << L << "),\n\t";
		outFile << ".z(z_L" << L << portTimeLabel << "),\n\t";
		outFile << ".zp(zp_L" << L << portTimeLabel << "),\n\t";
		outFile << ".a_out(a_L" << L << portTimeLabel << "),\n\t";
		outFile << ".R_FPsource(R_FP_source[" << RNGindex << "]), .zeroCenterSource(zeroCenterSource[" << RNGindex << "]),\n\t";
		outFile << ".CLK(CLK), .INIT(INIT), .CLK_TRAINING_flag(CLK_TRAINING_flag)\n);\n\n";
	}

}

void genFULLCONN_BP_block_Inst(std::ostream &outFile, sysConfig sConfig, int L, int RNGindex, int T) {
	int negone = -1;
	string blockLabel = "0";
	string portTimeLabel = "";
	if (T >= 0) {
		blockLabel = "T" + Int2Str(T);
		portTimeLabel = "_T" + Int2Str(T);
	}
	outFile << "FULLCONNBLOCKBP_L" << L << " FULLCONNBLOCKBP_L" << L << "_" << blockLabel << "(\n\t";
	outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L+1], "alpha_j_", "", "alpha_L" + Int2Str(L+1) + "_j_", "", negone) << "\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "gamma_j_", "", "gamma_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
	outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L+1], "SIGN_alpha_j_", "", "SIGN_alpha_L" + Int2Str(L+1) + "_j_", "", negone) << "\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[L], "SIGN_gamma_j_", "", "SIGN_gamma_L" + Int2Str(L) + "_j_", "", negone) << "\n\t";
	outFile << ".z(z_L" << L << portTimeLabel << "),\n\t";
	outFile << ".zp(zp_L" << L << portTimeLabel << "),\n\t";
	outFile << ".delta_in(delta_L" << L + 1 << portTimeLabel << "),\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) if (T < sConfig.maxTimeSteps-1) outFile << ".delta_next(delta_L" << L << "_T" << T + 1 << "),\n\t"; else outFile << ".delta_next(" << sConfig.NNobj.LYRSIZE[L] << "'b0),\n\t";
	outFile << ".SIGN_delta_in(SIGN_L" << L + 1 << portTimeLabel << "),\n\t";
	if (sConfig.NNobj.LYRinfo[L].LT == RCC) if (T < sConfig.maxTimeSteps - 1) outFile << ".SIGN_delta_next(SIGN_L" << L << "_T" << T + 1 << "),\n\t"; else outFile << ".SIGN_delta_next(" << sConfig.NNobj.LYRSIZE[L] << "'b0),\n\t";
	outFile << ".delta_out(delta_L" << L << portTimeLabel << "),\n\t";
	outFile << ".SIGN_delta_out(SIGN_L" << L << portTimeLabel << "),\n\t";
	outFile << ".R_BPsource(R_BP0[" << RNGindex << "]),\n\t";
	outFile << ".CLK(CLK), .INIT(INIT)\n);\n\n";

}



