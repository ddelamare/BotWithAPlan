#pragma once

#include "ThreatAnalyzer.h"



ThreatAnalyzer::ThreatAnalyzer()
{
}

// TODO: Possibly include extra for town halls and workers
// TODO: include compared armys 
double ThreatAnalyzer::GetThreat(ThreatModel* model)
{
	auto enemyCombinedScore = model->enemyMineralsLost + model->enemyGasLost + (.1*model->enemyBuildTimeLost);
	enemyCombinedScore /= (model->enemyBases + 1);
	auto selfCombinedScore = model->mineralsLost + model->gasLost + (.1* model->buildTimeLost);
	selfCombinedScore /= (model->friendlyBases + 1);
	auto diff = enemyCombinedScore - selfCombinedScore;
	// if diff is in ther eney's favor, score will be less than 1
	return 1.0 + (diff / ((model->gameTime + 1.0) *.25));
}

