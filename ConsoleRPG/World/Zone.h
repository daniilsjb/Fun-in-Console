#pragma once

#include <map>
#include <memory>
#include "Object.h"

class Zone
{
public:
	Zone(std::string name);
	~Zone();

	void addObject(std::unique_ptr<Object> object);
	void removeObject(std::string name);
	void search();
	void interactWith(std::string objectName, Game* context);

	const std::string name;

private:
	std::map<std::string, std::unique_ptr<Object>> objects;
};