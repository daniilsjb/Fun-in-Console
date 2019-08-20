#pragma once

#include <string>

class Game;

class Object {
public:
	Object(std::string name);
	virtual ~Object();

	virtual void onInteract(Game* context) {}

	const std::string name;
}; 