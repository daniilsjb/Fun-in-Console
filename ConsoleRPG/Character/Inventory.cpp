#include "Inventory.h"
#include <iostream>
#include "StringUtils.h"

using namespace std;

Inventory::Inventory(int capacity, int maxGold) : capacity(capacity), maxGold(maxGold) 
{
	weight = 0;
	gold = 0;
}

Inventory::~Inventory() 
{
}

int Inventory::getGold()
{
	return gold;
}

int Inventory::getWeight()
{
	return weight;
}

int Inventory::getCapacity()
{
	return capacity;
}

int Inventory::getAmount(string itemName)
{
	StringUtils::toLowerCase(itemName);
	if (inventory.find(itemName) != inventory.end())
	{
		return inventory[itemName].second;
	}
	return -1;
}

std::shared_ptr<Item> Inventory::getItem(string itemName)
{
	StringUtils::toLowerCase(itemName);
	if (inventory.find(itemName) != inventory.end())
	{
		return inventory[itemName].first;
	}
	return nullptr;
}

std::vector<std::pair<std::shared_ptr<Item>, int>> Inventory::getItems()
{
	vector<pair<shared_ptr<Item>, int>> result;
	for (auto &entry : inventory)
	{
		result.push_back(entry.second);
	}
	return result;
}

bool Inventory::hasItem(shared_ptr<Item> item)
{
	return hasItemByName(StringUtils::toLowerCaseCopy(item->name));
}

bool Inventory::hasItemByName(std::string itemName)
{
	StringUtils::toLowerCase(itemName);
	return (inventory.find(itemName) != inventory.end());
}

bool Inventory::isEmpty()
{
	return inventory.empty();
}

int Inventory::addItem(shared_ptr<Item> item, int count)
{
	string itemName = StringUtils::toLowerCaseCopy(item->name);
	int sumWeight = (item->weight * count);
	if (weight + sumWeight > capacity)
	{
		count = (int)((capacity - weight) / item->weight);
		if (count == 0) 
			return 0;
		sumWeight = (item->weight * count);
	}
	if (inventory.find(itemName) != inventory.end())
	{
		inventory[itemName].second += count;
	}
	else
	{
		inventory[itemName] = make_pair(item, count);
	}
	weight += sumWeight;
	return count;
}

bool Inventory::removeItem(string itemName, int count)
{
	StringUtils::toLowerCase(itemName);
	if (inventory.find(itemName) != inventory.end())
	{
		int itemWeight = inventory[itemName].first->weight;
		int removedExtra;
		inventory[itemName].second -= count;
		if (inventory[itemName].second <= 0)
		{
			removedExtra = abs(inventory[itemName].second);
			count -= removedExtra;
			inventory.erase(itemName);
		}
		weight -= (itemWeight * count);
		return true;
	}
	else
		return false;
}

int Inventory::addGold(int amount)
{
	if (gold + amount > maxGold)
	{
		amount = (maxGold - gold);
	}
	gold += amount;
	if (gold > maxGold)
		gold = maxGold;
	return amount;
}

int Inventory::removeGold(int amount)
{
	if (amount > gold)
		amount = gold;
	gold -= amount;
	if (gold < 0)
		gold = 0;
	return amount;
}

int Inventory::calculateEstimatedCost()
{
	int cost = 0;
	for (auto& entry : inventory)
	{
		cost += entry.second.first->estimatedCost * entry.second.second;
	}
	return cost;
}

void Inventory::increaseCapacity(int amount)
{
	capacity += amount;
}

void Inventory::decreaseCapacity(int amount)
{
	capacity -= amount;
	if (capacity < 0)
		capacity = 0;
}

int Inventory::transferItem(Inventory& destination, shared_ptr<Item> item, int count)
{
	string itemName = StringUtils::toLowerCaseCopy(item->name);
	if (inventory.find(itemName) != inventory.end())
	{
		pair<shared_ptr<Item>, int>& entry = inventory[itemName];
		if (entry.second < count)
			count = entry.second;
	}
	int added = destination.addItem(item, count);
	removeItem(itemName, added);
	return added;
}

void Inventory::transferItems(Inventory& destination, vector<pair<shared_ptr<Item>, int>> items)
{
	for (auto &item : items)
	{
		transferItem(destination, item.first, item.second);
	}
}

int Inventory::transferGold(Inventory& destination, int amount)
{
	int removed = removeGold(amount);
	int transferred = destination.addGold(removed);
	gold += (removed - transferred);
	return transferred;
}

void Inventory::transferAll(Inventory& destination)
{
	transferGold(destination, gold);
	vector<pair<shared_ptr<Item>, int>> items;
	for (auto &entry : inventory)
	{
		items.push_back(entry.second);
	}
	transferItems(destination, items);
}

void Inventory::print()
{
	cout << "Gold: " << gold << "\n";
	cout << "Weight: " << weight << "/" << capacity << "\n";
	if (inventory.empty())
	{
		cout << "Inventory is empty.\n";
		return;
	}
	for (auto &entry : inventory)
	{
		shared_ptr<Item>& item = entry.second.first;
		cout << entry.second.second << "x ";
		item->print();
	}
}