#ifndef MKCSTMOD
#define MKCSTMOD

#include <string>
#include "NN_DataTypes.h"

using namespace std;

void genCOSTMOD(sysConfig sConfig);
void genCOSTMOD_Inst(std::ostream &outFile, sysConfig sConfig);

#endif