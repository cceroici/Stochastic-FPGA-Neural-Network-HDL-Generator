#ifndef SOURCEF
#define SOURCEF

#include <string>
#include "NN_DataTypes.h"

using namespace std;


void genSource(sysConfig sConfig);
void genRNE_Diag(string Dir, int N);
void genSTOCH_Diag(string Dir, int N);
void genCHILD_Diag(string Dir, int NS, int NC);
void genFIT_Diag(string Dir, int NC);
void genME_Diag(string Dir, int NT, int NC);
void genNNGenericWave(sysConfig sConfig);
void genNNEval(sysConfig sConfig);
void genParameterExport(sysConfig sConfig);
void genMATLABConfigFile(sysConfig sConfig,  int NparameterExports);
void genNiosConfigFile(sysConfig sConfig, int NparameterExports);


#endif