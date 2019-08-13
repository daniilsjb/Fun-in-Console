#pragma once

#include "Object.h"

class Item : public Object
{
public:
	Item(std::string name, int weight, int cost, std::string desc);
	virtual ~Item();

	int weight;
	int estimatedCost;
	std::string description;

	virtual void print();

};