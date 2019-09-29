#include "ConsoleGameEngine.h"

class SpriteEditor : public ConsoleGameEngine
{
	Sprite sprite;
	int spriteWidth, spriteHeight;
	int spriteHalfX, spriteHalfY;
	std::wstring sprToSave, sprToLoad;

	int canvasWidth, canvasHeight;
	int canvasCenterX, canvasCenterY;
	int canvasStart;

	short brushColor = BG_WHITE;
	short brushCharacter = ' ';

	const int SPRITE_MIN = 1;
	const int SPRITE_MAX = 64;

	const int COLOR_SIZE = 5;
	const int HUES_COUNT = 6;

	bool OnStart() override
	{
		SetApplicationTitle(L"Sprite Editor");

		//Set names of the files to be generated and loaded
		sprToSave = L"new.spr";
		sprToLoad = L"new.spr";

		//Set dimensions of the sprite to be drawn
		spriteWidth = 16;
		spriteHeight = 16;

		//Canvas starts where the palette ends
		canvasStart = COLOR_SIZE * HUES_COUNT + HUES_COUNT;

		canvasCenterX = ((GetScreenWidth() - canvasStart) / 2) + canvasStart;
		canvasCenterY = GetScreenHeight() / 2;

		canvasWidth = GetScreenWidth() - canvasStart;
		canvasHeight = GetScreenHeight();

		//Clamp sprite dimensions to make sure they are valid
		if (spriteWidth < SPRITE_MIN) spriteWidth = SPRITE_MIN;
		if (spriteHeight < SPRITE_MIN) spriteHeight = SPRITE_MIN;

		if (spriteWidth > SPRITE_MAX) spriteWidth = SPRITE_MAX;
		if (spriteHeight > SPRITE_MAX) spriteHeight = SPRITE_MAX;

		spriteHalfX = (int)((float)spriteWidth * 0.5f + 0.5f);
		spriteHalfY = (int)((float)spriteHeight * 0.5f + 0.5f);

		sprite.Create(spriteWidth, spriteHeight);

		//We need to remove one pixel from our color dimensions because width and height are added after the first pixel
		//when rendering the palette
		int colorHeight = COLOR_SIZE - 1;
		int colorWidth = COLOR_SIZE - 1;

		//Draw the palette
		//The palette consists of 16 colors, and the editor supports a number of hues for each color
		//All of this could be done in a nested 'for' loop, however it's much easier to select appropriate shading this way
		for (int i = 0; i < 16; i++)
		{
			//Colors are offset to create an outline between them
			DrawFilledRectangle(0 * colorWidth + 0, i * colorHeight + (i * 2), 1 * colorWidth + 0, (i + 1) * colorHeight + (i * 2), PIXEL_QUARTER, i);
			DrawFilledRectangle(1 * colorWidth + 2, i * colorHeight + (i * 2), 2 * colorWidth + 2, (i + 1) * colorHeight + (i * 2), PIXEL_HALF, i);
			DrawFilledRectangle(2 * colorWidth + 4, i * colorHeight + (i * 2), 3 * colorWidth + 4, (i + 1) * colorHeight + (i * 2), PIXEL_THREEQUARTERS, i);
			DrawFilledRectangle(3 * colorWidth + 6, i * colorHeight + (i * 2), 4 * colorWidth + 6, (i + 1) * colorHeight + (i * 2), PIXEL_SOLID, i);
			DrawFilledRectangle(4 * colorWidth + 8, i * colorHeight + (i * 2), 5 * colorWidth + 8, (i + 1) * colorHeight + (i * 2), ' ', i * 16);
			DrawFilledRectangle(5 * colorWidth + 10, i * colorHeight + (i * 2), 6 * colorWidth + 10, (i + 1) * colorHeight + (i * 2), PIXEL_THREEQUARTERS, i | BG_WHITE);
		}

		return true;
	}

