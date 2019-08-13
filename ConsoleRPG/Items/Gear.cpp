#include "Gear.h"
#include "Game.h"
#include <iostream>

using namespace std;

Gear::Gear(string name, int weight, int cost, string desc, Slot slot) : Item(name, weight, cost, desc), slot(slot) {}

Gear::~Gear() {}

void Gear::onInteract(Game* context)
{
	context->player.equipment.equip(shared_from_this());
	cout << name << " equipped.\n";
}

void Gear::print()
{
	Item::print();
	if (modifiers.empty())
	{
		cout << "No stat modifiers.\n";
	}
	else
	{
		cout << "Stat modifiers:\n";
		for (auto &entry : modifiers)
		{
			int value = entry.second->getValue();
			cout << "	" << entry.second->name << ": " << (value > 0 ? "+" : "") << value << "\n";
		}
	}
	
}

void Gear::addModifier(Stats stat, string name, int amount)
{
	if (modifiers.find(stat) == modifiers.end())
	{
		modifiers[stat] = make_unique<Stat>(name, amount);
	}
}

void Gear::updateModifier(Stats stat, string name, int amount)
{
	if (modifiers.find(stat) != modifiers.end())
	{
		modifiers[stat].reset();
		modifiers[stat] = make_unique<Stat>(name, amount);
	}
}

void Gear::removeModifier(Stats stat)
{
	modifiers.erase(stat);
}

vector<pair<Stats, Stat>> Gear::getModifiers()
{
	vector<pair<Stats, Stat>> result;
	for (auto &entry : modifiers)
	{
		result.push_back(make_pair(entry.first, *entry.second));
	}
	return result;
}