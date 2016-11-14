#ifndef MIPSCOMPUTER_H_
#define MIPSCOMPUTER_H_
#define MAXMEM 8192
#include "GlobalBranchPredictor.h"
#include "LocalBranchPredictor.h"   

class MIPSComputer{ 
private:
	unsigned char Memory[MAXMEM];   
	int Reg[32];         
	unsigned int PC;             
	GlobalBranchPredictor globalPredictor;
	LocalBranchPredictor localPredictor;
public: 
	MIPSComputer();       
	void boot(char* file);     
	int run();    
	void printRegisters();
	void printBranchPredictionResult();
	~MIPSComputer();      
};

#endif  //MIPSCOMPUTER_H_