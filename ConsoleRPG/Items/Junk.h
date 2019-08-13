#pragma once

#include "Item.h"
#include <string>

class Junk : public Item
{
public:
	Junk(std::string name, int weight, int cost, std::string desc);
	~Junk();

	void onInteract(Game* context) override;

};

