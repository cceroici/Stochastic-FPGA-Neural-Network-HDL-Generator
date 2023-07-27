/* Chris Ceroici
August 14 2013
Generates a various Verilog files for decoding LDPC codes
*/

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <io.h>
#include <Windows.h>
#include <direct.h>
#include "genSource.h"
#include "BitOps.h"
#include "NN_DataTypes.h"	

using namespace std;

void genSource(sysConfig sConfig){
	ofstream outFile;
	outFile.open(sConfig.Dir+"NN.do");
	string DirSource = ""; // find proper source directory path
	char cd[1000];
	_getcwd(cd,1000);
	DirSource = cd;
	int Dloc;
	Dloc = DirSource.find("\OneDrive");
	string Drive = "OneDrive";
	if (Dloc<0 || Dloc > 1000){
		Dloc = DirSource.find("\SkyDrive");
		Drive = "SkyDrive";
	}
	DirSource = DirSource.substr(0,Dloc);
	for (int i=0;i<DirSource.length();i++){
		if (DirSource[i]=='\\')
			DirSource[i]='/';
	}
	DirSource = DirSource + Drive +"/VerilogSourceFiles/";

	outFile << "vlog TOPsim.v RNE.v SEED.v STOCH.v BCKDPROP.v FWDPROP.v COSTMOD.v DECC.v TRAINING.v ";
	if (sConfig.TSloading == uBlazeRotation)
		outFile << "uBlaze_sim.v ";
	if (sConfig.usesTSParallelization())
		outFile << "NETPAR_GRADIENT.v ";
	if (sConfig.optimizer == HW) 
		outFile << "PARAMS_sim.v ";
	else if (sConfig.optimizer == alternatingHardwareMask) {
		outFile << "PARAM_STORAGE.v PARAM_TRAINING_MASK.v ";
	}
	for (int l = (sConfig.NNobj.OUTPUTLYR); l > 0; l--) {
		if ((sConfig.NNobj.LYRinfo[l].LT == FC)|(sConfig.NNobj.LYRinfo[l].LT == RCC)) {
			outFile << "FULLCONNBLOCK_L" << l << ".v ";
		}
		if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			outFile << "CONVBLOCK_L" << l << ".v CONVBLOCKBP_L" << l << ".v ";
			for (int f = 0; f <sConfig. NNobj.LYRinfo[l].conv.filters; f++) {
				outFile << "CONVFILTER_L" << l << "_F" << f << ".v CONVFILTERBP_L" << l << "_F" << f << ".v ";
			}
		}
	}
	for (int l = (sConfig.NNobj.OUTPUTLYR)-1; l > 0; l--) {
		if ((sConfig.NNobj.LYRinfo[l].LT == FC) | (sConfig.NNobj.LYRinfo[l].LT == RCC)) {
			outFile << "FULLCONNBLOCKBP_L" << l << ".v ";
		}
	}
	outFile << "-y " + DirSource + "CT_SYNTH/" + " +libext+.v "; // add source directory
	outFile << "-y " + DirSource + "Stochastic/" + " +libext+.v "; // add source directory
	outFile << "-y " + DirSource + "HHMM/" +  " +libext+.v "; // add source directory
	outFile << "-y " + DirSource + "NN/" + " +libext+.v\n"; // add source directory

	outFile<<"vsim TOP -novopt\n\n";
	outFile<<"#run -all\n";
	//outFile << "do me_diag.do\n";
	outFile << "#run 200ms\n";
	outFile.close();
}

void genRNE_Diag(string Dir, int N){
	ofstream outFile;
	outFile.open(Dir + "RNE_Diag.do");

	outFile << "add wave -noupdate -color Magenta -radix unsigned -label Seed0 -group {RNE} /TOP/RNE0/S0\n\n";
	for (int i = 0; i < N; i++)
		outFile << "add wave -noupdate -color {Indian Red} -radix unsigned -label R" << i << " -group {RNE} /TOP/RNE0/R" << i << "\n";

}

