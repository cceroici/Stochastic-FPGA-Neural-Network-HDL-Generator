#ifndef MKSEED
#define MKSEED

#include <string>
#include "NN_DataTypes.h"

using namespace std;

string makeSeed(int L);
int makeSeedDec(int L);
void genSEED(sysConfig sConfig);
void genSEED_Inst(std::ostream &outFile, sysConfig sConfig);
#endif