/* Verlog Generation Tools */

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

using namespace std;

string Int2Str(int num);

string StringSeries(int N, string preLabel, string postLabel);
// Generate a string of indexed nets

string StringSeriesBound(int startBound, int endBound, string preLabel, string postLabel, bool reverse);
string StringSeriesBoundPort(int startBound, int endBound, string prePortLabel, string postPortLabel, string preLabel, string postLabel, bool reverse);
// Generate a string from a start bound to end bound

string StringSeries2(int N1, int N2, string Label1, string Label2, string Label3, string nLine, int nLineT, int COM);
// Generate a string of two-dimensional indexed nets 
// nLine -> String output for every new line
// nLineT -> wire string before or after start line (0 = before, 1 = after)
// COM -> include comma on last line (1 = yes, 0 = no)

string StringSeries3(int N1, int N2, int N3, string Label1, string Label2, string Label3, string Label4, string nLine, int nLineT, int COM);
// Generate a string of three-dimensional indexed nets 
// nLine -> String output for every new line
// nLineT -> wire string before or after start line (0 = before, 1 = after)
// COM -> include comma on last line (1 = yes, 0 = no)

string StringSeriesPort(int N, string Pname1, string Pname2, string preLabel, string postLabel, int& counter);
// same as StringSeries but includes a port name: .Portname_i_j(Label_i_j)
// counter: -1 uses provided index for port name. Otherwise, if counter>0 is used port names will increment with a single index counter
string StringSeriesPort2(int N1, int N2, string Pname1, string Pname2, string Pname3, string Label1, string Label2, string Label3, string nLine, int nLineT, int& counter);
// same as StringSeries2 but includes a port name: .Portname_i_j(Label_i_j)
// counter: -1 uses provided index for port name. Otherwise, if counter>0 is used port names will increment with a single index counter

string StringSeriesPort3(int N1, int N2, int N3, string Pname1, string Pname2, string Pname3, string Pname4, string Label1, string Label2, string Label3, string Label4, string nLine, int nLineT, int& counter);
// same as StringSeries3 but includes a port name: .Portname_i_j(Label_i_j)
// counter: -1 uses provided index for port name. Otherwise, if counter>0 is used port names will increment with a single index counter


void DeclareNet(std::ostream& outFile, string type, string size, int N, string preLabel, string postLabel);
// Generate a declaration for a wire or register.
// outFile -> destination file stream
// type -> "reg" or "wire"
// size -> size of wire array
// N -> number of wires
// preLabel/postLabel -> strings before and after index of wire name

void DeclareNetEqual(std::ostream& outFile, string type, string size, int N, string preLabel, string postLabel, string Equal);
// Generate a declaration for a wire or register.
// outFile -> destination file stream
// type -> "reg" or "wire"
// size -> size of wire array
// N -> number of wires
// preLabel/postLabel -> strings before and after index of wire name
// Equal -> equality of array elements (e.g. 5'd13)

void DeclareNetArray(std::ostream& outFile, string type, string size, int NN, int NArr, string Label0, string Label1, string Label2);
// Generate declarations for a series of wire or register arrays.
// outFile -> destination file stream
// type -> "reg" or "wire"
// size -> size of wire array
// NN -> number of wires per array
// NArr -> number of arrays of wires
// Label0,Label1,Label2 -> string label around index numbers for wire name

string Int2Str(int num) {

	stringstream ss;
	ss << num;
	return ss.str();

}

string StringSeries(int N, string preLabel, string postLabel, bool reverse) {

	stringstream ss;
	if (N == 0)
		return "";
	else {
		if (!reverse) {
			for (int i = 0; i < N - 1; i++)
				ss << preLabel << i << postLabel << ", ";
			ss << preLabel << N - 1 << postLabel;
		}
		else {
			for (int i = N - 1; i > 0; i--)
				ss << preLabel << i << postLabel << ", ";
			ss << preLabel << 0 << postLabel;
		}
		return ss.str();
	}
}

string StringSeriesBound(int startBound, int endBound, string preLabel, string postLabel, bool reverse) {

	stringstream ss;

	if (!reverse) {
		for (int i = startBound; i < endBound - 1; i++)
			ss << preLabel << i << postLabel << ", ";
		ss << preLabel << endBound - 1 << postLabel;
	}
	else {
		for (int i = endBound - 1; i > startBound; i--)
			ss << preLabel << i << postLabel << ", ";
		ss << preLabel << startBound << postLabel;
	}
	return ss.str();
}
string StringSeriesBoundPort(int startBound, int endBound, string prePortLabel, string postPortLabel, string preLabel, string postLabel, bool reverse) {

	stringstream ss;
	int portNumber = 0;
	if (!reverse) {
		for (int i = startBound; i < endBound - 1; i++)
			ss << "." << prePortLabel << portNumber++ << postPortLabel << "(" << preLabel << i << postLabel << "), ";
		ss << "." << prePortLabel << portNumber << postPortLabel << "(" << preLabel << endBound - 1 << postLabel << "), ";
	}
	else {
		portNumber = endBound - startBound-1;
		for (int i = endBound - 1; i > startBound; i--)
			ss << "." << prePortLabel << portNumber-- << postPortLabel << "(" << preLabel << i << postLabel << "), ";
		ss << "." << prePortLabel << portNumber << postPortLabel << "(" << preLabel << startBound << postLabel << "), ";
	}
	return ss.str();
}
string StringSeries2(int N1, int N2, string Label1, string Label2, string Label3, string nLine, int nLineT, int COM) {

	stringstream ss;

	for (int i = 0; i < N1; i++){
		if (!nLineT)
			ss << nLine;
		for (int j = 0; j < N2; j++){
			if (!COM&&j == (N2 - 1))
				ss << Label1 << i << Label2 << j << Label3 << "; ";
			else
				ss << Label1 << i << Label2 << j << Label3 << ", ";
		}
		if (nLineT)
			ss << nLine;
	}


	return ss.str();
}
string StringSeries3(int N1, int N2, int N3, string Label1, string Label2, string Label3, string Label4, string nLine, int nLineT, int COM){

	stringstream ss;

	for (int i = 0; i < N1; i++){
		if (!nLineT)
			ss << nLine;
		for (int j = 0; j < N2; j++){
			for (int k = 0; k < N3; k++){
				if (!COM&&j == (N2 - 1) && k == (N3 - 1))
					ss << Label1 << i << Label2 << j << Label3 << k << Label4 << "; ";
				else 
					ss << Label1 << i << Label2 << j << Label3 << k << Label4 << ", ";
			}
		}

		if (nLineT)
			ss << nLine;
	}

	return ss.str();
}

string StringSeriesPort(int N, string Pname1, string Pname2, string preLabel, string postLabel, int& counter) {

	if (N <= 0)
		return "";

	stringstream ss;

	for (int i = 0; i < N - 1; i++){
		if (counter<0)
			ss << "." << Pname1 << i << Pname2 << "(" << preLabel << i << postLabel << "), ";
		else
			ss<< "." << Pname1 << counter++ << Pname2 << "(" << preLabel << i << postLabel << "), ";

	}
	if (counter<0)
		ss << "." << Pname1 << N-1 << Pname2 << "(" << preLabel << N-1 << postLabel << "), ";
	else
		ss << "." << Pname1 << counter++ << Pname2 << "(" << preLabel << N - 1 << postLabel << ")";

	return ss.str();
}

