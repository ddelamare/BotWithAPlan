#pragma once
#include <cpp-sc2\include\sc2api\sc2_unit.h>
using namespace sc2;

struct KnownEnemyPresence
{
	Units enemies;
	int unitValue;
	Point3D location;
	int lastSeenLoop;
};
