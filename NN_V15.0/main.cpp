

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

#include "NN_DataTypes.h"
#include "main.h"
#include "genTOP.h"
#include "genPARAMS.h"
#include "genSEED.h"
#include "genRNE.h"
#include "genSTOCH.h"
#include "genDECC.h"
#include "genFWDPROP.h"
#include "genBCKDPROP.h"
#include "genCOSTMOD.h"
#include "genTRAINING.h"
#include "genCONV.h"
#include "genFULLCONN.h"
#include "genSource.h"
#include "genUBlazeSim.h"
#include "genNETPAR_GRADIENT.h"
#include "BitOps.h"
#include "Verilog.h"


using namespace std;

void main(){

	/* ----------- PARAMETERS ----------------- */
	sysConfig sConfig; // System configuration

	sConfig.TSloading = HW; // Training set rotation method
	sConfig.NNobj.CostFunction = MeanSquared; // Backpropagation cost function type (MeanSquared or BurstDifference)
	sConfig.NNobj.ActivationFunction = BurstGate; // Fully-connected Hidden layer activation function type
	sConfig.NTrainingSets = 5; // Initial number of active training sets (TS)
	sConfig.NTrainingSetsRAM = 20; // Number of TS in uBlaze_sim
	sConfig.maxNTrainingSets = sConfig.NTrainingSetsRAM; // Determines TS RAM size
	sConfig.maxTimeSteps = 3; // For recurrent networks
	sConfig.TrainingSetParallelization = -1; // Number of simultaneous netwworks for training parallelization (set =-1 to disable)
	sConfig.randomInitialization = true; // Randomize initial weights
	sConfig.optimizer = fullHardware; // fullHardware for full simultaneous parameter training or alternatingHardwareMask
	int parameterMaskSize = 100;
	sConfig.resistancePrecision = 6;

	if (sConfig.TSloading == uBlazeRotation)
		sConfig.NTrainingSets = 2; // set NTrainingSets = 2 when using SW rotation (one active, one buffer)

	sConfig.DP_in = 8; // Input precision
	sConfig.DP_out = 16; // Output precision
	sConfig.LFSR_S = 16; // LFSR size

	int N_INPUTS = 4*4;
	int N_OUTPUTS = 5;

	int N_HIDDENLYR;
	int layerN = 0;

	AddInputLayer(sConfig, N_INPUTS, 1, layerN);
	AddConvolutionalLayer(sConfig, 6, 4 * 4, 2, layerN);
	//AddFullyConnectedLayer(sConfig, 25, layerN);
	AddFullyConnectedLayer(sConfig, 8, layerN);
	N_HIDDENLYR = AddOutputLayer(sConfig, N_OUTPUTS, layerN);

	if (sConfig.optimizer == alternatingHardwareMask) {
		sConfig.parameterMask.mask_init(1 + N_HIDDENLYR, sConfig.NNobj.LYRinfo, parameterMaskSize);
		sConfig.parameterMask.generate_mask();
	}

	GenerateFiles(sConfig, N_INPUTS, N_OUTPUTS, N_HIDDENLYR);

	cin.get();
}