string StringSeriesPort2(int N1, int N2, string Pname1, string Pname2, string Pname3, string Label1, string Label2, string Label3, string nLine, int nLineT, int& counter) {

	stringstream ss;

	for (int i = 0; i < N1; i++){
		if (!nLineT)
			ss << nLine;
		for (int j = 0; j < N2; j++){
			if (counter<0)
				ss << "." << Pname1 << i << Pname2 << j << Pname3 << "(" << Label1 << i << Label2 << j << Label3 << "), ";
			else
				ss << "." << Pname1 << counter++ << Pname2 << "(" << Label1 << i << Label2 << j << Label3 << "), ";

		}
		if (nLineT)
			ss << nLine;
	}


	return ss.str();
}

string StringSeriesPort3(int N1, int N2, int N3, string Pname1, string Pname2, string Pname3, string Pname4, string Label1, string Label2, string Label3, string Label4, string nLine, int nLineT, int& counter){

	stringstream ss;

	for (int i = 0; i < N1; i++){
		if (!nLineT)
			ss << nLine;
		for (int j = 0; j < N2; j++){
			for (int k = 0; k < N3; k++){
				if (counter<0)
					ss << "." << Pname1 << i << Pname2 << j << Pname3 << k << Pname4 << "(" << Label1 << i << Label2 << j << Label3 << k << Label4 << "), ";
				else
					ss << "." << Pname1 << counter++ << Pname2 << "(" << Label1 << i << Label2 << j << Label3 << k << Label4 << "), ";
			}
		}

		if (nLineT)
			ss << nLine;
	}

	return ss.str();
}


void DeclareNet(std::ostream& outFile, string type, string size, int N, string preLabel, string postLabel) {

	//if (type != "wire"&&type != "reg"){
	//	cout << "invalid net type in declaration.\n";
	//	cin.get();
	//}
	if (N != 0) {

		if (size == "1")
			outFile << type << " ";
		else
			outFile << type << " [" << size << " - 1:0] ";
		for (int i = 0; i < N - 1; i++)
			outFile << preLabel << i << postLabel << ", ";
		outFile << preLabel << N - 1 << postLabel << ";\n";
	}
}

void DeclareDistributionNets(std::ostream& outFile, string type, int size, int Nnets, int LFSR_size, string preLabel, string postLabel) {
	if (Nnets != 0) {

		int NfullNetBlocks = floor(Nnets / size);
		int partialNetBlockSize = Nnets - NfullNetBlocks;
		int totalSize = Nnets * size + (size - LFSR_size % size);
		int junkBits = LFSR_size/2 - partialNetBlockSize;

		int netIndex = 0;
		if (NfullNetBlocks > 0) {
			outFile << type << " [" << size << "-1:0] ";
			for (int i = 0; i < NfullNetBlocks - 1; i++)
				outFile << preLabel << netIndex++ << postLabel << ", ";
			outFile << preLabel << netIndex++ << postLabel << ";\n";
		}
		if (partialNetBlockSize > 0)
			outFile << type << " [" << partialNetBlockSize << "-1:0] " << preLabel << netIndex++ << postLabel << "; \n";
		if (junkBits >0)
			outFile << type << " [" << junkBits << "-1:0] " << preLabel << "_junk" << postLabel << ";\n";
	}

}

void DeclareNetEqual(std::ostream& outFile, string type, string size, int N, string preLabel, string postLabel, string Equal) {

	//if (type != "wire"&&type != "reg"){
	//	cout << "invalid net type in declaration.\n";
	//	cin.get();
	//}

	if (size == "1")
		outFile << type << " ";
	else
		outFile << type << " [" << size << " - 1:0] ";
	for (int i = 0; i < N - 1; i++)
		outFile << preLabel << i << postLabel << " = " << Equal <<", ";
	outFile << preLabel << N - 1 << postLabel << " = " << Equal << ";\n";

}

void DeclareNetArray(std::ostream& outFile, string type, string size, int NN, int NArr, string Label0, string Label1, string Label2){
	
	//if (type != "wire"&&type != "reg"){
	//	cout << "invalid net type in declaration.\n";
	//	cin.get();
	//}

	for (int i = 0; i < NArr; i++){
		if (size == "1")
			outFile << type << " ";
		else
			outFile << type << " [" << size << " - 1:0] ";
		for (int j = 0; j < NN - 1; j++)
			outFile << Label0 << j << Label1 << i << Label2 << ", ";
		outFile << Label0 << NN - 1 << Label1 << i << Label2 << ";\n";
	}
	outFile << "\n";
	
}

void DeclareNetArrayPreserve(std::ostream& outFile, string type, string size, int NN, int NArr, string Label0, string Label1, string Label2){

	//if (type != "wire"&&type != "reg"){
	//	cout << "invalid net type in declaration.\n";
	//	cin.get();
	//}

	for (int i = 0; i < NArr; i++){
		if (size == "1")
			outFile << type << " ";
		else
			outFile << type << " [" << size << " - 1:0] ";
		for (int j = 0; j < NN - 1; j++)
			outFile << Label0 << j << Label1 << i << Label2 << ", ";
		outFile << Label0 << NN - 1 << Label1 << i << Label2 << " /* synthesis keep */;\n";
	}
	outFile << "\n";

}

void NN_DeclareNodeNet(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, string type, string Label0, string Label1) {
	// e.g.: input wire [3-1:0] dbeta_L1;

	for (int l = startLYR; l <= endLYR; l++) {
		outFile << type << " [" << LYRSIZE[l] << "-1:0] " << Label0 << l << Label1 << ";\n";
	}
}

void NN_DeclareTransferNodeNet(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, string Label0, string Label1, string netType, bool single) {
	if (!single) {
		for (int l = startLYR; l <= endLYR; l++) {
			string temp = Label0 + Int2Str(l) + "_j_";
			DeclareNet(outFile, netType, Int2Str(LYRSIZE[l-1]), LYRSIZE[l], temp, Label1);
		}
	}
	else
	{
		for (int l = startLYR; l <= endLYR; l++) {
			outFile << netType << " [" << LYRSIZE[l] << " - 1:0] " << Label0 << Int2Str(l) << ";\n";
			//string temp = Label0 + Int2Str(l) + "__";
			//DeclareNet(outFile, "wire", Int2Str(LYRSIZE[l]), LYRSIZE[l - 1], temp, Label1);
		}
	}
}

string NN_StringSeriesPort3(int LYRSIZE[10], int startLayer, int endLayer, string Pname1, string Pname2, string Pname3, string Pname4, string Label1, string Label2, string Label3, string Label4, string nLine, int nLineT, int& counter, bool reverseIndex) {

	stringstream ss;

	for (int l = startLayer; l <= endLayer; l++) {
		if (!nLineT)
			ss << nLine;
		if (!reverseIndex) {
			for (int j = 0; j < LYRSIZE[l-1]; j++) {
				for (int k = 0; k < LYRSIZE[l]; k++) {
					if (counter < 0)
						ss << "." << Pname1 << l << Pname2 << j << Pname3 << k << Pname4 << "(" << Label1 << l << Label2 << j << Label3 << k << Label4 << "), ";
					else
						ss << "." << Pname1 << counter++ << Pname2 << "(" << Label1 << l << Label2 << j << Label3 << k << Label4 << "), ";
				}
			}
		}
		else {
			for (int k = 0; k < LYRSIZE[l-1]; k++) {
				for (int j = 0; j < LYRSIZE[l]; j++) {
					if (counter < 0)
						ss << "." << Pname1 << l << Pname2 << j << Pname3 << k << Pname4 << "(" << Label1 << l << Label2 << j << Label3 << k << Label4 << "), ";
					else
						ss << "." << Pname1 << counter++ << Pname2 << "(" << Label1 << l << Label2 << j << Label3 << k << Label4 << "), ";
				}
			}
		}

		if (nLineT)
			ss << nLine;
	}

	return ss.str();
}

