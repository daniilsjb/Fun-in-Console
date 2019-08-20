#include "Stat.h"

using namespace std;

Stat::Stat(string name, int initialValue) : name(name), value(initialValue) {}

Stat::~Stat() {}

int Stat::getValue()
{
	return value;
}

void Stat::clamp()
{
	value = value < MIN_VALUE ? MIN_VALUE : (value > MAX_VALUE ? MAX_VALUE : value);
}

void Stat::increase(int amount)
{
	value += amount;
	clamp();
}

void Stat::decrease(int amount)
{
	value -= amount;
	clamp();
}