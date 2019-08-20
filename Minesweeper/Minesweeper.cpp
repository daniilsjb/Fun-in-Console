#include "ConsoleGameEngine.h"

#include <map>

//Change these values to get different configurations of Minesweeper
//Keep in mind that Windows console may get very, very slow if the field is too large. Those sizes work fine for me.
const int FIELD_WIDTH = 20;
const int FIELD_HEIGHT = 12;

//Cannot exceed the total number of cells in the field
//Note: mines get generated after user makes first move to make sure the first clicked cell isn't a mine.
//If you set this value too high relative to the field's overall size, it may take very long to generate mines correctly.
const int NUMBER_OF_MINES = 50;

//Size of each cell (aka width/height of sprites)
const int CELL_WIDTH = 8;
const int CELL_HEIGHT = 8;

class Minesweeper : public ConsoleGameEngine
{
	//Each cell can only be in one of those states at a time
	enum class CellState { REVEALED, MARKED, HIDDEN };

	struct Cell
	{
		CellState state = CellState::HIDDEN;

		bool isMine = false;
		bool mineClicked = false;	//Flag for the mine that was clicked by user in case they lost the game

		bool hovered = false;

		int neighbours = 0;

		int x, y;	//Position in screen space
	};

	Cell *minefield;

	std::map<std::string, Sprite*> sprites;

	bool gameStarted = false;
	bool gameOver = false;
	float gameOverTimer = 0.0f;

	int hovered_i = -1, hovered_j = -1;

