#include <iostream>
#include <cstdio>
#include "MIPSComputer.h"

using namespace std;

MIPSComputer::MIPSComputer()
{
	Reg[0]=0;    
	cout<<"Welcome!"<<endl;
}

void MIPSComputer::boot(char* file)
{
	FILE * f1;
	unsigned int m;
	int i=0;

	cout<<"MIPS computer is booting..."<<endl;
	if((f1=fopen(file,"rb"))==NULL){ 
		cout<<"Cannot open binary file!"<<endl;
		exit(1);
	}
	cout<<"Binary file is ready to load"<<endl;
	do{
		fread(&m,4,1,f1);
		Memory[i++]=(unsigned char)(m&0xFF);
		Memory[i++]=(unsigned char)((m>>8)&0xFF);
		Memory[i++]=(unsigned char)((m>>16)&0xFF);
		Memory[i++]=(unsigned char)((m>>24)&0xFF);
	}while(m);
	cout<<"Memory initialized!"<<endl;

	if(fclose(f1)){
		cout<<"Cannot close binary file!"<<endl;
		exit(1);
	}

	PC=0;
	cout<<"Boot Done!"<<endl<<endl;
}

int MIPSComputer::run()
{
	cout<<"MIPS computer starts execution..."<<endl;
	int op, rs, rt, rd, dat, adr, sft, func;
	unsigned int Ins;

	bool globalPredictionDecision;
	bool localPredictionDecision;
	bool branchResult;
	
	Ins = (Memory[PC+3]<<24)
		 |(Memory[PC+2]<<16)
		 |(Memory[PC+1]<<8)
		 | Memory[PC+0]; //Little-Endian   
	PC+=4;
	while(Ins){
		//fill in the following values
		op =(Ins>>26)&0x3f; //IR{31..26}
		rs = ; //IR{25..21}
		rt = ; //IR{20..16}
		rd = ; //IR{15..11}
		sft= ; //IR{10..6}
		func= ; //IR{5..0}
		dat= ; //IR{15..0}
		adr= ; //IR{25..0}<<2

		cout<<"Current Instruction: ";
		switch(op){
			//fill in all the missing instructions
			
			case 4: //BEQ
				globalPredictionDecision = globalPredictor.branchPredictionDecision();
				localPredictionDecision = localPredictor.branchPredictionDecision(PC-4);
				unsigned int oldPC = PC;

				if(Reg[rs] == Reg[rt])
					PC+=(dat<<2);

				branchResult = Reg[rs]==Reg[rt];
				globalPredictor.updatePredictor(branchResult, globalPredictionDecision);
				localPredictor.updatePredictor(oldPC-4, branchResult, localPredictionDecision);
				cout<<"beq"<<endl;
				break;
			case 5: //BNE
				globalPredictionDecision = globalPredictor.branchPredictionDecision();
				localPredictionDecision = localPredictor.branchPredictionDecision(PC-4);
				unsigned int oldPC = PC;

				if(Reg[rs] != Reg[rt])
					PC+=(dat<<2);

				branchResult = Reg[rs]!=Reg[rt];
				globalPredictor.updatePredictor(branchResult, globalPredictionDecision);
				localPredictor.updatePredictor(oldPC-4, branchResult, localPredictionDecision);
				cout<<"bne"<<endl;
				break;
			
			default:
				cout<<"(I or J)Instruction Error!"<<endl;
				return -1;
		}
		Ins = (Memory[PC+3]<<24)
			 |(Memory[PC+2]<<16)
			 |(Memory[PC+1]<<8)
			 | Memory[PC+0]; //Little-Endian   
		PC+=4;
	}
	cout<<"All instructions have been executed!"<<endl<<endl;
}

void MIPSComputer::printRegisters()
{
	cout<<"Registers' values:"<<endl<<endl;

	for(int i=0;i<32;i++){
		cout<<"Reg"<<i<<": ";
        	cout<<dec<<Reg[i];
		cout<<" || ";
		if((i+1)%4==0)
			cout<<endl;
	}
	cout<<endl;
}

void MIPSComputer::printBranchPredictionResult()
{
	cout<<"Hit ratio of global branch predictor: "<<globalPredictor.hitRatio()<<endl;
	cout<<"Hit ratio of local branch predictor: "<<localPredictor.hitRatio()<<endl;
}

MIPSComputer::~MIPSComputer()
{
	cout<<"MIPS computer shuts down!"<<endl;
}
