#include "PlanBotBase.h"

PlanBotBase::PlanBotBase()
{
	goalPicker = GoalPicker();
	planner = Planner();
	EconomyGoals = vector<BaseAction*>();
	ArmyGoals = vector<BaseAction*>();
	state.CurrentUnits = UnitMap();
	state.ArmyManager = &armyManager;
	threatAnalyzer = ThreatAnalyzer();
}
void PlanBotBase::Init()
{
	state.ArmyManager = &armyManager;

	// Must init after AvailableActions is populated
	// TODO: refactor
	planner.Init();
	shouldRecalcuate = true;

	Lost = false;
	StepCounter = STEPS_PER_GOAL;
}
void PlanBotBase::ChooseActionFromGoals(vector<BaseAction*> goals, const sc2::ObservationInterface * obs, sc2::ActionInterface * actions, sc2::QueryInterface * query, string name, vector<string>* messages, bool withRetry, bool& stopOthers)
{
	if (stopOthers) return;
	BaseAction* goal = nullptr;
	BaseAction* nextInPlan = nullptr;
	auto actionList = goalPicker.GetGoals(goals, obs, &state);
	bool allowDependencies = true;
	Debug()->DebugTextOut(name + " Goals:", Point2D(.2 * goalCategory, 0), Colors::White, 8);

	for (int i = 0; i < actionList.size(); i++)
	{
		if (get<0>(actionList[i]) <= 0) break; // Don't do goals with score 0;

		auto goal = std::get<1>(actionList[i]);
		auto resState = planner.GetResourceState(obs);

		Debug()->DebugTextOut(goal->GetName() + ":" + std::to_string(get<0>(actionList[i])), Point2D(.2 * goalCategory, .01 + (i*.01)), Colors::White, 8);

		auto plan = planner.CalculatePlan(resState, goal);
		if (plan.size() == 1)
		{
			nextInPlan = plan[0];
		}
		else if (allowDependencies)
		{
			nextInPlan = plan[plan.size() - 1];
		}

		if (goal && nextInPlan)
		{
			auto success = nextInPlan->Excecute(obs, actions, query, Debug(), &state);
			if (success)
			{
				auto msg = name + " GP:" + goal->GetName();
				if (nextInPlan && goal != nextInPlan)
					msg += " GS:" + nextInPlan->GetName();
				messages->push_back(msg);
				goal = nullptr;
				break;
			}
			else if (get<0>(actionList[i]) > 10 || goal->HoldOtherGoals(obs))
			{
				stopOthers = true;
				break;
			}
		}

		if (!withRetry) break; // If we aren't retrying, break out
							   // Only allow the top choice to build it's dependencies. Otherwise we build one of everything
		allowDependencies = false;
	}
	goalCategory++;
}

bool PlanBotBase::ShouldSurrender(const sc2::ObservationInterface * obs)
{
	auto probes = obs->GetFoodWorkers();
	auto minerals = obs->GetMinerals();
	auto armyCount = obs->GetFoodArmy();

	return probes == 0 && minerals < 50 && armyCount <= 2;
}

void PlanBotBase::DebugWriteInView(string message, Point2D relativePosition, DebugInterface* debug, const ObservationInterface* obs)
{
	debug->DebugTextOut(message, relativePosition);
}


