#pragma once

#include <memory>
#include <string>
#include <map>
#include "Zone.h"

class World
{
public:
	World();
	~World();

	Zone& getCurrentZone();

	void addZone(std::unique_ptr<Zone> zone);
	void removeZone(std::string name);

	void switchZone(std::string name);

	void print();

private:
	std::map<std::string, std::unique_ptr<Zone>> zones;
	std::string currentZone;
};