string NN_StringSeriesPort2(int LYRSIZE[10], int startLayer, int endLayer, string Pname1, string Pname2, string Pname3, string Label1, string Label2, string Label3, string nLine, int nLineT, int& counter) {

	stringstream ss;

	for (int l = startLayer; l <= endLayer; l++) {
		if (!nLineT)
			ss << nLine;
		for (int j = 0; j < LYRSIZE[l]; j++) {
			if (counter<0)
				ss << "." << Pname1 << l << Pname2 << j << Pname3 << "(" << Label1 << l << Label2 << j << Label3 << "), ";
			else
				ss << "." << Pname1 << counter++ << Pname2 << "(" << Label1 << l << Label2 << j << Label3 << "), ";

		}
		if (nLineT)
			ss << nLine;
	}


	return ss.str();
}

string NN_StringSeriesPort(int LYRSIZE[10], int startLayer, int endLayer, string Pname1, string Pname2, string Label1, string Label2) {

	stringstream ss;

	for (int l = startLayer; l <= endLayer; l++) {

		ss << "." << Pname1 << l << Pname2 << "(" << Label1 << l << Label2 << "), ";

	}
	ss << "\n\t";

	return ss.str();
}

string NN_StringSeries2(int LYRSIZE[10], int startLayer, int endLayer, string Label1, string Label2, string Label3, string nLine, int nLineT) {

	stringstream ss;

	for (int l = startLayer; l <= endLayer; l++) {
		if (!nLineT)
			ss << nLine;
		for (int j = 0; j < LYRSIZE[l]; j++) {
			ss << Label1 << l << Label2 << j << Label3 << ", ";


		}
		if (nLineT)
			ss << nLine;
	}


	return ss.str();
}

string NN_CV_StringSeries2(int LYRSIZE[10], int startLayer, int endLayer, int FilterSize, string Label1, string Label2, string Label3, string nLine, int nLineT) {

	stringstream ss;
	for (int f = 0; f < FilterSize; f++) {
		for (int l = startLayer; l <= endLayer; l++) {
			if (!nLineT)
				ss << nLine;
			for (int j = 0; j < LYRSIZE[l]; j++)
				ss << Label1 << l << "_F" << f << Label2 << j << Label3 << ", ";
			if (nLineT)
				ss << nLine;
		}

	}


	return ss.str();
}

string NN_StringSeries(int LYRSIZE[10], int startLayer, int endLayer, string Label1, string Label2) {

	stringstream ss;

	for (int l = startLayer; l <= endLayer; l++) {

		ss << Label1 << l << Label2 << ", ";

	}
	ss << "\n\t";

	return ss.str();
}

string NN_StringSeries3(int LYRSIZE[10], int startLayer, int endLayer, string Label1, string Label2, string Label3, string Label4, string nLine, int nLineT) {

	stringstream ss;

	for (int l = startLayer; l <= endLayer; l++) {
		if (!nLineT)
			ss << nLine;
			for (int j = 0; j < LYRSIZE[l-1]; j++) {
				for (int k = 0; k < LYRSIZE[l]; k++) {
					ss << Label1 << l << Label2 << j << Label3 << k << Label4 << ", ";
				}
			}
		if (nLineT)
			ss << nLine;
	}

	return ss.str();
}

void NN_DeclareNodeNetArray(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, string type, string Label0, string Label1, string Label2) {
	// e.g.: input wire [3 - 1:0] dalpha_L1_j_0, dalpha_L1_j_1, dalpha_L1_j_2;


	for (int l = startLYR; l <= endLYR; l++) {
	
		outFile << type << " [" << LYRSIZE[l-1] << " - 1:0] ";
		for (int j = 0; j < LYRSIZE[l]-1; j++)
			outFile << Label0 << l << Label1 << j << Label2 << ", ";
		outFile << Label0 << l << Label1 << LYRSIZE[l]-1 << Label2 << ";\n";
	}
	outFile << "\n";

}

void NN_DeclareNodeNetArray_alpha(std::ostream& outFile, sysConfig sConfig, int LYRSIZE[10], int startLYR, int endLYR, string type, string nodeName, bool portDeclaration) {
	// e.g.: input wire [3 - 1:0] dalpha_L1_j_0, dalpha_L1_j_1, dalpha_L1_j_2;

	string termination;
	if (!portDeclaration)
		termination = ";\n";
	else
		termination = ",\n";

	for (int l = startLYR; l <= endLYR; l++) {
		if (sConfig.NNobj.LYRinfo[l].LT == LSTM) {

		}
		if (sConfig.ConnectionType(l) == FC_CV) { // From fully connected to convolutional
			for (int f = 0; f < sConfig.NNobj.LYRinfo[l].size; f++) {
					if (!portDeclaration)
						outFile << type << " [" << sConfig.NNobj.LYRinfo[l].conv.Nconnections << " - 1:0] ";
					else if ((f>0) | (l>startLYR))
						outFile << "\t";
					outFile << nodeName << "_L" << l << "_F" << f << termination;
			}
		}
		else if (sConfig.ConnectionType(l) == CV_FC) { // From convolutional to fully connected

			for (int f = 0; f < sConfig.NNobj.LYRinfo[l-1].conv.filters; f++) {
					if (!portDeclaration)
						outFile << type << " [" << sConfig.NNobj.LYRinfo[l-1].conv.filterSize << " - 1:0] ";
					else if ((f>0) | (l>startLYR))
						outFile << "\t";
					for (int j = 0; j < LYRSIZE[l] - 1; j++)
						outFile << nodeName << "_L" << l << "_F" << f << "_j_" << j << ", ";
					outFile << nodeName << "_L" << l << "_F" << f <<  "_j_" << LYRSIZE[l] - 1 << termination;
				}
				//outFile << "\n";
		}
		else if (sConfig.ConnectionType(l) == CV_CV) { // From convolutional to convolutional
			for (int f0 = 0; f0 < sConfig.NNobj.LYRinfo[l].conv.filters; f0++) {
				for (int f1 = 0; f1 < sConfig.NNobj.LYRinfo[l-1].conv.filters; f1++) {
					if (!portDeclaration)
						outFile << type << " [" << LYRSIZE[l - 1] << " - 1:0] ";
					else if ((f1>0)|(f0>0)|(l>startLYR))
						outFile << "\t";
					for (int j = 0; j < LYRSIZE[l] - 1; j++)
						outFile << nodeName << "_L" << l << "_F" << f0 << "_F" << f1 << "_j_" << j << ", ";
					outFile << nodeName << "_L" << l << "_F" << f0 << "_F" << f1 << "_j_" << LYRSIZE[l] - 1 << termination;
				}
			}
		}
		else if (sConfig.ConnectionType(l) == FC_FC) { // From fully connected to fully connected
			if (!portDeclaration)
				outFile << type << " [" << LYRSIZE[l - 1] << " - 1:0] ";
			else if (l>startLYR)
				outFile << "\t";
			for (int j = 0; j < LYRSIZE[l] - 1; j++)
				outFile << nodeName << "_L" << l << "_j_" << j << ", ";
			outFile << nodeName << "_L" << l << "_j_" << LYRSIZE[l] - 1 << termination;
		}	
	}
	if (!portDeclaration)
		outFile << "\n";
	//else
	//	outFile << "\t";
}


