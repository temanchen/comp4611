#include <iostream>
#include <cstdlib>
#include "MIPSComputer.h"
using namespace std;

int main(int argc, char **argv)
{
	if(argc!=2)
	{
		cout<<"Usage: "<<argv[0]<<" [binary file]"<<endl;
		exit(1);
	}
	MIPSComputer computer;       
	computer.boot(argv[1]);   
	computer.run();  
	computer.printRegisters();
	computer.printBranchPredictionResult();
	return 0;
}
