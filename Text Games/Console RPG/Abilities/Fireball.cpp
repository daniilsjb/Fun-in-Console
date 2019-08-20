#include "Fireball.h"
#include "Mob.h"
#include "Stats.h"

using namespace std;

Fireball::Fireball(string name) : Ability(name) {}

Fireball::~Fireball() {}

bool Fireball::mayUse(Mob& caster, Mob& target)
{
	//Check if enough mana, etc
	//Simply returning true for now
	return true;
}

void Fireball::use(Mob& caster, Mob& target)
{
	int intellect = caster.stats[Stats::INTELLECT]->getValue();
	int armor = target.stats[Stats::ARMOR]->getValue();
	int mitigation = (int)(armor * 0.5f);
	int damage = (int)(BASE_DAMAGE + (intellect * 1.5f)) - mitigation;
	target.takeDamage(damage);
}