#include <iostream>
#include "GlobalBranchPredictor.h"

using namespace std;

GlobalBranchPredictor::GlobalBranchPredictor()
{
	totalBranch = 1;
	totalHit = 0;
}

bool GlobalBranchPredictor::branchPredictionDecision()
{
	return true;
}

void GlobalBranchPredictor::updatePredictor(bool branchResult, bool branchPredictionDecision)
{
}

double GlobalBranchPredictor::hitRatio()
{
	return 1.0*totalHit/totalBranch;
}

GlobalBranchPredictor::~GlobalBranchPredictor()
{
}