#ifndef MKTOP
#define MKTOP

#include <string>
#include "NN_DataTypes.h"

using namespace std;

void genTOP(sysConfig sConfig);
void genTOP_Quartus(sysConfig sConfig, string QuartusProjectName);
void genTOP_uBlaze(sysConfig sConfig);

void DeclareTopNets(std::ostream &outFile, sysConfig sConfig, bool Simulation);
void DeclareParameters(std::ostream &outFile, sysConfig sConfig);
void ControlLogic(std::ostream &outFile, sysConfig sConfig);
void QuartusPorts(std::ostream &outFile, sysConfig sConfig);
void NiosModule_Inst(std::ostream &outFile, sysConfig sConfig);
void uBlazeModule_Inst(std::ostream &outFile, sysConfig sConfig);

#endif