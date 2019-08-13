#pragma once

#include <string>

class Mob;

class Ability
{
public:
	Ability(std::string name);
	virtual ~Ability();

	const std::string name;

	virtual bool mayUse(Mob& caster, Mob& target) { return true; }
	virtual void use(Mob& caster, Mob& target) {}

};

