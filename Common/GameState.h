#pragma once
#include <map>
#include "sc2api\sc2_api.h"
#include <vector>
using namespace sc2;

// Holds data that only needs to be calculated once and then referenced or updated

typedef std::map<UNIT_TYPEID, int>	UnitMap;
class ArmyManager;
class BattleGroup;
struct GameState
{
	GameState() {
		ScoutingUnits = Units();
		BuildingWorkers = Units();
		ExpansionLocations = std::vector<Point3D>();
		ObservedUnits = UnitMap();
		CurrentUnits = UnitMap();
		MaxEnemyUnits = UnitMap();
	}
	Units ScoutingUnits;
	Units BuildingWorkers;
	std::vector<Point3D> ExpansionLocations;
	Point3D MineralDirection;
	Point2D EnemyBase;
	Point2D StartingLocation;
	UnitMap ObservedUnits;
	UnitMap CurrentUnits;
	UnitMap MaxEnemyUnits;
	ArmyManager* ArmyManager;
	bool HasCloakedUnits;
	UnitTypes UnitInfo;
	int MaxEnemyFood;
	Race opponentRace;
};