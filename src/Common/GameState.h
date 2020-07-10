#pragma once
#include <map>
#include "sc2api\sc2_api.h"
#include "Entity\ThreatModel.h"
#include <vector>
using namespace sc2;

// Holds data that only needs to be calculated once and then referenced or updated

typedef std::map<UNIT_TYPEID, int>	UnitMap;
typedef std::map<sc2::Tag, const Unit*>	UnitCache;
class ArmyManager;
struct BattleGroup;
struct GameState
{
	GameState() {
		ScoutingUnits = Units();
		BuildingWorkers = Units();
		ExpansionLocations = std::vector<Point3D>();
		CurrentUnits = UnitMap();
		MaxEnemyUnits = UnitMap();
		PrecomputedStartLocations = std::vector<std::pair<Point3D, Point3D>>();
	}
	Units ScoutingUnits;
	Units BuildingWorkers;
	std::vector<Point3D> ExpansionLocations;
	Point3D MineralDirection;
	Point2D EnemyBase;
	Point2D StartingLocation;
	UnitMap CurrentUnits;
	UnitMap MaxEnemyUnits;
	ArmyManager* ArmyManager;
	bool HasCloakedUnits;
	UnitTypes UnitInfo;
	int MaxEnemyFood;
	Race selfRace;
	Race opponentRace;
	ThreatModel threat;
	int KilledScouts = 0;
	std::vector<std::pair<Point3D, Point3D>> PrecomputedStartLocations;
	UnitCache EnemyUnits;
	std::vector<AvailableAbilities> AvailableAffordableAbilties;
	float averageUnspentResources = 0;
	float averageUnspentResourcesTotal = 0;
	int resourceSamples = 0;

};