void genSTOCH_Diag(string Dir, int N){
	ofstream outFile;
	outFile.open(Dir + "STOCH_Diag.do");
	for (int i = 0; i < N;i++)
		outFile << "add wave -noupdate -group {STOCH} -color {Orange} -radix unsigned -label P_D" << i << " /TOP/STOCH0/PD" << i << "\n";
	for (int i = 0; i < N; i++)
		outFile << "add wave -noupdate -group {STOCH} -color {Gray60} -radix unsigned -label R" << i << " /TOP/STOCH0/R" << i << "\n";
	for (int i = 0; i < N; i++)
		outFile << "add wave -noupdate -group {STOCH} -radix binary -label P_Stoch" << i << " /TOP/STOCH0/P" << i << "\n";
	for (int i = 0; i < N; i++)
		outFile << "add wave -noupdate -group {STOCH_long} -radix unsigned -label P_D_Long" << i << " /TOP/S2S_ALPHA_" << i << "/D\n";
	for (int i = 0; i < N; i++)
		outFile << "add wave -noupdate -group {STOCH_long} -radix binary -label P_s_Long" << i << " /TOP/S2S_ALPHA_" << i << "/S\n";
	for (int i = 0; i < N; i++)
		outFile << "add wave -noupdate -group {STOCH_long} -radix unsigned -label P_Rand_Long" << i << " /TOP/S2S_ALPHA_" << i << "/LFSR\n";
}

