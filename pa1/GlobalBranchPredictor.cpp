#include <iostream>
#include "GlobalBranchPredictor.h"

using namespace std;

GlobalBranchPredictor::GlobalBranchPredictor()
{
    totalBranch = 0;
    totalHit = 0;
    GlobalHistoryRegister = 0x0;
    for(int i=0; i<256; ++i){
        PatternHistoryTable[i] = 0x0;
    }
}

bool GlobalBranchPredictor::branchPredictionDecision()
{
    ++totalBranch;
    return PatternHistoryTable[GlobalHistoryRegister] >= 2;
}

void GlobalBranchPredictor::updatePredictor(bool branchResult, bool branchPredictionDecision)
{
    if(branchResult == branchPredictionDecision) ++totalHit;
    if(branchResult){
        if(PatternHistoryTable[GlobalHistoryRegister] < 4){
            ++(PatternHistoryTable[GlobalHistoryRegister]);
        }
        GlobalHistoryRegister <<= 1;
        GlobalHistoryRegister |= 0x1;
    }else{
        if(PatternHistoryTable[GlobalHistoryRegister] > 0){
            --(PatternHistoryTable[GlobalHistoryRegister]);
        }
        GlobalHistoryRegister <<= 1;
    }
}

double GlobalBranchPredictor::hitRatio()
{
    return (totalBranch == 0 ? 0.0 : 1.0 * totalHit / totalBranch);
}

GlobalBranchPredictor::~GlobalBranchPredictor()
{

}