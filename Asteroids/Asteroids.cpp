#include "ConsoleGameEngine.h"

class Asteroids : public ConsoleGameEngine
{
	struct SpaceObject
	{
		float x, y;
		float vx, vy;
		float angle;
		float scale;

		std::vector<std::pair<float, float>> model;

		bool removed = false;
	};

	SpaceObject player;
	std::vector<SpaceObject> asteroids;
	std::vector<SpaceObject> bullets;

	bool OnStart() override
	{
		SetApplicationTitle(L"Asteroids");

		srand(time(0));

		player.model = {
			{ 0.0f,-5.0f },
			{-2.5f, 2.5f },
			{ 2.5f, 2.5f }
		};

		ResetGame();

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		if (player.removed)
			ResetGame();

		if (GetKey(VK_LEFT).held)
			player.angle -= 5.0f * elapsedTime;
		if (GetKey(VK_RIGHT).held)
			player.angle += 5.0f * elapsedTime;

		if (GetKey(VK_UP).held)
		{
			player.vx += sinf(player.angle) * 20.0f * elapsedTime;
			player.vy += -cosf(player.angle) * 20.0f * elapsedTime;
		}

		if (GetKey(VK_SPACE).pressed)
			bullets.push_back(SpaceObject({ player.x, player.y, sinf(player.angle) * 40.0f, -cosf(player.angle) * 40.0f, 0.0f }));

		player.x += player.vx * elapsedTime;
		player.y += player.vy * elapsedTime;

		WrapCoordinates(player.x, player.y, player.x, player.y);

		ClearScreen();

		for (auto &asteroid : asteroids)
		{
			asteroid.x += asteroid.vx * elapsedTime;
			asteroid.y += asteroid.vy * elapsedTime;

			asteroid.angle += 0.5f * elapsedTime;

			WrapCoordinates(asteroid.x, asteroid.y, asteroid.x, asteroid.y);

			if (Collides(player.x, player.y, asteroid.x, asteroid.y, asteroid.scale))
				player.removed = true;

			DrawModel(asteroid.model, asteroid.x, asteroid.y, asteroid.angle, asteroid.scale, ' ', BG_YELLOW);
		}

		std::vector<SpaceObject> newAsteroids;

		for (auto &bullet : bullets)
		{
			bullet.x += bullet.vx * elapsedTime;
			bullet.y += bullet.vy * elapsedTime;

			if (bullet.x < 0.0f || bullet.x >= (float)GetScreenWidth() || bullet.y < 0.0f || bullet.y >= (float)GetScreenHeight())
			{
				bullet.removed = true;
			}
			else
			{
				for (auto &a : asteroids)
				{
					if (!a.removed && Collides(bullet.x, bullet.y, a.x, a.y, a.scale))
					{
						bullet.removed = true;
						a.removed = true;

						if (a.scale > 4.0f)
						{
							float angleA = ((float)rand() / (float)RAND_MAX) * 6.28318f;
							float angleB = ((float)rand() / (float)RAND_MAX) * 6.28318f;
							newAsteroids.push_back(SpaceObject({ a.x, a.y, sinf(angleA) * 10.0f, cosf(angleA) * 10.0f, 0.0f, a.scale * 0.5f, MakeAsteroidModel() }));
							newAsteroids.push_back(SpaceObject({ a.x, a.y, sinf(angleB) * 10.0f, cosf(angleB) * 10.0f, 0.0f, a.scale * 0.5f, MakeAsteroidModel() }));
						}

						break;
					}
				}
			}

			if (!bullet.removed)
				DrawPoint((int)bullet.x, (int)bullet.y);
		}

		bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [&](const SpaceObject& bullet) { return bullet.removed; }), bullets.end());
		asteroids.erase(std::remove_if(asteroids.begin(), asteroids.end(), [&](const SpaceObject& asteroid) { return asteroid.removed; }), asteroids.end());

		for (auto &asteroid : newAsteroids)
			asteroids.push_back(asteroid);

		if (asteroids.empty())
			SpawnAsteroids();

		DrawModel(player.model, player.x, player.y, player.angle);

		return true;
	}

	void ResetGame()
	{
		player.x = (float)GetScreenWidth() * 0.5f;
		player.y = (float)GetScreenHeight() * 0.5f;
		player.vx = 0.0f;
		player.vy = 0.0f;
		player.angle = 0.0f;
		player.removed = false;

		bullets.clear();
		asteroids.clear();

		SpawnAsteroids();
	}

	bool Collides(float x, float y, float cx, float cy, float radius)
	{
		return sqrt((cx - x) * (cx - x) + (cy - y) * (cy - y)) <= radius;
	}

	void WrapCoordinates(float x, float y, float &wx, float &wy)
	{
		wx = x;
		wy = y;

		if (x < 0.0f) wx += (float)GetScreenWidth();
		else if (x >= (float)GetScreenWidth()) wx -= (float)GetScreenWidth();

		if (y < 0.0f) wy += (float)GetScreenHeight();
		else if (y >= (float)GetScreenHeight()) wy -= (float)GetScreenHeight();
	}

	void SpawnAsteroids()
	{
		float right = player.angle + (3.14159f * 0.5f);
		float left = player.angle - (3.14159f * 0.5f);
		float angleA = ((float)rand() / (float)RAND_MAX) * 6.28318f;
		float angleB = ((float)rand() / (float)RAND_MAX) * 6.28318f;

		asteroids.push_back(SpaceObject({ player.x + sinf(right) * 50.0f, player.y + cosf(right) * 50.0f, sinf(angleA) * 10.0f, cosf(angleA) * 10.0f, 0.0f, 16.0f, MakeAsteroidModel() }));
		asteroids.push_back(SpaceObject({ player.x + sinf(left) * 50.0f, player.y + cosf(left) * 50.0f, sinf(angleB) * 10.0f, cosf(angleB) * 10.0f, 0.0f, 16.0f, MakeAsteroidModel() }));
	}

	std::vector<std::pair<float, float>> MakeAsteroidModel()
	{
		std::vector<std::pair<float, float>> model;

		int vertices = 20;
		for (int i = 0; i < vertices; i++)
		{
			float radius = ((float)rand() / (float)RAND_MAX) * 0.4f + 0.8f;
			float angle = ((float)i / (float)vertices) * 6.28318f;

			model.push_back({ sinf(angle) * radius, cosf(angle) * radius });
		}

		return model;
	}

	void DrawModel(std::vector<std::pair<float, float>> &model, float x, float y, float angle, float scale = 1.0f, short character = DEFAULT_CHAR, short color = DEFAULT_COLOR)
	{
		int vertices = model.size();
		std::vector<std::pair<float, float>> transformed(vertices);
		
		for (int i = 0; i < vertices; i++)
		{
			transformed[i].first = cosf(angle) * model[i].first - sinf(angle) * model[i].second;
			transformed[i].second = sinf(angle) * model[i].first + cosf(angle) * model[i].second;
		}

		for (int i = 0; i < vertices; i++)
		{
			transformed[i].first *= scale;
			transformed[i].second *= scale;
		}

		for (int i = 0; i < vertices; i++)
		{
			transformed[i].first += x;
			transformed[i].second += y;
		}

		for (int i = 0; i < vertices; i++)
		{
			int j = i + 1;
			DrawLine((int)transformed[i].first, (int)transformed[i].second,
				(int)transformed[j % vertices].first, (int)transformed[j % vertices].second, character, color);
		}
	}

	void DrawPoint(int x, int y, short character = DEFAULT_CHAR, short color = DEFAULT_COLOR) override
	{
		float wx, wy;
		WrapCoordinates((float)x, (float)y, wx, wy);
		ConsoleGameEngine::DrawPoint((int)wx, (int)wy, character, color);
	}
};

int main()
{
	Asteroids game;
	if (game.ConstructScreen(160, 100, 8, 8))
		game.Start();

	return 0;
}