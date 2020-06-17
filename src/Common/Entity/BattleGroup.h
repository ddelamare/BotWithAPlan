#pragma once
using namespace sc2;

enum class BattleMode {
	Attack,
	Retreat,
	Defend,
	Drop,
	Harrass
};

struct BattleGroup
{
	// Hold the mode the group is currently in.
	// Could be Attack, Retreat, Defend
	BattleMode mode = BattleMode::Defend;
	Units units;
	Point2D target;
	bool isClustered = false;
};
															   
