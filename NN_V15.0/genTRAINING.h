#ifndef MKTRNG
#define MKTRNG

#include <string>
#include "NN_DataTypes.h"

using namespace std;

void genTRAINING(sysConfig sConfig);
void genTRAINING_Inst(std::ostream &outFile, sysConfig sConfig, bool Simulation);

#endif