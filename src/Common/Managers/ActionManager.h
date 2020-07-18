#include "cpp-sc2/include/sc2api/sc2_agent.h"
#include "cpp-sc2/include/sc2api/sc2_unit.h"
#include "cpp-sc2/include/sc2api/sc2_interfaces.h"
#include "cpp-sc2/include/sc2api/sc2_control_interfaces.h"
#include <iostream>
#pragma once
using namespace sc2;
struct Command {

	Command() {}

	Command(const Unit* _unit,
		AbilityID _ability,
		Point2D _point,
		const Unit* _target,
		bool _queued_command = false)
	{
		unit = _unit;
		ability = _ability;
		point = _point;
		target = _target;
		queued_command = _queued_command;
	}

	bool equals (const Command& othercommand) {
		bool a = ability == othercommand.ability;
		bool b = point.x == othercommand.point.x && point.y == othercommand.point.y;
		bool c = (target == othercommand.target || (target != nullptr && othercommand.target  != nullptr && target->tag == othercommand.target->tag));
		bool d = queued_command == othercommand.queued_command;
		return a && b && c && d;
	}

	bool operator== (const sc2::UnitOrder& order) {
		return
			ability == order.ability_id
			&& point == order.target_pos
			&& (target == nullptr && order.target_unit_tag == Tag()|| target->tag == order.target_unit_tag);
	}

	const Unit* unit = nullptr;
	AbilityID ability = 0;
	Point2D point = Point2D();
	const Unit* target = nullptr;
	bool queued_command = false;
	bool alreadySent = false;
};

class ActionManager : public ActionInterface
{
	bool sent = false;
	ActionInterface* realInterface;
	std::map<sc2::Tag, Command> _commands;
public:
	ActionManager(ActionInterface* inner)
	{
		BeginStep(inner);
	}

	void BeginStep(ActionInterface* inner)
	{
		_commands.clear();
		sent = false;
		realInterface = inner;
	}

	void SendCommands(bool ignoreSelected)
	{
		if (sent) return;

		// Batch all general commands if they are same ability, target, and point
		// Handles special commands that must be batched
		auto i = _commands.begin();
		for (; i != _commands.end(); i++)
		{
			auto command = &i->second;
			Units toBatch;
		
			if (command->alreadySent ||
				(command->unit->is_selected && ignoreSelected)
				|| (command->unit->orders.size() == 1 && (*command) == command->unit->orders.front())) continue;
		
			for (auto j = i; j != _commands.end();)
			{
				// Incrementing here so i is left alone.. I. Iterators do not support + 1 only, ++
				j++;
				if (j == _commands.end()) break;
				auto othercommand = &j->second;
				if (othercommand->alreadySent ||
					(othercommand->unit != nullptr && othercommand->unit->is_selected && ignoreSelected)) continue;
		
				if (command->equals(*othercommand))
				{
					toBatch.push_back(othercommand->unit);
					command->alreadySent = true;
				}
			}
		
			if (toBatch.size())
			{
				command->alreadySent = true;
				toBatch.push_back(command->unit);
				if (command->target != nullptr) realInterface->UnitCommand(toBatch, command->ability, command->target, command->queued_command);
				else if (command->point.x > 0) realInterface->UnitCommand(toBatch, command->ability, command->point, command->queued_command);
				else realInterface->UnitCommand(toBatch, command->ability, command->queued_command);
			}
		}

		for (auto& command : _commands)
		{
			if (command.second.alreadySent || (command.second.unit->is_selected && ignoreSelected)) continue;

			if (command.second.target != nullptr) realInterface->UnitCommand(command.second.unit, command.second.ability, command.second.target, command.second.queued_command);
			else if (command.second.point.x > 0) realInterface->UnitCommand(command.second.unit, command.second.ability, command.second.point, command.second.queued_command);
			else realInterface->UnitCommand(command.second.unit, command.second.ability, command.second.queued_command);
			command.second.alreadySent = true;
		}

		sent = true;
	}

	void UnitCommand(const Unit* unit, AbilityID ability, bool queued_command = false)
	{
		if (!unit) return;
		_commands.insert_or_assign(unit->tag, Command(unit, ability, Point2D(), nullptr, queued_command));
	}

	void UnitCommand(const Unit* unit, AbilityID ability, const Point2D& point, bool queued_command = false)
	{
		if (!unit) return;
		_commands.insert_or_assign(unit->tag, Command(unit, ability, point, nullptr, queued_command));
	}

	void UnitCommand(const Unit* unit, AbilityID ability, const Unit* target, bool queued_command = false)
	{
		if (!unit || !target) return;
		_commands.insert_or_assign(unit->tag, Command(unit, ability, Point2D(), target, queued_command));
	}

	void UnitCommand(const Units& units, AbilityID ability, bool queued_move = false)
	{
		for (auto& unit : units)
		{
			UnitCommand(unit, ability, queued_move);
		}
	}

	void UnitCommand(const Units& units, AbilityID ability, const Point2D& point, bool queued_move = false)
	{
		for (auto& unit : units)
		{
			UnitCommand(unit, ability, point, queued_move);
		}
	}

	void UnitCommand(const Units& units, AbilityID ability, const Unit* target, bool queued_move = false)
	{
		for (auto& unit : units)
		{
			UnitCommand(unit, ability, target, queued_move);
		}
	}

	const std::vector<Tag>& Commands() const { return realInterface->Commands(); }
	void ToggleAutocast(Tag unit_tag, AbilityID ability) {
		realInterface->ToggleAutocast(unit_tag, ability);
	}
	void ToggleAutocast(const std::vector<Tag>& unit_tags, AbilityID ability) {
		realInterface->ToggleAutocast(unit_tags, ability);
	}
	void SendChat(const std::string& message, ChatChannel channel = ChatChannel::All) { realInterface->SendChat(message, channel); };
	void SendActions() { realInterface->SendActions(); };
};

