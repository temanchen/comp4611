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

//#define DEBUG true

#ifdef DEBUG
#define DEBUG_LOG(args...) printf(args)
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

MIPSComputer::IFIDReg MIPSComputer::insFetch(unsigned int pc)
{
    MIPSComputer::IFIDReg reg;
    reg.Ins = (Memory[pc+3] << 24)
            | (Memory[pc+2] << 16)
            | (Memory[pc+1] << 8)
            | Memory[pc+0];
    reg.PC = pc + 4;
    return reg;
}

MIPSComputer::IDEXReg MIPSComputer::insDecode(const MIPSComputer::IFIDReg &if_id_reg)
{
    MIPSComputer::IDEXReg reg;
    const unsigned int Ins = if_id_reg.Ins;
    const int op = (Ins >> 26) & 0x3f; //IR{31..26}
    const int func = Ins & 0x3f; //IR{5..0}

    cout << "Current instruction: ";
    switch(op){
        case RTYPE:
            switch(func){
                case ADD: cout << "add"; break;
                case SUB: cout << "sub"; break;
                case AND: cout << "and"; break;
                case OR: cout << "or"; break;
                case SLL: cout << "sll"; break;
                case SRL: cout << "srl"; break;
                case SLT: cout << "slt"; break;
                default: cerr << "Unsupported R-type Instruction";
            }
            break;
        case ADDI: cout << "addi"; break;
        case LW: cout << "lw"; break;
        case SW: cout << "sw"; break;
        case BEQ: cout << "beq"; break;
        case BNE: cout << "bne"; break;
        case JUMP: cout << "j"; break;
        default: cerr << "(I or J) Instruction Error!"; exit(-1);
    }
    cout << endl;

    // ALUSrcB
    reg.ALUSrcB = 0; // Reg[rt]
    if(op == RTYPE && (func == SLL || func == SRL))
        reg.ALUSrcB = 1; // sft
    else if(op == LW || op == SW || op == ADDI)
        reg.ALUSrcB = 2; // imm

    // ALU control signal
    reg.ALUCtrl = ADD;
    if(op == RTYPE)
        reg.ALUCtrl = func;
    else if(op == BEQ || op == BNE)
        reg.ALUCtrl = op;

    reg.PCSrc = 0; // PC+4
    if(op == BEQ || op == BNE)
        reg.PCSrc = 1; // branch target
    else if(op == JUMP)
        reg.PCSrc = 2; // jump target

    reg.MemRead = op == LW;
    reg.MemWrite = op == SW;
    reg.RegWrite = !(op == SW || op == BEQ || op == BNE || op == JUMP);

    reg.Ra = Reg[(Ins >> 21) & 0x1f]; //IR{25..21}
    const int rt = (Ins >> 16) & 0x1f; //IR{20..16}
    reg.Rb = Reg[rt];
    reg.rd = (Ins >> 11) & 0x1f; //IR{15..11}
    if(op == ADDI || op == LW || op == SW)
        reg.rd = rt;
    reg.sft = (Ins >> 6) & 0x1f; //IR{10..6}
    reg.imm = Ins & 0xffff; //IR{15..0}
    if(reg.imm & 0x8000) reg.imm |= 0xffff0000; // sign extension
    reg.addr = Ins & 0x3ffffff; //IR{25..0}
    reg.PC = if_id_reg.PC;
    return reg;
}

int MIPSComputer::ALU(int A, int B, int ALUCtrl)
{
    int res;
    switch(ALUCtrl){
        case ADD:
            res = A + B; break;
        case SUB:
            res = A - B; break;
        case AND:
            res = A & B; break;
        case OR:
            res = A | B; break;
        case SLT:
            res = (A < B ? 0x1 : 0x0); break;
        case SLL:
            res = A << B; break;
        case SRL:
            res = A >> B; break;
        case BEQ:
            res = A == B; break;
        case BNE:
            res = A != B; break;
        default:
            cerr << "Unsupported ALU Operation" << endl;
    }
    return res;
}

MIPSComputer::EXMEMReg MIPSComputer::execution(const MIPSComputer::IDEXReg &id_ex_reg)
{
    MIPSComputer::EXMEMReg reg;
    int OpA, OpB;
    OpA = id_ex_reg.Ra;
    switch(id_ex_reg.ALUSrcB){
        case 1:
            OpB = id_ex_reg.sft;
            break;
        case 2:
            OpB = id_ex_reg.imm;
            break;
        default:
            OpB = id_ex_reg.Rb;
    }
    reg.ALUOut = ALU(OpA, OpB, id_ex_reg.ALUCtrl);
    reg.rd = id_ex_reg.rd;
    reg.Rb = id_ex_reg.Rb; // Reg[rt], for sw
    reg.MemRead = id_ex_reg.MemRead;
    reg.MemWrite = id_ex_reg.MemWrite;
    reg.RegWrite = id_ex_reg.RegWrite;

    switch(id_ex_reg.PCSrc){
        case 1: // branch target
        {
            bool globalPredictionDecision = globalPredictor.branchPredictionDecision();
            bool localPredictionDecision = localPredictor.branchPredictionDecision(PC);
            globalPredictor.updatePredictor(reg.ALUOut == 1, globalPredictionDecision);
            localPredictor.updatePredictor(PC, reg.ALUOut == 1, localPredictionDecision);
            if(reg.ALUOut)
                PC = id_ex_reg.PC + (id_ex_reg.imm << 2);
            else
                PC = id_ex_reg.PC;
            break;
        }
        case 2: // jump target
            PC = (id_ex_reg.PC & 0xf0000000) | (id_ex_reg.addr << 2);
            break;
        default: // PC + 4
            PC = id_ex_reg.PC;
    }
    return reg;
}


MIPSComputer::MEMWBReg MIPSComputer::memAccess(const MIPSComputer::EXMEMReg &ex_mem_reg)
{
    MIPSComputer::MEMWBReg reg;
    reg.rd = ex_mem_reg.rd;
    reg.ALUOut = ex_mem_reg.ALUOut;
    reg.MemRead = ex_mem_reg.MemRead;
    reg.RegWrite = ex_mem_reg.RegWrite;
    if(ex_mem_reg.MemWrite){// sw
        int addr = ex_mem_reg.ALUOut;
        int data = ex_mem_reg.Rb;
        for(int i=0; i<4; ++i){
            Memory[addr + i] = data & 0xff;
            data >>= 8;
        }
    }

    if(ex_mem_reg.MemRead){// lw
        int addr = ex_mem_reg.ALUOut;
        reg.data = (Memory[addr + 3] << 24) |
                  (Memory[addr + 2] << 16) |
                  (Memory[addr + 1] << 8) |
                  (Memory[addr]);
    }

    return reg;
}

void MIPSComputer::writeBack(const MIPSComputer::MEMWBReg &reg)
{
    if(reg.RegWrite){
        Reg[reg.rd] = (reg.MemRead ? reg.data : reg.ALUOut);
    }
}

int MIPSComputer::run()
{
    cout << "MIPS computer starts execution ..." << endl;
    while(true){
        MIPSComputer::IFIDReg reg1 = insFetch(PC);
        if(!reg1.Ins) break;
        MIPSComputer::IDEXReg reg2 = insDecode(reg1);
        MIPSComputer::EXMEMReg reg3 = execution(reg2);
        MIPSComputer::MEMWBReg reg4 = memAccess(reg3);
        writeBack(reg4);
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
