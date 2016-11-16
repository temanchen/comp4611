#include <iostream>
#include <cstdio>
#include "MIPSComputer.h"

using namespace std;

// inst opcode/funct (hex)
// R type: add 0/20, sub 0/22, and 0/24,
//         or 0/25, sll 0/0, srl 0/2, slt 0/2a
// sll/srl: shift left/right logical
// slt: set less than

// I type: addi 8, lw 23, sw 2b, beq 4, bne 5
// J type: j 2
#ifdef DEBUG
#define DEBUG_LOG(args...) printf(args...)
#else
#define DEBUG_LOG(args...) do {} while(0);
#endif

#define RTYPE 0
#define SLL 0x0
#define SRL 0x2
#define ADD 0x20
#define SUB 0x22
#define AND 0x24
#define OR 0x25
#define SLT 0x2a

#define BEQ 0x4
#define BNE 0x5
#define ADDI 0x8
#define LW 0x23
#define SW 0x2b

#define JUMP 0x2

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
         | Memory[PC]; //Little-Endian   
    PC+=4;
    while(Ins){
        //fill in the following values
        op = (Ins >> 26) & 0x3f; //IR{31..26}
        rs = (Ins >> 21) & 0x1f; //IR{25..21}
        rt = (Ins >> 16) & 0x1f; //IR{20..16}
        rd = (Ins >> 11) & 0x1f; //IR{15..11}
        sft = (Ins >> 6) & 0x1f; //IR{10..6}
        func = Ins & 0x3f; //IR{5..0}
        dat = Ins & 0xffff; //IR{15..0}
        if(dat & 0x8000) dat |= 0xffff0000; // sign extension
        adr = Ins & 0x3ffffff; //IR{25..0}

        cout << "Current Instruction: ";
        switch(op){
            case RTYPE:
                switch(func){
                    case ADD:
                        cout << "add" << endl;
                        Reg[rd] = Reg[rs] + Reg[rt];
                        break;
                    case SUB:
                        cout << "sub" << endl;
                        Reg[rd] = Reg[rs] - Reg[rt];
                        break;
                    case AND:
                        cout << "and" << endl;
                        Reg[rd] = Reg[rs] & Reg[rt];
                        break;
                    case OR:
                        cout << "or" << endl;
                        Reg[rd] = Reg[rs] | Reg[rt];
                        break;
                    case SLL:
                        cout << "sll" << endl;
                        Reg[rd] = Reg[rs] << sft;
                        break;
                    case SRL:
                        cout << "srl" << endl;
                        Reg[rd] = Reg[rs] >> sft;
                        break;
                    case SLT:
                        cout << "slt" << endl;
                        Reg[rd] = (Reg[rs] < Reg[rt] ? 0x1 : 0x0);
                        break;
                    default:
                        cerr << "Unsupported R-type Instruction" << endl;
                        return -1;
                }
                break;

            case ADDI:
                cout << "addi" << endl;
                Reg[rt] = Reg[rs] + dat;
                break;
            case LW:
            {
                cout << "lw" << endl;
                int addr = Reg[rs] + dat;
                Reg[rt] = (Memory[addr + 3] << 24) |
                          (Memory[addr + 2] << 16) |
                          (Memory[addr + 1] << 8) |
                          (Memory[addr]);
                break;
            }
            case SW:
            {
                cout << "sw" << endl;
                int addr = Reg[rs] + dat;
                int data = Reg[rt];
                for(int i=0; i<4; ++i){
                    Memory[addr + i] = (unsigned char)(data & 0xff);
                    data >>= 8;
                }
                break;
            }
            case BEQ:
            {
                cout << "beq" << endl;
                globalPredictionDecision = globalPredictor.branchPredictionDecision();
                localPredictionDecision = localPredictor.branchPredictionDecision(PC-4);
                unsigned int oldPC = PC;

                if(Reg[rs] == Reg[rt])
                    PC+=(dat<<2);

                branchResult = Reg[rs]==Reg[rt];
                globalPredictor.updatePredictor(branchResult, globalPredictionDecision);
                localPredictor.updatePredictor(oldPC-4, branchResult, localPredictionDecision);
                break;
            }
            case BNE:
            {
                cout << "bne" <<endl;
                globalPredictionDecision = globalPredictor.branchPredictionDecision();
                localPredictionDecision = localPredictor.branchPredictionDecision(PC-4);
                unsigned int oldPC = PC;

                if(Reg[rs] != Reg[rt])
                    PC+=(dat<<2);

                branchResult = Reg[rs]!=Reg[rt];
                globalPredictor.updatePredictor(branchResult, globalPredictionDecision);
                localPredictor.updatePredictor(oldPC-4, branchResult, localPredictionDecision);
                break;
            }
            case JUMP:
                cout << "j" << endl;
                PC = (PC & 0xf0000000) | (adr << 2);
                break;
            default:
                cerr << "(I or J)Instruction Error!" << endl;
                return -1;
        }
        #ifdef DEBUG
        printRegisters();
        #endif
        Ins = (Memory[PC+3]<<24)
             |(Memory[PC+2]<<16)
             |(Memory[PC+1]<<8)
             | Memory[PC]; //Little-Endian   
        PC+=4;
    }
    cout << "All instructions have been executed!" << endl << endl;
    return 0;
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
