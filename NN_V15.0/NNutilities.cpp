/* Neural Network scripting utilities */


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


void DeclareForwardPropagationNets(std::ostream &outFile, sysConfig sConfig, bool Simulation, enum DeclarationTypes type, int TSpar) {
	if (type == ModuleDeclarationPortsOutput) {
		if (TSpar == -1) {
			NN_DeclareGenericNet(outFile, sConfig, 0, 0, "a", wire, ModuleDeclarationPortsInput, singleLayer, sConfig.IsRecurrent(), -1);
			NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a", wire, type, singleLayer, sConfig.IsRecurrent(), -1);
			NN_DeclareGenericNet(outFile, sConfig, 0, sConfig.NNobj.OUTPUTLYR, "z", wire, type, singleLayer, sConfig.IsRecurrent(), -1);
			NN_DeclareGenericNet(outFile, sConfig, 0, sConfig.NNobj.OUTPUTLYR, "zp", wire, type, singleLayer, sConfig.IsRecurrent(), -1);
		}
		else {
			NN_DeclareGenericNet(outFile, sConfig, 0, 0, "a", wire, ModuleDeclarationPortsInput, singleLayer, sConfig.IsRecurrent(), TSpar);
			NN_DeclareGenericNet(outFile, sConfig, 1, sConfig.NNobj.OUTPUTLYR, "a", wire, type, singleLayer, sConfig.IsRecurrent(), TSpar);
			NN_DeclareGenericNet(outFile, sConfig, 0, sConfig.NNobj.OUTPUTLYR, "z", wire, type, singleLayer, sConfig.IsRecurrent(), TSpar);
			NN_DeclareGenericNet(outFile, sConfig, 0, sConfig.NNobj.OUTPUTLYR, "zp", wire, type, singleLayer, sConfig.IsRecurrent(), TSpar);
		}
	}
	else {
		if (TSpar == -1) {
			NN_DeclareGenericNet(outFile, sConfig, 0, sConfig.NNobj.OUTPUTLYR, "a", wire, type, singleLayer, sConfig.IsRecurrent(), -1);
			NN_DeclareGenericNet(outFile, sConfig, 0, sConfig.NNobj.OUTPUTLYR, "z", wire, type, singleLayer, sConfig.IsRecurrent(), -1);
			NN_DeclareGenericNet(outFile, sConfig, 0, sConfig.NNobj.OUTPUTLYR, "zp", wire, type, singleLayer, sConfig.IsRecurrent(), -1);
		}
		else {
			NN_DeclareGenericNet(outFile, sConfig, 0, sConfig.NNobj.OUTPUTLYR, "a", wire, type, singleLayer, sConfig.IsRecurrent(), TSpar);
			NN_DeclareGenericNet(outFile, sConfig, 0, sConfig.NNobj.OUTPUTLYR, "z", wire, type, singleLayer, sConfig.IsRecurrent(), TSpar);
			NN_DeclareGenericNet(outFile, sConfig, 0, sConfig.NNobj.OUTPUTLYR, "zp", wire, type, singleLayer, sConfig.IsRecurrent(), TSpar);
		}
	}
}

void DeclareSingleLayerNet(std::ostream &outFile, sysConfig sConfig, string label, enum DeclarationTypes type, int TSpar) {
	int negone = -1;
	string TSparLabel = "";
	if (TSpar >= 0)
		TSparLabel = "_netPar" + Int2Str(TSpar);

	if (type == TopLevel) {
		if (!sConfig.IsRecurrent())
			outFile << "wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << " - 1:0] " << label << TSparLabel <<";\n";
		else
			outFile << "wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << " - 1:0] " << StringSeries(sConfig.maxTimeSteps, label +  "_T", TSparLabel) << ";\n";
	}
	else if (type == ModuleInstantiationPorts) {
		if (!sConfig.IsRecurrent())
			outFile << "." + label + "(" + label + TSparLabel + "),\n\t";
		else
			outFile << StringSeriesPort(sConfig.maxTimeSteps, label + "_T" + TSparLabel, "", label + "_T", TSparLabel, negone) << "\n\t";
	}
	else if (type == ModuleDeclarationPortList) {
		if (!sConfig.IsRecurrent())
			outFile << label << ",\n\t";
		else
			outFile << StringSeries(sConfig.maxTimeSteps, label + "_T", TSparLabel, negone) << ",\n\t";
	}
	else if (type == ModuleDeclarationPortsInput) {
		if (!sConfig.IsRecurrent())
			outFile << "input wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << "-1:0] " << label << ";\n";
		else
			outFile << "input wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << " - 1:0] " << StringSeries(sConfig.maxTimeSteps, label + "_T", TSparLabel) << ";\n";
	}
	else if (type == ModuleDeclarationPortsOutput) {
		if (!sConfig.IsRecurrent())
			outFile << "output wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << "-1:0] " << label << ";\n";
		else
			outFile << "output wire [" << sConfig.NNobj.LYRSIZE[sConfig.NNobj.OUTPUTLYR] << " - 1:0] " << StringSeries(sConfig.maxTimeSteps, label + "_T", TSparLabel) << ";\n";
	}
	
}

