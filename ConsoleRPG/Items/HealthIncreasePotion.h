#pragma once

#include "Consumable.h"

class HealthIncreasePotion : public Consumable
{
public:
	HealthIncreasePotion(std::string name, int weight, int cost, std::string desc);
	~HealthIncreasePotion();

	void onInteract(Game* context) override;

};

