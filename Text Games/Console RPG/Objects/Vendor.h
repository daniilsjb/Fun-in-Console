#pragma once

#include <vector>
#include <map>
#include "Object.h"
#include "Item.h"
#include <memory>
#include "Inventory.h"

class Vendor : public Object
{
public:
	Vendor(std::string name);
	~Vendor();

	void onInteract(Game* context) override;

	bool hasInStock(std::string itemName);

	void addItem(std::shared_ptr<Item> item, int count);
	void removeItem(std::string name, int count);

private:
	Inventory inventory = Inventory(10000, 100000);
};

