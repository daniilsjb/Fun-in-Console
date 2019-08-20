#include "Item.h"
#include <iostream>

using namespace std;

Item::Item(string name, int weight, int cost, std::string desc) : Object(name), weight(weight), estimatedCost(cost), description(desc) {}

Item::~Item() {}

void Item::print()
{
	cout << name << " [Weight: " << weight << ", Estimated cost: " << estimatedCost << ", Description: " << description << "]\n";
}