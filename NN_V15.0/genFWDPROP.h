#ifndef MKFWDPRP
#define MKFWDPRP

#include <string>
#include "NN_DataTypes.h"

using namespace std;

void genFWDPROP(sysConfig sConfig);
void genFWDPROP_Inst(std::ostream &outFile, sysConfig sConfig);

#endif