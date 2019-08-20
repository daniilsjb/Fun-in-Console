#include "ConsoleGameEngine.h"

#include <map>

const float GRAVITY = 90.0f;

const int TILE_WIDTH = 16;
const int TILE_HEIGHT = 16;

const int TILE_WIDTH_HALF = 8;
const int TILE_HEIGHT_HALF = 8;

class SimplePlatformer : public ConsoleGameEngine
{
	struct Animation
	{
		std::vector<Sprite*> animFrames;

		float animTimer = 0.0f;
		float animFreq;

		int animIndex = 0;
		int frameCount;

		Animation()
		{
			animFreq = 0.0f;
			frameCount = 0;
		}

		Animation(float frequency, std::vector<Sprite*> frames) : animFreq(frequency), animFrames(frames)
		{
			frameCount = frames.size();
		}

		void ResetAnimation()
		{
			animTimer = 0.0f;
			animIndex = 0;
		}

		Sprite* GetCurrentFrame(float timeStep)
		{
			animTimer += timeStep;
			if (animTimer >= animFreq)
			{
				animTimer = 0.0f;
				animIndex++;
				animIndex %= frameCount;
			}
			return animFrames[animIndex];
		}
	};

	struct Object
	{
		//Objects may want to use the engine's methods
		ConsoleGameEngine *engine;

		float x, y;

		bool solid;
		bool destroyed = false;

		//Object's current sprite, change it during animations and such
		Sprite *spr;	

		Object(ConsoleGameEngine *engine) : engine(engine) {}

		//Called once per frame, used to update object's position, state, etc
		virtual void OnUpdate(float elapsedTime) {}

		//Called once per frame just before drawing the sprite onto the screen, update sprite information here
		virtual void OnUpdateSprite(float elapsedTime) {}

		//Called whenever a collision has occured. Arguments may be used to analyze the collision and change the collided object's properties
		virtual void OnInteract(float &vx, float &vy, float &xdir, float &ydir, std::list<Object*> &objects) {}
	};

	struct Tile : public Object
	{
		Tile(ConsoleGameEngine *engine, std::string tileName) : Object(engine)
		{
			spr = sprites[tileName];
			solid = true;
		}
	};

	struct Player : public Object
	{
		//Character's physical properties
		float vx = 0.0f;

		float vy = 0.0f;
		float vymax = 80.0f;

		float speed = 60.0f;

		//Character has 2 versions of each sprite facing different directions
		//In this implementation we only have a walking animation, and its needs 2 separate versions to account for directions
		Animation walkingRight;
		Animation walkingLeft;

		Player(ConsoleGameEngine *engine) : Object(engine)
		{
			walkingRight = Animation(0.1f, { sprites["mario walk1 right"], sprites["mario walk2 right"], sprites["mario walk3 right"] });
			walkingLeft = Animation(0.1f, { sprites["mario walk1 left"], sprites["mario walk2 left"], sprites["mario walk3 left"] });

			solid = true;
		}

		void OnUpdate(float elapsedTime) override
		{
			//Apply physics to change player's position and velocity
			x += vx * elapsedTime;
			y -= vy * elapsedTime;
			vy -= GRAVITY * elapsedTime;
		}

		void OnUpdateSprite(float elapsedTime) override
		{
			//Update player's direction if player is moving horizontally to play correct animationss
			if (vx > 0.0f)
				direction = "right";
			else if (vx < 0.0f)
				direction = "left";

			if (vy != 0.0f)
			{
				//Player is jumping/falling down
				spr = sprites["mario jump " + direction];
			}
			else if (vx != 0.0f)
			{
				//Player is not jumping but is moving
				if (vx > 0)
				{
					spr = walkingRight.GetCurrentFrame(elapsedTime);
					walkingLeft.ResetAnimation();
				}
				else
				{
					spr = walkingLeft.GetCurrentFrame(elapsedTime);
					walkingRight.ResetAnimation();
				}
			}
			else
			{
				//Player isn't jumping or moving, so they are idle
				spr = sprites["mario idle " + direction];
			}

			//If player wasn't moving, then we also need to reset both animations
			if (vx == 0.0f)
			{
				walkingRight.ResetAnimation();
				walkingLeft.ResetAnimation();
			}
		}

	private:
		std::string direction = "right";	//By default player faces right direction
	};

	struct SecretBox : public Object
	{
		SecretBox(ConsoleGameEngine *engine, std::string color) : Object(engine), color(color)
		{
			spr = sprites["secret " + color];
			solid = true;
		}

