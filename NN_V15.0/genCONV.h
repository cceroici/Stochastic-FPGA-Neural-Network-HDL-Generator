#ifndef MKCNV
#define MKCNV

#include <string>
#include "NN_DataTypes.h"

using namespace std;

void GenerateConvolutionalConnectionMasks(sysConfig sConfig, LYR &LayerInfo, int layerN);
void genCONV_FWD_block(sysConfig sConfig, LYR LayerInfo);
void genCONV_FWD_filter(sysConfig sConfig, LYR LayerInfo, int F, int** Connections);
void genCONVBLOCK_BP_block(sysConfig sConfig, LYR LayerInfo, int** Connections);
void genCONV_BP_filter(sysConfig sConfig, LYR LayerInfo, int F, int** Connections);
void genCONV_FWD_block_Inst(std::ostream &outFile, sysConfig sConfig, int L, int RNGindex, int T);
void genCONV_BP_block_Inst(std::ostream &outFile, sysConfig sConfig, int L, int T);
void CollectConvolutionalNets(std::ostream &outFile, sysConfig sConfig, int layerN, string portName, string netName);
#endif