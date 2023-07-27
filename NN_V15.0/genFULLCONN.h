#ifndef MKFLCN
#define MKFLCN

#include <string>
#include "NN_DataTypes.h"

using namespace std;

void genFULLCONN_FWD_block(sysConfig sConfig, int L);
void genFULLCONN_BP_block(sysConfig sConfig, int L);

void genFULLCONN_FWD_block_Inst(std::ostream &outFile, sysConfig sConfig, int L, int RNGindex, int T);
void genFULLCONN_BP_block_Inst(std::ostream &outFile, sysConfig sConfig, int L, int RNGindex, int T);

#endif