void NN_DeclareNodeNetArray_generic(std::ostream& outFile, sysConfig sConfig, int LYRSIZE[10], int startLYR, int endLYR, string type, string nodeName, bool portDeclaration, bool portInst) {
	// e.g.: input wire [3 - 1:0] dalpha_L1_j_0, dalpha_L1_j_1, dalpha_L1_j_2;

	string termination;
	if (!portDeclaration)
		termination = ";\n";
	else
		termination = ",\n";

	for (int l = startLYR; l <= endLYR; l++) {
		if (!sConfig.IsRecurrent()) {
			if (sConfig.NNobj.LYRinfo[l].LT == CV) { // Convolutional
				for (int f = 0; f < sConfig.NNobj.LYRinfo[l].conv.filters; f++) {
					if (!portDeclaration)
						outFile << type << " [" << sConfig.NNobj.LYRinfo[l].conv.filterSize << " - 1:0] ";
					else if ((f > 0) | (l > startLYR))
						outFile << "\t";
					if (portInst)
						outFile << "." << nodeName << "_L" << l << "_F" << f << "(" << nodeName << "_L" << l << "_F" << f << ")" << termination;
					else
						outFile << nodeName << "_L" << l << "_F" << f << termination;
				}
			}
			else if (sConfig.NNobj.LYRinfo[l].LT == FC) { // Fully connected

				if (!portDeclaration)
					outFile << type << " [" << LYRSIZE[l] << " - 1:0] ";
				else if (l > startLYR)
					outFile << "\t";
				if (portInst)
					outFile << "." << nodeName << "_L" << l << "(" << nodeName << "_L" << l << ")" << termination;
				else
					outFile << nodeName << "_L" << l << termination;
			}
		}
		else {
			if (sConfig.NNobj.LYRinfo[l].LT == CV) { // Convolutional
				// Not implemented
			}
			else if ((sConfig.NNobj.LYRinfo[l].LT == FC) | (sConfig.NNobj.LYRinfo[l].LT == RCC)) { // Fully connected
				if (!portDeclaration)
					outFile << type << " [" << LYRSIZE[l] << " - 1:0] ";
				else if ((l > startLYR))
					outFile << "\t";
				for (int t = 0; t < sConfig.maxTimeSteps; t++) {

					if (portInst)
						outFile << "." << nodeName << "_L" << l << "_T" << t << "(" << nodeName << "_L" << l << "_T" << t << ")" << ", ";
					else {
						if (t==sConfig.maxTimeSteps-1)
							outFile << nodeName << "_L" << l << "_T" << t << termination;
						else
							outFile << nodeName << "_L" << l << "_T" << t << ", ";

					}

				}

			}

		}
	}
	if (!portDeclaration)
		outFile << "\n";
	else
		outFile << "\t";
}

void NN_DeclareNodeNetArray_genericD(std::ostream& outFile, sysConfig sConfig, int LYRSIZE[10], int startLYR, int endLYR, string size, string type, string nodeName, bool portDeclaration, bool portInst) {
	// e.g.: input wire [3 - 1:0] dalpha_L1_j_0, dalpha_L1_j_1, dalpha_L1_j_2;

	string termination;
	if (!portDeclaration)
		termination = ";\n";
	else
		termination = ",\n";

	if (!sConfig.IsRecurrent()) {
		for (int l = startLYR; l <= endLYR; l++) {
			if (sConfig.NNobj.LYRinfo[l].LT == CV) { // Convolutional
				for (int f = 0; f < sConfig.NNobj.LYRinfo[l].size; f++) {
					if (!portDeclaration)
						outFile << type << " [" << size << " - 1:0] ";
					else if ((f > 0) | (l > startLYR))
						outFile << "\t";
					for (int j = 0; j < sConfig.NNobj.LYRinfo[l].conv.filterSize - 1; j++) {
						if (portInst)
							outFile << "." << nodeName << "_L" << l << "_F" << f << "_" << j << "(" << nodeName << "_L" << l << "_F" << f << "_" << j << ")" << ", ";
						else
							outFile << nodeName << "_L" << l << "_F" << f << "_" << j << ", ";
					}
					if (portInst)
						outFile << "." << nodeName << "_L" << l << "_F" << f << "_" << sConfig.NNobj.LYRinfo[l].conv.filterSize - 1 << "(" << nodeName << "_L" << l << "_F" << f << "_" << sConfig.NNobj.LYRinfo[l].conv.filterSize - 1 << ")" << termination;
					else
						outFile << nodeName << "_L" << l << "_F" << f << "_" << sConfig.NNobj.LYRinfo[l].conv.filterSize - 1 << termination;
				}
			}
			else if (sConfig.NNobj.LYRinfo[l].LT == FC) { // Fully connected

				if (!portDeclaration)
					outFile << type << " [" << size << " - 1:0] ";
				else if (l > startLYR)
					outFile << "\t";
				for (int j = 0; j < LYRSIZE[l] - 1; j++) {
					if (portInst)
						outFile << "." << nodeName << "_L" << l << "_" << j << "(" << nodeName << "_L" << l << "_" << j << ")" << ", ";
					else
						outFile << nodeName << "_L" << l << "_" << j << ", ";
				}
				if (portInst)
					outFile << "." << nodeName << "_L" << l << "_" << LYRSIZE[l] - 1 << "(" << nodeName << "_L" << l << "_" << LYRSIZE[l] - 1 << ")" << termination;
				else
					outFile << nodeName << "_L" << l << "_" << LYRSIZE[l] - 1 << termination;
			}
		}
	}
	else {
		for (int l = startLYR; l <= endLYR; l++) {
			for (int t = 0; t < sConfig.maxTimeSteps; t++) {
				if (sConfig.NNobj.LYRinfo[l].LT == CV) { // Convolutional
					for (int f = 0; f < sConfig.NNobj.LYRinfo[l].size; f++) {
						// NOT IMPLEMENTED
					}
				}
				else if ((sConfig.NNobj.LYRinfo[l].LT == FC)| (sConfig.NNobj.LYRinfo[l].LT == RCC)) { // Fully connected
					if (!portDeclaration)
						outFile << type << " [" << size << " - 1:0] ";
					else if ((l > startLYR)|(t>0))
						outFile << "\t";
					for (int j = 0; j < LYRSIZE[l] - 1; j++) {
						if (portInst)
							outFile << "." << nodeName << "_L" << l << "_T" << t << "_" << j << "(" << nodeName << "_L" << l << "_T" << t << "_" << j << ")" << ", ";
						else
							outFile << nodeName << "_L" << l << "_T" << t << "_" << j << ", ";
					}
					if (portInst)
						outFile << "." << nodeName << "_L" << l << "_T" << t << "_" << LYRSIZE[l] - 1 << "(" << nodeName << "_L" << l << "_T" << t << "_" << LYRSIZE[l] - 1 << ")" << termination;
					else
						outFile << nodeName << "_L" << l << "_T" << t << "_" << LYRSIZE[l] - 1 << termination;
				}
			}
		}
	}
	if (!portDeclaration)
		outFile << "\n";
	else
		outFile << "\t";
}

void NN_CV_DeclareNodeNetArray(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, int FilterSize, string type, string Label0, string Label1, string Label2) {
	// e.g.: input wire [3 - 1:0] dalpha_L1_j_0, dalpha_L1_j_1, dalpha_L1_j_2;

	for (int f = 0; f < FilterSize; f++) {
		for (int l = startLYR; l <= endLYR; l++) {
			outFile << type << " [" << LYRSIZE[l - 1] << " - 1:0] ";
			for (int j = 0; j < LYRSIZE[l] - 1; j++)
				outFile << Label0 << l << "_F" << f << Label1 << j << Label2 << ", ";
			outFile << Label0 << l << Label1 << LYRSIZE[l] - 1 << Label2 << ";\n";
		}
		//outFile << "\n";
	}
}

