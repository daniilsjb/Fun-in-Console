#pragma once

#include "Item.h"
#include <string>
#include <memory>
#include "EquipmentSlot.h"
#include <map>
#include <vector>
#include "Stat.h"
#include "Stats.h"

class Gear : public Item, public std::enable_shared_from_this<Gear>
{
public:
	Gear(std::string name, int weight, int cost, std::string desc, Slot slot);
	~Gear();

	void onInteract(Game* context) override;

	void print() override;

	const Slot slot;

	void addModifier(Stats stat, std::string name, int amount);
	void updateModifier(Stats stat, std::string name, int amount);
	void removeModifier(Stats stat);

	std::vector<std::pair<Stats, Stat>> getModifiers();

private:
	std::map<Stats, std::unique_ptr<Stat>> modifiers;
};