	bool OnStart() override
	{
		SetApplicationTitle(L"Minesweeper");

		//Validate selected configuration
		if (NUMBER_OF_MINES > FIELD_WIDTH * FIELD_HEIGHT)
			return false;

		LoadSprites();

		minefield = new Cell[FIELD_WIDTH * FIELD_HEIGHT];

		srand(time(0));

		//Accumulate X and Y offsets to form outline between cells
		int xOffset = 1;
		int yOffset = 1;

		//Draw the grid
		for (int i = 0; i < FIELD_WIDTH; i++)
		{
			for (int j = 0; j < FIELD_HEIGHT; j++)
			{
				Cell &current = minefield[FIELD_WIDTH * j + i];

				current.x = i * CELL_WIDTH  + xOffset;
				current.y = j * CELL_HEIGHT + yOffset;

				DrawSprite(current.x, current.y, *sprites["hidden"]);

				yOffset++;
			}
			yOffset = 1;
			xOffset++;
		}

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		//The game is closed only after it's finished rendering its last frame, so the check is done at the start
		if (gameOver)
		{
			//Delay to let player see the minefield in its glory
			gameOverTimer += elapsedTime;
			if (gameOverTimer >= 5.0f)
				return false;
			else
				return true;
		}

		//Queue of updated cells to redraw
		std::queue<Cell> updated;

		if (GetKey(VK_LBUTTON).pressed)
		{
			int i, j;
			if (MouseWithinCell(i, j))
			{
				if (!gameStarted)
				{
					GenerateField(i, j);
					gameStarted = true;
				}

				Cell &clicked = minefield[FIELD_WIDTH * j + i];

				//A cell can only be revealed once
				if (clicked.state != CellState::REVEALED)
				{
					clicked.state = CellState::REVEALED;
					updated.push(clicked);

					if (clicked.isMine)
					{
						gameOver = true;
						clicked.mineClicked = true;
					}
					else if (clicked.neighbours == 0)
					{
						TriggerEmpty(clicked, i, j, updated);
					}
				}
			}
		}
		
		if (GetKey(VK_RBUTTON).pressed)
		{
			int i, j;
			if (MouseWithinCell(i, j))
			{
				Cell &clicked = minefield[FIELD_WIDTH * j + i];

				//Cannot change cell's state if it's already revealed
				if (clicked.state != CellState::REVEALED)
				{
					if (clicked.state == CellState::HIDDEN)
						clicked.state = CellState::MARKED;
					else
						clicked.state = CellState::HIDDEN;

					updated.push(clicked);
				}
			}
		}

		//Check if cursor is hovering over an empty cell
		int i, j;
		if (MouseWithinCell(i, j))
		{
			//If anything was hovered over before, it's not hovered over anymore
			if (hovered_i != -1 && hovered_j != -1)
			{
				minefield[FIELD_WIDTH * hovered_j + hovered_i].hovered = false;
				updated.push(minefield[FIELD_WIDTH * hovered_j + hovered_i]);
			}

			//Update currently hovered cell
			minefield[FIELD_WIDTH * j + i].hovered = true;
			updated.push(minefield[FIELD_WIDTH * j + i]);

			hovered_i = i;
			hovered_j = j;
		}
		else
		{
			//Since cursor is not within any of the cells, make sure no cells are considered hovered
			if (hovered_i != -1 && hovered_j != -1)
			{
				minefield[FIELD_WIDTH * hovered_j + hovered_i].hovered = false;
				updated.push(minefield[FIELD_WIDTH * hovered_j + hovered_i]);

				hovered_i = hovered_j = -1;
			}
		}

		//If game is over, reveal each cell
		if (gameOver)
		{
			for (int i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; i++)
			{
				minefield[i].state = CellState::REVEALED;
				minefield[i].hovered = false;
				updated.push(minefield[i]);
			}
		}

		//Draw every updated cell based on its current state
		while (!updated.empty())
		{
			Cell cell = updated.front();
			updated.pop();

			switch (cell.state)
			{
				case CellState::REVEALED:
				{
					if (cell.isMine)
					{
						if (cell.mineClicked)
							DrawSprite(cell.x, cell.y, *sprites["mine_clicked"]);
						else
							DrawSprite(cell.x, cell.y, *sprites["mine"]);
					}
					else
					{
						if (cell.neighbours > 0)
							DrawSprite(cell.x, cell.y, *sprites[std::to_string(cell.neighbours)]);
						else
							DrawSprite(cell.x, cell.y, *sprites["empty"]);
					}
					break;
				}

				case CellState::MARKED:
				{
					DrawSprite(cell.x, cell.y, *sprites["marked"]);
					break;
				}

				case CellState::HIDDEN:
				{
					if (cell.hovered)
						DrawSprite(cell.x, cell.y, *sprites["hovered"]);
					else
						DrawSprite(cell.x, cell.y, *sprites["hidden"]);
					break;
				}

				default: DrawSprite(cell.x, cell.y, *sprites["hidden"]); break;
			}
		}

		return true;
	}

	bool OnDestroy()
	{
		for (auto &s : sprites)
		{
			delete s.second;
		}

		return true;
	}

	void LoadSprites()
	{
		auto load = [&](std::string name, std::wstring fileName)
		{
			Sprite *spr = new Sprite(fileName);
			sprites[name] = spr;
		};

		load("empty", L"Sprites/cell_empty.spr");
		load("hidden", L"Sprites/cell_hidden.spr");
		load("mine", L"Sprites/cell_mine.spr");
		load("mine_clicked", L"Sprites/cell_clicked_mine.spr");
		load("marked", L"Sprites/cell_marked.spr");
		load("hovered", L"Sprites/cell_hovered.spr");
		load("1", L"Sprites/cell_1.spr");
		load("2", L"Sprites/cell_2.spr");
		load("3", L"Sprites/cell_3.spr");
		load("4", L"Sprites/cell_4.spr");
		load("5", L"Sprites/cell_5.spr");
		load("6", L"Sprites/cell_6.spr");
		load("7", L"Sprites/cell_7.spr");
		load("8", L"Sprites/cell_8.spr");
	}

