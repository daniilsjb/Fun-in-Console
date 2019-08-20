#include "Mob.h"
#include <iostream>
#include "Fireball.h"

using namespace std;

Mob::Mob(string name) : Object(name)
{
	health = 100;
	maxHealth = 100;
	dead = false;

	spellbook.learnAbility(make_shared<Fireball>("Fireball"));

	stats[Stats::STRENGTH] = make_unique<Stat>("Strength", 5);
	stats[Stats::INTELLECT] = make_unique<Stat>("Intellect", 5);
	stats[Stats::AGILITY] = make_unique<Stat>("Agility", 5);
	stats[Stats::ARMOR] = make_unique<Stat>("Armor", 5);
	stats[Stats::STAMINA] = make_unique<Stat>("Stamina", 5);
}

Mob::~Mob() 
{
}

void Mob::takeDamage(unsigned int amount)
{
	health -= amount;
	if (health <= 0)
	{
		health = 0;
		dead = true;
	}
	cout << name << " takes " << amount << " damage. Health: " << health << "/" << maxHealth << "\n";
}

bool Mob::isDead()
{
	return dead;
}

void Mob::addStats(std::vector<std::pair<Stats, Stat>>& modifiers)
{
	for (auto &modifier : modifiers)
	{
		(*stats[modifier.first]) += modifier.second;
	}
}

void Mob::removeStats(std::vector<std::pair<Stats, Stat>>& modifiers)
{
	for (auto &modifier : modifiers)
	{
		(*stats[modifier.first]) -= modifier.second;
	}
}