bool ** FullyConnectedMask(sysConfig sConfig, int layerN) {
	bool** connectionMask = new bool*[sConfig.NNobj.LYRSIZE[layerN]];
	for (int i = 0; i < sConfig.NNobj.LYRSIZE[layerN]; i++)
		connectionMask[i] = new bool[sConfig.NNobj.LYRSIZE[layerN - 1]];
	for (int i = 0; i < sConfig.NNobj.LYRSIZE[layerN]; i++)
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[layerN - 1]; j++)
			connectionMask[i][j] = true;
	return connectionMask;
}


bool ConnectionExists(NN NNobj, int layerN, int from, int to) {
	if (NNobj.LYRinfo[layerN].LT == CV) {
		int filterNodeNumber = to % NNobj.LYRinfo[layerN].conv.filterSize;
		if (filterNodeNumber > 0) return false;
		if (NNobj.LYRinfo[layerN].connectionMask[filterNodeNumber][from])
			return true;
		else
			return false;
	}
	else
		if (NNobj.LYRinfo[layerN].connectionMask[to][from])
			return true;
		else
			return false;
	return false;
}

bool AnyConnectionExists(NN NNobj, int layerN, int to) {
	if (NNobj.LYRinfo[layerN].LT == CV) {
		int filterNodeNumber = to % NNobj.LYRinfo[layerN].conv.filterSize;
		if (filterNodeNumber > 0) return false;
		for (int i = 0; i < NNobj.LYRSIZE[layerN - 1]; i++) {
			if (NNobj.LYRinfo[layerN].connectionMask[filterNodeNumber][i])
				return true;
		}
		return false;
	}
	else{
		for (int i = 0; i < NNobj.LYRSIZE[layerN - 1]; i++) {
			if (NNobj.LYRinfo[layerN].connectionMask[to][i])
				return true;
		}
		return false;
	}
	
}

bool NodeBelongsToFilter(NN NNobj, int layerN, int queriedNode, int queriedFilter) {
	if (NNobj.LYRinfo[layerN].LT != CV){
		cout << "WARNING: 'BelongsToFilter' called on non convolutional layer " << layerN << endl;
		return false;
	}
	else {
		int filterNodeStart = queriedFilter * NNobj.LYRinfo[layerN].conv.filterSize;
		int filterNodeEnd = (queriedFilter+1) * NNobj.LYRinfo[layerN].conv.filterSize - 1;
		if (queriedNode < filterNodeStart) return false;
		if (queriedNode > filterNodeEnd) return false;
		return true;
	}
}

void AddInputLayer(sysConfig& sConfig, int numberOfInputs, int numberOfChannels, int& layer_no) {
	sConfig.NNobj.LYRinfo[0].LT = FC;
	sConfig.NNobj.LYRinfo[0].size = numberOfInputs;
	sConfig.NNobj.LYRinfo[0].layerNumber = 0;
	layer_no = 0;
}

int AddOutputLayer(sysConfig& sConfig, int numberOfOutputs, int& layer_no) {
	++layer_no;
	sConfig.NNobj.LYRinfo[layer_no].LT = FC;
	sConfig.NNobj.LYRinfo[layer_no].size = numberOfOutputs;
	sConfig.NNobj.LYRinfo[layer_no].layerNumber = 0;
	return layer_no - 1;
}

void AddFullyConnectedLayer(sysConfig& sConfig, int numberOfNodes, int& layer_no) {
	++layer_no;
	sConfig.NNobj.LYRinfo[layer_no].LT = FC;
	sConfig.NNobj.LYRinfo[layer_no].size = numberOfNodes;
	sConfig.NNobj.LYRinfo[layer_no].layerNumber = layer_no;

}

void AddConvolutionalLayer(sysConfig& sConfig, int numberOfFilters, int connectionsPerNode, int stride, int& layer_no, bool overrideStrideCheck) {
	++layer_no;
	sConfig.NNobj.LYRinfo[layer_no].LT = CV;
	sConfig.NNobj.LYRinfo[layer_no].conv.filters = numberOfFilters;
	sConfig.NNobj.LYRinfo[layer_no].layerNumber = layer_no;
	sConfig.NNobj.LYRinfo[layer_no].conv.Nconnections =	connectionsPerNode;
	sConfig.NNobj.LYRinfo[layer_no].conv.stride = stride;
	// Steps: number nodes in filter required to cover input grid with a given stride
	int Steps_flt = ((float)sqrt(sConfig.NNobj.LYRinfo[layer_no-1].size) - (float)sqrt(sConfig.NNobj.LYRinfo[layer_no].conv.Nconnections)) / (float)sConfig.NNobj.LYRinfo[layer_no].conv.stride + 1;
	int Steps = (int)Steps_flt;
	if (!overrideStrideCheck) {
		if (Steps_flt != Steps) {
			cout << "ERROR: Filter step size inconsistent!\n";
			int errorInput;
			cin >> errorInput;
		}
	}
	sConfig.NNobj.LYRinfo[layer_no].conv.filterHeight = Steps;
	sConfig.NNobj.LYRinfo[layer_no].conv.filterWidth = Steps;
	sConfig.NNobj.LYRinfo[layer_no].conv.filterSize = sConfig.NNobj.LYRinfo[layer_no].conv.filterHeight*sConfig.NNobj.LYRinfo[layer_no].conv.filterWidth; // Number of nodes in filter
	sConfig.NNobj.LYRinfo[layer_no].size = sConfig.NNobj.LYRinfo[layer_no].conv.filters*sConfig.NNobj.LYRinfo[layer_no].conv.filterSize;
}

