#pragma once
#include "Common/Util.h"
#include "Common/UnitHelpers.h"
#include "Common\Entity\BattleGroup.h"
using namespace sc2;

class ArmyManager
{
	const double CLUSTER_DISTANCE_THRESHOLD = 10; // This value determines the threshold for being considered clustered
	const double CLUSTER_PERCENT_THRESHOLD = 0.75; // If this percent of units is close enough, call it clustered
private:
	bool IsClustered(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	void ClusterUnits(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	void AttackTarget(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	bool HasTarget(BattleGroup* group);
	void PartitionGroups(const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	const Unit* FindOptimalTarget(const Unit* unit, const ObservationInterface* obs, QueryInterface* query);

public:
	std::vector<BattleGroup> battleGroups;

	void SetTarget(BattleGroup* group, Point2D location);
	void SetMode(BattleGroup* group, BattleMode mode);
	void ManageGroups(const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
};