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
#include "genCONV.h"
#include "NN_DataTypes.h"
#include "Verilog.h"

using namespace std;

void GenerateConvolutionalConnectionMasks(sysConfig sConfig, LYR &LayerInfo, int layerN) {

	int ConnectionWidth = sqrt(LayerInfo.conv.Nconnections);
	int InputImageWidth = sqrt(sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber - 1]); // Width of input layer
	int Steps = (InputImageWidth - ConnectionWidth) / LayerInfo.conv.stride + 1;

	bool** connectionMask = new bool*[LayerInfo.conv.filterSize];
	for (int i = 0; i < LayerInfo.conv.filterSize; i++)
		connectionMask[i] = new bool[sConfig.NNobj.LYRSIZE[layerN-1]];

	int** connections = new int*[LayerInfo.conv.filterSize];
	for (int i = 0; i < LayerInfo.conv.filterSize; i++)
		connections[i] = new int[LayerInfo.conv.Nconnections];

	int** ConnectionMeshGrid = new int*[InputImageWidth];
	for (int i = 0; i<InputImageWidth; i++)
		ConnectionMeshGrid[i] = new int[InputImageWidth];
	for (int i = 0; i < InputImageWidth; i++) {
		for (int j = 0; j < InputImageWidth; j++) {
			ConnectionMeshGrid[i][j] = i * InputImageWidth + j;
		}
	}
	int index = 0;
	int w_pos = 0;
	int h_pos = 0;

	for (int d = 0; d < Steps; d++) {
		for (int g = 0; g < Steps; g++) {
			for (int h = h_pos; h < (h_pos + ConnectionWidth); h++) {
				for (int w = w_pos; w < (w_pos + ConnectionWidth); w++) {
					connections[d*Steps + g][index++] = ConnectionMeshGrid[h][w];
					//cout <<  connections[d*Steps + g][index-1] << " ";
				}
			}
			//cout << "\n";
			w_pos += LayerInfo.conv.stride;
			index = 0;
		}
		h_pos += LayerInfo.conv.stride;
		w_pos = 0;
	}



	for (int i = 0; i < LayerInfo.conv.filterSize; i++) 
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[layerN - 1]; j++)
			connectionMask[i][j] = false;

	for (int i = 0; i < LayerInfo.conv.filterSize; i++)
		for (int m = 0; m < LayerInfo.conv.Nconnections; m++)
			connectionMask[i][connections[i][m]] = true;

	/*
	for (int s = 0; s < LayerInfo.conv.filterSize; s++) {
		for (int i = 0; i < sConfig.NNobj.LYRinfo[layerN].conv.Nconnections; i++)
			cout << connections[s][i] << " ";
		cout << endl;
	}
	for (int i = 0; i < LayerInfo.conv.filterSize; i++) {
		for (int j = 0; j < sConfig.NNobj.LYRSIZE[layerN - 1]; j++)
			cout << connectionMask[i][j] << " ";
		cout << endl;
	}
	*/

	LayerInfo.connections = connections;
	LayerInfo.connectionMask = connectionMask;
}

