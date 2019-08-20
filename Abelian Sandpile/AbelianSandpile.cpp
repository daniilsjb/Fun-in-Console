#include "ConsoleGameEngine.h"

class AbelianSandpile : public ConsoleGameEngine
{
	int *currentSandpile;
	int *nextSandpile;

	bool OnStart() override
	{
		SetApplicationTitle(L"Abelian Sandpile");

		currentSandpile = new int[GetScreenWidth() * GetScreenHeight()];
		nextSandpile = new int[GetScreenWidth() * GetScreenHeight()];

		SecureZeroMemory(currentSandpile, sizeof(int) * GetScreenWidth() * GetScreenHeight());
		SecureZeroMemory(nextSandpile, sizeof(int) * GetScreenWidth() * GetScreenHeight());

		currentSandpile[GetScreenWidth() * (GetScreenHeight() / 2) + (GetScreenWidth() / 2)] = 100000;
		nextSandpile[GetScreenWidth() * (GetScreenHeight() / 2) + (GetScreenWidth() / 2)] = 100000;

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		for (int i = 0; i < GetScreenWidth(); i++)
		{
			for (int j = 0; j < GetScreenHeight(); j++)
			{
				int sand = currentSandpile[GetScreenWidth() * j + i];
				short color;
				switch (sand)
				{
					case 0: color = BG_BLACK; break;
					case 1: color = BG_GREEN; break;
					case 2: color = BG_BLUE; break;
					case 3: color = BG_YELLOW; break;
					case 4: color = BG_CYAN; break;
					default: color = BG_PINK; break;
				}
				DrawPoint(i, j, ' ', color);

				if (sand >= 4)
				{
					if (j > 0)
						nextSandpile[GetScreenWidth() * (j - 1) + i] += 1;

					if (j < GetScreenHeight() - 1)
						nextSandpile[GetScreenWidth() * (j + 1) + i] += 1;

					if (i > 0)
						nextSandpile[GetScreenWidth() * j + (i - 1)] += 1;

					if (i < GetScreenWidth() - 1)
						nextSandpile[GetScreenWidth() * j + (i + 1)] += 1;

					nextSandpile[GetScreenWidth() * j + i] -= 4;
				}
			}
		}

		std::memcpy(currentSandpile, nextSandpile, sizeof(int) * GetScreenWidth() * GetScreenHeight());

		return true; 
	}

	bool OnDestroy()
	{
		delete[] currentSandpile;
		delete[] nextSandpile;

		return true;
	}
};

int main()
{
	AbelianSandpile demo;
	if (demo.ConstructScreen(128, 128, 4, 4))
		demo.Start();

	return 0;
}