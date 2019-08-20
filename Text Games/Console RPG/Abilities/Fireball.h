#pragma once

#include "Ability.h"

class Fireball : public Ability
{
public:
	Fireball(std::string name);
	~Fireball();

	bool mayUse(Mob& caster, Mob& target) override;
	void use(Mob& caster, Mob& target) override;

private:
	const int BASE_DAMAGE = 10;
};

