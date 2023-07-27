#include <Windows.h>
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <io.h>
#include <direct.h>
#include "BitOps.h"

using namespace std;


double roundval(double n){
	double N;
	double t = n-floor(n);
	if (t>= 0.5) 
		N = ceil(n);
	else 
		N = floor(n);
	return N;
}          

string Dec2Bin(int n)
{
	if (n== 0) 
		return "0";
	if (n== 1) 
		return "1";
	if (n%2==0)
		return Dec2Bin(n/2) + "0";
	else
		return Dec2Bin(n/2) + "1";
}

string PadZeros(string A,int L){
	string B;
	int SL = A.length();
	int D = L-SL;
	for (int i=0;i<D;i++)
		B = B + "0";
	B = B + A;
	return B;
}


void PrintMatrix(int** A, int w, int h) {

	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++)
			cout << A[i, j] << " ";
		cout << "\n";
	}
}

int SumVector(int* A, int size) {
	int sum = 0;
	for (int i = 0; i < size; i++) {
		sum += A[i];
	}
	return sum;
}