void NN_DeclareNodeNetArray3(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, string type, string size, string Label0, string Label1, string Label2, string Label3, int DefaultValue) {

	// e.g.: input wire [DP_out - 1:0] dalphaD_L1_0_0, dalphaD_L1_0_1, dalphaD_L1_0_2, dalphaD_L1_1_0, dalphaD_L1_1_1, dalphaD_L1_1_2, dalphaD_L1_2_0, dalphaD_L1_2_1, dalphaD_L1_2_2;
	bool regInitialize = false;
	int declareSize = 0;
	if (type.find("reg") != std::string::npos) {
		regInitialize = true;
		declareSize = ceil(log2(DefaultValue + 1)) + 1;
	}

	if (!regInitialize) {
		for (int l = startLYR; l <= endLYR; l++) {

			outFile << type << " [" << size << " - 1:0] ";
			for (int j = 0; j < LYRSIZE[l-1] - 1; j++) {
				for (int k = 0; k < LYRSIZE[l]; k++)
					outFile << Label0 << l << Label1 << j << Label2 << k << Label3 << ", ";
			}
			for (int k = 0; k < LYRSIZE[l] - 1; k++)
				outFile << Label0 << l << Label1 << LYRSIZE[l - 1] - 1 << Label2 << k << Label3 << ", ";
			outFile << Label0 << l << Label1 << LYRSIZE[l - 1] - 1 << Label2 << LYRSIZE[l] - 1 << Label3 << ";\n";
		}
		outFile << "\n";
	}
	else {
		for (int l = startLYR; l <= endLYR; l++) {
			for (int j = 0; j < LYRSIZE[l-1]; j++) {
				for (int k = 0; k < LYRSIZE[l]; k++) {
					outFile << type << " [" << size << " - 1:0] ";
					outFile << Label0 << l << Label1 << j << Label2 << k << Label3 << " = " << declareSize << "'d" << DefaultValue << ";\n";
				}
			}
		}
		outFile << "\n";
	}

}

void NN_DeclareNodeNetArray2(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, string type, string size, string Label0, string Label1, string Label2, int DefaultValue) {

	// e.g.: input wire [DP_out - 1:0] dbetaD_L1_0, dbetaD_L1_1, dbetaD_L1_2;
	int declareSize = 0;
	bool regInitialize = false;
	if (type.find("reg") != std::string::npos) {
		regInitialize = true;
		declareSize = ceil(log2(DefaultValue + 1)) + 1;
	}

	if (!regInitialize) {
		for (int l = startLYR; l <= endLYR; l++) {

			outFile << type << " [" << size << " - 1:0] ";
			for (int j = 0; j < LYRSIZE[l] - 1; j++) {
				outFile << Label0 << l << Label1 << j << Label2 << ", ";
			}
			outFile << Label0 << l << Label1 << LYRSIZE[l] - 1 << Label2 << ";\n";
		}
		outFile << "\n";
	}
	else {
		for (int l = startLYR; l <= endLYR; l++) {

			for (int j = 0; j < LYRSIZE[l]; j++) {
				outFile << type << " [" << size << " - 1:0] ";
				outFile << Label0 << l << Label1 << j << Label2 << " = " << declareSize << "'d" << DefaultValue << ";\n";
			}
		}
		outFile << "\n";
	}
}

void NN_DeclareParameterMaskNets(std::ostream& outFile, sysConfig sConfig, DeclarationTypes declarationType) {
	string termination = ";\n";
	int endLineChars = 2;
	if ((declarationType == ModuleInstantiationPorts) | (declarationType == ModuleDeclarationPortList)) {
		termination = ",\n\t";
		endLineChars = 2;
	}
	bool isPortDeclaration = (declarationType == ModuleInstantiationPorts);
	string netSize = "";
	string netTypeText = "";
	if (declarationType == ModuleDeclarationPortsInput)
		netTypeText += "input ";
	else if (declarationType == ModuleDeclarationPortsOutput)
		netTypeText += "output ";
	
	stringstream outputString;

	// paramD
	if (declarationType == ModuleDeclarationPortsOutput)
		outputString << "output reg [" << sConfig.DP_in << "-1:0] ";
	else if (declarationType == ModuleDeclarationPortsInput)
		outputString << "input wire [" << sConfig.DP_in << "-1:0] ";
	else if (declarationType == TopLevel)
		outputString << "wire [" << sConfig.DP_in << "-1:0] ";
	for (int n = 0; n < sConfig.parameterMask.maskSize; n++) {
		if (declarationType == ModuleInstantiationPorts)
			outputString << ".paramD" << n << "(";
		outputString << "paramD" << n;
		if (declarationType == ModuleInstantiationPorts)
			outputString << "), ";
		else
			outputString << ", ";
	}
	outputString.seekp(-endLineChars, outputString.cur);
	outputString << termination;
	// SIGN_param
	if (declarationType == ModuleDeclarationPortsOutput)
		outputString << "output reg ";
	else if (declarationType == ModuleDeclarationPortsInput)
		outputString << "input wire ";
	else if (declarationType == TopLevel)
		outputString << "wire ";
	for (int n = 0; n < sConfig.parameterMask.maskSize; n++) {
		if (declarationType == ModuleInstantiationPorts)
			outputString << ".SIGN_param" << n << "(";
		outputString << "SIGN_param" << n;
		if (declarationType == ModuleInstantiationPorts)
			outputString << "), ";
		else
			outputString << ", ";
	}
	outputString.seekp(-endLineChars, outputString.cur);
	outputString << termination;
	// grad
	if (declarationType == ModuleDeclarationPortsOutput)
		outputString << "output wire ";
	else if (declarationType == ModuleDeclarationPortsInput)
		outputString << "input wire ";
	else if (declarationType == TopLevel)
		outputString << "wire ";
	for (int n = 0; n < sConfig.parameterMask.maskSize; n++) {
		if (declarationType == ModuleInstantiationPorts)
			outputString << ".grad" << n << "(";
		outputString << "grad" << n;
		if (declarationType == ModuleInstantiationPorts)
			outputString << "), ";
		else
			outputString << ", ";
	}
	outputString.seekp(-endLineChars, outputString.cur);
	outputString << termination;
	// SIGN_grad
	if (declarationType == ModuleDeclarationPortsOutput)
		outputString << "output wire ";
	else if (declarationType == ModuleDeclarationPortsInput)
		outputString << "input wire ";
	else if (declarationType == TopLevel)
		outputString << "wire ";
	for (int n = 0; n < sConfig.parameterMask.maskSize; n++) {
		if (declarationType == ModuleInstantiationPorts)
			outputString << ".SIGN_grad" << n << "(";
		outputString << "SIGN_grad" << n;
		if (declarationType == ModuleInstantiationPorts)
			outputString << "), ";
		else
			outputString << ", ";
	}
	outputString.seekp(-endLineChars, outputString.cur);
	outputString << termination;
	// gradD
	if (declarationType == ModuleDeclarationPortsOutput)
		outputString << "input wire [" << sConfig.DP_in << "-1:0] ";
	else if (declarationType == ModuleDeclarationPortsInput)
		outputString << "output wire [" << sConfig.DP_in << "-1:0] ";
	else if (declarationType == TopLevel)
		outputString << "wire [" << sConfig.DP_in << "-1:0] ";
	for (int n = 0; n < sConfig.parameterMask.maskSize; n++) {
		if (declarationType == ModuleInstantiationPorts)
			outputString << ".gradD" << n << "(";
		outputString << "gradD" << n;
		if (declarationType == ModuleInstantiationPorts)
			outputString << "), ";
		else
			outputString << ", ";
	}
	outputString.seekp(-endLineChars, outputString.cur);
	outputString << termination;
	// SIGN_gradD
	if (declarationType == ModuleDeclarationPortsOutput)
		outputString << "input wire ";
	else if (declarationType == ModuleDeclarationPortsInput)
		outputString << "output wire ";
	else if (declarationType == TopLevel)
		outputString << "wire ";
	for (int n = 0; n < sConfig.parameterMask.maskSize; n++) {
		if (declarationType == ModuleInstantiationPorts)
			outputString << ".SIGN_gradD" << n << "(";
		outputString << "SIGN_gradD" << n;
		if (declarationType == ModuleInstantiationPorts)
			outputString << "), ";
		else
			outputString << ", ";
	}
	outputString.seekp(-endLineChars, outputString.cur);
	outputString << termination;
	// Resistance init
	if (declarationType == ModuleDeclarationPortsOutput)
		outputString << "output reg [" << sConfig.resistancePrecision << "-1:0] ";
	else if (declarationType == ModuleDeclarationPortsInput)
		outputString << "input wire [" << sConfig.resistancePrecision << "-1:0] ";
	else if (declarationType == TopLevel)
		outputString << "wire [" << sConfig.resistancePrecision << "-1:0] ";
	for (int n = 0; n < sConfig.parameterMask.maskSize; n++) {
		if (declarationType == ModuleInstantiationPorts)
			outputString << ".resistance_INIT" << n << "(";
		outputString << "resistance_INIT" << n;
		if (declarationType == ModuleInstantiationPorts)
			outputString << "), ";
		else
			outputString << ", ";
	}
	outputString.seekp(-endLineChars, outputString.cur);
	outputString << termination;
	// Resistance new
	if (declarationType == ModuleDeclarationPortsOutput)
		outputString << "input wire [" << sConfig.resistancePrecision << "-1:0] ";
	else if (declarationType == ModuleDeclarationPortsInput)
		outputString << "output reg [" << sConfig.resistancePrecision << "-1:0] ";
	else if (declarationType == TopLevel)
		outputString << "wire [" << sConfig.resistancePrecision << "-1:0] ";
	for (int n = 0; n < sConfig.parameterMask.maskSize; n++) {
		if (declarationType == ModuleInstantiationPorts)
			outputString << ".resistance" << n << "(";
		outputString << "resistance" << n;
		if (declarationType == ModuleInstantiationPorts)
			outputString << "), ";
		else
			outputString << ", ";
	}
	outputString.seekp(-endLineChars, outputString.cur);
	outputString << termination;
	outFile << outputString.str();
}

