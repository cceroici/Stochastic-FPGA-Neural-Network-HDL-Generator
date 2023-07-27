/* Chris Ceroici
August 14 2013
Generates a Verilog File with binary seeds for LFSRs used in LDPC decoder
*/

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <ctime>
#include <time.h>
#include <sstream>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include "genSEED.h"
#include "BitOps.h"
#include "NN_DataTypes.h"
#include "Verilog.h"

using namespace std;

string makeSeed(int L);

void genSEED(sysConfig sConfig){
	// NS: number of seeds (# of children x # of transitions)
	srand(time(0));

	int S = 4; // number of unique seed sets

	ofstream outFile;
	outFile.open(sConfig.Dir + "SEED.v");
	outFile<<"// Chris Ceroici\n"<<endl;
	outFile<<"// LFSR seed signals for "<< sConfig.LFSR_S<<" bit LFSRs\n\n";
	outFile<<"module SEED(INIT,";
	for (int i=0;i<sConfig.RandNums.Seeds -1;i=i+2) // i=i+2 : alternaate to mix up seed pairs (using same LFSR)
		outFile<<"S"<<i<<",";
	for (int i = 1; i<sConfig.RandNums.Seeds - 1; i = i + 2)
		outFile << "S" << i << ",";
	outFile<<"S"<< sConfig.RandNums.Seeds -1<<");\n\nparameter LFSR_S = "<< sConfig.LFSR_S<<"; // LFSR size\n\n";
	outFile<<"input INIT; // Initialization signal (new set)\n\n";
	outFile<<"output [LFSR_S-1:0] ";
	for (int i=0;i<sConfig.RandNums.Seeds -1;i++)
		outFile<<"S"<<i<<",";
	outFile<<"S"<< sConfig.RandNums.Seeds -1<<";\n\n";
	outFile<<"integer n1 = 0; // Set counter (Decoder)\n";
	outFile<<"integer n2 = 0; // Set counter (Channel simulator)\n";
	outFile<<"integer ndiff = 1; // differential counter\n\n";

	outFile<<"// Set 1 Seeds: \n";
	int j=0;
	for (int i=0;i<sConfig.RandNums.Seeds;i++){
		//outFile<<"assign S"<<i<<"[LFSR_S-1:0] = "<<LFSR_S<<"'b"<<makeSeed(LFSR_S)<<endl;
		outFile<<"reg [LFSR_S-1:0] S"<<i<<"reg = "<< sConfig.LFSR_S<<"'b"<<makeSeed(sConfig.LFSR_S)<<";\n";
	}
	outFile<<"\n";
	for (int i=0;i<sConfig.RandNums.Seeds;i++){
		//outFile<<"assign S"<<i<<"[LFSR_S-1:0] = "<<LFSR_S<<"'b"<<makeSeed(LFSR_S)<<endl;
		outFile<<"assign S"<<i<<"[LFSR_S-1:0] = "<<"S"<<i<<"reg;\n";
	}

	outFile<<"\nalways @(posedge INIT) begin\n\n";

	outFile << "\tif ((n1==" << S << ")&(ndiff!=" << S << ")) begin\n";
	outFile << "\t\tn1 = 1'b1;\n";
	outFile << "\t\tndiff = ndiff + 1'b1;\n";
	outFile << "\tend\n";
	outFile << "\telse if (n1==" << S << ") begin\n";
	outFile << "\t\tn1 = 1'b1;\n";
	outFile << "\t\tndiff = 1'b1;\n";
	outFile << "\tend\n";
	outFile << "\telse n1 = n1+1'b1;\n\n";

	outFile << "\tif (n1==1) n2 = ndiff;\n";
	outFile << "\telse if (n2==" << S << ") n2 = 1;\n";
	outFile << "\telse n2 = n2 + 1'b1;\n\n";

	for (int i = 0;i<S;i++){
		outFile<<"\tif (n1=="<<i+1<<") begin\n";
		for (int j=0;j<sConfig.RandNums.Seeds /2;j++){
			outFile<<"\t\tS"<<j<<"reg [LFSR_S-1:0] <= "<< sConfig.LFSR_S<<"'b"<<makeSeed(sConfig.LFSR_S)<<";\n";
		}
		outFile<<"\tend\n";
		outFile<<"\tif (n2=="<<i+1<<") begin\n";
		for (int j= sConfig.RandNums.Seeds /2;j<sConfig.RandNums.Seeds;j++){
			outFile<<"\t\tS"<<j<<"reg [LFSR_S-1:0] <= "<< sConfig.LFSR_S<<"'b"<<makeSeed(sConfig.LFSR_S)<<";\n";
		}
		outFile<<"\tend\n";
	}
	
	outFile<<"end\n";
	outFile<<"endmodule";
	outFile.close();
}


string makeSeed(int L){
	string seed;
	string t;
	double u=0;
	while (u==0){    // generate nonzero #
		u = (double) rand()/RAND_MAX;
		u = u*1; // ************************************************************
		
		u = roundval(u*(pow(2.0,L)-1));
		//cout << L << ": " << u << endl;
	}
	int r1 = 0;
	int r2 = 0;
	seed = PadZeros(Dec2Bin(u),L);
	for (int i=0;i<L;i++){   // swap bits
		r1 = 0;
		r2 = 0;
		while(r1==r2){       // generate two different #s
			r1 = (double) rand()/RAND_MAX*(L-1);
			r2 = (double) rand()/RAND_MAX*(L-1);
		}
		t = seed[r1];       // swaps bits at random locations
		seed[r1] = seed[r2];
		if (t=="0")
			seed[r2] = '0';
		else
			seed[r1] = '1';
	}
	return seed;
}

int makeSeedDec(int L) {

	double u = 0;
	while (u == 0) {    // generate nonzero #
		u = (double)rand() / RAND_MAX;
		u = u*0.6;
		u = roundval(u*pow(2.0, L));
	}
	return u;
}


void genSEED_Inst(std::ostream &outFile, sysConfig sConfig) {
	int negone = -1;
	int Lindex = 0;
	outFile << "//	LFSR Seed files\n";
	outFile << "SEED SEED0(\n\t";
	ofstream outFile_ext;
	outFile_ext.open(sConfig.Dir + "SEED_portDefinitions.v");
	outFile << "`include \"SEED_portDefinitions.v\"\n\t";
	outFile_ext << StringSeriesPort(sConfig.RandNums.Seeds, "S", "", "SD", "", negone) << "\n\t";
	outFile << ".INIT(SDcount[" << 2 * sConfig.LFSR_S - 1 << "])\n";
	outFile << ");\n\n";
	outFile_ext.close();
}


