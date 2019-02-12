#pragma once
#include "Common\GameState.h"
#include "Common\Resource.h"	  // LOG
#include "rapidjson\include\rapidjson\document.h"						 
#include "rapidjson\include\rapidjson\writer.h"
#include "rapidjson\include\rapidjson\stringbuffer.h"
#include "Common\Entity\UnitBattleData.h"
#include <fstream>
#include <sstream>
using namespace std;
class BattleAnalyzer
{
	// These will store the unit currently in battle 
	UnitMap leftSideUnits;
	UnitMap rightSideUnits;
	static std::map<int, UnitBattleData> unitData;
public:
	BattleAnalyzer(string filepath);
	~BattleAnalyzer();


	double GetRelativeStrength(UnitTypeID leftUnit, UnitTypeID right_Unit, GameState* state);
	Weapon* GetBestWeapon(UnitTypeData* leftUnit, UnitTypeData* rightUnit, GameState* state);
	double CalculateWeaponHitDamage(Weapon* weapon, UnitTypeData* enemyType);
	int GetEstimatedAttacksToKill(double damage, UnitTypeData * enemyType);
	void LoadConfigFromFile(string filepath, bool forceClear);
	int PredictWinner(double lhsRelStr, int lhsCount, double rhsRelStr, int rhsCount, UnitTypeData* lhsUnit, UnitTypeData* rhsUnit);
	double PredictSurvivors(double lhsRelStr, int lhsCount, double rhsRelStr, int rhsCount, UnitTypeData* lhsUnit, UnitTypeData* rhsUnit);
	double CalcuateExponent(UnitTypeData * lhsUnit, int lhsCount);
	double PredictSurvivorsAlt(double lhsRelStr, int lhsCount, double rhsRelStr, int rhsCount);
};

