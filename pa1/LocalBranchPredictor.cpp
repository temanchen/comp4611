#include <iostream>
#include "LocalBranchPredictor.h"

using namespace std;

LocalBranchPredictor::LocalBranchPredictor()
{
	totalBranch = 1;
	totalHit = 0;
}

bool LocalBranchPredictor::branchPredictionDecision(unsigned int PC)
{
	return true;
}

void LocalBranchPredictor::updatePredictor(unsigned int PC, bool branchResult, bool branchPredictionDecision)
{
}

double LocalBranchPredictor::hitRatio()
{
	return 1.0*totalHit/totalBranch;
}

LocalBranchPredictor::~LocalBranchPredictor()
{
}