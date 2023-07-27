#ifndef MKVLOG
#define MKVLOG

#include "NN_DataTypes.h"

enum DeclarationTypes { TopLevel, ModuleInstantiationPorts, ModuleDeclarationPortList, ModuleDeclarationPortsInput, ModuleDeclarationPortsOutput};
enum ParameterTypes { miscellaneous, miscellaneousDec,
					  alphaStoch, betaStoch, alphaSign, betaSign, alphaDec, betaDec,
					  gammaStoch, gammaSign, gammaDec, 
					  alpha_lstmStoch, beta_lstmStoch, alpha_lstmDec, beta_lstmDec, alpha_lstmSign, beta_lstmSign,
					  alphaStochGrad, betaStochGrad, alphaStochGradSign, betaStochGradSign,
					  gammaStochGrad, gammaStochGradSign,
					};
enum ConnectionGeometry { previousLayerWeight, bias, singleLayer, recurrentLayer};
enum NetTypes {wire, reg, wireRecurrent, regRecurrent};

using namespace std;

#include <string>


string Int2Str(int num);
string StringSeries(int N, string preLabel, string postLabel, bool reverse = false);
string StringSeriesBound(int startBound, int endBound, string preLabel, string postLabel, bool reverse = false);
string StringSeriesBoundPort(int startBound, int endBound, string prePortLabel, string postPortLabel, string preLabel, string postLabel, bool reverse);
string StringSeries2(int N1, int N2, string Label1, string Label2, string Label3, string nLine, int nLineT, int COM);
string StringSeries3(int N1, int N2, int N3, string Label1, string Label2, string Label3, string Label4, string nLine, int nLineT, int COM);
string StringSeriesPort(int N, string Pname1, string Pname2, string preLabel, string postLabel, int& counter);
string StringSeriesPort2(int N1, int N2, string Pname1, string Pname2, string Pname3, string Label1, string Label2, string Label3, string nLine, int nLineT, int& counter);
string StringSeriesPort3(int N1, int N2, int N3, string Pname1, string Pname2, string Pname3, string Pname4, string Label1, string Label2, string Label3, string Label4, string nLine, int nLineT, int& counter);
void DeclareNet(std::ostream& outFile, string type, string size, int N, string preLabel, string postLabel);
void DeclareDistributionNets(std::ostream& outFile, string type, int size, int Nnets, int LFSR_size, string preLabel, string postLabel);
void DeclareNetEqual(std::ostream& outFile, string type, string size, int N, string preLabel, string postLabel, string Equal);
void DeclareNetArray(std::ostream& outFile, string type, string size, int NN, int NArr, string Label0, string Label1, string Label2);
void DeclareNetArrayPreserve(std::ostream& outFile, string type, string size, int NN, int NArr, string Label0, string Label1, string Label2);
void NN_DeclareNodeNet(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, string type, string Label0, string Label1);
void NN_DeclareTransferNodeNet(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, string Label0, string Label1, string netType, bool single);
string NN_StringSeriesPort3(int LYRSIZE[10], int startLayer, int endLayer, string Pname1, string Pname2, string Pname3, string Pname4, string Label1, string Label2, string Label3, string Label4, string nLine, int nLineT, int& counter, bool reverseIndex);
string NN_StringSeriesPort2(int LYRSIZE[10], int startLayer, int endLayer, string Pname1, string Pname2, string Pname3, string Label1, string Label2, string Label3, string nLine, int nLineT, int& counter);
string NN_StringSeriesPort(int LYRSIZE[10], int startLayer, int endLayer, string Pname1, string Pname2, string Label1, string Label2);
string NN_StringSeries2(int LYRSIZE[10], int startLayer, int endLayer, string Label1, string Label2, string Label3, string nLine, int nLineT);
string NN_CV_StringSeries2(int LYRSIZE[10], int startLayer, int endLayer, int FilterSize, string Label1, string Label2, string Label3, string nLine, int nLineT);
string NN_StringSeries(int LYRSIZE[10], int startLayer, int endLayer, string Label1, string Label2);
string NN_StringSeries3(int LYRSIZE[10], int startLayer, int endLayer, string Label1, string Label2, string Label3, string Label4, string nLine, int nLineT);
void NN_DeclareNodeNetArray(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, string type, string Label0, string Label1, string Label2);
void NN_CV_DeclareNodeNetArray(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, int FilterSize, string type, string Label0, string Label1, string Label2);
void NN_DeclareNodeNetArray3(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, string type, string size, string Label0, string Label1, string Label2, string Label3, int = 0);
void NN_DeclareNodeNetArray2(std::ostream& outFile, int LYRSIZE[10], int startLYR, int endLYR, string type, string size, string Label0, string Label1, string Label2, int = 0);
void NN_DeclareNodeNetArray_alpha(std::ostream& outFile, struct sysConfig sConfig, int LYRSIZE[10], int startLYR, int endLYR, string type, string nodeName, bool = false);

// Write Generic NN paramter nets. Options: | PortDeclaration  net0, net1,\n  | PortInstantiation .net(net) |
//void NN_DeclareNodeNetArray_generic(std::ostream& outFile, struct sysConfig sConfig, int LYRSIZE[10], int startLYR, int endLYR, string type, string nodeName, bool = false, bool = false);
//void NN_DeclareNodeNetArray_genericD(std::ostream& outFile, struct sysConfig sConfig, int LYRSIZE[10], int startLYR, int endLYR, string size, string type, string nodeName, bool portDeclaration, bool = false);

void NN_DeclareWeightNets(std::ostream& outFile, sysConfig sConfig, int LYRSIZE[10], int startLYR, int endLYR, NetTypes netType, DeclarationTypes declarationType, bool = false, bool = false, string  = "", string = "");
void NN_DeclareDecimalWeightNets(std::ostream& outFile, sysConfig sConfig, int LYRSIZE[10], int startLYR, int endLYR, NetTypes netType, DeclarationTypes declarationType);

void NN_DeclareGenericNet(std::ostream& outFile, sysConfig sConfig, int startLYR, int endLYR, string netLabel, NetTypes netType, DeclarationTypes declarationType, ConnectionGeometry connectionGeometry, bool isRecurrent, int = -1);
void NN_DeclareGenericDecimalNet(std::ostream& outFile, sysConfig sConfig, int startLYR, int endLYR, string netLabel, NetTypes netType, string precision, DeclarationTypes declarationType, ConnectionGeometry connectionGeometry, bool isRecurrent);
string NN_DeclareNetLine(int layerN, struct NN NNobj, int FilterSize, string termination, string netName, NetTypes netType, string Label0, string Label1, ParameterTypes parameterType, DeclarationTypes declarationType, ConnectionGeometry connectionGeometry, string = "DP_in", int = -1, string = "");
string NN_DeclareRecurrentNetLine(int layerN, struct NN NNobj, int FilterSize, string termination, string netName, NetTypes netType, string Label0, string Label1, ParameterTypes parameterType, DeclarationTypes declarationType, ConnectionGeometry connectionGeometry, string = "DP_in", int = 0, int = -1);
void GetNetLineConditions(string &netLabel, bool &isStochastic, ParameterTypes &parameterType, ConnectionGeometry &connectionGeometry);
void TerminateLine(std::ostream& outFile, string line, int cursorOffset, string terminationText);
void NN_DeclareParameterMaskNets(std::ostream& outFile, sysConfig sConfig, DeclarationTypes declarationType);
#endif