#include "ConsoleGameEngine.h"

#include <map>

class FlappyBird : public ConsoleGameEngine
{
	struct Tube
	{
		float x;
		int gapY;

		Tube(float x, int gapY) : x(x), gapY(gapY) {}
	};
	
	struct Cloud
	{
		float x;
		int y;

		Sprite* spr;

		Cloud(float x, int y, Sprite* spr) : x(x), y(y), spr(spr) {}
	};

	int playerX;
	float playerY;

	float vy;
	float maxVelocity = 37.5f;

	float gravity = 40.0f;

	int birdWidth;
	int birdHeight;

	int tubeWidth;
	int tubeHeight;

	float tubeSpeed = 35.0f;

	int gapHeight;

	std::list<Tube*> tubes;

	int cloudWidth;
	int cloudHeight;

	float cloudTimer = 0.0f;
	float cloudSpeed = 40.0f;

	std::list<Cloud*> clouds;

	bool gameOver = false;
	float gameOverTimer = 0.0f;

	std::map<std::string, Sprite*> sprites;

	bool OnStart() override
	{
		SetApplicationTitle(L"Flappy Bird");

		//Prepare sprites and relevant data
		LoadSprites();

		birdWidth = sprites["bird_up"]->GetWidth();
		birdHeight = sprites["bird_up"]->GetHeight();

		tubeWidth = sprites["tube_part"]->GetWidth();
		tubeHeight = sprites["tube_part"]->GetHeight();

		cloudWidth = sprites["cloud_1"]->GetWidth();
		cloudHeight = sprites["cloud_1"]->GetHeight();

		gapHeight = tubeHeight * 4;

		//Set player's initial position and velocity
		playerX = (int)(GetScreenWidth() / 4);
		playerY = (float)GetScreenHeight() / 2.0f;

		vy = maxVelocity;

		//Add tubes and clouds to start off
		srand(time(0));

		//We must stagger the tubes by offsetting 2 of them
		AddTube();
		AddTube((float)GetScreenWidth() / 2.0f);
		AddTube((float)GetScreenWidth());

		AddCloud();

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		//Create a small pause if the game is over
		if (gameOver)
		{
			gameOverTimer += elapsedTime;
			if (gameOverTimer >= 2.0f)
				return false;
			else
				return true;
		}	

		//Allow player to give the bird some velocity, but only if the bird is already falling down
		if (GetKey(' ').pressed)
		{
			if (vy < 0)
				vy = maxVelocity;
		}

		//Apply gravity to player
		playerY -= vy * elapsedTime;
		vy -= gravity * elapsedTime;

		//If player has hit an edge of the screen, then the game is over
		if (playerY < 0.0f)
		{
			playerY = 0.0f;
			gameOver = true;
		}
		else if (playerY + (float)birdHeight > (float)GetScreenHeight())
		{
			playerY = (float)(GetScreenHeight() - birdHeight);
			gameOver = true;
		}

		//Update each tube's position
		for (auto &tube : tubes)
		{
			tube->x -= tubeSpeed * elapsedTime;

			//If player has hit a tube, then the game is over
			//We bias the player a bit because it doesn't feel really nice if bird's outline hits a tube and it counts as game over
			if (playerX + birdWidth - 2 >= (int)tube->x && playerX + 1 < (int)tube->x + tubeWidth)
			{
				if (playerY + 1 < (float)tube->gapY || playerY + birdHeight - 1 >= (float)(tube->gapY + gapHeight))
					gameOver = true;
			}
		}

		//The first tube is always the leftmost, so we may check only the first tube's visibility
		if (tubes.front()->x < -tubeWidth)
		{
			delete tubes.front();
			tubes.pop_front();

			//We can prepare a new tube right away but offset it to preserve some distance to the other ones
			AddTube((float)GetScreenWidth() / 4.0f);
		}

		//Spawn a new cloud every 1.5 seconds
		cloudTimer += elapsedTime;
		if (cloudTimer >= 1.5f)
		{
			AddCloud();
			cloudTimer = 0.0f;
		}

		//Move every cloud
		for (auto &cloud : clouds)
		{
			cloud->x -= cloudSpeed * elapsedTime;
		}

		//The first cloud is the leftmost, so check just that for visibility
		if (clouds.front()->x < -cloudWidth)
		{
			delete clouds.front();
			clouds.pop_front();
		}

		//Draw background
		Fill(0, 0, GetScreenWidth() * GetScreenHeight(), L' ', BG_CYAN);

		//Draw clouds
		for (auto &cloud : clouds)
		{
			DrawSpriteAlpha((int)cloud->x, cloud->y, *cloud->spr, BG_CYAN);
		}

		//Draw tubes
		for (auto &tube : tubes)
		{
			for (int ty = 0; ty < GetScreenHeight(); ty += tubeHeight)
			{
				if (ty >= tube->gapY && ty < tube->gapY + gapHeight)
					continue;

				if (ty + tubeHeight == tube->gapY)
					DrawSprite((int)tube->x, ty, *sprites["tube_tip"]);
				else if (ty == tube->gapY + gapHeight)
					DrawSprite((int)tube->x, ty, *sprites["tube_tip"]);
				else
					DrawSpriteAlpha((int)tube->x, ty, *sprites["tube_part"], BG_CYAN);
			}
		}

		//Draw player
		if (vy >= 0)
			DrawSpriteAlpha(playerX, (int)playerY, *sprites["bird_up"], BG_CYAN);
		else
			DrawSpriteAlpha(playerX, (int)playerY, *sprites["bird_down"], BG_CYAN);

		return true;
	}

	bool OnDestroy() override
	{
		for (auto &spr : sprites)
		{
			delete spr.second;
		}

		return true;
	}

	void LoadSprites()
	{
		auto load = [&](std::string spriteName, std::wstring fileName)
		{
			Sprite *spr = new Sprite(fileName);
			sprites[spriteName] = spr;
		};

		load("bird_up", L"Sprites/bird_up.spr");
		load("bird_down", L"Sprites/bird_down.spr");
		load("tube_part", L"Sprites/tube_part.spr");
		load("tube_tip", L"Sprites/tube_tip.spr");
		load("cloud_1", L"Sprites/cloud_1.spr");
		load("cloud_2", L"Sprites/cloud_2.spr");
		load("cloud_3", L"Sprites/cloud_3.spr");
		load("cloud_4", L"Sprites/cloud_4.spr");
	}

	void AddTube(float offset = 0.0f)
	{
		float x = (float)(GetScreenWidth() + tubeWidth + offset);
		int gapY = GenerateGap();
		tubes.push_back(new Tube(x, gapY));
	}

	int GenerateGap()
	{
		int availableSpace = (GetScreenHeight() / tubeHeight) - 1;	//Exclusive because we don't want tubes that start at the bottom of the screen
		int gapSpace = gapHeight / tubeHeight;

		//There should always be at least 1 part of a tube at the top and at least 1 part of the tube at the bottom, accounting for the gap space
		int position = rand() % (availableSpace - 1 - gapSpace) + 1;
		return position * tubeHeight;	//Bring gap's position on the tube into screen dimensions
	}

	void AddCloud()
	{
		float x = (float)GetScreenWidth();
		int y = rand() % (GetScreenHeight() - (cloudHeight * 2)) + cloudHeight;	//The cloud's top edge must be within screen's visibility
		Sprite *spr = sprites["cloud_" + std::to_string(rand() % 3 + 1)];
		clouds.push_back(new Cloud(x, y, spr));
	}
};

int main()
{
	FlappyBird demo;
	if (demo.ConstructScreen(156, 80, 8, 8))
		demo.Start();

	return 0;
}