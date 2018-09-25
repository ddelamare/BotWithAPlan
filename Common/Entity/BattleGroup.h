#pragma once

enum BattleMode {
	Attack,
	Retreat,
	Defend,
	Drop
};

struct BattleGroup
{
	BattleGroup() {}
	// Hold the mode the group is currently in.
	// Could be Attack, Retreat, Defend
	BattleMode mode;
	Units units;
	Point2D target;
};

