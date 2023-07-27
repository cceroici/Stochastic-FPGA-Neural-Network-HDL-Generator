#ifndef MKRNE
#define MKRNE

#include <string>
#include "NN_DataTypes.h";

using namespace std;

void genRNE(sysConfig sConfig);
void genRNE_Inst(std::ostream &outFile, sysConfig sConfig);

#endif