void genNNGenericWave(sysConfig sConfig) {
	ofstream outFile;
	outFile.open(sConfig.Dir + "wave_NNGeneric.do");
	outFile << "onerror {resume}\n";
	outFile << "quietly WaveActivateNextPane  0\n";
	string PARAM_module = "PARAMS0";
	if (sConfig.optimizer == alternatingHardwareMask) PARAM_module = "PARAM_STORAGE0";
	outFile << "add wave -noupdate -color Gray65 /TOP/" << PARAM_module  << "/TrainingActive\n";

	if (!sConfig.usesTSParallelization()) {
		if (!sConfig.IsRecurrent()) {
			for (int l = 0; l <= sConfig.NNobj.OUTPUTLYR; l++) {
				if (sConfig.NNobj.LYRinfo[l].LT == FC) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
						outFile << "add wave -noupdate -expand -group Outputs -group { Layer " << l << " } -color Orange -radix unsigned /TOP/aD_L" << l << "_" << j << "\n";
				}
				if (sConfig.NNobj.LYRinfo[l].LT == CV) {
					for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
						for (int j = 0; j < sConfig.NNobj.LYRinfo[l].conv.filterSize; j++)
							outFile << "add wave -noupdate -expand -group Outputs -group { Layer " << l << " } -group { Filter " << f << " } -color Orange -radix unsigned /TOP/aD_L" << l << "_F" << f << "_" << j << "\n";
					}
				}
			}
		}
		else for (int t = 0; t < sConfig.maxTimeSteps; t++) {
			for (int l = 0; l <= sConfig.NNobj.OUTPUTLYR; l++) {
				if ((sConfig.NNobj.LYRinfo[l].LT == FC) | (sConfig.NNobj.LYRinfo[l].LT == RCC)) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
						outFile << "add wave -noupdate -expand -group Outputs -group { Layer " << l << " } -color Orange -radix unsigned /TOP/aD_L" << l << "_T" << t << "_" << j << "\n";
				}
				if (sConfig.NNobj.LYRinfo[l].LT == CV) {
					for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
						for (int j = 0; j < sConfig.NNobj.LYRinfo[l].conv.filterSize; j++)
							outFile << "add wave -noupdate -expand -group Outputs -group { Layer " << l << " } -group { Filter " << f << " } -color Orange -radix unsigned /TOP/aD_L" << l << "_F" << f << "_T" << t << "_" << j << "\n";
					}
				}
			}
		}
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++){
		if (!sConfig.IsRecurrent()) {
			for (int l = 0; l <= sConfig.NNobj.OUTPUTLYR; l++) {
				if (sConfig.NNobj.LYRinfo[l].LT == FC) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
						outFile << "add wave -noupdate -expand -group Outputs -group { Layer " << l << " netPar" << ts << " } -color Orange -radix unsigned /TOP/aD_L" << l << "_" << j << "_netPar" << ts << "\n";
				}
				if (sConfig.NNobj.LYRinfo[l].LT == CV) {
					for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
						for (int j = 0; j < sConfig.NNobj.LYRinfo[l].conv.filterSize; j++)
							outFile << "add wave -noupdate -expand -group Outputs -group { Layer " << l << " netPar" << ts << " } -group { Filter " << f << " } -color Orange -radix unsigned /TOP/aD_L" << l << "_F" << f << "_" << j << "_netPar" << ts << "\n";
					}
				}
			}
		}
		else for (int t = 0; t < sConfig.maxTimeSteps; t++) {
			for (int l = 0; l <= sConfig.NNobj.OUTPUTLYR; l++) {
				if ((sConfig.NNobj.LYRinfo[l].LT == FC) | (sConfig.NNobj.LYRinfo[l].LT == RCC)) {
					for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++)
						outFile << "add wave -noupdate -expand -group Outputs -group { Layer " << l << " netPar" << ts << " } -color Orange -radix unsigned /TOP/aD_L" << l << "_T" << t << "_" << j << "_netPar" << ts << "\n";
				}
				if (sConfig.NNobj.LYRinfo[l].LT == CV) {
					for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
						for (int j = 0; j < sConfig.NNobj.LYRinfo[l].conv.filterSize; j++)
							outFile << "add wave -noupdate -expand -group Outputs -group { Layer " << l << " netPar" << ts << " } -group { Filter " << f << " } -color Orange -radix unsigned /TOP/aD_L" << l << "_F" << f << "_T" << t << "_" << j << "_netPar" << ts << "\n";
					}
				}
			}
		}
		}
	}
	if (!sConfig.usesTSParallelization()) {
		if (!sConfig.IsRecurrent()) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++)
				outFile << "add wave -noupdate -expand -group Outputs -group { Training Output } -color Blue -radix unsigned /TOP/YD" << j << "\n";
		}
		else for (int t = 0; t < sConfig.maxTimeSteps; t++) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++)
				outFile << "add wave -noupdate -expand -group Outputs -group { Training Output } -color Blue -radix unsigned /TOP/YD_T" << t << "_" << j << "\n";
		}
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent()) {
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++)
					outFile << "add wave -noupdate -expand -group Outputs -group { Training Output netPar " << ts << " } -color Blue -radix unsigned /TOP/YD" << j << "_netPar" << ts << "\n";
			}
			else for (int t = 0; t < sConfig.maxTimeSteps; t++) {
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR]; j++)
					outFile << "add wave -noupdate -expand -group Outputs -group { Training Output " << ts << " } -color Blue -radix unsigned /TOP/YD_T" << t << "_" << j << "_netPar" << ts << "\n";
			}
		}
	}
	// Parameters
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		if (sConfig.NNobj.LYRinfo[l].LT == FC | sConfig.NNobj.LYRinfo[l].LT == RCC) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l - 1]; k++) {
					outFile << "add wave -noupdate -expand -group Params -group { AlphaD L" << l << " } -color Cyan -radix unsigned /TOP/" << PARAM_module  << "/alphaD_L" << l << "_" << k << "_" << j << "\n";
				}
			}
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
				outFile << "add wave -noupdate -expand -group Params -group { AlphaD L" << l << " } -color Gold /TOP/" << PARAM_module  << "/SIGN_alpha_L" << l << "_j_" << j << "\n";
			}
			outFile << "add wave -noupdate -expand -group Params -group { AlphaD L" << l << " } -color White -radix unsigned /TOP/" << PARAM_module  << "/RESISTANCE_alpha_L" << l << "\n";

			if (sConfig.NNobj.LYRinfo[l].LT == RCC) {
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					for (int k = 0; k < sConfig.NNobj.LYRSIZE[l]; k++) {
						outFile << "add wave -noupdate -expand -group Params -group { GammaD L" << l << " } -color Cyan -radix unsigned /TOP/" << PARAM_module  << "/gammaD_L" << l << "_" << k << "_" << j << "\n";
					}
				}
				for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
					outFile << "add wave -noupdate -expand -group Params -group { GammaD L" << l << " } -color Gold /TOP/" << PARAM_module  << "/SIGN_gamma_L" << l << "_j_" << j << "\n";
				}
				outFile << "add wave -noupdate -expand -group Params -group { GammaD L" << l << " } -color White -radix unsigned /TOP/" << PARAM_module  << "/RESISTANCE_gamma_L" << l << "\n";
			}

			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
				outFile << "add wave -noupdate -expand -group Params -group { BetaD L" << l << " } -color Orange -radix unsigned /TOP/" << PARAM_module  << "/betaD_L" << l << "_" << j << "\n";
			}
			outFile << "add wave -noupdate -expand -group Params -group { BetaD L" << l << " } -color Yellow /TOP/" << PARAM_module  << "/SIGN_beta_L" << l << "\n";
			outFile << "add wave -noupdate -expand -group Params -group { BetaD L" << l << " } -color White -radix unsigned /TOP/" << PARAM_module  << "/RESISTANCE_beta_L" << l << "\n";
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
				for (int j = 0; j < sConfig.NNobj.LYRinfo[l-1].size; j++) {
					if (ConnectionExists(sConfig.NNobj, l, j, sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f)))
						outFile << "add wave -noupdate -expand -group Params -group { AlphaD L" << l << " } -group {Filter " << f << "} -color Cyan -radix unsigned /TOP/" << PARAM_module  << "/alphaD_L" << l << "_" << j << "_" << sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f) << "\n";
				}
			}

			for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
				outFile << "add wave -noupdate -expand -group Params -group { AlphaD L" << l << " } -group {Filter " << f << "} -color Gold /TOP/" << PARAM_module  << "/SIGN_alpha_L" << l << "_j_" << sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f) << "\n";
			}
			
			outFile << "add wave -noupdate -expand -group Params -group { AlphaD L" << l << " }  -color White -radix unsigned /TOP/" << PARAM_module  << "/RESISTANCE_alpha_L" << l << "\n";

			for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
				outFile << "add wave -noupdate -expand -group Params -group { BetaD L" << l << " } -color Orange -radix unsigned /TOP/" << PARAM_module  << "/betaD_L" << l << "_" << f << "\n";
			}
			outFile << "add wave -noupdate -expand -group Params -group { BetaD L" << l << " } -color Yellow /TOP/" << PARAM_module  << "/SIGN_beta_L" << l << "\n";
			outFile << "add wave -noupdate -expand -group Params -group { BetaD L" << l << " } -color White -radix unsigned /TOP/" << PARAM_module  << "/RESISTANCE_beta_L" << l << "\n";
		}
		else if ((sConfig.NNobj.LYRinfo[l].LT == CV)&(sConfig.NNobj.LYRinfo[l - 1].LT == CV)) {
			// *********************************** NEED TO IMPLEMENT
			cout << "ERROR: INVALID LAYER CONFIGURATION (genSource)\n";

		}

	}

	//outFile << "add wave -noupdate -format Analog-Step -height 74 -max 15029.0 -radix unsigned /TOP/ERROR\n";
	outFile << "add wave -noupdate -format Analog-Step -height 74 -max 100.0 -radix unsigned /TOP/ERROR_MOVAVG\n";
	if (!sConfig.usesTSParallelization()) {
		if (!sConfig.IsRecurrent())
			outFile << "add wave -noupdate /TOP/eps\n";
		else for (int t = 0; t < sConfig.maxTimeSteps; t++)
			outFile << "add wave -noupdate /TOP/eps_T" << t << "\n";
	}
	else {
		for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) {
			if (!sConfig.IsRecurrent())
				outFile << "add wave -noupdate /TOP/eps_netPar" << ts << "\n";
			else for (int t = 0; t < sConfig.maxTimeSteps; t++)
				outFile << "add wave -noupdate /TOP/eps_T" << t << "_netPar" << ts << "\n";
		}
	}

	outFile << "TreeUpdate [SetDefaultTree]\n";
	outFile << "quietly WaveActivateNextPane\n";
	if (!sConfig.usesTSParallelization())
		outFile << "add wave -noupdate -color White -radix unsigned /TOP/TRAINING_SET\n";
	else for (int ts = 0; ts < sConfig.TrainingSetParallelization; ts++) 
		outFile << "add wave -noupdate -color White -radix unsigned /TOP/TRAINING_SET_netPar" << ts << "\n";
	outFile << "add wave -noupdate -color { Cornflower Blue } /TOP/CLK_TRAINING_flag\n";
	outFile << "TreeUpdate [SetDefaultTree]\n";
}