void genCONV_FWD_block(sysConfig sConfig, LYR LayerInfo){
	
	ofstream outFile;
	outFile.open(sConfig.Dir + "CONVBLOCK_L" + Int2Str(LayerInfo.layerNumber) + ".v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Convolutional Layer " << Int2Str(LayerInfo.layerNumber) << " \n\n";

	outFile << "module CONVBLOCK_L" << LayerInfo.layerNumber << "(\n\t";
	outFile << "zeroCenterSource,\n\t";
	outFile << "R_FPsource, \n\t";
	outFile << "a,\n\t";
	outFile << StringSeries(LayerInfo.conv.filters, "a_MEM_ACTIVE_F", "") << ",\n\t";
	outFile << StringSeries(LayerInfo.conv.filters, "alpha_F", "") << ",\n\t";
	outFile << "beta,\n\t";
	outFile << StringSeries(LayerInfo.conv.filters, "SIGN_alpha_F", "") << ",\n\t";
	outFile << "SIGN_beta,\n\t";
	outFile << StringSeries(LayerInfo.conv.filters, "z_F", "") << ",\n\t";
	outFile << StringSeries(LayerInfo.conv.filters, "zp_F", "") << ",\n\t";
	outFile << StringSeries(LayerInfo.conv.filters, "a_out_F", "") << ",\n\t";
	outFile << "CLK, INIT\n);\n\n";

	// ---------------------------    port definitions

	outFile << "input wire zeroCenterSource;\n";
	outFile << "input wire R_FPsource;\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber-1] << " - 1:0] a;\n";
	outFile << "input wire [" << LayerInfo.conv.filterSize << "-1:0] " << StringSeries(LayerInfo.conv.filters, "a_MEM_ACTIVE_F", "") << ";\n";
	outFile << "input wire [" << LayerInfo.conv.Nconnections << "-1:0] " << StringSeries(LayerInfo.conv.filters, "alpha_F", "") << ";\n";
	outFile << "input wire [" << LayerInfo.conv.filters << "-1:0] beta;\n";
	outFile << "input wire [" << LayerInfo.conv.Nconnections << "-1:0] " << StringSeries(LayerInfo.conv.filters, "SIGN_alpha_F", "") << ";\n";
	outFile << "input wire [" << LayerInfo.conv.filters << "-1:0] SIGN_beta;\n";
	outFile << "input wire CLK, INIT;\n";
	outFile << "\n";

	outFile << "output wire [" << LayerInfo.conv.filterSize << "-1:0] " << StringSeries(LayerInfo.conv.filters, "z_F", "") << ";\n";
	outFile << "output wire [" << LayerInfo.conv.filterSize << "-1:0] " << StringSeries(LayerInfo.conv.filters, "zp_F", "") << ";\n";
	outFile << "output wire [" << LayerInfo.conv.filterSize << "-1:0] " << StringSeries(LayerInfo.conv.filters, "a_out_F", "") << ";\n";
	outFile << "\n";
	outFile << "wire [" << LayerInfo.size << "-1:0] zeroCenter, R_FP;\n";
	outFile << "SS_RERANDOMIZER RERANDOMIZER_zC(.CLK(CLK), .INIT(INIT), .IN(zeroCenterSource), .OUT(zeroCenter));\n";
	outFile << "defparam RERANDOMIZER_zC.N = " << LayerInfo.size << "; \n";
	outFile << "SS_RERANDOMIZER RERANDOMIZER_FP(.CLK(CLK), .INIT(INIT), .IN(R_FPsource), .OUT(R_FP));\n";
	outFile << "defparam RERANDOMIZER_FP.N = " << LayerInfo.size << "; \n\n";
	outFile << "\n";
	int d_index = 0;
	int Rindex = 0;
	for (int l = 0; l < LayerInfo.conv.filters; l++) {
		outFile << "CONVFILTER_L" << LayerInfo.layerNumber << "_F" << l << " CONVFILTER_L" << LayerInfo.layerNumber << "_F" << l << "_0(.d(zeroCenter[" << d_index + LayerInfo.conv.filterSize - 1 << ":" << d_index << "]),.R_FP(R_FP[" << Rindex + LayerInfo.conv.filterSize << " - 1:" << Rindex << "]),.a(a), .a_MEM_ACTIVE(a_MEM_ACTIVE_F" << l << "), .alpha(alpha_F" << l << "), .beta(beta[" << l << "]), .SIGN_alpha(SIGN_alpha_F" << l << "), .SIGN_beta(SIGN_beta[" << l << "]), .z(z_F" << l << "), .zp(zp_F" << l << "), .a_out(a_out_F" << l << "), .CLK(CLK), .INIT(INIT)); \n";
		d_index += LayerInfo.conv.filterSize;
		Rindex += LayerInfo.conv.filterSize;
	}
	outFile << "\n";
	outFile << "endmodule\n";

	for (int f = 0; f < LayerInfo.conv.filters; f++)
		genCONV_FWD_filter(sConfig, LayerInfo, f, LayerInfo.connections);
		
}

