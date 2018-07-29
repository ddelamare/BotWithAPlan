#pragma once
#include <map>
#include "sc2api\sc2_api.h"
using namespace sc2;

// Holds data that only needs to be calculated once and then referenced or updated

enum YesNo {
	Undefinded = 0,
	Yes,
	No
};

struct GameState
{
	GameState() {
		ScoutingProbes = Units();
		BuildingProbes = Units();
	}
	Units ScoutingProbes;
	Units BuildingProbes;
	Point3D MineralDirection;
	Point2D EnemyBase;
};