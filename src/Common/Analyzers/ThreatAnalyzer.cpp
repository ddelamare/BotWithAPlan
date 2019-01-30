#include "ThreatAnalyzer.h"



ThreatAnalyzer::ThreatAnalyzer()
{
}

// TODO: Possibly include extra for town halls and workers
// TODO: include compared armys 
double ThreatAnalyzer::GetThreat(ThreatModel* model)
{
	auto enemyCombinedScore = model->enemyMineralsLost + model->enemyGasLost + model->enemyBuildTimeLost;
	auto selfCombinedScore = model->mineralsLost + model->gasLost + model->buildTimeLost;
	auto diff = enemyCombinedScore - selfCombinedScore;
	// if diff is negative, score will be less than 1
	return 1.0 + (diff / ((selfCombinedScore + 1.0) * 4.0));
}

