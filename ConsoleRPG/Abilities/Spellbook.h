#pragma once

#include <map>
#include <string>
#include <memory>
#include "Ability.h"
#include <vector>

class Spellbook
{
public:
	Spellbook();
	~Spellbook();

	std::shared_ptr<Ability> getAbility(std::string name);
	const std::vector<std::shared_ptr<Ability>> getAbilities();

	void learnAbility(std::shared_ptr<Ability> ability);
	void unlearnAbility(std::shared_ptr<Ability> ability);
	void unlearnAbilityByName(std::string name);

private:
	std::map<std::string, std::shared_ptr<Ability>> abilities;
};