void PlanBotBase::DebugDrawState(chrono::time_point<chrono::steady_clock> startTime)
{
	for (int i = 0; i < state.ExpansionLocations.size(); i++)
	{
		Debug()->DebugTextOut(std::to_string(i + 1), state.ExpansionLocations[i]);
	}

	auto endTime = Clock::now();

	//Output minerals lost and estiamte threat.
	Debug()->DebugTextOut("Minerals Lost:" + std::to_string(state.threat.mineralsLost) + " Enemy Minerals Lost:" + std::to_string(state.threat.enemyMineralsLost), Point2D(0, 0.4));
	Debug()->DebugTextOut("Gas Lost:" + std::to_string(state.threat.gasLost) + " Enemy Gas Lost:" + std::to_string(state.threat.enemyGasLost), Point2D(0, .42));
	Debug()->DebugTextOut("Time Lost:" + std::to_string(state.threat.buildTimeLost) + " Enemy Time Lost:" + std::to_string(state.threat.enemyBuildTimeLost), Point2D(0, .44));
	Debug()->DebugTextOut("Game Loop:" + std::to_string(state.threat.gameTime), Point2D(0, .46));
	Debug()->DebugTextOut("Threat Score: " + std::to_string(this->threatAnalyzer.GetThreat(&state.threat)), Point2D(0, .48));
	Debug()->DebugTextOut("Loop Time: " + to_string(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()), Point2D(0, .50));

}

void PlanBotBase::UpdateGameState()
{
	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();

	state.threat.gameTime = obs->GetGameLoop();
	state.threat.friendlyBases = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall()).size();
	state.threat.enemyBases = obs->GetUnits(sc2::Unit::Alliance::Enemy, IsTownHall()).size();

	// Store the number of each unit they have
	auto enemyUnits = obs->GetUnits(sc2::Unit::Alliance::Enemy);
	UnitMap cUnits;
	int maxFood = 0;
	for (auto unit : enemyUnits)
	{
		cUnits[unit->unit_type]++;
		maxFood += state.UnitInfo[unit->unit_type].food_required;
	}
	state.MaxEnemyFood = max(state.MaxEnemyFood, maxFood);
	for (auto type : cUnits)
	{
		state.MaxEnemyUnits[type.first] = max(state.MaxEnemyUnits[type.first], type.second);
		LOG(4) << (int)type.first << " " << state.MaxEnemyUnits[type.first];
	}
}

void PlanBotBase::RemoveIdleScouts()
{
	// Clear out scouting probes if they're not scouting
	for (auto worker : state.ScoutingUnits)
	{
		if (IsIdle()(*worker))
		{
			VectorHelpers::RemoveFromVector(&state.ScoutingUnits, worker);
		}
	}
}

void PlanBotBase::ChooseGoals()
{
	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();

	if (StepCounter == STEPS_PER_GOAL)
	{
		debugMessages.clear();
		bool stopOthers = false;
		ChooseActionFromGoals(EconomyGoals, obs, actions, query, "Econ", &debugMessages, true, stopOthers);
		ChooseActionFromGoals(BuildOrderGoals, obs, actions, query, "BuildOrder", &debugMessages, true, stopOthers);
		ChooseActionFromGoals(ArmyGoals, obs, actions, query, "Army", &debugMessages, true, stopOthers);
		ChooseActionFromGoals(TacticsGoals, obs, actions, query, "Tactics", &debugMessages, false, stopOthers);
		ChooseActionFromGoals(UpgradeGoals, obs, actions, query, "Upgrade", &debugMessages, false, stopOthers);
		StepCounter = 0;
	}
	StepCounter++;
	goalCategory = 0;
}

void PlanBotBase::BalanceWorkerAssignments()
{
	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();
	// Reassign Excess Workers
	auto townhalls = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
	Units extraWorkers;
	for (auto th : townhalls)
	{
		if (th->assigned_harvesters > th->ideal_harvesters)
		{
			// Tell all extra workers to stop so they can be reassigned
			//TODO: only reassign if another town hall needs workers
			auto overWorkers = Util::FindNearbyUnits(sc2::Unit::Alliance::Self, IsWorker(), th->pos, obs, 15);
			int diff = th->assigned_harvesters - th->ideal_harvesters;
			if (overWorkers.size() < diff) continue; // If no workers in area continue
			extraWorkers = Units(overWorkers.begin(), overWorkers.begin() + diff);
		}
	}

	auto idleUnits = obs->GetUnits(Unit::Alliance::Self, IsIdleWorker());
	idleUnits.insert(idleUnits.end(), extraWorkers.begin(), extraWorkers.end());
	for (int i = 0; i < idleUnits.size(); i++)
	{
		// Don't reassign scouts
		if (VectorHelpers::FoundInVector(state.ScoutingUnits, idleUnits[i])) continue;

		auto resource = Util::FindNearestResourceNeedingHarversters(idleUnits[i], obs, query);
		if (resource)
		{
			actions->UnitCommand(idleUnits[i], ABILITY_ID::HARVEST_GATHER, resource);
		}
		else
		{
			// Send to scout
			if (state.ScoutingUnits.size() == 0 && state.KilledScouts < 3)
			{
				actions->UnitCommand(idleUnits[i], ABILITY_ID::MOVE, state.EnemyBase);
				state.ScoutingUnits.push_back(idleUnits[i]);
			}
			break;
		}
	}
}

