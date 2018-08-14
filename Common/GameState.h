#pragma once
#include <map>
#include "sc2api\sc2_api.h"
#include <vector>
#include <map>
using namespace sc2;

// Holds data that only needs to be calculated once and then referenced or updated

typedef std::map<UNIT_TYPEID, int>	UnitMap;


struct GameState
{
	GameState() {
		ScoutingProbes = Units();
		BuildingProbes = Units();
		ExpansionLocations = std::vector<Point3D>();
		ObservedUnits = UnitMap();
	}
	Units ScoutingProbes;
	Units BuildingProbes;
	std::vector<Point3D> ExpansionLocations;
	Point3D MineralDirection;
	Point2D EnemyBase;
	UnitMap ObservedUnits;

};