#include <Windows.h>
#include <string>
#include <chrono>

#define DEG_TO_RAD(deg) ((deg) * 3.14159f/180)

using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 40;

float fBallX = nScreenWidth / 2.f;
float fBallY = nScreenHeight / 2.f;

float fBallA;

float fBallSpeed = 30.f;

const int nPaddleAX = 10;
float fPaddleAY = nScreenHeight / 2.f;

int nScoreA = 0;

const int nPaddleBX = 110;
float fPaddleBY = nScreenHeight / 2.f;

int nScoreB = 0;

float fPaddleSpeed = 20.f;

int main()
{
	wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	auto t1 = chrono::system_clock::now();
	auto t2 = chrono::system_clock::now();

	fBallA = rand() % 360;

	while (true)
	{
		t2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = t2 - t1;
		float fElapsedTime = elapsedTime.count();
		t1 = t2;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			if (fPaddleAY > 1)
				fPaddleAY -= fPaddleSpeed * fElapsedTime;
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			if (fPaddleAY < nScreenHeight - 4)
				fPaddleAY += fPaddleSpeed * fElapsedTime;
		}

		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			if (fPaddleBY > 1)
				fPaddleBY -= fPaddleSpeed * fElapsedTime;
		}

		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			if (fPaddleBY < nScreenHeight - 4)
				fPaddleBY += fPaddleSpeed * fElapsedTime;
		}

		float dx = cosf(DEG_TO_RAD(fBallA)) * fBallSpeed * fElapsedTime;
		float dy = sinf(DEG_TO_RAD(fBallA)) * fBallSpeed * fElapsedTime;
		fBallX += dx;
		fBallY += dy;

		int nBallX = (int)fBallX;
		int nBallY = (int)fBallY;

		if (nBallX <= 0 || nBallX >= nScreenWidth)
		{
			if (nBallX == 0)
				nScoreB++;
			else
				nScoreA++;

			fBallA = rand() % 360;
			fBallX = nScreenWidth / 2.f;
			fBallY = nScreenHeight / 2.f;
		}
		else if (nBallY <= 0 || nBallY >= nScreenHeight)
		{
			fBallA *= -1;
			fBallY -= dy;
		}
		else if (screen[nScreenWidth * (int)fBallY + (int)fBallX] == '#')
		{
			if (nBallX == nPaddleAX)
			{
				float dy = fBallY - (fPaddleAY + 2);
				float dyNorm = dy / 2;
				float newAngle = 45 * dyNorm;
				fBallA = newAngle;
			}
			else
			{
				float dy = fBallY - (fPaddleBY + 2);
				float dyNorm = dy / 2;
				float newAngle = 45 * (dyNorm * -1) + 180;
				fBallA = newAngle;
			}
		}

		for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';

		swprintf_s(&screen[nScreenWidth * 10 + 30 - 9], 9, L"%8d", nScoreA);

		swprintf_s(&screen[nScreenWidth * 10 + 90 - 9], 9, L"%8d", nScoreB);

		for (int i = 0; i < 4; i++)
		{
			screen[nScreenWidth * (int)(fPaddleAY + i) + nPaddleAX] = '#';
			screen[nScreenWidth * (int)(fPaddleBY + i) + nPaddleBX] = '#';
		}

		for (int y = 0; y < nScreenHeight; y++)
		{
			screen[nScreenWidth * y + (int)(nScreenWidth / 2.f)] = '|';
		}

		screen[nScreenWidth * (int)fBallY + (int)fBallX] = 'O';

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
	}

	return 0;
}