void NN_DeclareWeightNets(std::ostream& outFile, sysConfig sConfig, int LYRSIZE[10], int startLYR, int endLYR, NetTypes netType, DeclarationTypes declarationType, bool gradients, bool polarity, string suffix, string label_suffix) {
	
	string termination;
	if ((declarationType == ModuleInstantiationPorts) | (declarationType == ModuleDeclarationPortList))
		termination = ",\n\t";
	else
		termination = ";\n";
	for (int l = startLYR; l <= endLYR; l++) {
		if (!gradients) {
			if (!polarity) {
				outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, alphaStoch, declarationType, previousLayerWeight, "DP_in", -1, label_suffix);
				if (sConfig.NNobj.LYRinfo[l].LT == RCC) outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, gammaStoch, declarationType, recurrentLayer, "DP_in", -1, label_suffix);
				outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, betaStoch, declarationType, singleLayer, "DP_in", -1, label_suffix);
			}
			else {
				outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, alphaSign, declarationType, previousLayerWeight, "DP_in", -1, label_suffix);
				if (sConfig.NNobj.LYRinfo[l].LT == RCC) outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, gammaSign, declarationType, recurrentLayer, "DP_in", -1, label_suffix);
				outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, betaSign, declarationType, singleLayer, "DP_in", -1, label_suffix);
			}
		}
		else {
			if (!polarity) {
				outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, alphaStochGrad, declarationType, previousLayerWeight, "DP_in", -1, label_suffix);
				if (sConfig.NNobj.LYRinfo[l].LT == RCC) outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, gammaStochGrad, declarationType, recurrentLayer, "DP_in", -1, label_suffix);
				outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, betaStochGrad, declarationType, singleLayer, "DP_in", -1, label_suffix);
			}
			else {
				outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, alphaStochGradSign, declarationType, previousLayerWeight, "DP_in", -1, label_suffix);
				if (sConfig.NNobj.LYRinfo[l].LT == RCC) outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, gammaStochGradSign, declarationType, recurrentLayer, "DP_in", -1, label_suffix);
				outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", suffix, betaStochGradSign, declarationType, singleLayer, "DP_in", -1, label_suffix);
			}
		}
	}
	
}

void NN_DeclareDecimalWeightNets(std::ostream& outFile, sysConfig sConfig, int LYRSIZE[10], int startLYR, int endLYR, NetTypes netType, DeclarationTypes declarationType) {

	string termination;
	if ((declarationType == ModuleInstantiationPorts) | (declarationType == ModuleDeclarationPortList))
		termination = ",\n\t";
	else
		termination = ";\n";

	for (int l = startLYR; l <= endLYR; l++) {
			outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", "", alphaDec, declarationType, previousLayerWeight, "DP_in", -1, "");
			if (sConfig.NNobj.LYRinfo[l].LT == RCC) outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", "", gammaDec, declarationType, recurrentLayer);
			outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, "", netType, "", "", betaDec, declarationType, singleLayer, "DP_in", -1, "");
	}
}

void NN_DeclareGenericNet(std::ostream& outFile, sysConfig sConfig, int startLYR, int endLYR, string netLabel, NetTypes netType, DeclarationTypes declarationType, ConnectionGeometry connectionGeometry, bool isRecurrent, int TSpar) {
	string termination;
	if ((declarationType == ModuleInstantiationPorts) | (declarationType == ModuleDeclarationPortList))
		termination = ",\n\t";
	else
		termination = ";\n";

	if (TSpar == -1) {
		for (int l = startLYR; l <= endLYR; l++) {
			if (!isRecurrent)
					outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, netLabel, netType, "", "", miscellaneous, declarationType, connectionGeometry, "DP_in", -1, "");
			else
				outFile << NN_DeclareRecurrentNetLine(l, sConfig.NNobj, 0, termination, netLabel, netType, "", "", miscellaneous, declarationType, connectionGeometry, "", sConfig.maxTimeSteps);
		}
	}
	else
	{
		for (int l = startLYR; l <= endLYR; l++) {
			if (!isRecurrent)
				outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, netLabel, netType, "", "", miscellaneous, declarationType, connectionGeometry, "", TSpar);
			else
				outFile << NN_DeclareRecurrentNetLine(l, sConfig.NNobj, 0, termination, netLabel, netType, "", "", miscellaneous, declarationType, connectionGeometry, "", sConfig.maxTimeSteps, TSpar);
		}
	}
}

void NN_DeclareGenericDecimalNet(std::ostream& outFile, sysConfig sConfig, int startLYR, int endLYR, string netLabel, NetTypes netType, string precision, DeclarationTypes declarationType, ConnectionGeometry connectionGeometry, bool isRecurrent) {
	string termination;
	
	if ((declarationType == ModuleInstantiationPorts) | (declarationType == ModuleDeclarationPortList))
		termination = ",\n\t";
	else
		termination = ";\n";

	for (int l = startLYR; l <= endLYR; l++) {
		if (!isRecurrent)
			outFile << NN_DeclareNetLine(l, sConfig.NNobj, 0, termination, netLabel, netType, "", "", miscellaneousDec, declarationType, connectionGeometry, precision);
		else
			outFile << NN_DeclareRecurrentNetLine(l, sConfig.NNobj, 0, termination, netLabel, netType, "", "", miscellaneousDec, declarationType, connectionGeometry, precision,sConfig.maxTimeSteps);
	}
}

