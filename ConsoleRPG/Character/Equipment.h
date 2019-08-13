#pragma once

#include <map>
#include "Gear.h"
#include <memory>
#include "Inventory.h"
#include <string>
#include "EquipmentSlot.h"

class Mob;

class Equipment
{
public:
	Equipment(Mob* owner);
	~Equipment();

	std::shared_ptr<Gear> getItem(std::string itemName);

	void equip(std::shared_ptr<Gear> item);
	void unequip(Slot slot);

	void unequipAll();

	void print();

private:
	Mob* owner;

	std::map<Slot, std::pair<std::shared_ptr<Gear>, std::string>> equipment;

};

