#include "HealthIncreasePotion.h"
#include "Game.h"
#include <iostream>

using namespace std;

HealthIncreasePotion::HealthIncreasePotion(string name, int weight, int cost, string desc) : Consumable(name, weight, cost, desc) {}

HealthIncreasePotion::~HealthIncreasePotion() {}

void HealthIncreasePotion::onInteract(Game* context)
{
	int amount = rand() % 25 + 10;
	context->player.maxHealth += amount;
	context->player.health += amount;
	Consumable::onInteract(context);
	cout << name << " used. Health increased by " << amount << "\n";
}