void genNNEval(sysConfig sConfig) {
	ofstream outFile;
	outFile.open(sConfig.Dir + "evaluate.do");

	outFile << "force -freeze sim:/TOP/";
	if (sConfig.optimizer == HW) outFile << "PARAMS0";
	else if (sConfig.optimizer == alternatingHardwareMask) outFile << "PARAM_STORAGE0";
	outFile << "/TrainingActive 0 0\n";
	for (int n = 0; n < sConfig.NTrainingSets; n++) {
		outFile << "force -freeze sim:/TOP/TRAINING_SET " << Dec2Bin(n) << " 0\n";
		outFile << "run 4ms\n\n";
	}

}

void genParameterExport(sysConfig sConfig) {
	ofstream outFile;
	outFile.open(sConfig.Dir + "saveParams.do");
	string PARAM_module = "PARAMS0";
	if (sConfig.optimizer == alternatingHardwareMask) PARAM_module = "PARAM_STORAGE0";
	outFile << "add list \\\n";
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		if ((sConfig.NNobj.LYRinfo[l].LT == FC)&(sConfig.NNobj.LYRinfo[l - 1].LT == FC)) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l - 1]; k++) {
					outFile << "sim:/TOP/" << PARAM_module  << "/alphaD_L" << l << "_" << k << "_" << j << " \\\n";
				}
			}
		}
		else if ((sConfig.NNobj.LYRinfo[l].LT == CV)&(sConfig.NNobj.LYRinfo[l - 1].LT == FC)) {
			for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
				for (int j = 0; j < sConfig.NNobj.LYRinfo[l - 1].size; j++) {
					if (ConnectionExists(sConfig.NNobj, l, j, sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f)))
						outFile << "sim:/TOP/" << PARAM_module  << "/alphaD_L" << l << "_" << j << "_" << sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f) << " \\\n";
				}
			}
		}
		else if ((sConfig.NNobj.LYRinfo[l].LT == FC)&(sConfig.NNobj.LYRinfo[l - 1].LT == CV)) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
				for (int k = 0; k < sConfig.NNobj.LYRSIZE[l - 1]; k++) {
					outFile << "sim:/TOP/" << PARAM_module  << "/alphaD_L" << l << "_" << k << "_" << j << " \\\n";
				}
			}
		}
	}
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		if ((sConfig.NNobj.LYRinfo[l].LT == FC)&(sConfig.NNobj.LYRinfo[l - 1].LT == FC)) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
				outFile << "sim:/TOP/" << PARAM_module  << "/SIGN_alpha_L" << l << "_j_" << j << " \\\n";
			}
		}
		else if ((sConfig.NNobj.LYRinfo[l].LT == CV)&(sConfig.NNobj.LYRinfo[l - 1].LT == FC)) {
			for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
				outFile << "sim:/TOP/" << PARAM_module  << "/SIGN_alpha_L" << l << "_j_" << sConfig.NNobj.LYRinfo[l].conv.FilterStartNode(f) << " \\\n";
			}
		}
		else if ((sConfig.NNobj.LYRinfo[l].LT == FC)&(sConfig.NNobj.LYRinfo[l - 1].LT == CV)) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
				outFile << "sim:/TOP/" << PARAM_module  << "/SIGN_alpha_L" << l << "_j_" << j << " \\\n";
			}
		}
	}
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		if (sConfig.NNobj.LYRinfo[l].LT == FC) {
			for (int j = 0; j < sConfig.NNobj.LYRSIZE[l]; j++) {
				outFile << "sim:/TOP/" << PARAM_module  << "/betaD_L" << l << "_" << j << " \\\n";
			}
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
				outFile << "sim:/TOP/" << PARAM_module  << "/betaD_L" << l << "_" << f << " \\\n";
			}
		}
	}
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		outFile << "sim:/TOP/" << PARAM_module  << "/SIGN_beta_L" << l << " \\\n";
	}
	outFile << "\n";
	outFile << "write list list.lst\n";
	outFile.close();
	ofstream outFile2;
	outFile2.open(sConfig.Dir + "Model_size.txt");
	for (int l = 0; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		if (sConfig.NNobj.LYRinfo[l].LT == FC)
			outFile2 << "F" << sConfig.NNobj.LYRSIZE[l] << " ";
		else if (sConfig.NNobj.LYRinfo[l].LT == CV)
			outFile2 << "C" << sConfig.NNobj.LYRSIZE[l] << "_" << sConfig.NNobj.LYRinfo[l].conv.filterSize << " ";
	}
	outFile2.close();

}


