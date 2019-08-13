#include "Junk.h"
#include <iostream>
#include "Game.h"

using namespace std;

Junk::Junk(std::string name, int weight, int cost, std::string desc) : Item(name, weight, cost, desc) {}

Junk::~Junk() {}

void Junk::onInteract(Game* context)
{
	cout << description << " Cannot be used.\n";
}