#include "ConsoleGameEngine.h"

class Snake : public ConsoleGameEngine
{
	std::vector<COORD> snake;

	int dirX, dirY;

	bool mayMove;
	bool mayChangeDir;

	COORD apple;

	float timer;

	bool OnStart() override
	{
		SetConsoleTitle(L"Snake");

		snake = { 
			{ (short)(GetScreenWidth() / 2), (short)(GetScreenHeight() / 2) },
			{ (short)(GetScreenWidth() / 2 - 1), (short)(GetScreenHeight() / 2) },
			{ (short)(GetScreenWidth() / 2 - 2), (short)(GetScreenHeight() / 2) }
		};

		dirX = 1;
		dirY = 0;

		mayMove = false;
		mayChangeDir = false;

		timer = 0.0f;

		apple = GenerateApple();

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		if (GetKey(VK_UP).pressed)
		{
			if (dirY != 1 && mayChangeDir)
			{
				dirX = 0;
				dirY = -1;
				mayChangeDir = false;
			}
		}

		if (GetKey(VK_DOWN).pressed)
		{
			if (dirY != -1 && mayChangeDir)
			{
				dirX = 0;
				dirY = 1;
				mayChangeDir = false;
			}
		}

		if (GetKey(VK_LEFT).pressed)
		{
			if (dirX != 1 && mayChangeDir)
			{
				dirX = -1;
				dirY = 0;
				mayChangeDir = false;
			}
		}

		if (GetKey(VK_RIGHT).pressed)
		{
			if (dirX != -1 && mayChangeDir)
			{
				dirX = 1;
				dirY = 0;
				mayChangeDir = false;
			}
		}

		timer += elapsedTime;
		if (timer >= 0.15f)
		{
			mayMove = true;
			mayChangeDir = true;
			timer = 0.0f;
		}
		else
			mayMove = false;
		
		Fill(0, 0, GetScreenWidth() * GetScreenHeight(), PIXEL_SOLID, FG_GREEN);

		DrawPoint(apple.X, apple.Y, PIXEL_SOLID, FG_RED);

		for (int i = snake.size() - 1; i >= 0; i--)
		{
			DrawPoint(snake[i].X, snake[i].Y, PIXEL_SOLID, FG_DARK_GREEN);

			if (mayMove)
			{
				if (i == 0)
				{
					snake[i].X += dirX;
					snake[i].Y += dirY;

					if (snake[i].X == apple.X && snake[i].Y == apple.Y)
					{
						apple = GenerateApple();
						snake.push_back(snake[snake.size() - 1]);
					}
				}	
				else
					snake[i] = snake[i - 1];
			}
		}

		DrawPoint(snake[0].X, snake[0].Y, PIXEL_SOLID, FG_DARK_YELLOW);

		if (snake[0].X < 0 || snake[0].X >= GetScreenWidth() || snake[0].Y < 0 || snake[0].Y >= GetScreenHeight())
			return false;

		for (size_t i = 1; i < snake.size(); i++)
		{
			if (snake[0].X == snake[i].X && snake[0].Y == snake[i].Y)
				return false;
		}

		return true;
	}

	COORD GenerateApple()
	{
		bool appleIsValid = false;

		COORD newApple;

		while (!appleIsValid)
		{
			newApple.X = rand() % GetScreenWidth();
			newApple.Y = rand() % GetScreenHeight();

			bool appleCollided = false;
			for (auto &snakePart : snake)
			{
				if (newApple.X == snakePart.X && newApple.Y == snakePart.Y)
				{
					appleCollided = true;
				}
			}

			appleIsValid = !appleCollided;
		}

		return newApple;
	}
};

int main()
{
	Snake demo;
	if (demo.ConstructScreen(64, 32, 16, 16))
		demo.Start();
}