string NN_DeclareNetLine(int layerN, NN NNobj, int FilterSize, string termination, string netLabel, NetTypes netType, string Label0, string suffix, ParameterTypes parameterType, DeclarationTypes declarationType, ConnectionGeometry connectionGeometry, string decimalPrecision, int TSpar, string label_suffix) {

	stringstream outputString;
	bool isStochastic = true;
	bool isRecurrent = false;
	bool isConvolutional = false;
	bool isPortDeclaration = (declarationType == ModuleInstantiationPorts);

	GetNetLineConditions(netLabel, isStochastic, parameterType, connectionGeometry);

	string port_preLabel = "";
	string port_postLabel = "";
	string netSize = "";
	string netTypeText = "";
	if (declarationType == ModuleDeclarationPortsInput)
		netTypeText += "input ";
	else if (declarationType == ModuleDeclarationPortsOutput)
		netTypeText += "output ";

	if (netType == wire)
		netTypeText += "wire";
	else if (netType == reg)
		netTypeText += "reg";

	bool** connectionMask = NNobj.LYRinfo[layerN].connectionMask;

	string netLabelNew = netLabel + "_L" + Int2Str(layerN);
	if (TSpar > -1)
		netLabelNew += "_netPar" + Int2Str(TSpar);
		

	if (isStochastic) { // stochastic net
		int startLayer = layerN;
		int endLayer = layerN;
		if (connectionGeometry == previousLayerWeight)
			startLayer = layerN - 1;
		if (isPortDeclaration)
		{
			if (TSpar == -1) {
				port_preLabel = "." + netLabel + "_L" + Int2Str(layerN);
				port_postLabel = ")";
			}
			else {
				port_preLabel = "." + netLabel + "_L" + Int2Str(layerN);
				port_postLabel = ")";
			}
		}
		else if ((declarationType == TopLevel) | (declarationType == ModuleDeclarationPortsInput) | (declarationType == ModuleDeclarationPortsOutput)) {
			if (NNobj.LYRinfo[layerN].LT == FC) netSize = netTypeText + " [" + Int2Str(NNobj.LYRSIZE[startLayer]) + " - 1:0] ";
			else if (NNobj.LYRinfo[layerN].LT == CV) {
				netSize = netTypeText + " [" + Int2Str(NNobj.LYRinfo[layerN].conv.Nconnections) + " - 1:0] ";
				if (connectionGeometry == singleLayer) {
					if (netLabel == "alpha" | netLabel == "beta" | netLabel == "SIGN_beta" | netLabel == "dbeta" | netLabel == "SIGN_dbeta") netSize = netTypeText + " [" + Int2Str(NNobj.LYRinfo[layerN].conv.filters) + " - 1:0] ";
					else  netSize = netTypeText + " [" + Int2Str(NNobj.LYRSIZE[layerN]) + " - 1:0] ";
				}
			}
		}

		if (connectionGeometry == singleLayer) {
			//if (NNobj.LYRinfo[layerN].LT == CV) netSize = netTypeText + " [" + Int2Str(NNobj.LYRinfo[layerN].filters) + " - 1:0] ";
			string port_preLabelNew = port_preLabel;
			if (isPortDeclaration) port_preLabelNew += label_suffix + "(";
			outputString << port_preLabelNew << netSize << netLabelNew << suffix << port_postLabel << termination;
		}
		else
		{
			string port_preLabelNew = port_preLabel;
			outputString << netSize;
			for (int j = 0; j < NNobj.LYRSIZE[endLayer]; j++) {
				if (AnyConnectionExists(NNobj, layerN, j)) {
					port_preLabelNew = port_preLabel;

					if (isPortDeclaration) port_preLabelNew = port_preLabel + "_j_" + Int2Str(j) + label_suffix + "(";
					outputString << port_preLabelNew << netLabelNew << "_j_" << j << suffix << port_postLabel << ", ";
				}
			}
			outputString.seekp(-2, outputString.cur);
			outputString << termination;
		}
		
	}
	else { // decimal net
		int startLayer = layerN;
		int endLayer = layerN;
		if (connectionGeometry == previousLayerWeight)
			startLayer = layerN - 1;
		if (isPortDeclaration)
		{
			if (TSpar == -1) {
				port_preLabel = "." + netLabel + "_L" + Int2Str(layerN);
				port_postLabel = ")";
			}
			else {
				port_preLabel = "." + netLabel + "_L" + Int2Str(layerN) + "netPar" + Int2Str(TSpar);
				port_postLabel = ")";
			}
		}

		else if ((declarationType == TopLevel) | (declarationType == ModuleDeclarationPortsInput) | (declarationType == ModuleDeclarationPortsOutput)) {
			netSize = netTypeText + " [" + decimalPrecision + " - 1:0] ";
		}

		if (connectionGeometry == singleLayer) {
			string port_preLabelNew = port_preLabel;
			if (isPortDeclaration) port_preLabelNew += "_0(";
			outputString << netSize;
			int numberOfNets;
			if (NNobj.LYRinfo[layerN].LT == CV) numberOfNets = NNobj.LYRinfo[layerN].conv.filters;
			else numberOfNets = NNobj.LYRSIZE[endLayer];
			for (int j = 0; j < numberOfNets -1; j++) {
				outputString << port_preLabelNew <<  netLabel << "_L" << layerN << "_" << j << port_postLabel << ", ";
				if (isPortDeclaration) port_preLabelNew = port_preLabel + "_" + Int2Str(j + 1) + "(";
			}
			outputString << port_preLabelNew << netLabel << "_L" << layerN << "_" << numberOfNets - 1 << port_postLabel << termination;
		}
		else if (connectionGeometry == previousLayerWeight)
		{

			string port_preLabelNew = port_preLabel;
			bool netGenerated = false;
			for (int i = 0; i < NNobj.LYRSIZE[startLayer]; i++) {
				outputString << netSize;
				netGenerated = false;
				for (int j = 0; j < NNobj.LYRSIZE[endLayer]; j++) {
					if (ConnectionExists(NNobj,layerN,i,j)) {
						if (isPortDeclaration) port_preLabelNew = port_preLabel + "_" + Int2Str(i) + "_" + Int2Str(j) + "(";
						outputString << port_preLabelNew << netLabel << "_L" << layerN << "_" << i << "_" << j << port_postLabel << ", ";
						netGenerated = true;
					}
				}
				if (!netGenerated) {
					//outputString.seekp(netSize.size() + 1, outputString.cur);
					string placeholder = outputString.str();
					placeholder.erase(placeholder.length() - netSize.length(), placeholder.length());
					outputString.str(placeholder);
					outputString.seekp(placeholder.length(), outputString.cur);
				}
				if (netGenerated){
					outputString.seekp(-2, outputString.cur);
					outputString << termination;
				}
			}
		
		}
		
	}
	return outputString.str();
}


