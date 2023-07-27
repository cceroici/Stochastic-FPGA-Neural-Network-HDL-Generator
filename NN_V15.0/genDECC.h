#ifndef MKDEC
#define MKDEC

#include <string>
#include "NN_DataTypes.h";

using namespace std;

void genDECC(sysConfig sConfig);
void genDECC_Inst(std::ostream &outFile, sysConfig sConfig);
void genDECC_NodeOutputExport(std::ostream &outFile, sysConfig sConfig);

#endif
