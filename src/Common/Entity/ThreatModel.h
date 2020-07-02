#pragma once
struct ThreatModel
{
public:
	unsigned int mineralsLost = 0;
	unsigned int gasLost = 0;
	unsigned int buildTimeLost = 0;
	unsigned int knownEnemyValue = 0;
	unsigned int enemyMineralsLost = 0;
	unsigned int enemyGasLost = 0;
	unsigned int enemyBuildTimeLost = 0;
	unsigned int gameTime = 0;
	unsigned int enemyBases = 0;
	unsigned int friendlyBases = 0;
};

