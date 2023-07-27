#ifndef MKSTOCH
#define MKTOCH

#include <string>
#include "NN_DataTypes.h"

using namespace std;

string makeSeed(int L);
void genSTOCH(sysConfig sConfig);
void genSTOCH_Inst(std::ostream &outFile, sysConfig sConfig);
#endif