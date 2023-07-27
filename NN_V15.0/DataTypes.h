#ifndef MKDATTYPE
#define MKDATTYPE


using namespace std;

struct state{
	int Level; // level index
	int Snum; // state number within level
	string type; // state type: abstract or producer
	int target; // target level if abstract or symbol produced
};
/*
struct transition{
	int prob; // transition probability
	int sourceS; // source state
	int destS; // destination state
	int Lindex; // level index
};
*/
#endif