		void OnInteract(float &vx, float &vy, float &xdir, float &ydir, std::list<Object*> &objects) override
		{
			if (!depleted)
			{
				//Check if the secret box is above the object it's collided with
				if (ydir <= -0.75f)
				{
					depleted = true;
					spr = sprites["secret " + color + " depleted"];

					//Spawn a coin just above the box
					Coin *coin = new Coin(engine);
					coin->x = x;
					coin->y = y - TILE_HEIGHT;
					objects.push_back(coin);
				}
			}
		}

	private:
		std::string color;
		bool depleted = false;
	};

	struct Coin : public Object
	{
		Animation spinning;

		Coin(ConsoleGameEngine *engine) : Object(engine)
		{
			spinning = Animation(0.1f, { sprites["coin spinning 1"], sprites["coin spinning 2"], sprites["coin spinning 3"], sprites["coin spinning 4"] });

			solid = false;
		}

		void OnUpdateSprite(float elapsedTime) override
		{
			spr = spinning.GetCurrentFrame(elapsedTime);
		}

		void OnInteract(float &vx, float &vy, float &xdir, float &ydir, std::list<Object*> &objects) override
		{
			engine->PlayAudioClip(audioClips["coin"]);
			destroyed = true;
		}
	};

	//We need to store each object in the scene including the player, but we also need a separate pointer for the player
	//to have a faster access to it and its special properties.
	std::list<Object*> objects;
	Player *player;

	int mapWidth = 50;
	int mapHeight = 15;
	std::wstring map;

	float halfScreenWidth;
	float halfScreenHeight;

	static std::map<std::string, Sprite*> sprites;
	static std::map<std::string, int> audioClips;

	bool OnStart() override
	{
		SetApplicationTitle(L"Simple Platformer");

		LoadSprites();

		LoadAudio();
		StartAudio();
		PlayAudioClip(audioClips["main"], true);

		map += L"..................................................";
		map += L".............####......####.....####..............";
		map += L".................................................$";
		map += L"..BB?BB?........................................$$";
		map += L".........................................O.....$$$";
		map += L"............$...........................BBB...$$$$";
		map += L"...OO...P..$$....OO.....OO.......$$$$$........####";
		map += L"####################...####...................####";
		map += L"####################.........#................####";
		map += L"####################..........#...............####";
		map += L"####################...............O..........####";
		map += L"#..............................O...####.......####";
		map += L"#......................OO...####..............####";
		map += L"#O....................####....................####";
		map += L"##...B$$$$..BB...##BBB........................####";

		ParseMap();

		halfScreenWidth = (float)GetScreenWidth() / 2.0f;
		halfScreenHeight = (float)GetScreenHeight() / 2.0f;
		
		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		//Only move player horizontally if they've pressed a button
		player->vx = 0.0f;

		if (GetKey(VK_RIGHT).held)
		{
			player->vx = player->speed;
		}

		if (GetKey(VK_LEFT).held)
		{
			player->vx = -player->speed;
		}

		//Player may only jump if they are not already in air
		if (GetKey(VK_UP).pressed)
		{
			if (player->vy == 0.0f)
			{
				player->vy = player->vymax;
				PlayAudioClip(audioClips["jump"]);
			}
		}

		//Player must get updated before every other object in the scene
		player->OnUpdate(elapsedTime);

		//Update objects and handle collisions
		for (auto &object : objects)
		{
			if (object == player)
				continue;

			object->OnUpdate(elapsedTime);

			//Find player's and current object's center positions
			float ox1 = player->x + (float)TILE_WIDTH_HALF;
			float oy1 = player->y + (float)TILE_HEIGHT_HALF;
			float ox2 = object->x + (float)TILE_WIDTH_HALF;
			float oy2 = object->y + (float)TILE_HEIGHT_HALF;

			//Find distances between them
			float dx = ox2 - ox1;
			float dy = oy2 - oy1;

			//Normalize these distances to get direction vector
			float magInv = 1.0f / sqrtf(dx*dx + dy*dy);
			float xdir = dx * magInv;
			float ydir = dy * magInv;

			if (abs(dx) <= TILE_WIDTH && abs(dy) <= TILE_HEIGHT)
			{
				//Collision has happened
				object->OnInteract(player->vx, player->vy, xdir, ydir, objects);

				//Solid objects affect player's position and velocity
				if (object->solid)
				{
					//Only apply changes to player if the object is within a certain range relative to each direction
					if (ydir <= -0.75f)
					{
						//Collided with an object above
						player->y = object->y + TILE_HEIGHT;
						if (player->vy > 0)
							player->vy *= -1;
					}
					else if (ydir >= 0.75f)
					{
						//Collided with an object below
						player->y = object->y - TILE_HEIGHT;
						player->vy = 0.0f;
					}

					if (xdir >= 0.75f)
					{
						//Collided with an object to the right
						player->x -= player->speed * elapsedTime;
					}
					else if (xdir <= -0.75f)
					{
						//Collided with an object to the left
						player->x += player->speed * elapsedTime;
					}
				}
			}
			else
			{
				//No collision
			}
		}

		objects.remove_if([](const Object* obj) { return obj->destroyed; });

		//Draw background
		Fill(0, 0, GetScreenWidth() * GetScreenHeight(), L' ', BG_CYAN);

		//Draw each object in the scene. Offset it by player's position to achieve the effect of camera following the player
		for (auto &object : objects)
		{
			object->OnUpdateSprite(elapsedTime);

			if (object->spr != nullptr)
				DrawSpriteAlpha((int)(object->x - player->x + halfScreenWidth), (int)(object->y - player->y + halfScreenHeight), *object->spr, BG_PINK);
		}

		return true;
	}