void genMATLABConfigFile(sysConfig sConfig, int NparameterExports) {

	ofstream outFile;
	outFile.open(sConfig.Dir + "Network.config");

	for (int l = 0; l < sConfig.NNobj.OUTPUTLYR + 1; l++) {
		if (sConfig.NNobj.LYRinfo[l].LT == FC)
		{
			outFile << "F" << sConfig.NNobj.LYRSIZE[l] << "\n";
			
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == RCC)
		{
			outFile << "R" << sConfig.NNobj.LYRSIZE[l] << "\n";
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == CV)
		{
			outFile << "C" << sConfig.NNobj.LYRinfo[l].size << "_" << sConfig.NNobj.LYRinfo[l].conv.filterSize << "_" << sConfig.NNobj.LYRinfo[l].conv.stride << "_" << sConfig.NNobj.LYRinfo[l].conv.Nconnections << "\n";
		}
	}
	outFile << "***\n";
	outFile << sConfig.NTrainingSets << "\n";
	outFile << NparameterExports << "\n";

	outFile.close();

}

void genNiosConfigFile(sysConfig sConfig, int NparameterExports) {

	ofstream outFile;
	outFile.open(sConfig.Dir + "NiosNNConfig.h");


	outFile << "/* Nios Neural Network definitions for " << sConfig.Dir << " */\n\n";

	int NparameterSigns = 0;

	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR + 1; l++) {
		if ((sConfig.NNobj.LYRinfo[l].LT == FC)&(sConfig.NNobj.LYRinfo[l - 1].LT == FC))
			NparameterSigns += sConfig.NNobj.LYRSIZE[l];
		else if ((sConfig.NNobj.LYRinfo[l].LT == FC)&(sConfig.NNobj.LYRinfo[l - 1].LT == CV))
			NparameterSigns += sConfig.NNobj.LYRSIZE[l] * sConfig.NNobj.LYRSIZE[l - 1];
		else if ((sConfig.NNobj.LYRinfo[l].LT == CV)&(sConfig.NNobj.LYRinfo[l - 1].LT == FC))
			NparameterSigns += sConfig.NNobj.LYRSIZE[l];

		NparameterSigns += 1;
	}

	outFile << "#define NLayers " << sConfig.NNobj.N_HIDDENLYR + 2 << "\n";
	outFile << "#define TotalNParameters " << sConfig.NNobj.N_TOTALCONNECTIONS + NparameterSigns << "\n";
	outFile << "#define TotalNParamExports " << NparameterExports << "\n";
	outFile << "#define NTrainingSets " << sConfig.NTrainingSets << "\n";
	outFile << "#define TrainingSetSizeInputs " << sConfig.NTrainingSets*sConfig.NNobj.LYRSIZE[0] << "\n";
	outFile << "#define TrainingSetSizeOutputs " << sConfig.NTrainingSets*sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << "\n";

	outFile << "\n";

	for (int l = 0; l < sConfig.NNobj.OUTPUTLYR + 1; l++) {
		if (sConfig.NNobj.LYRinfo[l].LT == FC)
			outFile << "#define LayerType_" << l << "_FC  " << sConfig.NNobj.LYRSIZE[l] << "\n";
		else if (sConfig.NNobj.LYRinfo[l].LT == CV)
			outFile << "#define LayerType_" << l << "_CV " << sConfig.NNobj.LYRSIZE[l] << "\n";
	}

	
	outFile.close();

}