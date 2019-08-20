#include "Spellbook.h"
#include "StringUtils.h"
#include <iostream>

using namespace std;

Spellbook::Spellbook() {}

Spellbook::~Spellbook() {}

shared_ptr<Ability> Spellbook::getAbility(string name)
{
	StringUtils::toLowerCase(name);
	if (abilities.find(name) != abilities.end())
	{
		return abilities[name];
	}
	else
		return nullptr;
}

const vector<shared_ptr<Ability>> Spellbook::getAbilities()
{
	vector<shared_ptr<Ability>> result;
	for (auto &entry : abilities)
	{
		result.push_back(entry.second);
	}
	return result;
}

void Spellbook::learnAbility(shared_ptr<Ability> ability)
{
	if (getAbility(ability->name) == nullptr)
	{
		string abilityName = StringUtils::toLowerCaseCopy(ability->name);
		abilities[abilityName] = ability;
	}
	else
		cout << ability->name << " is already learned.\n";
}

void Spellbook::unlearnAbility(shared_ptr<Ability> ability)
{
	unlearnAbilityByName(ability->name);
}

void Spellbook::unlearnAbilityByName(string name)
{
	if (getAbility(name) != nullptr)
	{
		string abilityName = StringUtils::toLowerCaseCopy(name);
		abilities.erase(abilityName);
	}
	else
		cout << name << " has not been learned yet.\n";
}