#pragma once
#include "Common/Util.h"
#include "Common/UnitHelpers.h"
#include "Common\Entity\BattleGroup.h"
#include "Common\Analyzers\ThreatAnalyzer.h"
using namespace sc2;

class ArmyManager
{
	const double CLUSTER_DISTANCE_THRESHOLD_MIN = 10; // This value determines the threshold for being considered clustered
	const double CLUSTER_DISTANCE_THRESHOLD_MAX = 30; // Ignore units too far away from center 
	const double CLUSTER_PERCENT_THRESHOLD_MIN = 0.30; // If clusterinf percentage drops below this mark, flag as unclustered
	const double CLUSTER_PERCENT_THRESHOLD_MAX = 0.50; // Once not clustered, has to reach this percentage to be considered clustered again
	const double CLUSTER_MOVE_THRESHOLD = 2.5; //If unit with this distance of target and has not specfic target, don't move
	std::vector<UnitTypeID> backLineUnits;
	std::vector<std::pair<Point2D, BattleMode>> requestedActions;
private:
	bool IsClustered(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	void ClusterUnits(BattleGroup* group,bool includeAll, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	void AttackTarget(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	bool HasTarget(BattleGroup* group);
	void PartitionGroups(const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	const Unit* FindOptimalTarget(const Unit* unit, const ObservationInterface* obs, QueryInterface* query, GameState* state);
	void SneakToTarget(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	Units cachedEnemyArmy;
	Units cachedEnemies;  
	Units cachedHighPriorityEnemies;
	bool HasThermalLance;
	ThreatAnalyzer threatAnalyzer;
	void SetTarget(BattleGroup* group, Point2D location);
	void SetMode(BattleGroup* group, BattleMode mode);
public:
	std::vector<BattleGroup> battleGroups;
	ArmyManager();

	bool ShouldUnitsRetreat(Units units, Units enemies, GameState* state);


	void RequestAction(Point2D target, BattleMode action);
	void ManageGroups(const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
};