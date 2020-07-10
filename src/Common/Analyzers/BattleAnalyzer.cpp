#include "BattleAnalyzer.h"
#include "Common/Util/UnitFIlters.h"
using namespace sc2;
using namespace rapidjson;
std::map<int, UnitBattleData> BattleAnalyzer::unitData;



BattleAnalyzer::BattleAnalyzer(string filepath)
{
	LoadConfigFromFile(filepath, false);
}


BattleAnalyzer::~BattleAnalyzer()
{
}


// Calculate relative strength of one unit to another. Basically returns an enemy units killed per game second.
// TODO: Account for special units like carrier who have 0 attack str in api
double BattleAnalyzer::GetRelativeStrength(UnitTypeID leftUnit, UnitTypeID rightUnit, GameState* state)
{
	auto relStr = 0.0;

	// Load unit info
	auto luType = state->UnitInfo[leftUnit];
	auto ruType = state->UnitInfo[rightUnit];


	Weapon* bestWeapon  = GetBestWeapon(&luType, &ruType);

	// Fail conditions
	if (bestWeapon == nullptr) return 0;
	auto weaponDamage = CalculateWeaponHitDamage(bestWeapon, &ruType);

	// Save outselves from exceptions later
	if (weaponDamage == 0.0 || bestWeapon->speed == 0) return 0;
	
	auto attacksToKill = GetEstimatedAttacksToKill(bestWeapon, &ruType);

	relStr = (float)1 / (attacksToKill * bestWeapon->speed);

	return  relStr;
}

// Get the correct weapon to use.
Weapon* BattleAnalyzer::GetBestWeapon(UnitTypeData* luType, UnitTypeData* ruType)
{
	Weapon* bestWeapon = nullptr;
	double maxDamage = 0.0;
	for (int i = 0; i < luType->weapons.size(); i++)
	{
		if (luType->weapons[i].type == Weapon::TargetType::Any
			|| unitData[(int)ruType->unit_type_id].movementType == (int)luType->weapons[i].type)
		{

			auto damagePerHit = CalculateWeaponHitDamage(&luType->weapons[i], ruType);
			if (damagePerHit > maxDamage)
			{
				bestWeapon = &luType->weapons[i];
				maxDamage = damagePerHit;
			}
		}
	}
	return bestWeapon;
}


double BattleAnalyzer::CalculateWeaponHitDamage(Weapon* weapon, UnitTypeData* enemyType)
{
	//TODO: skip weapons that can't target unit
	float damagePerHit = weapon->damage_;
	for (int i = 0; i < weapon->damage_bonus.size(); i++)
	{
		if (VectorHelpers::FoundInVector(enemyType->attributes, weapon->damage_bonus[i].attribute))
		{
			damagePerHit += weapon->damage_bonus[i].bonus;
		}
	}

	// Damage is per projectile or beam. Must scale
	damagePerHit *= weapon->attacks;
	return damagePerHit;
}

// TODO: multiply armor reduction based on weapon attacks
int BattleAnalyzer::GetEstimatedAttacksToKill(Weapon* weapon, UnitTypeData* enemyType)
{
	if (!weapon) return INT_MAX;
	auto damage = CalculateWeaponHitDamage(weapon, enemyType);		
	if (damage == 0) return INT_MAX;

	auto battleData = unitData[enemyType->unit_type_id];
	auto hitsToKill = ((double)battleData.maxShields / damage) + (double(battleData.maxHealth) / (damage - enemyType->armor));
	return ceil(hitsToKill);
}

// For optimized for ranged units
// TODO: Confidence interval?
// TODO: Could be a toss up
// TODO: Dynamic exponent based on unit range and stackability (including size and flying or not)
int BattleAnalyzer::PredictWinner(double lhsRelStr, int lhsCount, double rhsRelStr, int rhsCount, UnitBattleData* lhsUnit, UnitBattleData* rhsUnit, GameState* state)
{
	auto lhsArmyStr = lhsRelStr * pow(lhsCount, CalcuateExponent(lhsUnit, lhsCount, rhsUnit, rhsCount, state));
	auto rhsArmyStr = rhsRelStr * pow(rhsCount, CalcuateExponent(rhsUnit, rhsCount, lhsUnit, lhsCount, state));

	//Extra explicit cases;
	if (lhsArmyStr == rhsArmyStr) return 0;
	if (lhsArmyStr > rhsArmyStr) return 1;
	if (rhsArmyStr > lhsArmyStr) return 2;

	// fail case? How can we get here?
	return -1;
}

//TODO: Account for more variables?
// - Packing efficiency	 ie with naivly controlled units, few can actually be firing
// - hp regen			 ie likely only significant where it increases attacks to kill
// - range disparity	 ie slightly increase or decrease lhs exponent if there is a significant range disparity
// - Dynamic exponent based on unit range and stackability (including size and flying or not)
// TODO: Add pessimistic, mid, and optimistic results.
// TODO: Calculate actual exponent based on survivors
double BattleAnalyzer::PredictSurvivors(double lhsRelStr, int lhsCount, double rhsRelStr, int rhsCount, UnitBattleData* lhsUnit, UnitBattleData* rhsUnit, GameState* state)
{
	if (lhsRelStr == 0.0) return 0;
	if (rhsRelStr == 0.0) return lhsCount;

	auto lhsExp = CalcuateExponent(lhsUnit, lhsCount, rhsUnit, rhsCount, state);
	auto rhsExp = CalcuateExponent(rhsUnit, rhsCount, lhsUnit, lhsCount, state);

	return PredictSurvivors(lhsRelStr, lhsCount, rhsRelStr, rhsCount, lhsUnit, rhsUnit, lhsExp,rhsExp, state);
}

