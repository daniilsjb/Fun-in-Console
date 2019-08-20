#pragma once

#include "Item.h"
#include <string>

class Consumable : public Item
{
public:
	Consumable(std::string name, int weight, int cost, std::string desc);
	virtual ~Consumable();

	void onInteract(Game* context) override;

};