void genCONV_FWD_filter(sysConfig sConfig, LYR LayerInfo, int F, int** Connections) {

	ofstream outFile;
	outFile.open(sConfig.Dir + "CONVFILTER_L" + Int2Str(LayerInfo.layerNumber) + "_F" + Int2Str(F) + ".v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Convolutional Layer " << Int2Str(LayerInfo.layerNumber) << " Filter " << F << " \n\n";

	outFile << "module CONVFILTER_L" << LayerInfo.layerNumber << "_F" << F << "(\n\t";
	outFile << "d,\n\t";
	outFile << "R_FP,\n\t";
	outFile << "a,\n\t";
	outFile << "a_MEM_ACTIVE,\n\t";
	outFile << "alpha,\n\t";
	outFile << "beta,\n\t";
	outFile << "SIGN_alpha,\n\t";
	outFile << "SIGN_beta,\n\t";
	outFile << "z,\n\t";
	outFile << "zp,\n\t";
	outFile << "a_out,\n\t";
	outFile << "CLK, INIT\n);\n\n";

	outFile << "input wire [" << LayerInfo.conv.filterSize << " - 1:0] d;\n";
	outFile << "input wire [" << LayerInfo.conv.filterSize << " - 1:0] R_FP;\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber - 1] << " - 1:0] a;\n";
	outFile << "input wire [" << LayerInfo.conv.filterSize << " - 1:0] a_MEM_ACTIVE;\n";
	outFile << "input wire [" << LayerInfo.conv.Nconnections << " - 1:0] alpha;\n";
	outFile << "input wire beta;\n";
	outFile << "input wire [" << LayerInfo.conv.Nconnections << " - 1:0] SIGN_alpha;\n";
	outFile << "input wire SIGN_beta;\n";
	outFile << "input wire CLK, INIT;\n";
	outFile << "\n";

	outFile << "output wire [" << LayerInfo.conv.filterSize << "-1:0] " <<"z;\n";
	outFile << "output wire [" << LayerInfo.conv.filterSize << "-1:0] " << "zp;\n";
	outFile << "output wire [" << LayerInfo.conv.filterSize << "-1:0] " << "a_out;\n";
	outFile << "\n";

	for (int s = 0; s < LayerInfo.conv.filterSize; s++) {
		outFile << "NN_CONVNODE CONVNODE_" << s << "(.a({";
		for (int j = (LayerInfo.conv.Nconnections - 1); j > 0; j--)
			outFile << "a[" << Connections[s][j] << "],";
		outFile << "a[" << Connections[s][0] << "]}), ";
		outFile << ".a_MEM_ACTIVE(a_MEM_ACTIVE[" << s << "]), ";
		outFile << ".d(d[" << s << "]), .r(R_FP[" << s << "]), .alpha(alpha), .beta(beta), .SIGN_alpha(SIGN_alpha), .SIGN_beta(SIGN_beta), .z(z[" << s << "]), .zp(zp[" << s << "]), .a_out(a_out[" << s << "]), .CLK(CLK), .INIT(INIT));\n";
	}
	for (int s = 0; s < LayerInfo.conv.filterSize; s++)
		outFile << "defparam CONVNODE_" << s << ".N = " << LayerInfo.conv.Nconnections << ";\n";

	outFile << "\nendmodule";
	outFile.close();
}

