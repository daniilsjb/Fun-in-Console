#include "World.h"
#include "StringUtils.h"
#include <iostream>

using namespace std;

World::World() {}

World::~World() {}

Zone& World::getCurrentZone()
{
	return *zones[currentZone];
}

void World::addZone(unique_ptr<Zone> zone)
{
	string zoneName = StringUtils::toLowerCaseCopy(zone->name);
	if (zones.find(zoneName) == zones.end())
		zones[zoneName] = move(zone);
}

void World::removeZone(string name)
{
	StringUtils::toLowerCase(name);
	if (zones.find(name) != zones.end())
		zones.erase(name);
}

void World::switchZone(string name)
{
	StringUtils::toLowerCase(name);
	if (zones.find(name) != zones.end())
	{
		currentZone = name;
		cout << "Arrived to " << name << ".\n";
	}
	else
		cout << "No such zone found.\n";
}

void World::print()
{
	for (auto &entry : zones)
	{
		cout << "-" << entry.second->name << "\n";
	}
}