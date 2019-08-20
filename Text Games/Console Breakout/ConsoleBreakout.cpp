#include <Windows.h>
#include <chrono>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

const float PI = 3.141592f;

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = nScreenWidth / 2.f;
const int nPlayerY = (int)(nScreenHeight / 4.f + nScreenHeight / 2.f);
float fPlayerSpeed = 45.f;
const int PLAYER_LENGTH = 8;

int nPlayerLives = 3;

bool bGameOver = false;
bool bVictory;

float fBallX = nScreenWidth / 2.f;
float fBallY = nScreenHeight / 2.f;
float fBallA = PI / 2;
float fBallSpeed = 35.f;

const float EXTREME_ANGLE = PI / 4.f;

const int PADDLE_ROWS = 10;
const int PADDLE_COLS = 20;

vector<pair<int, int>> paddles;

int main()
{
	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
	for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	auto t1 = chrono::system_clock::now();
	auto t2 = chrono::system_clock::now();

	int offset = (int)(nScreenWidth / 12.f);
	int paddleLength = (nScreenWidth - offset * 2) / PADDLE_COLS;

	for (int row = 0; row < PADDLE_ROWS; row++)
	{
		for (int col = 0; col < PADDLE_COLS; col++)
		{
			paddles.push_back(make_pair(col * paddleLength + offset, row + offset / 2));
		}
	}

	while (!bGameOver)
	{
		t2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = t2 - t1;
		float fElapsedTime = elapsedTime.count();
		t1 = t2;

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			if (fPlayerX > 0)
				fPlayerX -= fPlayerSpeed * fElapsedTime;
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			if (fPlayerX < nScreenWidth - PLAYER_LENGTH)
				fPlayerX += fPlayerSpeed * fElapsedTime;
		}

		float dx = cosf(fBallA) * fBallSpeed * fElapsedTime;
		float dy = sinf(fBallA) * fBallSpeed * fElapsedTime;
		fBallX += dx;
		fBallY += dy;

		if (fBallY >= nScreenHeight)
		{
			nPlayerLives--;
			fBallX = nScreenWidth / 2.f;
			fBallY = nScreenHeight / 2.f;
			fBallA = PI / 5.5f * -1;
			fPlayerX = nScreenWidth / 2.f;
		}
		if (fBallX < 0 || fBallX >= nScreenWidth)
		{
			fBallA *= -1;
			fBallA += PI;
			fBallX -= dx;
		}
		if (fBallY < 0)
		{
			fBallA *= -1;
			fBallY -= dy;
		}

		if (screen[nScreenWidth * (int)fBallY + (int)fBallX] == '#')
		{
			float dx = fBallX - (fPlayerX + PLAYER_LENGTH / 2);
			float dxNorm = dx / (PLAYER_LENGTH / 2);
			fBallA = (EXTREME_ANGLE * dxNorm) - (PI / 2);
		}


		if (screen[nScreenWidth * (int)fBallY + (int)fBallX] == '=')
		{
			for (size_t i = 0; i < paddles.size(); i++)
			{	
				int nPaddleX = paddles[i].first;
				int nPaddleY = paddles[i].second;
				if ((round(fBallY) == nPaddleY) && (int)fBallX >= nPaddleX && (int)fBallX <= nPaddleX + paddleLength)
				{
					float tempDy = (float)nPaddleY - fBallY;
					int dy;
					if (tempDy < 0)
						dy = 1;
					else
						dy = -1;
					float dx = fBallX - (nPaddleX + paddleLength / 2);
					float dxNorm = dx / (paddleLength / 2);
					fBallA = (-(PI / 2) + (EXTREME_ANGLE * dxNorm)) * dy;
					paddles.erase(paddles.begin() + i);
					break;
				}
			}
		}

		if (nPlayerLives <= 0)
		{
			bGameOver = true;
			bVictory = false;
		}

		if (paddles.empty())
		{
			bGameOver = true;
			bVictory = true;
		}

		for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = L' ';

		for (int i = 0; i < PLAYER_LENGTH; i++)
		{
			screen[nScreenWidth * nPlayerY + ((int)fPlayerX + i)] = '#';
		}

		for (auto &paddle : paddles)
		{
			for (int i = 0; i < paddleLength; i++)
			{
				screen[nScreenWidth * paddle.second + (paddle.first + i)] = '=';
			}
		}

		screen[nScreenWidth * (int)fBallY + (int)fBallX] = 'O';

		swprintf_s(&screen[nScreenWidth * (int)(offset / 4) + (int)(offset / 4)], 9, L"LIVES: %1d", nPlayerLives);

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
	}

	CloseHandle(hConsole);

	if (bVictory)
		cout << "\nCongratulations on your outstanding victory!\n";
	else
		cout << "\nMaybe next time.\n";

	return 0;
}