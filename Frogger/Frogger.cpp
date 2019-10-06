#include "ConsoleGameEngine.h"
#include <map>

class Frogger : public ConsoleGameEngine
{
	//Player data
	float playerX;
	float playerY;

	float playerStep = 16.0f;

	bool playerDead = false;
	bool playerWon = false;

	enum { UP, DOWN, LEFT, RIGHT } playerDirection;

	//Each object is associated with a type, which holds shared data that won't be modified
	struct ObjectType
	{
		float y;			//Each type of object only exists within a single row, so it will never move up or down
		float velocity;		//May be either negative or positive depending on the direction
		float distance;		//Distance between objects of the same type
		Sprite* spr;

		int width = 0;
		int height = 0;

		ObjectType(float y, float velocity, float distance, Sprite* spr) : y(y), velocity(velocity), distance(distance), spr(spr)
		{
			if (spr != nullptr)
			{
				width = spr->GetWidth();
				height = spr->GetHeight();
			}
		}
	};

	//Represents all dynamic objects in the game (i.e. cars and logs)
	struct Object
	{
		float x;
		ObjectType* type;

		Object(float x, ObjectType* type) : x(x), type(type) {}
	};

	//For a lack of a better word, a 'finish' cell is what the player is trying to reach
	struct Finish
	{
		float x;
		float y = 0.0f;	//Finish is always located at the first row
		bool occupied = false;
		bool fly = false;

		Finish(float x) : x(x) {}
	};

	std::vector<ObjectType*> types;
	std::list<Object*> objects;

	std::vector<Finish*> finishCells;
	int finishedCount = 0;

	//The game is paused whenever player dies or wins
	float pauseTime = 3.0f;
	float pauseTimer = 0.0f;

	//Data for the nasty nasty fly
	float flySpawnTime = 10.0f;
	float flyActiveTime = 3.0f;
	float flyTimer = 0.0f;
	int flyPosition = 0;
	bool flyActive = false;

	//Starting and ending points of two most important regions in the game
	int waterStart = 15;
	int waterEnd = waterStart + (16 * 5);
	int roadStart = waterEnd + 16;
	int roadEnd = roadStart + (16 * 5);

	std::map<std::string, Sprite*> sprites;

