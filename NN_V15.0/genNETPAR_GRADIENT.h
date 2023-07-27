#ifndef MKNETPAR
#define MKNETPAR

#include <string>
#include "NN_DataTypes.h"

using namespace std;

void genNETPAR_GRADIENT(sysConfig sConfig);
void genNETPAR_GRADIENT_Inst(std::ostream &outFile, sysConfig sConfig);

#endif