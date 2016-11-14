#ifndef GLOBALBRANCHPREDICTOR_H_
#define GLOBALBRANCHPREDICTOR_H_

class GlobalBranchPredictor{
private:
	unsigned char GlobalHistoryRegister;
	unsigned char PatternHistoryTable[256];
	int totalBranch;
	int totalHit;
public:
	GlobalBranchPredictor();
	bool branchPredictionDecision();
	void updatePredictor(bool branchResult, bool branchPredictionDecision);
	double hitRatio();
	~GlobalBranchPredictor();
};

#endif //GLOBALBRANCHPREDICTOR_H_