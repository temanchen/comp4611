#include <iostream>
#include "LocalBranchPredictor.h"

using namespace std;

LocalBranchPredictor::LocalBranchPredictor()
{
    totalBranch = 0;
    totalHit = 0;
    for(int i=0; i<10; ++i){
        LocalHistoryRegisters[i] = 0x0;
        for(int j=0; j<256; ++j){
            PatternHistoryTables[i][j] = 0x0;
        }
    }
}

bool LocalBranchPredictor::branchPredictionDecision(unsigned int PC)
{
    ++totalBranch;
    unsigned int idx = PC % 10;
    return PatternHistoryTables[idx][LocalHistoryRegisters[idx]] >= 2;
}

void LocalBranchPredictor::updatePredictor(unsigned int PC, bool branchResult, bool branchPredictionDecision)
{
    if(branchResult == branchPredictionDecision) ++totalHit;
    unsigned int idx = PC % 10;
    if(branchResult){
        if(PatternHistoryTables[idx][LocalHistoryRegisters[idx]] < 4){
            ++(PatternHistoryTables[idx][LocalHistoryRegisters[idx]]);
        }
        LocalHistoryRegisters[idx] <<= 1;
        LocalHistoryRegisters[idx] |= 0x1;
    }else{
        if(PatternHistoryTables[idx][LocalHistoryRegisters[idx]] > 0){
            --(PatternHistoryTables[idx][LocalHistoryRegisters[idx]]);
        }
        LocalHistoryRegisters[idx] <<= 1;
    }
}

double LocalBranchPredictor::hitRatio()
{
    return (totalBranch == 0 ? 0.0 : 1.0 * totalHit / totalBranch);
}

LocalBranchPredictor::~LocalBranchPredictor()
{

}