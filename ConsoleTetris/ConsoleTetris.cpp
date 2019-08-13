#include <Windows.h>
#include <string>
#include <thread>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 40;

int nBoardWidth = 20;
int nBoardHeight = 16;

int nCurrentTetromino;
wstring sCurrentTetromino;

int nCurrentX = nBoardWidth/2;
int nCurrentY = 0;

bool bRotating = true;

int nFallingSpeed = 20;
int nSpeedCount = 0;

int nPieceCount = 0;

bool bGameOver = false;

wstring board;

wstring Rotated(wstring piece)
{
	for (int x = 0; x < 2; x++)
	{
		for (int y = x; y < 4 - x - 1; y++)
		{
			wchar_t temp = piece.c_str()[4 * y + x];
			piece[4 * y + x] = piece[4 * (4 - x - 1) + y];
			piece[4 * (4 - x - 1) + y] = piece[4 * (4 - y - 1) + (4 - x - 1)];
			piece[4 * (4 - y - 1) + (4 - x - 1)] = piece[4 * x + (4 - y - 1)];
			piece[4 * x + (4 - y - 1)] = temp;
		}
	}

	//Check if upper-most row is empty
	bool bFirstRowIsEmpty = true;
	for (int i = 0; i < 4; i++)
	{
		if (piece.c_str()[i] != '.')
		{
			bFirstRowIsEmpty = false;
		}
	}
	//If it is, push it to the bottom
	if (bFirstRowIsEmpty)
	{
		piece.erase(0, 4);
		piece += L"....";
	}

	//Check if the left-most column is empty
	vector<int> emptyColoumnIndices;
	for (int j = 0; j < 4; j++)
	{
		if (piece.c_str()[4 * j + 0] == '.')
		{
			emptyColoumnIndices.push_back(4 * j + 0);
		}
	}
	//If it is, push it to the right
	if (emptyColoumnIndices.size() == 4)
	{
		for (int j = 0; j < 4; j++)
		{
			piece.erase(emptyColoumnIndices[j], 1);
			piece.insert(emptyColoumnIndices[j] + 3, 1, '.');
		}
	}
	return piece;
}

bool PieceFits(const wstring &piece, int x, int y)
{
	for (int px = 0; px < 4; px++)
	{
		for (int py = 0; py < 4; py++)
		{
			if (piece.c_str()[4 * py + px] != '.')
			{
				if (x + px >= 0 && x + px < nBoardWidth && y + py >= 0 && y + py < nBoardHeight)
				{
					if (board.c_str()[nBoardWidth * (y + py) + (x + px)] != '.')
						return false;
				}
			}
		}
	}
	return true;
}

