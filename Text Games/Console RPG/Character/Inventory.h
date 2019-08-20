#pragma once

#include "Item.h"
#include <map>
#include <string>
#include <vector>
#include <memory>

class Inventory
{
public:
	Inventory(int maxCapacity, int maxGold);
	~Inventory();

	int getGold();
	int getWeight();
	int getCapacity();
	int getAmount(std::string itemName);

	std::shared_ptr<Item> getItem(std::string itemName);
	std::vector<std::pair<std::shared_ptr<Item>, int>> getItems();

	bool isEmpty();
	bool hasItem(std::shared_ptr<Item> item);
	bool hasItemByName(std::string itemName);

	int addItem(std::shared_ptr<Item> item, int count = 1);
	bool removeItem(std::string itemName, int count = 1);

	int addGold(int amount);
	int removeGold(int amount);

	int calculateEstimatedCost();

	void increaseCapacity(int amount);
	void decreaseCapacity(int amount);

	int transferItem(Inventory& destination, std::shared_ptr<Item> item, int count = 1);
	void transferItems(Inventory& destination, std::vector<std::pair<std::shared_ptr<Item>, int>> items);

	int transferGold(Inventory& destination, int amount);

	void transferAll(Inventory& destination);

	void print();

private:
	int gold;
	int maxGold;

	int weight;
	int capacity;

	std::map<std::string, std::pair<std::shared_ptr<Item>, int>> inventory;
};

