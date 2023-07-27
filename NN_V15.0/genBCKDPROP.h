#ifndef MKBCKPRP
#define MKBCKPRP

#include <string>
#include "NN_DataTypes.h"

using namespace std;

void genBCKDPROP(sysConfig sConfig);
void gradientCalculationBlocks(std::ostream &outFile, sysConfig sConfig);
void genBCKPROP_Inst(std::ostream &outFile, sysConfig sConfig);
#endif