	bool OnStart() override
	{
		SetApplicationTitle(L"Frogger");

		LoadSprites();

		ResetPlayer();

		//Finish cells aren't perfectly aligned with the other tiles
		//They start after a tile half, and the distance between them is 2 tiles (3 if we count the finish cell itself)
		for (int i = 0; i < 5; i++)
			finishCells.push_back(new Finish(7.0f + 48.0f * i));

		//A few precomputed distances for object types
		float smallDist = (float)GetScreenWidth() / 3.0f;
		float largeDist = (float)GetScreenWidth() / 2.0f;

		//Create all the object types
		types.push_back(new ObjectType(GetScreenHeight() - 32.0f, -20.0f, smallDist, sprites["Yellow Car"]));
		types.push_back(new ObjectType(GetScreenHeight() - 48.0f, 20.0f, smallDist, sprites["Purple Car"]));
		types.push_back(new ObjectType(GetScreenHeight() - 64.0f, -20.0f, smallDist, sprites["Bulldozer"]));
		types.push_back(new ObjectType(GetScreenHeight() - 80.0f, 40.0f, largeDist, sprites["Police Car"]));
		types.push_back(new ObjectType(GetScreenHeight() - 96.0f, -20.0f, largeDist, sprites["Truck"]));
		types.push_back(new ObjectType(GetScreenHeight() - 128.0f, 30.0f, smallDist, sprites["Large Log"]));
		types.push_back(new ObjectType(GetScreenHeight() - 144.0f, -20.0f, largeDist, sprites["Small Log"]));
		types.push_back(new ObjectType(GetScreenHeight() - 160.0f, 40.0f, smallDist, sprites["Large Log"]));
		types.push_back(new ObjectType(GetScreenHeight() - 176.0f, -45.0f, largeDist, sprites["Small Log"]));
		types.push_back(new ObjectType(GetScreenHeight() - 192.0f, 30.0f, smallDist, sprites["Large Log"]));

		//Spawn the objects, make them spaced based on the distance of their types
		//Keep in mind that some objects go from left to right and some objects go from right to left, so spawn them accordingly
		//Road
		for (int i = -2; i <= 1; i++)
			objects.push_back(new Object((float)GetScreenWidth() + types[0]->distance * i, types[0]));

		for (int i = 2; i >= -1; i--)
			objects.push_back(new Object(0.0f + types[1]->distance * i, types[1]));

		for (int i = -2; i <= 1; i++)
			objects.push_back(new Object((float)GetScreenWidth() + types[2]->distance * i, types[2]));

		for (int i = 1; i >= -1; i--)
			objects.push_back(new Object(0.0f + types[3]->distance * i, types[3]));

		for (int i = -1; i <= 1; i++)
			objects.push_back(new Object((float)GetScreenWidth() + types[4]->distance * i, types[4]));

		//Water
		for (int i = 2; i >= -1; i--)
			objects.push_back(new Object(0.0f + types[5]->distance * i, types[5]));

		for (int i = -1; i <= 1; i++)
			objects.push_back(new Object((float)GetScreenWidth() + types[6]->distance * i, types[6]));

		for (int i = 2; i >= -1; i--)
			objects.push_back(new Object(0.0f + types[7]->distance * i, types[7]));

		for (int i = -1; i <= 1; i++)
			objects.push_back(new Object((float)GetScreenWidth() + types[8]->distance * i, types[8]));

		for (int i = 2; i >= -1; i--)
			objects.push_back(new Object(0.0f + types[9]->distance * i, types[9]));

		//Make the nasty nasty fly happy by adding some randomization
		srand(time(0));

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		//Update timing

		//Create a small pause if player is dead before respawning them
		if (playerDead)
		{
			pauseTimer += elapsedTime;
			if (pauseTimer >= pauseTime)
			{
				pauseTimer = 0.0f;
				ResetPlayer();
			}
		}

		//Let player witness the fruit of their victory by making a small pause
		if (playerWon)
		{
			pauseTimer += elapsedTime;
			if (pauseTimer >= pauseTime)
				return false;
			else
				return true;
		}

		//Since the fly is so busy its timer is always updated, though its meaning depends on the fly's state
		flyTimer += elapsedTime;
		if (flyActive && flyTimer >= flyActiveTime)
		{
			flyTimer -= flyActiveTime;
			finishCells[flyPosition]->fly = false;
			flyActive = false;
		}
		if (!flyActive && flyTimer >= flySpawnTime)
		{
			flyTimer -= flySpawnTime;
			flyPosition = rand() % (5 - finishedCount);
			finishCells[flyPosition]->fly = true;
			flyActive = true;
		}

		//Handle input, but only if the player is alive
		if (!playerDead)
		{
			if (GetKey(VK_UP).released)
			{
				if (playerY - playerStep >= 0.0f)
					playerY -= playerStep;

				//Unless player has touched a finish cell, we don't want them to be at the finish row
				if (playerY >= 0.0f && playerY < waterStart)
				{
					if (TouchedFinish() == nullptr)
						playerY += playerStep;
				}

				playerDirection = UP;
			}

			if (GetKey(VK_DOWN).pressed)
			{
				if (playerY + playerStep < (float)GetScreenHeight())
					playerY += playerStep;

				playerDirection = DOWN;
			}

			if (GetKey(VK_LEFT).released)
			{
				if (playerX - playerStep >= 0.0f)
					playerX -= playerStep;
				else
					playerX = 0.0f;	//Make sure player's position is clamped

				playerDirection = LEFT;
			}

			if (GetKey(VK_RIGHT).released)
			{
				if (playerX + playerStep < (float)GetScreenWidth() - 16.0f)
					playerX += playerStep;
				else
					playerX = (float)GetScreenWidth() - 16.0f;	//Make sure player's position is clamped

				playerDirection = RIGHT;
			}
		}
		
		//Update objects

		//We need to see if player is touching any objects, as well as update their positions
		Object* collided = nullptr;
		for (auto obj : objects)
		{
			//Update object position
			float velocity = obj->type->velocity;
			float distance = obj->type->distance;
			obj->x += velocity * elapsedTime;

			//Objects are reused by being moved to their starting position when they aren't visible anymore
			//However, objects are moved to a certain distance outside of the screen, so their visibility is determined by direction
			//This way we always have a circular queue of same objects infinitely moving across the screen
			if (velocity > 0 && obj->x > (float)GetScreenWidth())
				obj->x = -distance;
			else if (velocity < 0 && obj->x < -obj->type->width)
				obj->x = (float)GetScreenWidth() + distance;

			//There is no point in collision detection if they aren't on the same row
			if (obj->type->y != playerY)
				continue;

			//Collision detection is done based on the distance between the objects' centers
			float halfObjWidth = obj->type->width * 0.5f;
			float halfPlayerWidth = 8.0f;

			//Collision detection is done differently for cars and logs
			//For a car it's enough to even touch player, but for logs it's required that player is entirely on them
			float playerSpace = (playerY > roadStart) ? halfPlayerWidth : 0.0f;

			//If the absolute distance between centers is less than sum of half widths, a collision has occured
			float dx = abs((obj->x + halfObjWidth) - (playerX + halfPlayerWidth));
			if (dx <= halfObjWidth + playerSpace)
				collided = obj;
		}

		//Now we may update player's state if they are alive
		if (!playerDead)
		{
			//See if player has reached the finish row
			if (playerY >= 0 && playerY < waterStart)
			{
				//Find the finish cell they are touching
				Finish* finish = TouchedFinish();
				if (finish != nullptr)
				{
					if (finish->occupied || finish->fly)
					{
						//Collided with an already occupied cell or a fly, so it's a game over
						playerDead = true;
					}
					else
					{
						//Use this cell
						finish->occupied = true;
						finishedCount++;

						//Occupied cells are moved to the end to make it easier for fly to choose an unoccupied cell
						std::partition(finishCells.begin(), finishCells.end(), [](const Finish* f) { return !f->occupied; });

						//Naturally, player has won if every cell has been occupied
						if (finishedCount == finishCells.size())
							playerWon = true;

						ResetPlayer();
					}
				}
			}
			else if (playerY >= waterStart && playerY < waterEnd)
			{
				//Player is within the water region
				if (collided != nullptr)
				{
					//Move with the log if player is on top of one
					playerX += collided->type->velocity * elapsedTime;

					//The player dies if they were fully taken outside the screen. RIP.
					if (playerX < -16 || playerX > GetScreenWidth())
						playerDead = true;
				}
				else
				{
					//Otherwise the player has drowned, the game is over
					playerDead = true;
				}
			}
			else if (playerY >= roadStart && playerY < roadEnd)
			{
				//Player is within the road region
				if (collided != nullptr)
				{
					//Car has hit a player, so it's a game over
					playerDead = true;
				}
			}
		}

		//Render

		//Fill the upper half of the screen with water and the lower half with road
		int heightHalf = GetScreenHeight() / 2;
		Fill(0, 0, GetScreenWidth() * heightHalf, ' ', BG_BLUE);
		Fill(0, heightHalf + 1, GetScreenWidth() * (heightHalf - 1), ' ', BG_BLACK);

		//Draw the first grass edge, which is half a tile
		DrawPartialSpriteAlpha(0, 0, *sprites["Grass Edge"], 0, 0, 8, 16, BG_PINK);

		//Everything between edges goes in order FINISH -> LEFT -> RIGHT
		int n = 0;
		for (int x = 8; x < GetScreenWidth() - 8; x += 16)
		{
			if (n == 0)
				DrawSpriteAlpha(x, 0, *sprites["Grass Finish"], BG_PINK);
			else if (n == 1)
				DrawSpriteAlpha(x, 0, *sprites["Grass Left"], BG_PINK);
			else
				DrawSpriteAlpha(x, 0, *sprites["Grass Right"], BG_PINK);

			n = (n + 1) % 3;
		}

		//Draw the second grass edge, which is the other half a tile
		DrawPartialSpriteAlpha(GetScreenWidth() - 8, 0, *sprites["Grass Edge"], 8, 0, 8, 16, BG_PINK);

		//Draw ground where player starts and between water and road
		for (int x = 0; x < GetScreenWidth(); x += 16)
		{
			DrawSpriteAlpha(x, waterEnd, *sprites["Ground"], BG_PINK);
			DrawSpriteAlpha(x, roadEnd + 1, *sprites["Ground"], BG_PINK);
		}

		//Player sprite depends on their state
		Sprite* playerSprite = nullptr;

		//The player's death sprite is dependent on whether they were drowned or hit by a car
		if (playerDead)
		{
			if (playerY < heightHalf)
				playerSprite = sprites["Frogger Drowned"];
			else
				playerSprite = sprites["Frogger Killed"];
		}
		else
		{
			switch (playerDirection)
			{
				case UP: playerSprite = sprites["Frogger Up"]; break;
				case DOWN: playerSprite = sprites["Frogger Down"]; break;
				case LEFT: playerSprite = sprites["Frogger Left"]; break;
				case RIGHT: playerSprite = sprites["Frogger Right"]; break;
				default: playerSprite = sprites["Frogger Up"]; break;
			}
		}

		//We want to draw player below other objects when dead and above when alive
		if (playerDead)
			DrawSpriteAlpha((int)playerX, (int)playerY, *playerSprite, BG_PINK);

		for (auto obj : objects)
			DrawSpriteAlpha((int)obj->x, (int)obj->type->y, *obj->type->spr, BG_PINK);

		if (!playerDead)
			DrawSpriteAlpha((int)playerX, (int)playerY, *playerSprite, BG_PINK);

		//Visualize finish cells' states if needed
		for (auto finish : finishCells)
		{
			if (finish->occupied)
				DrawSpriteAlpha((int)finish->x, (int)finish->y, *sprites["Frogger Saved"], BG_PINK);
			else if (finish->fly)
				DrawSpriteAlpha((int)finish->x, (int)finish->y, *sprites["Fly"], BG_PINK);
		}

		return true;
	}

