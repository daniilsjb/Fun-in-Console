#include "Equipment.h"
#include <iostream>
#include "StringUtils.h"
#include "Mob.h"

using namespace std;

Equipment::Equipment(Mob* owner) : owner(owner)
{
	equipment[Slot::Head] = make_pair(nullptr, "Head");
	equipment[Slot::Shoulders] = make_pair(nullptr, "Shoulders");
	equipment[Slot::Back] = make_pair(nullptr, "Back");
	equipment[Slot::Chest] = make_pair(nullptr, "Chest");
	equipment[Slot::Hands] = make_pair(nullptr, "Hands");
	equipment[Slot::Legs] = make_pair(nullptr, "Legs");
	equipment[Slot::Feet] = make_pair(nullptr, "Feet");
	equipment[Slot::MainHand] = make_pair(nullptr, "Main Hand");
	equipment[Slot::OffHand] = make_pair(nullptr, "Off-Hand");
}

Equipment::~Equipment() {}

std::shared_ptr<Gear> Equipment::getItem(std::string itemName)
{
	for (auto &entry : equipment)
	{
		if (entry.second.first != nullptr && StringUtils::toLowerCaseCopy(entry.second.first->name) == itemName)
			return entry.second.first;
	}
	return nullptr;
}

void Equipment::equip(shared_ptr<Gear> item)
{
	if (owner->inventory.hasItem(item))
	{
		unequip(item->slot);
		equipment[item->slot].first = item;
		owner->inventory.removeItem(item->name);
		owner->addStats(item->getModifiers());
	}
	else
		cout << "Cannot equip item that's not in the inventory.\n";
}

void Equipment::unequip(Slot slot)
{
	shared_ptr<Gear> unequipped = equipment[slot].first;
	if (unequipped != nullptr)
	{
		owner->inventory.addItem(unequipped);
		equipment[slot].first = nullptr;
		owner->removeStats(unequipped->getModifiers());
	}
}

void Equipment::unequipAll()
{
	for (auto &entry : equipment)
	{
		if (entry.second.first != nullptr)
			unequip(entry.second.first->slot);
	}
}

void Equipment::print()
{
	for (auto &entry : equipment)
	{
		pair<shared_ptr<Gear>, string>& slot = entry.second;
		cout << slot.second << ": ";
		if (slot.first == nullptr)
			cout << "empty\n";
		else
			slot.first->print();
	}
}