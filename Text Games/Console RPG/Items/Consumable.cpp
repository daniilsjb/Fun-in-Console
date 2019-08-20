#include "Consumable.h"
#include "Game.h"

using namespace std;

Consumable::Consumable(string name, int weight, int cost, string desc) : Item(name, weight, cost, desc) {}

Consumable::~Consumable() {}

void Consumable::onInteract(Game* context)
{
	context->player.inventory.removeItem(name);
}