void PlanBotBase::DefendBase()
{
	auto obs = Observation();
	auto tHalls = obs->GetUnits(Unit::Alliance::Self, IsTownHall());
	for (auto th : tHalls)
	{
		auto nearbyUnits = Util::FindNearbyUnits(Unit::Alliance::Enemy, IsNotFiller(), th->pos, obs, 25);
		if (nearbyUnits.size())
		{
			// RALLY THE TROOPS!
			armyManager.RequestAction(nearbyUnits[0]->pos, BattleMode::Defend);

		}
	}
}

void PlanBotBase::CancelBuildingsUnderAttack()
{
	auto obs = Observation();
	auto actions = Actions();
	// Cancel all buildings that are close to being killed
	auto inProgess = obs->GetUnits(sc2::Unit::Alliance::Self, InProgressUnits());
	for (auto ipBuilding : inProgess)
	{
		if (ipBuilding->build_progress > .1)
		{
			auto hp = ipBuilding->shield + ipBuilding->health;
			auto hpMax = (ipBuilding->shield_max + ipBuilding->shield_max) * ipBuilding->build_progress;
			if (hp / hpMax < .15)
			{
				actions->UnitCommand(ipBuilding, ABILITY_ID::CANCEL);
			}
		}
	}
}

void PlanBotBase::ManageUnits()
{
	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();

	armyManager.ManageGroups(obs, query, actions, &state, Debug());

	for (int i = 0; i < microManagers.size(); i++)
	{
		microManagers[i]->Execute(obs, actions, query, Debug(), &state);
	}
}


void PlanBotBase::OnUnitDestroyed(const Unit* unit)
{
	if (VectorHelpers::FoundInVector(state.ScoutingUnits, unit))
	{
		this->state.KilledScouts++;
		VectorHelpers::RemoveFromVector(&state.ScoutingUnits, unit);
	}

	if (!state.HasCloakedUnits &&
		Util::FindNearbyUnits(sc2::Unit::Alliance::Enemy, IsEnemy(), unit->pos, Observation(), 10.0).size() == 0)
	{
		// Killed but no nearby enemies? Must be cloaked
		this->state.HasCloakedUnits = true;
	}

	// Estimate resources lost on both sides
	if (unit->alliance == Unit::Alliance::Self)
	{
		state.CurrentUnits[unit->unit_type]--;

		state.threat.mineralsLost += state.UnitInfo[unit->unit_type].mineral_cost;
		state.threat.gasLost += state.UnitInfo[unit->unit_type].vespene_cost;
		state.threat.buildTimeLost += state.UnitInfo[unit->unit_type].build_time;
	}
	else
	{
		state.threat.enemyMineralsLost += state.UnitInfo[unit->unit_type].mineral_cost;
		state.threat.enemyGasLost += state.UnitInfo[unit->unit_type].vespene_cost;
		state.threat.enemyBuildTimeLost += state.UnitInfo[unit->unit_type].build_time;
	}
}

