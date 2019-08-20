#include "Sign.h"
#include <iostream>
#include "Game.h"

using namespace std;

Sign::Sign(string name, string message) : Object(name), message(message) {}

Sign::~Sign() {}

void Sign::onInteract(Game* context)
{
	cout << message;
}