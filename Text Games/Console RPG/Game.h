#pragma once

#include "Input.h"
#include "Mob.h"
#include "World.h"

class Game
{
public:
	Game::Game();
	Game::~Game();

	bool bGameOver = false;

	void start();

	World world;

	Mob player = Mob("player");

private:
	void evaluate(Input::FormattedInput input);
};