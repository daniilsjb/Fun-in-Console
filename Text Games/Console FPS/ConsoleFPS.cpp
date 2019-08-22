#include <Windows.h>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;

float fPlayerA = 0.0f;

float fSpeed = 5.0f;

float fFoV = 3.14159f / 4.0f;
float fDepth = 16.0f;

wstring map;

int nMapWidth = 16;
int nMapHeight = 16;

int main()
{
	wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	map.append(L"################");
	map.append(L"#..#...........#");
	map.append(L"#..#......##...#");
	map.append(L"#....#.........#");
	map.append(L"#.........######");
	map.append(L"#..##..........#");
	map.append(L"#...#......#...#");
	map.append(L"#..........#...#");
	map.append(L"#..............#");
	map.append(L"###.##.........#");
	map.append(L"#..............#");
	map.append(L"#..............#");
	map.append(L"####....####...#");
	map.append(L"#.......#......#");
	map.append(L"#..#....#......#");
	map.append(L"################");

	auto t1 = chrono::system_clock::now();
	auto t2 = chrono::system_clock::now();

	while (true)
	{
		t2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = t1 - t2;
		float fElapsedTime = elapsedTime.count();
		t1 = t2;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;
			if (map.c_str()[nMapWidth * (int)fPlayerX + (int)fPlayerY] == '#') {
				fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;
				fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;
			}
		}
			
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;
			if (map.c_str()[nMapWidth * (int)fPlayerX + (int)fPlayerY] == '#') {
				fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;
				fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerA += (fSpeed * 0.75f) * fElapsedTime;

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerA -= (fSpeed * 0.75f) * fElapsedTime;

		for (int x = 0; x < nScreenWidth; x++)
		{
			float fRayAngle = (fPlayerA - fFoV / 2.0f) + ((float)x / (float)nScreenWidth) * fFoV;

			float fStep = 0.1f;
			float fDistanceToWall = 0.0f;

			bool bHitWall = false;
			bool bBoundary = false;

			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStep;

				float fRayX = fPlayerX + sinf(fRayAngle) * fDistanceToWall;
				float fRayY = fPlayerY + cosf(fRayAngle) * fDistanceToWall;

				int nRayX = (int)fRayX;
				int nRayY = (int)fRayY;

				if (nRayX < 0 || nRayX >= nMapWidth || nRayY < 0 || nRayY >= nMapHeight)
				{
					bHitWall = true;
					fDistanceToWall = fDepth;
				}
				else
				{
					if (map.c_str()[nMapHeight * nRayX + nRayY] == '#')
					{
						bHitWall = true;

						vector<pair<float, float>> p;

						for (int tx = 0; tx < 2; tx++)
						{
							for (int ty = 0; ty < 2; ty++)
							{
								float vx = (float)nRayX + tx - fPlayerX;
								float vy = (float)nRayY + ty - fPlayerY;

								float distance = sqrt(vx*vx + vy*vy);

								float dot = (sinf(fRayAngle) * vx / distance) + (cosf(fRayAngle) * vy / distance);
								p.push_back(make_pair(distance, dot));
							}
						}

						sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) { return left.first < right.first; });

						float fBound = 0.01f;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
						if (acos(p.at(2).second) < fBound) bBoundary = true;
					}
				}
			}

			int nCeiling = (float)(nScreenHeight / 2.0f) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';
			if (!bBoundary)
			{
				if (fDistanceToWall <= fDepth / 4.0f) nShade = 0x2588;
				else if (fDistanceToWall < fDepth / 3.0f) nShade = 0x2593;
				else if (fDistanceToWall < fDepth / 2.0f) nShade = 0x2592;
				else if (fDistanceToWall < fDepth) nShade = 0x2591;
			}

			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y <= nCeiling)
				{
					screen[nScreenWidth * y + x] = ' ';
				}
				else if (y > nCeiling && y <= nFloor)
				{
					screen[nScreenWidth * y + x] = nShade;
				}
				else
				{
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25f) nShade = '#';
					else if (b < 0.5f) nShade = 'x';
					else if (b < 0.75f) nShade = '.';
					else if (b < 0.9f) nShade = '-';
					else nShade = ' ';
					screen[nScreenWidth * y + x] = nShade;
				}
			}

		}

		for (int i = 0; i < nMapWidth; i++) {
			for (int j = 0; j < nMapHeight; j++) {
				int nScreenIndex = nScreenWidth * i + j;
				int nMapIndex = nMapWidth * i + j;
				screen[nScreenIndex] = map.c_str()[nMapIndex];
				int nPlayerIndex = nMapWidth * (int)fPlayerX + (int)fPlayerY;
				if (nMapIndex == nPlayerIndex)
				{
					screen[nScreenIndex] = 'P';
				}
			}
		}
		
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	CloseHandle(hConsole);

	return 0;

}