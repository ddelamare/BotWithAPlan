#pragma once
using namespace sc2;

enum BattleMode {
	Attack,
	Retreat,
	Defend,
	Drop,
	Harrass
};

struct BattleGroup
{
	BattleGroup() {}
	// Hold the mode the group is currently in.
	// Could be Attack, Retreat, Defend
	BattleMode mode;
	Units units;
	Point2D target;
	bool isClustered;
};
															   
