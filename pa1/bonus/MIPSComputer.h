#ifndef MIPSCOMPUTER_H_
#define MIPSCOMPUTER_H_
#define MAXMEM 8192
#include "../GlobalBranchPredictor.h"
#include "../LocalBranchPredictor.h"   

class MIPSComputer{ 
private:
    unsigned char Memory[MAXMEM];   
    int Reg[32];         
    unsigned int PC;             
    GlobalBranchPredictor globalPredictor;
    LocalBranchPredictor localPredictor;

    struct IFIDReg
    {
        unsigned int Ins;
        unsigned int PC;
    };

    struct IDEXReg
    {
        int ALUSrcB; // 0 Reg[rt], 1 sft, 2 SignExtImm 
        int ALUCtrl; // add, sub, and, or, sll, srl, slt, beq, bne
        int PCSrc; // 0 PC+4, 1 branch target, 2 jump target
        bool MemRead;
        bool MemWrite;
        bool RegWrite;
        int Ra;
        int Rb;
        int rd;
        int sft;
        int imm;
        int addr;
        unsigned int PC;
    };

    struct EXMEMReg
    {
        int rd;
        int ALUOut;
        int Rb;
        bool MemRead;
        bool MemWrite;
        bool RegWrite;
    };

    struct MEMWBReg
    {
        int rd;
        int data;
        bool RegWrite;
    };

    IFIDReg insFetch(unsigned int PC);
    IDEXReg insDecode(const IFIDReg &if_id_reg);
    int ALU(int A, int B, int ALUCtrl);
    EXMEMReg execution(const IDEXReg &id_ex_reg);
    MEMWBReg memAccess(const EXMEMReg &ex_mem_reg);
    void writeBack(const MEMWBReg &reg);

public: 
    MIPSComputer();       
    void boot(char* file);     
    int run();    
    void printRegisters();
    void printBranchPredictionResult();
    ~MIPSComputer();      
};

#endif  //MIPSCOMPUTER_H_