void PlanBotBase::OnGameStart()
{
	LOG(1) << "Bot initialized" << endl;

	auto players = Observation()->GetGameInfo().player_info;
	LOG(1) << Util::GetStringFromRace(players[0].race_actual) << endl;
	LOG(1) << Util::GetStringFromRace(players[1].race_actual) << endl;

	auto townhalls = Observation()->GetUnits(IsTownHall());
	if (townhalls.size() > 0)
	{
		auto nexus = townhalls[0];

		Actions()->UnitCommand(nexus, ABILITY_ID::SMART, nexus->pos);
		auto enemyLocations = Observation()->GetGameInfo().enemy_start_locations;
		if (enemyLocations.size() == 1)
			state.EnemyBase = enemyLocations[0];
		else if (enemyLocations.size() == 4)
			state.EnemyBase = enemyLocations[0];
		else
			state.EnemyBase = enemyLocations[0];

		state.StartingLocation = nexus->pos;

		// Get all minerals and sort by x , then y pos
		auto minerals = Observation()->GetUnits(Unit::Alliance::Neutral, IsMineralField());
		int count = 0;
		Point3D sum = Point3D();
		const int MINERAL_DISTANCE_THRESHOLD = 150;
		while (minerals.size() > 0)
		{
			Point3D origMineral = minerals[0]->pos;
			sum = Point3D();
			count = 0;
			for (int j = 0; j < minerals.size();)
			{
				auto cluster = std::vector<Point3D>();
				auto dis = DistanceSquared3D(origMineral, minerals[j]->pos);
				if (dis < MINERAL_DISTANCE_THRESHOLD)
				{
					sum += minerals[j]->pos;
					count++;
					// Erase this element
					minerals.erase(minerals.begin() + j, minerals.begin() + j + 1);
				}
				else
				{
					j++;
				}
			}

			state.ExpansionLocations.push_back(sum / count);
		}
		// Remove starting pos as expansion location
		auto closest_mineral = Util::FindClosestPoint(state.ExpansionLocations, nexus->pos);
		state.ExpansionLocations.erase(std::remove_if(state.ExpansionLocations.begin(), state.ExpansionLocations.end(), [closest_mineral](Point3D p) {return p == closest_mineral; }));
		closest_mineral = Util::FindClosestPoint(state.ExpansionLocations, Point3D(state.EnemyBase.x, state.EnemyBase.y, 0));
		state.ExpansionLocations.erase(std::remove_if(state.ExpansionLocations.begin(), state.ExpansionLocations.end(), [closest_mineral](Point3D p) {return p == closest_mineral; }));

		// Sort expansions by distance to home base
		std::sort(state.ExpansionLocations.begin(), state.ExpansionLocations.end(), Sorters::sort_by_distance(state.StartingLocation));


		// Calculate building offset
		auto nearMinerals = Util::FindNearbyUnits(sc2::Unit::Alliance::Neutral, IsMineralField(), Util::ToPoint3D(state.StartingLocation), Observation(), 15);

		//Calc mineral vector and normalize
		int visibleMinerals = 0;
		Point3D nearSum;
		for (auto min : nearMinerals)
		{
			if (min->display_type == Unit::DisplayType::Visible)
			{
				nearSum += min->pos;
				visibleMinerals++;
			}
		}
		if (visibleMinerals > 0)
		{
			nearSum /= visibleMinerals;
			nearSum = nearSum - nexus->pos;
			nearSum.z = 0;
			Normalize3D(nearSum);
			state.MineralDirection = nearSum;
		}
	}

	//Cache unit info
	state.UnitInfo = Observation()->GetUnitTypeData();


	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();
	if (obs->GetPlayerID() == 1)
	{
		state.selfRace = obs->GetGameInfo().player_info[0].race_actual;
		state.opponentRace = obs->GetGameInfo().player_info[1].race_actual;
	}
	else
	{
		state.selfRace = obs->GetGameInfo().player_info[1].race_actual;
		state.opponentRace = obs->GetGameInfo().player_info[0].race_actual;
	}

}
