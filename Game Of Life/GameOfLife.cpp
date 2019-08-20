#include "ConsoleGameEngine.h"

class GameOfLife : public ConsoleGameEngine
{
	//We need 2 separate versions of the game to make sure changes in one don't instantly affect the other
	//Then we may just swap them around
	bool *cells = nullptr;
	bool *nextCells = nullptr;

	//User may pause the game to analyze it or change/remove cells
	bool active = false;

	float timer = 0.0f;

	bool OnStart() override
	{
		SetApplicationTitle(L"Game of Life");

		cells = new bool[GetScreenWidth() * GetScreenHeight()];
		nextCells = new bool[GetScreenWidth() * GetScreenHeight()];

		//Initially all the cells are dead
		for (int i = 0; i < GetScreenWidth() * GetScreenHeight(); i++)
		{
			cells[i] = false;
			nextCells[i] = false;
		}

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		//User is only allowed to add/remove cells when the game is paused
		if (!active)
		{
			if (GetKey(VK_LBUTTON).held)
				cells[GetScreenWidth() * GetMouseY() + GetMouseX()] = true;

			if (GetKey(VK_RBUTTON).held)
				cells[GetScreenWidth() * GetMouseY() + GetMouseX()] = false;

			if (GetKey(VK_RETURN).pressed)
				active = true;
		}
		else
		{
			//User may pause the game when it's active
			if (GetKey(' ').pressed)
			{
				active = false;
				timer = 0.0f;
			}
		}

		if (active)
		{
			//Next cells should always start same as current cells
			for (int i = 0; i < GetScreenWidth() * GetScreenHeight(); i++)
			{
				nextCells[i] = cells[i];
			}

			timer += elapsedTime;

			//But we only actually want to update next cells if enough time has passed
			if (timer >= 0.25f)
			{
				timer = 0.0f;

				for (int i = 0; i < GetScreenWidth(); i++)
				{
					for (int j = 0; j < GetScreenHeight(); j++)
					{
						int liveNeighbours = 0;

						//Count the amount of living neighbours
						for (int xOffset = -1; xOffset <= 1; xOffset++)
						{
							for (int yOffset = -1; yOffset <= 1; yOffset++)
							{
								if (xOffset == 0 && yOffset == 0)
									continue;

								//Comment this block if you want wrap-around
								if (i + xOffset < 0 || j + yOffset < 0 || i + xOffset >= GetScreenWidth() || j + yOffset >= GetScreenHeight())
									continue;

								int nextX = (i + xOffset);
								int nextY = (j + yOffset);

								//Comment this block if you don't want wrap-around
								/*if (i + xOffset < 0)
									nextX = GetScreenWidth() - 1;
								else
									nextX = (i + xOffset) % GetScreenWidth();

								if (j + yOffset < 0)
									nextY = GetScreenHeight() - 1;
								else
									nextY = (j + yOffset) % GetScreenHeight();*/

								if (cells[GetScreenWidth() * nextY + nextX])
									liveNeighbours++;
							}
						}

						//Apply Game of Life's rules to the current cell
						int currentCell = GetScreenWidth() * j + i;

						if (cells[currentCell])
						{
							if (liveNeighbours < 2 || liveNeighbours > 3)
								nextCells[currentCell] = false;
							else
								nextCells[currentCell] = true;
						}
						else if (liveNeighbours == 3)
							nextCells[currentCell] = true;
						else
							nextCells[currentCell] = false;
					}
				}
			}
		}

		//We need to see whether the game is finished so we could automatically pause it
		bool finished = true;

		for (int i = 0; i < GetScreenWidth() * GetScreenHeight(); i++)
		{
			//We want to draw cells even if the game is not active
			short color = cells[i] ? BG_WHITE : BG_BLACK;
			Draw(i, ' ', color);

			//But we want to swap them around only if it is
			if (active)
			{
				cells[i] = nextCells[i];

				//Game isn't finished if there is at least 1 living cell
				if (cells[i])
					finished = false;
			}
		}
		
		//If the game was active, then we need to check if it's been finished after this iteration
		if (active)
			active = !finished;

		return true;
	}

	bool OnDestroy() override
	{
		delete[] cells;
		delete[] nextCells;

		return true;
	}

};

int main()
{
	GameOfLife app;
	if (app.ConstructScreen(64, 32, 16, 16))
		app.Start();

	return 0;
}