void genCONVBLOCK_BP_block(sysConfig sConfig, LYR LayerInfo, int** Connections) {


	ofstream outFile;
	outFile.open(sConfig.Dir + "CONVBLOCKBP_L" + Int2Str(LayerInfo.layerNumber) + ".v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Convolutional Backpropagation Layer " << Int2Str(LayerInfo.layerNumber) << " \n\n";

	outFile << "module CONVBLOCKBP_L" << LayerInfo.layerNumber << "(\n\t";
	NN_DeclareNodeNetArray_alpha(outFile, sConfig, sConfig.NNobj.LYRSIZE, LayerInfo.layerNumber + 1, LayerInfo.layerNumber + 1, "", "alpha", true);
	NN_DeclareNodeNetArray_alpha(outFile, sConfig, sConfig.NNobj.LYRSIZE, LayerInfo.layerNumber + 1, LayerInfo.layerNumber + 1, "", "SIGN_alpha", true);

	outFile << StringSeries(LayerInfo.conv.filters, "zp_F", "") << ",\n\t";
	outFile << "delta_L" << LayerInfo.layerNumber+1 << ",\n\t";
	outFile << "SIGN_delta_L" << LayerInfo.layerNumber+1 << ",\n\t";
	outFile << StringSeries(LayerInfo.conv.filters, "delta_out_F", "") << ",\n\t";
	outFile << StringSeries(LayerInfo.conv.filters, "SIGN_delta_out_F", "") << ",\n\t";
	outFile << "R_BP_source,\n\t";
	outFile << "CLK, INIT\n);\n\n";

	NN_DeclareNodeNetArray_alpha(outFile, sConfig, sConfig.NNobj.LYRSIZE, LayerInfo.layerNumber + 1, LayerInfo.layerNumber + 1, "input wire", "alpha");
	NN_DeclareNodeNetArray_alpha(outFile, sConfig, sConfig.NNobj.LYRSIZE, LayerInfo.layerNumber + 1, LayerInfo.layerNumber + 1, "input wire", "SIGN_alpha");
	outFile << "input wire [" << LayerInfo.conv.filterSize << "-1:0] " << StringSeries(LayerInfo.conv.filters, "zp_F", "") << ";\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1] << "-1:0] " << "delta_L" << LayerInfo.layerNumber + 1 << ";\n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1] << "-1:0] " << "SIGN_delta_L" << LayerInfo.layerNumber + 1 << ";\n";
	outFile << "input wire R_BP_source;\n";
	outFile << "input wire CLK, INIT;\n";
	outFile << "\n";

	outFile << "output wire [" << LayerInfo.conv.filterSize << "-1:0] " << StringSeries(LayerInfo.conv.filters, "delta_out_F", "") << ";\n";
	outFile << "output wire [" << LayerInfo.conv.filterSize << "-1:0] " << StringSeries(LayerInfo.conv.filters, "SIGN_delta_out_F", "") << ";\n";
	outFile << "\n";

	outFile << "wire [" << LayerInfo.size << "-1:0] R_BP, R_FP;\n";
	outFile << "SS_RERANDOMIZER RERANDOMIZER_zC(.CLK(CLK), .INIT(INIT), .IN(R_BP_source), .OUT(R_BP));\n";
	outFile << "defparam RERANDOMIZER_zC.N = " << LayerInfo.size << ";\n\n";

	int negone = -1;
	int Rindex = 0;
	for (int f = 0; f < LayerInfo.conv.filters; f++) {
		outFile << "CONVFILTERBP_L" << LayerInfo.layerNumber << "_F" << f << " CONVFILTERBP_L" << LayerInfo.layerNumber << "_F" << f << "_0(\n\t";
		outFile << ".delta(delta_L" << LayerInfo.layerNumber + 1 << "),\n\t";
		outFile << ".SIGN_delta(SIGN_delta_L" << LayerInfo.layerNumber + 1 << "),\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1], "alpha_L2_j_", "", "alpha_L2_F" + Int2Str(f) + "_j_", "", negone) << "\n\t";
		outFile << StringSeriesPort(sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1], "SIGN_alpha_L2_j_", "", "SIGN_alpha_L2_F" + Int2Str(f) + "_j_", "", negone) << "\n\t";
		outFile << ".zp(zp_F" << f << "),\n\t";
		outFile << ".delta_out(delta_out_F" << f << "),\n\t";
		outFile << ".SIGN_delta_out(SIGN_delta_out_F" << f << "),\n\t";
		outFile << ".R_BP(R_BP[" << Rindex + LayerInfo.conv.filterSize << "-1:" << Rindex << "]),\n\t";
		outFile << ".CLK(CLK), .INIT(INIT)\n";
		outFile << ");\n\n";
		Rindex += LayerInfo.conv.filterSize;
	}

	outFile << "\nendmodule";
	outFile.close();

	for (int f = 0; f < LayerInfo.conv.filters; f++)
		genCONV_BP_filter(sConfig, LayerInfo, f, Connections);



}