double BattleAnalyzer::PredictSurvivors(double lhsRelStr, int lhsCount, double rhsRelStr, int rhsCount, UnitBattleData* lhsUnit, UnitBattleData* rhsUnit, double lhsExp, double rhsExp, GameState* state)
{
	auto survivorsSqaured = pow(lhsCount, lhsExp) - ((rhsRelStr / lhsRelStr)*pow(rhsCount, rhsExp));

	// If we modified the exponent, it's more accureate to use average exp for root
	auto combinedExp = (1 / (((lhsExp + rhsExp) / 2)));

	// They'd lose their imaginary army
	if (survivorsSqaured < 0) return -pow(-survivorsSqaured, combinedExp);

	return pow(survivorsSqaured, combinedExp);
}

double BattleAnalyzer::CalcuateExponent(UnitBattleData* lhsUnit, int lhsCount, UnitBattleData* rhsUnit, int rhsCount, GameState* state)
{
	double exponent = 2.0;

	if (lhsUnit->movementType != 2) //Flying
	{
		exponent = 1.7;
	}

	Weapon* bestWeapon = GetBestWeapon(&state->UnitInfo[lhsUnit->id], &state->UnitInfo[rhsUnit->id]);
	Weapon* enemyWeapon = GetBestWeapon(&state->UnitInfo[rhsUnit->id], &state->UnitInfo[lhsUnit->id]);

	if (!bestWeapon || !enemyWeapon) return exponent;

	auto rangeDifference = bestWeapon->range - enemyWeapon->range;
	
	// increase the exponent only if the lhsUnit has a packing efficiency advantage. EG, more units can attack when units are at maximum engagment distance
	// The idea here is that when two ranged armies engage, more of the units with longer range will be able to fire. Test marines vs adepts to see diference.
	if (rangeDifference > 0 && lhsUnit->movementType != 2)
	{
		// Seems to overestimate strength against melee
		auto rowsFiring = ceil(rangeDifference / lhsUnit->radius);
		exponent += rowsFiring / 55;    //magic number that seems to work 
	}
	
	//// Melee units
	if (bestWeapon->range <= 1)
	{
		return 1.4;
	}

	return exponent;
}

void BattleAnalyzer::LoadConfigFromFile(string filepath, bool forceClear)
{
	// Don't double load
	if (!unitData.empty() && !forceClear)
		return;
	unitData.clear();
	std::fstream jsonFiles;
	jsonFiles.open(filepath);
	std::stringstream stream;

	// Read whole file into stream
	stream << jsonFiles.rdbuf();
	Document unitDoc;
	unitDoc.Parse(stream.str().c_str());
	//static const char* kTypeNames[] =
	//{ "Null", "False", "True", "Object", "Array", "String", "Number" };
	// Loop through each unit
	for (Value::ConstValueIterator itr = unitDoc.Begin(); itr != unitDoc.End(); ++itr) {

		// Skip non-units
		if (!itr->HasMember("maxHealth")) continue;

		// Pull all fields
		int id = itr->FindMember("id")->value.GetInt();
		string name = itr->FindMember("name")->value.GetString();
		int maxHealth = itr->FindMember("maxHealth")->value.GetInt();
		int maxShields = 0;
		int movementType = 0;
		if (itr->HasMember("maxShields"))
			maxShields = itr->FindMember("maxShields")->value.GetInt();
		movementType = itr->FindMember("movementType")->value.GetInt();
		float radius;
		if (itr->HasMember("radius"))
			radius = itr->FindMember("radius")->value.GetFloat();
		auto unitBattleData = UnitBattleData();
		unitBattleData.id = id;
		unitBattleData.name = name;
		unitBattleData.maxHealth = maxHealth;
		unitBattleData.maxShields = maxShields;
		unitBattleData.movementType = movementType;
		unitBattleData.radius = radius;

		unitData[id] = unitBattleData;
		//for (Value::ConstMemberIterator itr2 = itr->MemberBegin();
		//	itr2 != itr->MemberEnd(); ++itr2)
		//{
		//	printf("Type of member %s is %s\n",
		//		itr2->name.GetString(), kTypeNames[itr2->value.GetType()]);
		//}
	}
	LOG(1) << "Loaded Unit Data\n";

}


double BattleAnalyzer::EstimateActualExponent(int actualSurvivorsLeft, int actualSurviorsRight, double lhsRelStr, int lhsCount, double rhsRelStr, int rhsCount, UnitBattleData* lhsUnit, UnitBattleData* rhsUnit, int depth, GameState* state)
{
	//Preconditions: actualSurvivors > 0
	if (lhsCount <= 1) return 0;
	auto lhsExp = CalcuateExponent(lhsUnit, lhsCount, rhsUnit, rhsCount, state);
	auto rhsExp = CalcuateExponent(rhsUnit, rhsCount, lhsUnit, lhsCount, state);
	double expVariance = 2.0;
	for (int i = 0; i < depth; i++)
	{
		double predictedLeft = PredictSurvivors(lhsRelStr, lhsCount, rhsRelStr, rhsCount, lhsUnit, rhsUnit, lhsExp, rhsExp, state);

		if (predictedLeft < actualSurvivorsLeft)
		{
			// increase exponent
			lhsExp += expVariance;
		}
		else if (predictedLeft == actualSurvivorsLeft)
		{
			return lhsExp;
		}
		else if (predictedLeft > actualSurvivorsLeft)
		{
			 // decrease exponent
			lhsExp -= expVariance;
		}
		// Shrink the window every time
		expVariance /= 2;
	}
	return lhsExp;
}