void GenerateFiles(sysConfig sConfig, int N_INPUTS, int N_OUTPUTS, int N_HIDDENLYR) {
	int NparameterExports = 0;

	sConfig.NNobj.N_HIDDENLYR = N_HIDDENLYR;
	sConfig.NNobj.OUTPUTLYR = 1 + sConfig.NNobj.N_HIDDENLYR;
	sConfig.NNobj.LYRSIZE[0] = N_INPUTS;
	for (int l = 1; l < sConfig.NNobj.OUTPUTLYR; l++) {
		sConfig.NNobj.LYRSIZE[l] = sConfig.NNobj.LYRinfo[l].size;
	}
	sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] = N_OUTPUTS;

	sConfig.NNobj.N_TOTALNODES = 0;
	for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
		if (sConfig.NNobj.LYRinfo[l].LT == FC | sConfig.NNobj.LYRinfo[l].LT == RCC) {
			sConfig.NNobj.N_TOTALNODES += sConfig.NNobj.LYRSIZE[l];
			sConfig.NNobj.LYRinfo[l].connectionMask = FullyConnectedMask(sConfig, l);
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			sConfig.NNobj.N_TOTALNODES += sConfig.NNobj.LYRinfo[l].size;
			GenerateConvolutionalConnectionMasks(sConfig, sConfig.NNobj.LYRinfo[l], l);
		}
	}

	sConfig.NNobj.TimeDependence = Stationary;
	for (int l = 0; l < sConfig.NNobj.N_HIDDENLYR; l++) {
		if (sConfig.NNobj.LYRinfo[l + 1].LT == RCC)
			sConfig.NNobj.TimeDependence = Recurrent;
		if (sConfig.NNobj.LYRinfo[l + 1].LT == LSTM)
			sConfig.NNobj.TimeDependence = Recurrent;
	}

	int N_inputSignals = sConfig.NNobj.LYRSIZE[0];
	if (sConfig.IsRecurrent())
		N_inputSignals *= sConfig.maxTimeSteps;

	int N_TP = 0; // Number of transition parameters
	N_TP += N_inputSignals; // inputs
	for (int l = 1; l < (2 + sConfig.NNobj.N_HIDDENLYR); l++) {
		if (sConfig.NNobj.LYRinfo[l].LT == FC) { // alphas
			if ((sConfig.NNobj.LYRinfo[l - 1].LT == FC) | (sConfig.NNobj.LYRinfo[l - 1].LT == RCC))
				N_TP += sConfig.NNobj.LYRSIZE[l] * sConfig.NNobj.LYRSIZE[l - 1]; // alphas
			else if (sConfig.NNobj.LYRinfo[l - 1].LT == CV)
				N_TP += sConfig.NNobj.LYRSIZE[l] * sConfig.NNobj.LYRinfo[l - 1].conv.filters * sConfig.NNobj.LYRinfo[l - 1].conv.filterSize; // alphas
			N_TP += sConfig.NNobj.LYRSIZE[l]; // betas
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == RCC) {
			if (sConfig.NNobj.LYRinfo[l - 1].LT == FC)
				N_TP += sConfig.NNobj.LYRSIZE[l] * sConfig.NNobj.LYRSIZE[l - 1]; // alphas
			else if (sConfig.NNobj.LYRinfo[l - 1].LT == CV)
				N_TP += 0; // alphas //INCOMPLETE
			N_TP += sConfig.NNobj.LYRSIZE[l] * sConfig.NNobj.LYRSIZE[l]; // gammas
			N_TP += sConfig.NNobj.LYRSIZE[l]; // betas
		}
		else if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			N_TP += sConfig.NNobj.LYRinfo[l].conv.filters * sConfig.NNobj.LYRinfo[l].conv.Nconnections; // alphas
			N_TP += sConfig.NNobj.LYRinfo[l].conv.filters;  // betas
		}
	}
	sConfig.NNobj.N_TOTALCONNECTIONS = N_TP - N_inputSignals;

	sConfig.RandNums.short_d2s = N_TP;
	sConfig.RandNums.long_d2s = sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR];
	if (sConfig.IsRecurrent()) sConfig.RandNums.long_d2s *= sConfig.maxTimeSteps;
	if (sConfig.usesTSParallelization()) {
		sConfig.RandNums.short_d2s += (sConfig.TrainingSetParallelization - 1)*sConfig.NNobj.LYRSIZE[0];
		sConfig.RandNums.long_d2s += (sConfig.TrainingSetParallelization - 1)*sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR];
	}


	int N_RN_ADDSUB_bits_ADDSUB_error = 1; // COSTMOD ADDSUB
	int N_RN_ADDSUB_bits_ADDSUB_FP = 0;
	int N_RN_ADDSUB_bits_ADDSUB_BP = 0;
	int N_RN_ADDSUB_bits_ADDSUB_BP_CV = 0;
	int N_RN_ADDSUB_bits_ADDSUB_BPTT = 0;
	int N_RN_ADDSUB_bits_zeroCenter = 0;

	for (int l = 1; l < sConfig.NNobj.N_HIDDENLYR + 2; l++) { // Forward propagation ADDSUB
		if ((sConfig.NNobj.LYRinfo[l].LT == FC) | (sConfig.NNobj.LYRinfo[l].LT == RCC))
			N_RN_ADDSUB_bits_ADDSUB_FP += sConfig.LFSR_S / 2;
		else if (sConfig.NNobj.LYRinfo[l].LT == CV)
			N_RN_ADDSUB_bits_ADDSUB_FP += sConfig.LFSR_S / 2;
	}
	for (int l = 1; l < sConfig.NNobj.N_HIDDENLYR + 2; l++) { // Forward propagation 0 centering (d)
		if ((sConfig.NNobj.LYRinfo[l].LT == FC) | (sConfig.NNobj.LYRinfo[l].LT == RCC))
			N_RN_ADDSUB_bits_zeroCenter += (sConfig.LFSR_S / 2);
		else if (sConfig.NNobj.LYRinfo[l].LT == CV)
			N_RN_ADDSUB_bits_zeroCenter += sConfig.LFSR_S / 2;
		if (sConfig.usesTSParallelization())
			N_RN_ADDSUB_bits_zeroCenter *= sConfig.TrainingSetParallelization;
	}
	for (int l = 1; l < sConfig.NNobj.N_HIDDENLYR + 2; l++) { // Back propagation ADDSUB
		N_RN_ADDSUB_bits_ADDSUB_BP += sConfig.LFSR_S / 2;
		if (sConfig.NNobj.LYRinfo[l].LT == CV) // Additional bit for parameter gradient averaging
			N_RN_ADDSUB_bits_ADDSUB_BP_CV += sConfig.LFSR_S / 2;
		if (sConfig.IsRecurrent()) // Additional bit for parameter recurrent (time) averaging
			N_RN_ADDSUB_bits_ADDSUB_BPTT += RN_ADDSUB_BPTT_BITS_PER_LAYER;
	}

	int N_RN_ADDSUB_bits_NETPAR = 0;
	if (sConfig.usesTSParallelization()) {
		for (int l = 1; l <= sConfig.NNobj.OUTPUTLYR; l++) {
			if (sConfig.NNobj.LYRinfo[l].LT == FC) N_RN_ADDSUB_bits_NETPAR += sConfig.NNobj.LYRSIZE[l] * sConfig.NNobj.LYRSIZE[l - 1] + sConfig.NNobj.LYRSIZE[l];
			else if (sConfig.NNobj.LYRinfo[l].LT == RCC) N_RN_ADDSUB_bits_NETPAR += sConfig.NNobj.LYRSIZE[l] * sConfig.NNobj.LYRSIZE[l - 1] + sConfig.NNobj.LYRSIZE[l] * sConfig.NNobj.LYRSIZE[l] + sConfig.NNobj.LYRSIZE[l];
		}
		N_RN_ADDSUB_bits_ADDSUB_FP *= sConfig.TrainingSetParallelization;
		N_RN_ADDSUB_bits_ADDSUB_BP *= sConfig.TrainingSetParallelization;
		N_RN_ADDSUB_bits_ADDSUB_BP_CV *= sConfig.TrainingSetParallelization;
		N_RN_ADDSUB_bits_ADDSUB_BPTT *= sConfig.TrainingSetParallelization;
		N_RN_ADDSUB_bits_ADDSUB_error *= sConfig.TrainingSetParallelization*sConfig.DP_in;

	}



	sConfig.RandNums.short_ADDSUB_error = ceil((float)N_RN_ADDSUB_bits_ADDSUB_error / (sConfig.LFSR_S / 2));
	sConfig.RandNums.short_ADDSUB_FP = ceil((float)N_RN_ADDSUB_bits_ADDSUB_FP / (sConfig.LFSR_S / 2));
	sConfig.RandNums.short_zeroCenter = ceil((float)N_RN_ADDSUB_bits_zeroCenter / (sConfig.LFSR_S / 2));
	sConfig.RandNums.short_ADDSUB_BP = ceil((float)N_RN_ADDSUB_bits_ADDSUB_BP / (sConfig.LFSR_S / 2));
	sConfig.RandNums.short_ADDSUB_BP_CV = ceil((float)N_RN_ADDSUB_bits_ADDSUB_BP_CV / (sConfig.LFSR_S / 2));
	sConfig.RandNums.short_ADDSUB_BPTT = ceil((float)N_RN_ADDSUB_bits_ADDSUB_BPTT / (sConfig.LFSR_S / 2));
	sConfig.RandNums.short_ADDSUB_NETPAR = ceil((float)N_RN_ADDSUB_bits_NETPAR / (sConfig.LFSR_S / 2));

	if (sConfig.IsRecurrent()) {
		sConfig.RandNums.short_zeroCenter *= sConfig.maxTimeSteps;
		sConfig.RandNums.short_ADDSUB_FP *= sConfig.maxTimeSteps;
	}
	sConfig.RandNums.short_d2s += sConfig.RandNums.short_zeroCenter;
	sConfig.RandNums.calculateTotal_RNs();

	cout << "Generating HDL for " << N_HIDDENLYR << " hidden state(s)..." << endl;

	sConfig.Dir = ""; // find proper directory path

	char cd[1000];
	_getcwd(cd, 1000);
	sConfig.Dir = cd;
	int Dloc;
	Dloc = sConfig.Dir.find("\\Verilog");
	sConfig.Dir = sConfig.Dir.substr(0, Dloc);

	stringstream ss;
	for (int l = 0; l < sConfig.NNobj.OUTPUTLYR; l++) {
		if (sConfig.NNobj.LYRinfo[l].LT == FC)
			ss << "F" << sConfig.NNobj.LYRSIZE[l] << "_";
		else if (sConfig.NNobj.LYRinfo[l].LT == RCC)
			ss << "RC" << sConfig.NNobj.LYRSIZE[l] << "_";
		else if (sConfig.NNobj.LYRinfo[l].LT == LSTM)
			ss << "LSTM" << sConfig.NNobj.LYRSIZE[l] << "_";
		else if (sConfig.NNobj.LYRinfo[l].LT == CV)
			ss << "C" << sConfig.NNobj.LYRinfo[l].conv.filters << "." << sConfig.NNobj.LYRinfo[l].conv.Nconnections << "." << sConfig.NNobj.LYRinfo[l].conv.stride << "_";
	}

	ss << "F" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR];

	string TSloadType = "HW";
	if (sConfig.TSloading == uBlazeRotation)
		TSloadType = "uBlazeRot";

	string optimizerType = "fullHWparameter";
	if (sConfig.optimizer == alternatingHardwareMask)
		optimizerType = "alternatingParameterMask";

	//Dir = Dir + "Verilog\\Generated Files\\C" + NCs + "_" + ts;
	sConfig.Dir = sConfig.Dir + "\\Verilog\\CodeGeneration\\Generated Files\\NN_v" + VERSION + "_" + ss.str() + "_TSetsRAM." + Int2Str(sConfig.NTrainingSetsRAM) + "_" + TSloadType + "_" + optimizerType;
	if (sConfig.IsRecurrent()) sConfig.Dir += "_Tmax" + Int2Str(sConfig.maxTimeSteps);
	if (sConfig.usesTSParallelization()) sConfig.Dir += "_TSpar" + Int2Str(sConfig.TrainingSetParallelization);
	_mkdir(sConfig.Dir.c_str());
	sConfig.Dir = sConfig.Dir + "\\";
	cout << "Generating files in: " + sConfig.Dir << endl;
	cout << "Generating top level module...\n";

	genTOP(sConfig);
	genTOP_uBlaze(sConfig);
	//genTOP_Quartus(sConfig, QuartusProjectName);
	cout << "Generating Random Number Engine...\n";
	genRNE(sConfig);
	cout << "Generating Seed module...\n";
	genSEED(sConfig);
	cout << "Generating Stochastic Converters...\n";
	genSTOCH(sConfig);
	genDECC(sConfig);
	cout << "Generating Training Set Manager...\n";
	genTRAINING(sConfig);
	if (sConfig.TSloading == uBlazeRotation)
		genUBlazeSim(sConfig);
	cout << "Generating Cost Function Evaluator and Parameter module...\n";
	genCOSTMOD(sConfig);
	if (sConfig.usesTSParallelization()) genNETPAR_GRADIENT(sConfig);
	if (sConfig.optimizer == fullHardware) genPARAMS_sim(sConfig, NparameterExports);
	else if (sConfig.optimizer == alternatingHardwareMask) {
		genPARAM_MASK_STORAGE(sConfig);
		genPARAM_TRAINING_MASK(sConfig);
	}
	for (int l = (sConfig.NNobj.OUTPUTLYR); l > 0; l--) {
		if ((sConfig.NNobj.LYRinfo[l].LT == FC) | (sConfig.NNobj.LYRinfo[l].LT == RCC)) {
			genFULLCONN_FWD_block(sConfig, l);
			if (l < sConfig.NNobj.OUTPUTLYR)
				genFULLCONN_BP_block(sConfig, l);
		}
		if (sConfig.NNobj.LYRinfo[l].LT == CV) {
			genCONV_FWD_block(sConfig, sConfig.NNobj.LYRinfo[l]);
			genCONVBLOCK_BP_block(sConfig, sConfig.NNobj.LYRinfo[l], sConfig.NNobj.LYRinfo[l].connections);
		}
	}
	cout << "Generating Forward/Backward network interleavers...\n";
	genFWDPROP(sConfig);
	genBCKDPROP(sConfig);
	cout << "Generating source files...\n";
	genSource(sConfig);
	genParameterExport(sConfig);
	genNNGenericWave(sConfig);
	genNNEval(sConfig);
	genMATLABConfigFile(sConfig, NparameterExports);
	genNiosConfigFile(sConfig, NparameterExports);
	cout << "File generation complete.\n";
}