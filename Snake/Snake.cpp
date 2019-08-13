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
		SetApplicationTitle(L"Snake");

		//Set snake to initally consist of 3 parts, its head in the center
		snake = { 
			{ (short)(GetScreenWidth() / 2), (short)(GetScreenHeight() / 2) },
			{ (short)(GetScreenWidth() / 2 - 1), (short)(GetScreenHeight() / 2) },
			{ (short)(GetScreenWidth() / 2 - 2), (short)(GetScreenHeight() / 2) }
		};

		//By default snake points to the right
		dirX = 1;
		dirY = 0;

		//Snake can't move instantly
		mayMove = false;
		mayChangeDir = false;

		timer = 0.0f;

		apple = GenerateApple();

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		//Handle snake movement. Snake can't move into itself, and there is a cooldown for changing its direction.
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

		//Handle timing
		timer += elapsedTime;
		if (timer >= 0.15f)	//Time is considered in seconds (1.0f = 1000 milliseconds)
		{
			mayMove = true;
			mayChangeDir = true;
			timer = 0.0f;
		}
		else
			mayMove = false;
		
		//Draw background
		Fill(0, 0, GetScreenWidth() * GetScreenHeight(), PIXEL_SOLID, FG_GREEN);

		//Draw apple
		DrawPoint(apple.X, apple.Y, PIXEL_SOLID, FG_RED);

		//Draw the snake from tail to head
		for (int i = snake.size() - 1; i >= 0; i--)
		{
			//Draw current body part
			DrawPoint(snake[i].X, snake[i].Y, PIXEL_SOLID, FG_DARK_GREEN);

			//Move snake if possible
			if (mayMove)
			{
				if (i == 0)
				{
					//If it's the head, move it forward
					snake[i].X += dirX;
					snake[i].Y += dirY;

					//Check if the apple has been eaten
					if (snake[i].X == apple.X && snake[i].Y == apple.Y)
					{
						apple = GenerateApple();

						//Add a new body part to where the tail used to be (it won't move yet in this iteration, essentially extending the snake)
						snake.push_back(snake[snake.size() - 1]);	
					}
				}	
				else
					snake[i] = snake[i - 1];	//Else just move the body part to where the next body part is located
			}
		}

		//Highlight snake's head
		DrawPoint(snake[0].X, snake[0].Y, PIXEL_SOLID, FG_DARK_YELLOW);

		//If snake's head hits screen boundaries, game is over
		if (snake[0].X < 0 || snake[0].X >= GetScreenWidth() || snake[0].Y < 0 || snake[0].Y >= GetScreenHeight())
			return false;

		//If snake's head hits another body part, game is over
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