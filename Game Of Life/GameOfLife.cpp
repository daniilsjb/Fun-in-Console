#include "ConsoleGameEngine.h"

class GameOfLife : public ConsoleGameEngine
{
	bool *cells = nullptr;
	bool *nextCells = nullptr;

	int cellCount = 0;

	bool active = false;

	float timer = 0.f;

	bool OnStart() override
	{
		SetApplicationTitle(L"Game of Life");

		cellCount = GetScreenWidth() * GetScreenHeight();

		cells = new bool[cellCount];
		nextCells = new bool[cellCount];

		for (int i = 0; i < cellCount; i++)
		{
			cells[i] = false;
			nextCells[i] = false;
		}

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		if (!active)
		{
			if (GetKey(VK_LBUTTON).held)
				cells[GetScreenWidth() * GetMouseY() + GetMouseX()] = true;

			if (GetKey(VK_RBUTTON).held)
				cells[GetScreenWidth() * GetMouseY() + GetMouseX()] = false;

			if (GetKey(VK_RETURN).pressed)
				active = true;
		}

		timer += elapsedTime;

		if (GetKey(' ').pressed)
		{
			active = false;
			timer = 0.f;
		}

		if (active && timer >= 0.25f)
		{
			timer = 0.f;

			for (int i = 0; i < GetScreenWidth(); i++)
			{
				for (int j = 0; j < GetScreenHeight(); j++)
				{
					int liveNeighbours = 0;

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

		bool finished = true;

		for (int i = 0; i < cellCount; i++)
		{
			short color = cells[i] ? BG_WHITE : BG_BLACK;
			Draw(i, ' ', color);

			if (active)
			{
				cells[i] = nextCells[i];

				if (cells[i])
					finished = false;
			}
		}
		
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