string NN_DeclareRecurrentNetLine(int layerN, NN NNobj, int FilterSize, string termination, string netLabel, NetTypes netType, string Label0, string Label1, ParameterTypes parameterType, DeclarationTypes declarationType, ConnectionGeometry connectionGeometry, string decimalPrecision, int timeSteps, int TSpar) {

	stringstream outputString;
	bool isStochastic = true;
	bool isRecurrent = false;
	bool isPortDeclaration = (declarationType == ModuleInstantiationPorts);

	GetNetLineConditions(netLabel, isStochastic, parameterType, connectionGeometry);

	string port_preLabel = "";
	string port_postLabel = "";
	string netSize = "";
	string netTypeText = "";
	if (declarationType == ModuleDeclarationPortsInput)
		netTypeText += "input ";
	else if (declarationType == ModuleDeclarationPortsOutput)
		netTypeText += "output ";

	if (netType == wire)
		netTypeText += "wire";
	else if (netType == reg)
		netTypeText += "reg";

	string netLabelNew = netLabel + "_L" + Int2Str(layerN);
	string netParLabel = "";
	if (TSpar > -1)
		netParLabel += "_netPar" + Int2Str(TSpar);

	if (isStochastic) { // stochastic net
		int startLayer = layerN;
		int endLayer = layerN;
		if (connectionGeometry == previousLayerWeight)
			startLayer = layerN - 1;
		if ((declarationType == TopLevel) | (declarationType == ModuleDeclarationPortsInput) | (declarationType == ModuleDeclarationPortsOutput)) {
			netSize = netTypeText + " [" + Int2Str(NNobj.LYRSIZE[startLayer]) + " - 1:0] ";
		}

		outputString << netSize;
		for (int t = 0; t < timeSteps; t++) {


			if (connectionGeometry == singleLayer) {
				if (isPortDeclaration)
				{
					port_preLabel = "." + netLabel +  "_L" + Int2Str(layerN) + "_T" + Int2Str(t) ;
					port_postLabel = ")";
				}
				string port_preLabelNew = port_preLabel;
				if (isPortDeclaration) port_preLabelNew += "(";
				outputString << port_preLabelNew << netLabel << "_L" << layerN << "_T" << t << netParLabel << port_postLabel;
				if (t == (timeSteps - 1))
					outputString << termination;
				else
					outputString << ", ";
			}
			else
			{
				string port_preLabelNew = port_preLabel;
				if (isPortDeclaration) port_preLabelNew += "_j_0(";
				outputString << netSize;
				for (int j = 0; j < NNobj.LYRSIZE[endLayer] - 1; j++) {
					outputString << port_preLabelNew << netLabel << "_L" << layerN << "_T" << t << netParLabel << "_j_" << j << port_postLabel << ", ";
					port_preLabelNew = port_preLabel;
					if (isPortDeclaration) port_preLabelNew = port_preLabel + "_j_" + Int2Str(j + 1) + "(";
				}
				outputString << port_preLabelNew << netLabel  << "_L" << layerN << "_T" << t << netParLabel << "_j_" << NNobj.LYRSIZE[endLayer] - 1 << port_postLabel << termination;
			}
		}

	}
	else { // decimal net
		for (int t = 0; t < timeSteps; t++) {
			int startLayer = layerN;
			int endLayer = layerN;
			if (connectionGeometry == previousLayerWeight)
				startLayer = layerN - 1;
			if (isPortDeclaration)
			{
				port_preLabel = "." + netLabel + "_L" + Int2Str(layerN) + "_T" + Int2Str(t);
				port_postLabel = ")";
			}

			else if ((declarationType == TopLevel) | (declarationType == ModuleDeclarationPortsInput) | (declarationType == ModuleDeclarationPortsOutput)) {
				netSize = netTypeText + " [" + decimalPrecision + " - 1:0] ";
			}

			if (connectionGeometry == singleLayer) {
				string port_preLabelNew = port_preLabel;
				if (isPortDeclaration) port_preLabelNew += "_0(";
				outputString << netSize;
				for (int j = 0; j < NNobj.LYRSIZE[endLayer] - 1; j++) {
					outputString << port_preLabelNew << netLabel << "_L" << layerN <<  "_T" << t <<  "_" << j << port_postLabel << ", ";
					if (isPortDeclaration) port_preLabelNew = port_preLabel + "_" + Int2Str(j + 1) + "(";
				}
				outputString << port_preLabelNew << netLabel << "_L" << layerN << "_T" << t << "_" << NNobj.LYRSIZE[endLayer] - 1 << port_postLabel << termination;
			}
			else
			{
				string port_preLabelNew = port_preLabel;
				if (isPortDeclaration) port_preLabelNew = port_preLabel + "_0_0(";

				for (int i = 0; i < NNobj.LYRSIZE[startLayer]; i++) {
					outputString << netSize;
					for (int j = 0; j < NNobj.LYRSIZE[endLayer] - 1; j++) {
						if (isPortDeclaration) port_preLabelNew = port_preLabel + "_" + Int2Str(i) + "_" + Int2Str(j) + "(";
						outputString << port_preLabelNew << netLabel << "_L" << layerN << "_T" << t <<  "_" << i << "_" << j << port_postLabel << ", ";
					}
					if (isPortDeclaration) port_preLabelNew = port_preLabel + "_" + Int2Str(i) + "_" + Int2Str(NNobj.LYRSIZE[endLayer] - 1) + "(";
					outputString << port_preLabelNew << netLabel << "_L" << layerN << "_T" << t << "_" << i << "_" << NNobj.LYRSIZE[endLayer] - 1 << port_postLabel << termination;
				}
			}
		}
	}

	return outputString.str();
}

void GetNetLineConditions(string &netLabel, bool &isStochastic, ParameterTypes &parameterType, ConnectionGeometry &connectionGeometry) {
	if ((parameterType == miscellaneous)) {

	}
	else if (parameterType == miscellaneousDec) {
		isStochastic = false;
	} // STOCHASTIC: 
	else if (parameterType == alphaStoch) {
		netLabel = "alpha";
		connectionGeometry = previousLayerWeight;
	}
	else if (parameterType == betaStoch) {
		netLabel = "beta";
		connectionGeometry = singleLayer;
	}
	else if (parameterType == gammaStoch) {
		netLabel = "gamma";
		connectionGeometry = recurrentLayer;
	}
	else if (parameterType == alphaDec) {
		netLabel = "alphaD";
		isStochastic = false;
		connectionGeometry = previousLayerWeight;
	}
	else if (parameterType == betaDec) {
		netLabel = "betaD";
		isStochastic = false;
		connectionGeometry = singleLayer;
	}
	else if (parameterType == gammaDec) {
		netLabel = "gammaD";
		isStochastic = false;
		connectionGeometry = recurrentLayer;
	}
	else if (parameterType == alphaSign) {
		netLabel = "SIGN_alpha";
		connectionGeometry = previousLayerWeight;
	}
	else if (parameterType == betaSign) {
		netLabel = "SIGN_beta";
		connectionGeometry = singleLayer;
	}
	else if (parameterType == gammaSign) {
		netLabel = "SIGN_gamma";
		connectionGeometry = recurrentLayer;
	}
	else if (parameterType == alphaSign) {
		netLabel = "SIGN_alpha";
		connectionGeometry = previousLayerWeight;
	}
	else if (parameterType == betaSign) {
		netLabel = "SIGN_beta";
		connectionGeometry = singleLayer;
	}
	else if (parameterType == gammaSign) {
		netLabel = "SIGN_gamma";
		connectionGeometry = recurrentLayer;
	}
	else if (parameterType == alphaStochGrad) {
		netLabel = "dalpha";
		connectionGeometry = previousLayerWeight;
	}
	else if (parameterType == betaStochGrad) {
		netLabel = "dbeta";
		connectionGeometry = singleLayer;
	}
	else if (parameterType == gammaStochGrad) {
		netLabel = "dgamma";
		connectionGeometry = recurrentLayer;
	}
	else if (parameterType == alphaStochGradSign) {
		netLabel = "SIGN_dalpha";
		connectionGeometry = previousLayerWeight;
	}
	else if (parameterType == betaStochGradSign) {
		netLabel = "SIGN_dbeta";
		connectionGeometry = singleLayer;
	}
	else if (parameterType == gammaStochGradSign) {
		netLabel = "SIGN_dgamma";
		connectionGeometry = recurrentLayer;
	}
	else { stringstream ss; ss << "ERROR: invalid net type: " << parameterType << " (Verilog.cpp NN_DeclareNetLine\n"; cout << ss.str(); }
}

void TerminateLine(std::ostream& outFile, string line, int cursorOffset, string terminationText) {
	stringstream lineStream;
	lineStream << line;
	lineStream.seekp(cursorOffset, lineStream.cur);
	lineStream << terminationText;
	outFile << lineStream.str();
}