void genCONV_BP_filter(sysConfig sConfig, LYR LayerInfo, int F, int** Connections) {

	ofstream outFile;
	outFile.open(sConfig.Dir + "CONVFILTERBP_L" + Int2Str(LayerInfo.layerNumber) + "_F" + Int2Str(F) + ".v");
	outFile << "// Chris Ceroici\n" << endl;
	outFile << "// Convolutional Backpropagation Layer " << Int2Str(LayerInfo.layerNumber) << " Filter " << F << " \n\n";
	outFile << "module CONVFILTERBP_L" << LayerInfo.layerNumber << "_F" << F << "(\n\t";
	outFile << "delta,\n\t";
	outFile << "SIGN_delta,\n\t";
	outFile << StringSeries(sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1], "alpha_L2_j_", "") << ",\n\t";
	outFile << StringSeries(sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1], "SIGN_alpha_L2_j_", "") << ",\n\t";
	outFile << "zp,\n\t";
	outFile << "delta_out,\n\t";
	outFile << "SIGN_delta_out,\n\t";
	outFile << "R_BP,\n\t";
	outFile << "CLK, INIT\n";
	outFile << ");\n\n";

	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1] << " - 1:0] delta; \n";
	outFile << "input wire [" << sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1] << " - 1:0] SIGN_delta; \n";
	outFile << "input wire [" << LayerInfo.conv.filterSize << " - 1:0] " << StringSeries(sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1], "alpha_L2_j_", "") << ";\n";
	outFile << "input wire [" << LayerInfo.conv.filterSize << " - 1:0] " << StringSeries(sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1], "SIGN_alpha_L2_j_", "") << ";\n";
	outFile << "input wire [" << LayerInfo.conv.filterSize << " - 1:0] zp;\n";
	outFile << "input wire [" << LayerInfo.conv.filterSize << " - 1:0] R_BP;\n";
	outFile << "input wire CLK, INIT;\n";
	outFile << "\n";
	outFile << "output wire [" << LayerInfo.conv.filterSize << " - 1:0] " << "delta_out;\n";
	outFile << "output wire [" << LayerInfo.conv.filterSize << " - 1:0] " << "SIGN_delta_out;\n";
	outFile << "\n";
	
	for (int s = 0; s < LayerInfo.conv.filterSize; s++) {
		outFile << "NN_CONV_DELTABP NN_CONV_DELTABP_" << s << "(.delta(delta), .SIGN_delta(SIGN_delta), .alpha({";
		for (int j = (sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber+1]-1); j > 0; j--)
			outFile << "alpha_L2_j_" << j << "[" << s << "],";
		outFile << "alpha_L2_j_" << 0 << "[" << s << "]}),";
		outFile << " .SIGN_alpha({";
		for (int j = (sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1] - 1); j > 0; j--)
			outFile << "SIGN_alpha_L2_j_" << j << "[" << s << "],";
		outFile << "SIGN_alpha_L2_j_" << 0 << "[" << s << "]}),";
		outFile << " .delta_out(delta_out[" << s << "]), .SIGN_out(SIGN_delta_out[" << s << "]), .zp(zp[" << s << "]),";
		outFile << ".R_condition(R_BP[" << s << "]),.CLK(CLK), .INIT(INIT)); \n";
	}
	for (int s = 0; s < LayerInfo.conv.filterSize; s++)
		outFile << "defparam NN_CONV_DELTABP_" << s << ".N = " << sConfig.NNobj.LYRSIZE[LayerInfo.layerNumber + 1] << ";\n";

	outFile << "\nendmodule";
	outFile.close();
}


void genCONV_FWD_block_Inst(std::ostream &outFile, sysConfig sConfig, int L, int RNGindex, int T) {
	int negone = -1;
	outFile << "CONVBLOCK_L" << L << " CONVBLOCK_L" << L << "_0(\n\t";
	outFile << ".zeroCenterSource(zeroCenterSource[" << L-1 << "]),\n\t";
	outFile << ".R_FPsource(R_FP_source[" << L-1 << "]),\n\t";
	outFile << ".a(a_L" << L - 1 << "), \n\t";
	CollectConvolutionalNets(outFile, sConfig, L, "a_MEM_ACTIVE", "a_MEM_ACTIVE");
	for (int f = 0; f < sConfig.NNobj.LYRinfo[L].conv.filters; f++)
	{
		stringstream alphaLine;
		alphaLine << ".alpha_F" << f << "({";
		for (int j = sConfig.NNobj.LYRinfo[L].conv.Nconnections-1; j >= 0; j--)
			alphaLine << "alpha_L" << L << "_j_" << sConfig.NNobj.LYRinfo[L].conv.FilterStartNode(f) << "[" << j << "], ";
		alphaLine.seekp(-2, alphaLine.cur);
		alphaLine << "}),\n\t";
		outFile << alphaLine.str();
	}
	outFile << ".beta(beta_L" << L << "),\n\t";
	for (int f = 0; f < sConfig.NNobj.LYRinfo[L].conv.filters; f++)
	{
		stringstream alphaLine;
		alphaLine << ".SIGN_alpha_F" << f << "({";
		for (int j = sConfig.NNobj.LYRinfo[L].conv.Nconnections-1; j >= 0; j--)
			alphaLine << "SIGN_alpha_L" << L << "_j_" << sConfig.NNobj.LYRinfo[L].conv.FilterStartNode(f) << "[" << j << "], ";
		alphaLine.seekp(-2, alphaLine.cur);
		alphaLine << "}),\n\t";
		outFile << alphaLine.str();
	}
	outFile << ".SIGN_beta(SIGN_beta_L" << L << "),\n\t";
	CollectConvolutionalNets(outFile, sConfig, L, "z", "z");
	CollectConvolutionalNets(outFile, sConfig, L, "zp", "zp");
	CollectConvolutionalNets(outFile, sConfig, L, "a_out", "a");

	outFile << ".CLK(CLK), .INIT(INIT)\n";
	outFile << ");\n\n";
}

