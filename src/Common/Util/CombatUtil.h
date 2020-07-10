#pragma once
#pragma once
#include "sc2api\sc2_api.h"
#include "UnitFilters.h"
#include "VectorHelpers.h"
#include <vector>
namespace Util {

	static double CalculateWeaponHitDamage(Weapon* weapon, UnitTypeData* enemyType)
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

	static Weapon* GetBestWeapon(const Unit* attacker,const Unit* reciever, GameState* state)
	{
		Weapon* bestWeapon = nullptr;
		auto unittypeAttacker = state->UnitInfo[attacker->unit_type];
		auto unittypeReciever = state->UnitInfo[reciever->unit_type];

		bool isFlying = IsFlying()(*reciever);

		double maxDamage = 0.0;
		for (int i = 0; i < unittypeAttacker.weapons.size(); i++)
		{
			if (unittypeAttacker.weapons[i].type == Weapon::TargetType::Any
				|| (!isFlying && unittypeAttacker.weapons[i].type == Weapon::TargetType::Ground)
				|| (isFlying && unittypeAttacker.weapons[i].type == Weapon::TargetType::Air))
			{
				auto damagePerHit = CalculateWeaponHitDamage(&unittypeAttacker.weapons[i], &unittypeReciever);
				if (damagePerHit > maxDamage)
				{
					bestWeapon = &unittypeAttacker.weapons[i];
					maxDamage = damagePerHit;
				}
			}
		}
		return bestWeapon;
	}

	static bool CanAttack(const Unit* attacker,const  Unit* reciever, GameState* state)
	{
		return GetBestWeapon(attacker, reciever, state) != nullptr;
	}
}