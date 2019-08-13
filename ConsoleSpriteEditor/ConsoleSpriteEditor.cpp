#include "ConsoleGameEngine.h"

class SpriteEditor : public ConsoleGameEngine
{
	Sprite sprite;
	int spriteWidth, spriteHeight;
	int spriteHalfX, spriteHalfY;
	std::wstring spriteName;

	int canvasWidth, canvasHeight;
	int canvasCenterX, canvasCenterY;
	int canvasStart;

	short brushColor = BG_WHITE;

	bool OnStart() override
	{
		//Set name of the file to be generated
		spriteName = L"brick_wall.spr";

		//Set dimensions of the sprite to be drawn
		spriteWidth = 16;
		spriteHeight = 16;

		//Canvas dimensions get generated relative to screen dimensions
		canvasStart = GetScreenWidth() / 4;

		canvasCenterX = ((GetScreenWidth() - canvasStart) / 2) + canvasStart;
		canvasCenterY = GetScreenHeight() / 2;

		canvasWidth = GetScreenWidth() - canvasStart;
		canvasHeight = GetScreenHeight();

		//Create sprite to draw into if given dimensions are valid
		if (spriteWidth < 1 || spriteWidth > canvasWidth || spriteHeight < 1 || spriteHeight > canvasHeight)
			return false;

		spriteHalfX = (int)((float)spriteWidth / 2.0f + 0.5f);
		spriteHalfY = (int)((float)spriteHeight / 2.0f + 0.5f);

		sprite.Create(spriteWidth, spriteHeight);

		//Draw the palette
		int colorHeight = GetScreenHeight() / 16;
		short color = 0;
		for (int i = 0; i < 16; i++)
		{
			DrawFilledRectangle(0, i * colorHeight, canvasStart - 2, i * colorHeight + colorHeight, ' ', color);
			color += 16;
		}

		//Draw the line to indicate separation between canvas and palette
		DrawLine(canvasStart - 1, 0, canvasStart - 1, GetScreenHeight() - 1, ' ', BG_GRAY);

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
				sprite.SetColor(GetMouseX() - canvasCenterX + spriteHalfX, GetMouseY() - canvasCenterY + spriteHalfY, brushColor);
			}
			
		}
		if (GetKey(VK_LBUTTON).pressed || GetKey(VK_LBUTTON).released)
		{
			//If the mouse is within palette boundaries, change brush color
			if (GetMouseX() < canvasStart - 1)
				brushColor = GetScreenColor(GetMouseX(), GetMouseY());
		}

		if (GetKey(VK_LCONTROL).held && GetKey('S').pressed)
		{
			sprite.Save(spriteName);
		}

		if (GetKey(VK_LCONTROL).held && GetKey('L').pressed)
		{
			//Load sprite from file
			Sprite result(spriteName);

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
			DrawLine(leftX, topY, leftX, bottomY, ' ', BG_DARK_GRAY);

		if (spriteWidth < canvasWidth)
			DrawLine(rightX, topY, rightX, bottomY, ' ', BG_DARK_GRAY);

		if (spriteHeight < canvasHeight - 1)
			DrawLine(leftX, topY, rightX, topY, ' ', BG_DARK_GRAY);

		if (spriteHeight < canvasHeight)
			DrawLine(leftX, bottomY, rightX, bottomY, ' ', BG_DARK_GRAY);

		//Draw the sprite onto the canvas
		DrawSprite(canvasCenterX - spriteHalfX, canvasCenterY - spriteHalfY, sprite);

		return true;
	}
};

int main()
{
	SpriteEditor editor;
	if (editor.ConstructScreen(128, 64, 8, 8))
		editor.Start();

	return 0;
}