	bool OnDestroy() override
	{
		for (auto &object : objects)
			delete object;

		for (auto &sprite : sprites)
			delete sprite.second;

		return true;
	}

	void ParseMap()
	{
		for (int i = 0; i < mapWidth; i++)
		{
			for (int j = 0; j < mapHeight; j++)
			{
				wchar_t c = map[mapWidth * j + i];

				//Empty tiles may be ignored
				if (c == '.') continue;

				Object *object;

				switch (c)
				{
					case '#':
					{
						object = new Tile(this, "tile ground 1");
						break;
					}
					case '$':
					{
						object = new Tile(this, "tile ground 2");
						break;
					}
					case '?':
					{
						object = new SecretBox(this, "dark red");
						break;
					}
					case 'B':
					{
						object = new Tile(this, "tile brick 2");
						break;
					}
					case 'P':
					{
						player = new Player(this);
						object = player;
						break;
					}
					case 'O':
					{
						object = new Coin(this);
						break;
					}
				}

				object->x = (float)(i * TILE_WIDTH);
				object->y = (float)(j * TILE_HEIGHT);

				objects.push_back(object);
			}
		}
	}

	void LoadAudio()
	{
		auto load = [&](std::string clipName, std::wstring fileName)
		{
			audioClips[clipName] = LoadAudioClip(fileName);
		};

		load("main", L"Audio/main.wav");
		load("jump", L"Audio/jump.wav");
		load("coin", L"Audio/coin.wav");
	}

	void LoadSprites()
	{
		auto load = [&](std::string spriteName, std::wstring fileName)
		{
			Sprite *spr = new Sprite(fileName);
			sprites[spriteName] = spr;
		};

		load("coin spinning 1", L"Sprites/coin_spinning_1.spr");
		load("coin spinning 2", L"Sprites/coin_spinning_2.spr");
		load("coin spinning 3", L"Sprites/coin_spinning_3.spr");
		load("coin spinning 4", L"Sprites/coin_spinning_4.spr");

		load("mario idle right", L"Sprites/mario_idle_right.spr");
		load("mario idle left", L"Sprites/mario_idle_left.spr");
		load("mario jump right", L"Sprites/mario_jump_right.spr");
		load("mario jump left", L"Sprites/mario_jump_left.spr");
		load("mario walk1 right", L"Sprites/mario_walk1_right.spr");
		load("mario walk1 left", L"Sprites/mario_walk1_left.spr");
		load("mario walk2 right", L"Sprites/mario_walk2_right.spr");
		load("mario walk2 left", L"Sprites/mario_walk2_left.spr");
		load("mario walk3 right", L"Sprites/mario_walk3_right.spr");
		load("mario walk3 left", L"Sprites/mario_walk3_left.spr");

		load("secret dark red", L"Sprites/secret_dark_red.spr");
		load("secret dark red depleted", L"Sprites/secret_dark_red_depleted.spr");
		load("secret red", L"Sprites/secret_red.spr");
		load("secret red depleted", L"Sprites/secret_red_depleted.spr");
		load("secret yellow", L"Sprites/secret_yellow.spr");
		load("secret yellow depleted", L"Sprites/secret_yellow_depleted.spr");

		load("tile brick 1", L"Sprites/tile_brick_1.spr");
		load("tile brick 2", L"Sprites/tile_brick_2.spr");
		load("tile ground 1", L"Sprites/tile_ground_1.spr");
		load("tile ground 2", L"Sprites/tile_ground_2.spr");
	}
};

std::map<std::string, SimplePlatformer::Sprite*> SimplePlatformer::sprites;
std::map<std::string, int> SimplePlatformer::audioClips;

int main()
{
	SimplePlatformer demo;
	if (demo.ConstructScreen(224, 140, 4, 4))
		demo.Start();

	return 0;
}