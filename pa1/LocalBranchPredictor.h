#ifndef LOCALBRANCHPREDICTOR_H_
#define LOCALBRANCHPREDICTOR_H_

class LocalBranchPredictor{
private:
	unsigned char LocalHistoryRegisters[10];
	unsigned char PatternHistoryTables[10][256];
	int totalBranch;
	int totalHit;
public:
	LocalBranchPredictor();
	bool branchPredictionDecision(unsigned int PC);
	void updatePredictor(unsigned int PC, bool branchResult, bool branchPredictionDecision);
	double hitRatio();
	~LocalBranchPredictor();
};

#endif //LOCALBRANCHPREDICTOR_H_