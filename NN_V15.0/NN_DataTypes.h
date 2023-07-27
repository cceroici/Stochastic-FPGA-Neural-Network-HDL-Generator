#ifndef MKNNDATTYPE
#define MKNNDATTYPE

enum TSType { HW, uBlazeRotation };
enum ActivationFunctionType { BurstGate, ReLU };
enum CostFunctionGradient { MeanSquared, BurstDifference };
enum LayerType { FC, RCC, LSTM, CV };
enum NetworkTimeDependence { Stationary, Recurrent };
enum TransitionType { FC_FC, CV_FC, FC_CV, CV_CV };
enum OptimizerType {fullHardware, alternatingHardwareMask};

#include "Verilog.h"
#include "BitOps.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

struct ConvolutionalLayer {
	int stride; // horizontal/vertical
	int filterWidth;
	int filterHeight;
	int filterSize;
	int filters;
	int Nconnections; // Number of connections per node
	int FilterStartNode(int filterNumber) {
		if (filterNumber > filters)
		{
			cout << "WARNING: 'FilterStartNode' input filterNumber greater than number of filters." << endl;
			return 0;
		}
		return filterNumber * filterSize;
	}
	int FilterEndNode(int filterNumber) {
		if (filterNumber > filters)
		{
			cout << "WARNING: 'FilterStartNode' input filterNumber greater than number of filters." << endl;
			return 0;
		}
		return (filterNumber + 1) * filterSize - 1;
	}
};


struct LYR { // layer
	LayerType LT;
	int layerNumber;
	int size; // Number of nodes (FC)
	int** connections;
	bool** connectionMask;
	ConvolutionalLayer conv;

	int GetFilterConnectionIndex(int previousLayerSize, int nodeNumber,  int connectionNumber) {
		if (LT == CV) {
			if (nodeNumber >= conv.filterSize) {
				int filterNodeNumber = nodeNumber % conv.filterSize;
				nodeNumber = filterNodeNumber;
			}
			for (int i = 0; i < conv.Nconnections; i++) {
				//cout << connections[i][connectionNumber] << " ";
				if (connections[nodeNumber][i] == connectionNumber) {
					return i;
				}
			}
			return -99;
		}
		else {
			return connectionNumber;
		}
	}


};

struct ParameterMaskMapping {
	int layer;
	int node;
	int node_prev;

	int mapping;
	int set;
};

struct OptimizerParameterMasking {
	int maskSize;
	int Nmasks;

	ParameterMaskMapping *alpha_mask = new ParameterMaskMapping[10000];
	ParameterMaskMapping *beta_mask = new ParameterMaskMapping[10000];

	int excessParams = 0;

	int Nbeta = 0;
	int Nalpha = 0;
	int Nparam = 0;

	void mask_init(int outputLayer, LYR layers[], int msksize) {
		Nbeta = 0;
		Nalpha = 0;
		Nparam = 0;
		maskSize = msksize;

		int alpha_no = 0;
		int beta_no = 0;
		for (int l = 1; l <= outputLayer; l++) {
			if (layers[l].LT == FC) {
				for (int i = 0; i < layers[l - 1].size; i++) {
					for (int j = 0; j < layers[l].size; j++) {
						alpha_mask[alpha_no].layer = l;
						alpha_mask[alpha_no].node = j;
						alpha_mask[alpha_no].node_prev = i;
						alpha_no++;
					}
				}
				for (int j = 0; j < layers[l].size; j++) {
					beta_mask[beta_no].layer = l;
					beta_mask[beta_no].node = j;
					beta_no++;
				}
				Nbeta += layers[l].size;
				Nalpha += layers[l].size * layers[l - 1].size;
			}
		}

		Nparam = Nalpha + Nbeta;
	};

	void generate_mask() {

		Nmasks = ceil((double)Nparam / (double)maskSize);
		excessParams = Nmasks * maskSize - Nparam;

		int* alpha_assigned = new int[Nalpha];
		int* beta_assigned = new int[Nbeta];
		for (int i = 0; i < Nalpha; i++)	alpha_assigned[i] = 0;
		for (int i = 0; i < Nbeta; i++)		beta_assigned[i] = 0;

		vector<int> shuffledMask;
		for (int i = 0; i < Nparam; i++) shuffledMask.push_back(i);
		random_shuffle(shuffledMask.begin(), shuffledMask.end());

		int mask_no = 0;
		int mask_index = 0;
		int param_counter = 0;
		for (std::vector<int>::iterator it = shuffledMask.begin(); it != shuffledMask.end(); ++it)
		{
			if (*it >= Nalpha) {
				beta_mask[*it - Nalpha].mapping = mask_index;
				beta_mask[*it - Nalpha].set = mask_no;
			}
			else {
				alpha_mask[*it].mapping = mask_index;
				alpha_mask[*it].set = mask_no;
			}
			param_counter++;
			mask_index++;
			if (mask_index >= maskSize) {
				mask_no++;
				mask_index = 0;
			}
		}

		ofstream outFile;
		outFile.open("parameterMaskDebug.txt");
		for (int n = 0; n < Nmasks; n++) {
			outFile << "\nset " << n << ": ";
			for (int m = 0; m < maskSize; m++) {
				for (int i = 0; i < Nalpha; i++) {
					if ((alpha_mask[i].set == n) && (alpha_mask[i].mapping == m)) {
						outFile << m << "-alpha" << i << " ";
						break;
					}
				}
				for (int i = 0; i < Nbeta; i++) {
					if ((beta_mask[i].set == n) && (beta_mask[i].mapping == m)) {
						outFile << m << "-beta" << i << " ";
						break;
					}
				}
			}
			outFile << endl;
		}
		outFile << endl;
		for (int n = 0; n < Nmasks; n++) {
			outFile << "\nn " << n << ": ";
			for (int m = 0; m < maskSize; m++) {
				for (int i = 0; i < Nalpha; i++) {
					if ((alpha_mask[i].set == n) && (alpha_mask[i].mapping == m)) {
						outFile << m << "-alpha_L" << alpha_mask[i].layer << "_" << alpha_mask[i].node_prev << "_" << alpha_mask[i].node << " ";
						break;
					}
				}
				for (int i = 0; i < Nbeta; i++) {
					if ((beta_mask[i].set == n) && (beta_mask[i].mapping == m)) {
						outFile << m << "-beta_L" << beta_mask[i].layer << "_" << beta_mask[i].node << " ";
						break;
					}
				}
			}
			outFile << endl;
		}
		outFile << endl;
		outFile.close();
	}
	
};

