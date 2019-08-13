#pragma once

#include "Object.h"
#include "Inventory.h"
#include "Equipment.h"
#include "Stat.h"
#include "Stats.h"
#include "Spellbook.h"
#include <memory>
#include <vector>

class Mob : public Object
{
public:
	Mob(std::string name);
	virtual ~Mob();

	void takeDamage(unsigned int amount);

	bool isDead();

	int health;
	int maxHealth;

	Inventory inventory = Inventory(200, 10000);
	Equipment equipment = Equipment(this);
	Spellbook spellbook;

	void addStats(std::vector<std::pair<Stats, Stat>>& modifiers);
	void removeStats(std::vector<std::pair<Stats, Stat>>& modifiers);

	std::map<Stats, std::unique_ptr<Stat>> stats;

private:
	bool dead;
	
};