int main()
{
	wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"#..................#";
	board += L"####################";

	wstring tetromino[7];
	tetromino[0] += L"A...";
	tetromino[0] += L"A...";
	tetromino[0] += L"A...";
	tetromino[0] += L"A...";

	tetromino[1] += L"B...";
	tetromino[1] += L"BB..";
	tetromino[1] += L".B..";
	tetromino[1] += L"....";

	tetromino[2] += L"CC..";
	tetromino[2] += L"CC..";
	tetromino[2] += L"....";
	tetromino[2] += L"....";

	tetromino[3] += L".D..";
	tetromino[3] += L".D..";
	tetromino[3] += L"DD..";
	tetromino[3] += L"....";

	tetromino[4] += L"E...";
	tetromino[4] += L"E...";
	tetromino[4] += L"EE..";
	tetromino[4] += L"....";

	tetromino[5] += L".F..";
	tetromino[5] += L"FF..";
	tetromino[5] += L"F...";
	tetromino[5] += L"....";

	tetromino[6] += L"GGG.";
	tetromino[6] += L".G..";
	tetromino[6] += L"....";
	tetromino[6] += L"....";
	
	nCurrentTetromino = rand() % 7;
	sCurrentTetromino = tetromino[nCurrentTetromino];

	while (!bGameOver)
	{
		//Game tick
		this_thread::sleep_for(50ms);

		//Handle user's input
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			if (bRotating)
			{
				wstring rotated = Rotated(sCurrentTetromino);
				if (PieceFits(rotated, nCurrentX, nCurrentY))
				{
					sCurrentTetromino = rotated;
				}
				bRotating = false;
			}
		}
		else
			bRotating = true;

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			if (PieceFits(sCurrentTetromino, nCurrentX, nCurrentY + 1))
				nCurrentY++;
		}

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
		{
			if (PieceFits(sCurrentTetromino, nCurrentX - 1, nCurrentY))
				nCurrentX--;
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
		{
			if (PieceFits(sCurrentTetromino, nCurrentX + 1, nCurrentY))
				nCurrentX++;
		}

		//Move piece down
		nSpeedCount++;
		if (nFallingSpeed == nSpeedCount)
		{
			nSpeedCount = 0;
			//Freeze the piece if it has collided with anything
			if (PieceFits(sCurrentTetromino, nCurrentX, nCurrentY + 1))
			{
				nCurrentY++;
			}
			else
			{
				for (int xOffset = 0; xOffset < 4; xOffset++)
				{
					for (int yOffset = 0; yOffset < 4; yOffset++)
					{
						wchar_t piece = sCurrentTetromino.c_str()[4 * yOffset + xOffset];
						if (piece != '.')
						{
							board[nBoardWidth * (nCurrentY + yOffset) + (nCurrentX + xOffset)] = piece;
						}
					}
				}

				//Generate new piece
				nCurrentX = nBoardWidth / 2;
				nCurrentY = 0;
				nCurrentTetromino = rand() % 7;
				sCurrentTetromino = tetromino[nCurrentTetromino];

				nPieceCount++;
				if (nPieceCount % 10 == 0 && nFallingSpeed > 5)
					nFallingSpeed--;

				//If it doesn't fit straight away, the game is over
				bGameOver = !PieceFits(sCurrentTetromino, nCurrentX, nCurrentY);
			}
		}

		//Check for full rows and clear them
		bool bCleared = false;
		for (int y = 0; y < nBoardHeight - 1; y++)
		{
			bool bFullRow = true;
			for (int x = 1; x < nBoardWidth - 1; x++)
			{
				if (board.c_str()[nBoardWidth * y + x] == '.') bFullRow = false;
			}

			if (bFullRow)
			{
				for (int x = 1; x < nBoardWidth - 1; x++)
				{
					board[nBoardWidth * y + x] = '=';
				}
				bCleared = true;
			}
		}

		//Draw the board
		for (int x = 0; x < nBoardWidth; x++)
		{
			for (int y = 0; y < nBoardHeight; y++)
			{
				wchar_t piece = board.c_str()[nBoardWidth * y + x];
				if (piece != '.')
					screen[nScreenWidth * (y + 1) + x] = piece;
				else
					screen[nScreenWidth * (y + 1) + x] = ' ';
			}
		}

		//Replace cleared rows with empty rows
		if (bCleared)
		{
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(300ms);

			for (int y = 0; y < nBoardHeight - 1; y++)
			{
				if (board.c_str()[nBoardWidth * y + 1] == '=')
				{
					board.erase(nBoardWidth * y, nBoardWidth);
					board.insert(0, L"#..................#");
				}
			}
		}

		//Draw the current piece
		for (int xOffset = 0; xOffset < 4; xOffset++)
		{
			for (int yOffset = 0; yOffset < 4; yOffset++)
			{
				wchar_t piece = sCurrentTetromino.c_str()[4 * yOffset + xOffset];
				if (piece != '.')
				{
					screen[nScreenWidth * (nCurrentY + yOffset + 1) + (nCurrentX + xOffset)] = piece;
				}
			}
		}

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	CloseHandle(hConsole);

	cout << "Game over!\n";
	system("pause");


	return 0;
}