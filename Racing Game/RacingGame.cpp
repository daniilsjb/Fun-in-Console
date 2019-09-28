#include "ConsoleGameEngine.h"

class RacingGame : public ConsoleGameEngine
{
	//Player information
	float elapsedDistance = 0.0f;
	float speed = 0.0f;
	float carCenter = 0.0f;			//Normalized (from -1 to 1)
	float playerCurvature = 0.0f;	//Player's accumulated curvature. You may think of this as car's current direction.

	//Track information
	float currentCurvature = 0.0f;	//Current track section's curvature
	float trackCurvature = 0.0f;	//Track's accumulated curvature
	float totalDistance = 0.0f;		//Total length of the track
	std::vector<std::pair<float, float>> track;		//Each track section consists of curvature and length

	//Lap information
	int currentLap = 0;
	std::vector<float> laps;

	Sprite* carForward = nullptr;
	Sprite* carLeft = nullptr;
	Sprite* carRight = nullptr;

	bool OnStart() override
	{
		SetApplicationTitle(L"Retro Arcade Racing Game");

		//Load sprites
		carForward = new Sprite(L"Sprites/car_forward.spr");
		carLeft = new Sprite(L"Sprites/car_left.spr");
		carRight = new Sprite(L"Sprites/car_right.spr");

		//Create track
		track.push_back(std::make_pair(0.0f, 10.0f));
		track.push_back(std::make_pair(0.0f, 100.0f));
		track.push_back(std::make_pair(1.0f, 100.0f));
		track.push_back(std::make_pair(0.0f, 200.0f));
		track.push_back(std::make_pair(-1.0f, 100.0f));
		track.push_back(std::make_pair(0.0f, 200.0f));
		track.push_back(std::make_pair(1.0f, 200.0f));
		track.push_back(std::make_pair(0.0f, 100.0f));
		track.push_back(std::make_pair(-1.0f, 200.0f));
		track.push_back(std::make_pair(0.0f, 400.0f));

		//Find total distance
		for (auto s : track)
			totalDistance += s.second;

		//We will only store latest 5 laps
		laps = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		//Accumulate speed
		if (GetKey(VK_UP).held)
			speed += 2.0f * elapsedTime;
		else
			speed -= 1.0f * elapsedTime;

		//Let player steer the car. Naturally, it's harder to turn with higher speed
		int direction = 0;
		if (GetKey(VK_LEFT).held)
		{
			playerCurvature -= 0.7f * elapsedTime * (1.0f - speed * 0.5f);
			direction = -1;
		}
		if (GetKey(VK_RIGHT).held)
		{
			playerCurvature += 0.7f * elapsedTime * (1.0f - speed * 0.5f);
			direction = 1;
		}

		//If player's curvature is very different from track curvature, then they are off track and should be slowed down
		if (fabs(playerCurvature - trackCurvature) >= 0.8f)
			speed -= 5.0f * elapsedTime;

		//Clamp speed
		if (speed < 0.0f) speed = 0.0f;
		else if (speed > 1.0f) speed = 1.0f;

		//Update lap time
		laps[currentLap] += elapsedTime;

		//Move the car forward and update lap
		elapsedDistance += 70.0f * speed * elapsedTime;
		if (elapsedDistance >= totalDistance)
		{
			elapsedDistance -= totalDistance;
			currentLap = (currentLap + 1) % laps.size();
			laps[currentLap] = 0.0f;
		}

		//Find current track section based on elapsed distance
		float offset = 0.0f;
		int currentTrack = 0;
		while (currentTrack < (int)track.size() && offset <= elapsedDistance)
		{
			offset += track[currentTrack].second;
			currentTrack++;
		}

		//Linearly interpolate current section's curvature
		float targetCurvature = track[currentTrack - 1].first;
		float curvatureStep = (targetCurvature - currentCurvature) * elapsedTime * speed;
		currentCurvature += curvatureStep;

		//Accumulate track curvature
		trackCurvature += (currentCurvature) * elapsedTime * speed;

		//Draw sky
		for (int x = 0; x < GetScreenWidth(); x++)
		{
			for (int y = 0; y < GetScreenHeight() / 2; y++)
			{
				DrawPoint(x, y, PIXEL_SOLID, y < GetScreenHeight() / 4 ? FG_DARK_BLUE : FG_BLUE);
			}
		}

		//Draw hill, which scrolls depending on track curvature
		for (int x = 0; x < GetScreenWidth(); x++)
		{
			int hillHeight = (int)(fabs(sinf(x * 0.01f + trackCurvature) * 16.0f));
			for (int y = GetScreenHeight() / 2 - hillHeight; y < GetScreenHeight() / 2; y++)
			{
				DrawPoint(x, y, PIXEL_SOLID, FG_DARK_YELLOW);
			}
		}

		//Draw the ground
		for (int y = 0; y < GetScreenHeight() / 2; y++)
		{
			float perspective = (float)y / ((float)GetScreenHeight() * 0.5f);	//Normalized (0 is far, 1 is near)

			//Apply track's current curvature and perspective to the road
			float middle = 0.5f + currentCurvature * powf(1.0f - perspective, 3);
			float roadWidth = 0.1f + perspective * 0.8f;
			float clipWidth = roadWidth * 0.15f;

			//We only care about half of the road because it's symmetrical anyway
			roadWidth *= 0.5f;

			//Find boundaries of grass, clips, and the road
			float leftGrass = (middle - roadWidth - clipWidth) * GetScreenWidth();
			float leftClip = (middle - roadWidth) * GetScreenWidth();
			float rightGrass = (middle + roadWidth + clipWidth) * GetScreenWidth();
			float rightClip = (middle + roadWidth) * GetScreenWidth();

			//Oscilate grass and clip colors, taking into account perspective and elapsed distance
			short grassColor = sinf(20.0f * powf(1.0f - perspective, 2) + elapsedDistance * 0.1f) > 0.0f ? FG_GREEN : FG_DARK_GREEN;
			short clipColor = sinf(80.0f * powf(1.0f - perspective, 2) + elapsedDistance * 0.1f) > 0.0f ? FG_RED : FG_WHITE;
			//Highlight the road if we're at start/finish
			short roadColor = (currentTrack - 1) == 0 ? FG_WHITE : FG_GRAY;

			int row = GetScreenHeight() / 2 + y;

			for (int x = 0; x < GetScreenWidth(); x++)
			{
				if (x >= 0 && x < leftGrass)
					DrawPoint(x, row, PIXEL_SOLID, grassColor);
				else if (x >= leftGrass && x < leftClip)
					DrawPoint(x, row, PIXEL_SOLID, clipColor);
				else if (x >= leftClip && x < rightClip)
					DrawPoint(x, row, PIXEL_SOLID, roadColor);
				else if (x >= rightClip && x < rightGrass)
					DrawPoint(x, row, PIXEL_SOLID, clipColor);
				else if (x >= rightGrass && x < GetScreenWidth())
					DrawPoint(x, row, PIXEL_SOLID, grassColor);
			}
		}

		//Car's position depends on how off-track player is. In the ideal case player will always be in the middle.
		carCenter = playerCurvature - trackCurvature;
		//Find car's leftmost coordinate, taking into account sprite width
		int carStart = GetScreenWidth() / 2 + (int)(carCenter * GetScreenWidth() * 0.5f) - 8;

		//Select sprite based on direction
		Sprite* currentSprite = nullptr;
		switch (direction)
		{
			case -1:
				currentSprite = carLeft;
				break;
			case 0:
				currentSprite = carForward;
				break;
			case 1:
				currentSprite = carRight;
				break;
		}

		DrawSpriteAlpha(carStart, (int)(GetScreenHeight() * 0.8f), *currentSprite, BG_PINK);

		//Convert time from seconds into formatted wide-string
		auto displayTime = [](float time)
		{
			int minutes = (int)(time / 60.0f);
			int seconds = (int)(time - (minutes * 60.0f));
			int milliseconds = (int)((time - (float)seconds) * 1000.0f);
			return std::to_wstring(minutes) + L':' + std::to_wstring(seconds) + L':' + std::to_wstring(milliseconds);
		};

		//Draw each lap's elapsed time
		for (int i = 0; i < (int)laps.size(); i++)
		{
			DisplayText(3, i + 3, displayTime(laps[i]), BG_DARK_BLUE, (i == currentLap) ? FG_GREEN : FG_WHITE);
		}

		return true;
	}
};

int main()
{
	RacingGame demo;
	if (demo.ConstructScreen(160, 100, 8, 8))
		demo.Start();
}