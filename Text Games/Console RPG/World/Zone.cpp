#include "Zone.h"
#include <iostream>
#include <algorithm>
#include "StringUtils.h"
#include "Enemy.h"
#include "Sign.h"

using namespace std;

Zone::Zone(string name) : name(name) {}

Zone::~Zone() {}

void Zone::addObject(std::unique_ptr<Object> object)
{
	string name = StringUtils::toLowerCaseCopy(object->name);
	objects[name] = move(object);
}

void Zone::removeObject(string name)
{
	StringUtils::toLowerCase(name);
	objects.erase(name);
}

void Zone::search()
{
	cout << "Objects found:\n";
	for (auto &object : objects)
	{
		cout << "-" << object.second->name << "\n";
	}
}

void Zone::interactWith(string objectName, Game* context)
{
	if (objects.find(objectName) != objects.end())
	{
		objects[objectName]->onInteract(context);
	}
	else
	{
		cout << "There is no " << objectName << " in this zone.\n";
	}
}