#ifndef MKPRMS
#define MKPRMS

#include <string>
#include "NN_DataTypes.h";

using namespace std;

void genPARAMS_sim(sysConfig sConfig, int& NparameterExports);
void genPARAMS_sim_Inst(std::ostream &outFile, sysConfig sConfig);
void genPARAMS_smoothGradBlockDeclaration(std::ostream &outFile, sysConfig sConfig, int smoothGradSize);
void genPARAMS_parameterExportBlock(std::ostream &outFile, sysConfig sConfig, int& NparameterExports);
void genPARAM_MASK_STORAGE(sysConfig sConfig);
void genPARAM_TRAINING_MASK(sysConfig sConfig);
void genPARAM_MASK_STORAGE_Inst(std::ostream &outFile, sysConfig sConfig);
void genPARAM_TRAINING_MASK_Inst(std::ostream &outFile, sysConfig sConfig);

#endif