	void GenerateField(int initial_i, int initial_j)
	{
		//Populate the field with mines
		for (int i = 0; i < NUMBER_OF_MINES; i++)
		{
			int mine_i, mine_j;
			bool validMine = false;
			while (!validMine)
			{
				//Find a cell without a mine that isn't user's initial move
				mine_i = rand() % FIELD_WIDTH;
				mine_j = rand() % FIELD_HEIGHT;
				if (!minefield[FIELD_WIDTH * mine_j + mine_i].isMine && mine_i != initial_i && mine_j != initial_j)
				{
					validMine = true;
				}
			}
			minefield[FIELD_WIDTH * mine_j + mine_i].isMine = true;
		}

		//Count each cell's neighbours
		for (int i = 0; i < FIELD_WIDTH; i++)
		{
			for (int j = 0; j < FIELD_HEIGHT; j++)
			{
				Cell &current = minefield[FIELD_WIDTH * j + i];
				//We're not interested in cell's neighbours if it's a mine
				if (!current.isMine)
				{
					//Above
					if (IsMine(i - 1, j - 1)) current.neighbours++;
					if (IsMine(i, j - 1)) current.neighbours++;
					if (IsMine(i + 1, j - 1)) current.neighbours++;

					//Left and Right
					if (IsMine(i - 1, j)) current.neighbours++;
					if (IsMine(i + 1, j)) current.neighbours++;

					//Below
					if (IsMine(i - 1, j + 1)) current.neighbours++;
					if (IsMine(i, j + 1)) current.neighbours++;
					if (IsMine(i + 1, j + 1)) current.neighbours++;
				}
			}
		}
	}

	bool IsMine(int i, int j)
	{
		if (i < 0 || i >= FIELD_WIDTH || j < 0 || j >= FIELD_HEIGHT)
			return false;

		return minefield[FIELD_WIDTH * j + i].isMine;
	}

	bool MouseWithinCell(int &mouse_i, int &mouse_j)
	{
		for (int i = 0; i < FIELD_WIDTH; i++)
		{
			for (int j = 0; j < FIELD_HEIGHT; j++)
			{
				Cell &current = minefield[FIELD_WIDTH * j + i];

				if (GetMouseX() >= current.x && GetMouseX() < current.x + CELL_WIDTH &&
					GetMouseY() >= current.y && GetMouseY() < current.y + CELL_HEIGHT)
				{
					mouse_i = i;
					mouse_j = j;
					return true;
				}
			}
		}
		return false;
	}

	void TriggerEmpty(Cell &original, int i, int j, std::queue<Cell> &revealed)
	{
		struct EmptyCell
		{
			Cell cell;
			int i, j;
		};

		std::queue<EmptyCell> emptyCells;
		emptyCells.push({ original, i, j });

		while (!emptyCells.empty())
		{
			EmptyCell current = emptyCells.front();
			emptyCells.pop();

			for (int iOffset = -1; iOffset <= 1; iOffset++)
			{
				for (int jOffset = -1; jOffset <= 1; jOffset++)
				{
					int neighbour_i = (current.i + iOffset);
					int	neighbour_j = (current.j + jOffset);

					if (neighbour_i < 0 || neighbour_i >= FIELD_WIDTH || neighbour_j < 0 || neighbour_j >= FIELD_HEIGHT)
						continue;

					Cell &neighbour = minefield[FIELD_WIDTH * neighbour_j + neighbour_i];

					if (neighbour.state == CellState::REVEALED)
						continue;

					if (!neighbour.isMine)
					{
						if (neighbour.neighbours == 0)
						{
							emptyCells.push({ neighbour, neighbour_i, neighbour_j });
						}
						neighbour.state = CellState::REVEALED;
						revealed.push(neighbour);
					}
				}
			}
		}
	}

};

int main()
{
	//Screen dimensions are calculated based on provided configuration. In addition to the actual space required to fit each cell, it also
	//includes space inbetween cells to form the outline
	int screenWidth = (FIELD_WIDTH * CELL_WIDTH) + (FIELD_WIDTH + 1);
	int screenHeight = (FIELD_HEIGHT * CELL_HEIGHT) + (FIELD_HEIGHT + 1);

	Minesweeper demo;
	if (demo.ConstructScreen(screenWidth, screenHeight, 4, 4))
		demo.Start();
}