	bool OnUpdate(float elapsedTime) override
	{
		if (GetKey(VK_LBUTTON).held)
		{
			//If the mouse is within sprite boundaries, draw into it
			if (GetMouseX() >= canvasCenterX - spriteHalfX && GetMouseX() < canvasCenterX + spriteHalfX &&
				GetMouseY() >= canvasCenterY - spriteHalfY && GetMouseY() < canvasCenterY + spriteHalfY)
			{
				int sprX = GetMouseX() - canvasCenterX + spriteHalfX;
				int sprY = GetMouseY() - canvasCenterY + spriteHalfY;
				sprite.SetColor(sprX, sprY, brushColor);
				sprite.SetCharacter(sprX, sprY, brushCharacter);
			}
		}

		if (GetKey(VK_LBUTTON).pressed || GetKey(VK_LBUTTON).released)
		{
			//If the mouse is within palette boundaries, change brush color
			if (GetMouseX() < canvasStart - 1)
			{
				brushColor = GetScreenColor(GetMouseX(), GetMouseY());
				brushCharacter = GetScreenCharacter(GetMouseX(), GetMouseY());
			}
		}

		if (GetKey(VK_RIGHT).pressed)
			MoveSpriteRight();

		if (GetKey(VK_LEFT).pressed)
			MoveSpriteLeft();

		if (GetKey(VK_UP).pressed)
			MoveSpriteUp();

		if (GetKey(VK_DOWN).pressed)
			MoveSpriteDown();

		if (GetKey('M').pressed)
			MirrorSprite();

		if (GetKey(VK_LCONTROL).held && GetKey('S').pressed)
			sprite.Save(sprToSave);

		if (GetKey(VK_LCONTROL).held && GetKey('L').pressed)
			LoadSprite();

		//Clear the canvas by filling each row with empty space
		for (int i = 0; i < GetScreenHeight(); i++)
		{
			Fill(canvasStart, i, GetScreenWidth() - canvasStart, ' ', BG_BLACK);
		}

		//Check whether bottom/right boundaries should be offset if sprite dimensions are odd
		int oddOffsetX = (int)(spriteWidth % 2 != 0);
		int oddOffsetY = (int)(spriteHeight % 2 != 0);

		//Find the sprite's boundaries
		int leftX = canvasCenterX - spriteHalfX - 1;
		int topY = canvasCenterY - spriteHalfY - 1;
		int rightX = canvasCenterX + spriteHalfX - oddOffsetX;
		int bottomY = canvasCenterY + spriteHalfY - oddOffsetY;

		//Draw boundaries if they fit into the canvas
		if (spriteWidth < canvasWidth - 1)
			DrawLine(leftX, topY, leftX, bottomY, ' ', BG_WHITE);

		if (spriteWidth < canvasWidth)
			DrawLine(rightX, topY, rightX, bottomY, ' ', BG_WHITE);

		if (spriteHeight < canvasHeight - 1)
			DrawLine(leftX, topY, rightX, topY, ' ', BG_WHITE);

		if (spriteHeight < canvasHeight)
			DrawLine(leftX, bottomY, rightX, bottomY, ' ', BG_WHITE);

		//Draw the sprite onto the canvas
		DrawSprite(canvasCenterX - spriteHalfX, canvasCenterY - spriteHalfY, sprite);

		return true;
	}

	void MoveSpriteRight()
	{
		for (int j = 0; j < spriteHeight; j++)
		{
			for (int i = spriteWidth - 1; i > 0; i--)
			{
				sprite.SetCharacter(i, j, sprite.GetCharacter(i - 1, j));
				sprite.SetColor(i, j, sprite.GetColor(i - 1, j));
			}

			sprite.SetCharacter(0, j, BG_BLACK);
			sprite.SetColor(0, j, BG_BLACK);
		}
	}

	void MoveSpriteLeft()
	{
		for (int j = 0; j < spriteHeight; j++)
		{
			for (int i = 0; i < spriteWidth - 1; i++)
			{
				sprite.SetCharacter(i, j, sprite.GetCharacter(i + 1, j));
				sprite.SetColor(i, j, sprite.GetColor(i + 1, j));
			}

			sprite.SetCharacter(spriteWidth - 1, j, BG_BLACK);
			sprite.SetColor(spriteWidth - 1, j, BG_BLACK);
		}
	}

	void MoveSpriteUp()
	{
		for (int i = 0; i < spriteWidth; i++)
		{
			for (int j = 0; j < spriteHeight - 1; j++)
			{
				sprite.SetCharacter(i, j, sprite.GetCharacter(i, j + 1));
				sprite.SetColor(i, j, sprite.GetColor(i, j + 1));
			}
			sprite.SetCharacter(i, spriteHeight - 1, BG_BLACK);
			sprite.SetColor(i, spriteHeight - 1, BG_BLACK);
		}
	}

	void MoveSpriteDown()
	{
		for (int i = 0; i < spriteWidth; i++)
		{
			for (int j = spriteHeight - 1; j > 0; j--)
			{
				sprite.SetCharacter(i, j, sprite.GetCharacter(i, j - 1));
				sprite.SetColor(i, j, sprite.GetColor(i, j - 1));
			}
			sprite.SetCharacter(i, 0, BG_BLACK);
			sprite.SetColor(i, 0, BG_BLACK);
		}
	}

	void MirrorSprite()
	{
		Sprite mirrored;
		mirrored.Create(spriteWidth, spriteHeight);

		for (int i = 0; i < spriteWidth; i++)
		{
			for (int j = 0; j < spriteHeight; j++)
			{
				mirrored.SetCharacter((spriteWidth - 1) - i, j, sprite.GetCharacter(i, j));
				mirrored.SetColor((spriteWidth - 1) - i, j, sprite.GetColor(i, j));
			}
		}

		sprite.Copy(mirrored);
	}

	void LoadSprite()
	{
		//Load sprite from file
		Sprite result(sprToLoad);

		//Carry on only if loaded sprite can fit into the canvas
		if (result.GetWidth() > 0 && result.GetWidth() < canvasWidth && result.GetHeight() > 0 && result.GetHeight() < canvasHeight)
		{
			//Update sprite information
			spriteWidth = result.GetWidth();
			spriteHeight = result.GetHeight();

			spriteHalfX = (int)((float)spriteWidth / 2.0f + 0.5f);
			spriteHalfY = (int)((float)spriteHeight / 2.0f + 0.5f);

			sprite.Copy(result);
		}
	}

};

int main()
{
	SpriteEditor editor;
	if (editor.ConstructScreen(156, 96, 8, 8))
		editor.Start();

	return 0;
}