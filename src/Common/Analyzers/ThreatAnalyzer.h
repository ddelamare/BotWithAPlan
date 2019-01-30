#pragma once
#include "Common\Entity\ThreatModel.h"
class ThreatAnalyzer
{
public:
	ThreatAnalyzer();
	double GetThreat(ThreatModel* model);
};