struct NN {
	int N_HIDDENLYR;
	int LYRSIZE[10];
	int OUTPUTLYR;
	int N_TOTALNODES;
	LYR LYRinfo[10];
	int N_TOTALCONNECTIONS;
	CostFunctionGradient CostFunction;
	ActivationFunctionType ActivationFunction;
	NetworkTimeDependence TimeDependence;
};

struct RandN {

	int short_d2s;
	int short_zeroCenter;
	int short_ADDSUB_FP;
	int short_ADDSUB_BP;
	int short_ADDSUB_BP_CV;
	int short_ADDSUB_BPTT;
	int short_ADDSUB_error;
	int short_ADDSUB_NETPAR;
    int long_d2s;
	int short_total;
	int long_total;
	int Seeds;

	void calculateTotal_RNs() {
		short_total = short_d2s + short_ADDSUB_FP + short_ADDSUB_BP + short_ADDSUB_BP_CV + short_ADDSUB_BPTT + short_ADDSUB_error + short_ADDSUB_NETPAR;
		long_total = long_d2s;
		Seeds = ceil((double)short_total / 2) + long_total;
	}
};

struct sysConfig {
	string Dir;
	int NTrainingSets;
	int NTrainingSetsRAM;
	int maxNTrainingSets;
	int maxTimeSteps;
	int TrainingSetParallelization;
	TSType TSloading;
	NN NNobj;
	RandN RandNums;
	OptimizerType optimizer;
	OptimizerParameterMasking parameterMask;
	int DP_in;
	int DP_out;
	int LFSR_S;
	bool randomInitialization;
	int resistancePrecision;

	bool IsRecurrent() {
		if (NNobj.TimeDependence == Recurrent)
			return true;
		else
			return false;
	}

	bool usesTSParallelization() {
		if (TrainingSetParallelization > 1)
			return true;
		else
			return false;
	}

	TransitionType ConnectionType(int currentLayer) {
		if (currentLayer == 0)
			return FC_FC;
		else if (((NNobj.LYRinfo[currentLayer].LT == FC) | (NNobj.LYRinfo[currentLayer].LT == RCC)) && (NNobj.LYRinfo[currentLayer - 1].LT == FC) | (NNobj.LYRinfo[currentLayer - 1].LT == RCC))
			return FC_FC;
		else if (((NNobj.LYRinfo[currentLayer].LT == FC) | (NNobj.LYRinfo[currentLayer].LT == RCC)) && (NNobj.LYRinfo[currentLayer - 1].LT == CV))
			return CV_FC;
		else if (((NNobj.LYRinfo[currentLayer-1].LT == FC) | (NNobj.LYRinfo[currentLayer-1].LT == RCC)) && (NNobj.LYRinfo[currentLayer].LT == CV))
			return FC_CV;
		else if ((NNobj.LYRinfo[currentLayer].LT == CV) && (NNobj.LYRinfo[currentLayer - 1].LT == CV))
			return CV_CV;
		else {
			cout << "ERROR: INVALID CONNECTION TYPE\n";
			return FC_FC;
		}
	}

	bool IsConnectionType(int currentLayer, TransitionType queryType) {

		if (ConnectionType(currentLayer) == queryType)
			return true;
		else
			return false;
	}

};


// NN Utilities
void DeclareForwardPropagationNets(std::ostream &outFile, sysConfig sConfig, bool Simulation, enum DeclarationTypes type, int = -1);
void DeclareSingleLayerNet(std::ostream &outFile, sysConfig sConfig, string label, enum DeclarationTypes type, int = -1);
bool ** FullyConnectedMask(sysConfig sConfig, int layerN);
bool ConnectionExists(NN NNobj, int layerN, int from, int to);
bool AnyConnectionExists(NN NNobj, int layerN, int to);
bool NodeBelongsToFilter(NN NNobj, int layerN, int queriedNode, int queriedFilter);
void AddInputLayer(sysConfig& sConfig, int numberOfInputs, int numberOfChannels, int& layer_no);
int AddOutputLayer(sysConfig& sConfig, int numberOfOutputs, int& layer_no);
void AddFullyConnectedLayer(sysConfig& sConfig, int numberOfNodes, int& layer_no);
void AddConvolutionalLayer(sysConfig& sConfig, int numberOfFilters, int connectionsPerNode, int stride, int& layer_no, bool = false);
#endif