#pragma once

#include "Mob.h"

class Enemy : public Mob
{
public:
	Enemy(std::string name);
	~Enemy();

	void onInteract(Game* context) override;

	virtual std::shared_ptr<Ability> chooseAbility(Mob& target);

};

