#ifndef MKUBLZS
#define MKUBLZS

#include <string>
#include "NN_DataTypes.h"

using namespace std;

void genUBlazeSim(sysConfig sConfig);
void genUBlazeSim_Inst(std::ostream &outFile, sysConfig sConfig);

#endif