void genCONV_BP_block_Inst(std::ostream &outFile, sysConfig sConfig, int L, int T) {
	int negone = -1;
	outFile << "// Level " << L + 1 << "->" << L << " backpropagation\n";
	outFile << "CONVBLOCKBP_L" << L << " CONVBLOCKBP_L" << L << "_0(\n\t";
	for (int f = 0; f < sConfig.NNobj.LYRinfo[L].conv.filters; f++)
	{
		stringstream alphaLine;
		for (int j = sConfig.NNobj.LYRSIZE[L + 1] - 1; j >= 0; j--) {
			alphaLine << ".alpha_L" << L + 1 << "_F" << f << "_j_" << j << "(alpha_L" << L + 1 << "_j_" << j;
			alphaLine << "[" << sConfig.NNobj.LYRinfo[L].conv.FilterEndNode(f) << ":" << sConfig.NNobj.LYRinfo[L].conv.FilterStartNode(f) << "]), ";
		}
		alphaLine.seekp(-2, alphaLine.cur);
		alphaLine << ",\n\t";
		outFile << alphaLine.str();
	}
	for (int f = 0; f < sConfig.NNobj.LYRinfo[L].conv.filters; f++)
	{
		stringstream alphaLine;
		for (int j = sConfig.NNobj.LYRSIZE[L + 1] - 1; j >= 0; j--) {
			alphaLine << ".SIGN_alpha_L" << L + 1 << "_F" << f << "_j_" << j << "(SIGN_alpha_L" << L + 1 << "_j_" << j;
			alphaLine << "[" << sConfig.NNobj.LYRinfo[L].conv.FilterEndNode(f) << ":" << sConfig.NNobj.LYRinfo[L].conv.FilterStartNode(f) << "]), ";
		}
		alphaLine.seekp(-2, alphaLine.cur);
		alphaLine << ",\n\t";
		outFile << alphaLine.str();
	}
	CollectConvolutionalNets(outFile, sConfig, L, "zp", "zp");
	outFile << ".delta_L" << L + 1 << "(delta_L" << L + 1 << "),\n\t";
	outFile << ".SIGN_delta_L" << L + 1 << "(SIGN_L" << L + 1 << "),\n\t";
	CollectConvolutionalNets(outFile, sConfig, L, "delta_out", "delta");
	CollectConvolutionalNets(outFile, sConfig, L, "SIGN_delta_out", "SIGN");
	//for (int f = 0; f < sConfig.NNobj.LYRinfo[L].size; f++)
	//	outFile << ".delta_out_L" << L << "_F" << f << "(delta_L" << L << "_F" << f << "), ";
	//for (int f = 0; f < sConfig.NNobj.LYRinfo[L].size; f++)
	//	outFile << ".SIGN_delta_out_L" << L << "_F" << f << "(SIGN_L" << L << "_F" << f << "),\n\t";
	outFile << ".R_BP_source(R_BP" << L << "[1]),\n\t";
	outFile << ".CLK(CLK), .INIT(BP_FRAME)\n";
	outFile << ");\n\n";
}


void CollectConvolutionalNets(std::ostream &outFile, sysConfig sConfig, int layerN, string portName, string netName) {

	for (int f = 0; f < sConfig.NNobj.LYRinfo[layerN].conv.filters; f++)
	{
		stringstream line;
		line << "." << portName << "_F" << f << "({";
		for (int j = sConfig.NNobj.LYRSIZE[layerN]-1; j >= 0; j--)
			if (NodeBelongsToFilter(sConfig.NNobj, layerN, j, f))
				line << netName << "_L" << layerN << "[" << j << "], ";
		line.seekp(-2, line.cur);
		line << "}),\n\t";
		outFile << line.str();
	}
}