	bool OnDestroy() override
	{
		for (auto obj : objects)
			delete obj;

		for (auto type : types)
			delete type;

		for (auto spr : sprites)
			delete spr.second;

		return true;
	}

	void ResetPlayer()
	{
		playerX = GetScreenWidth() * 0.5f;
		playerY = GetScreenHeight() - 16.0f;
		playerDirection = UP;
		playerDead = false;
	}

	void LoadSprites()
	{
		auto load = [&](std::string sprName, std::wstring fileName)
		{
			sprites[sprName] = new Sprite(fileName);
		};

		load("Frogger Up", L"Sprites/frogger_up.spr");
		load("Frogger Down", L"Sprites/frogger_down.spr");
		load("Frogger Left", L"Sprites/frogger_left.spr");
		load("Frogger Right", L"Sprites/frogger_right.spr");
		load("Frogger Saved", L"Sprites/frogger_saved.spr");
		load("Frogger Killed", L"Sprites/frogger_killed.spr");
		load("Frogger Drowned", L"Sprites/frogger_drowned.spr");
		load("Yellow Car", L"Sprites/yellow_car_left.spr");
		load("Purple Car", L"Sprites/purple_car_right.spr");
		load("Bulldozer", L"Sprites/bulldozer_left.spr");
		load("Police Car", L"Sprites/police_car_right.spr");
		load("Truck", L"Sprites/truck_left.spr");
		load("Ground", L"Sprites/ground.spr");
		load("Grass Edge", L"Sprites/grass_edge.spr");
		load("Grass Left", L"Sprites/grass_left.spr");
		load("Grass Right", L"Sprites/grass_right.spr");
		load("Grass Finish", L"Sprites/grass_finish.spr");
		load("Fly", L"Sprites/fly.spr");
		load("Small Log", L"Sprites/small_log.spr");
		load("Large Log", L"Sprites/large_log.spr");
	}

	Finish* TouchedFinish()
	{
		//Search for a finish cell player is fully within
		for (auto finish : finishCells)
		{
			float dx = abs((finish->x + 8.0f) - (playerX + 8.0f));
			if (dx <= 8.0f)
			{
				return finish;
			}
		}
		return nullptr;
	}
};

int main()
{
	Frogger demo;
	if (demo.ConstructScreen(224, 208, 4, 4))
		demo.Start();

	return 0;
}