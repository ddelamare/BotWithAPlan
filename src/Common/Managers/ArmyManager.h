#pragma once
#include "Common/Util/Util.h"
#include "Common/Util/UnitHelpers.h"
#include "Common\Entity\BattleGroup.h"
#include "Common\Analyzers\ThreatAnalyzer.h"
#include "Common/Entity/KnownEnemyPresence.h"
using namespace sc2;

class ArmyManager
{
	const double CLUSTER_DISTANCE_THRESHOLD_MIN = 15; // This value determines the threshold for being considered clustered
	const double CLUSTER_DISTANCE_THRESHOLD_MAX = 30; // Ignore units too far away from center 
	const double CLUSTER_PERCENT_THRESHOLD_MIN = 0.30; // If clusterinf percentage drops below this mark, flag as unclustered
	const double CLUSTER_PERCENT_THRESHOLD_MAX = 0.50; // Once not clustered, has to reach this percentage to be considered clustered again
	const double CLUSTER_MOVE_THRESHOLD = 2.5; //If unit with this distance of target and has not specfic target, don't move
	std::vector<UnitTypeID> backLineUnits;
	std::vector<UnitTypeID> nonAutoAttackUnits;
	std::vector<std::pair<Point2D, BattleMode>> requestedActions;
	std::vector<KnownEnemyPresence*> knownEnemyPresences;
	int TARGETING_TYPE = 1;
private:
	void AttackTarget(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	bool HasTarget(BattleGroup* group);
	void PartitionGroups(const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	const Unit* FindOptimalTarget(const Unit* unit, const ObservationInterface* obs, QueryInterface* query, GameState* state);
	int GetUnitTypeRange(UnitTypeData* unitInfo);
	void SneakToTarget(BattleGroup* group, const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
	Units cachedEnemyArmy;
	Units cachedEnemies;  
	Units cachedHighPriorityEnemies;
	bool HasThermalLance;
	ThreatAnalyzer threatAnalyzer;
	void SetTarget(BattleGroup* group, Point2D location);
	void SetMode(BattleGroup* group, BattleMode mode);
	void UpdateKnownEnemyPositions(const ObservationInterface* obs, DebugInterface* debug, GameState* state);
	bool ShouldUnitsRetreat(std::pair<Point3D, Units> cluster, std::vector<KnownEnemyPresence*> enemyClusters, const ObservationInterface* obs, QueryInterface* query, GameState* state);
	int GetTargetingType(std::string targeting_type);
	double GetTargetScore(const Unit* unit, const Unit* eu, GameState* state);

public:
	std::vector<BattleGroup> battleGroups;
	ArmyManager();
	void SetTargetingType(std::string targeting_type);
	void RequestAction(Point2D target, BattleMode action);
	void ManageGroups(const ObservationInterface* obs, QueryInterface* query, ActionInterface* action, GameState* state, DebugInterface* debug);
};