/*********************************************************************
** This file contains the main function for the wall hack (ESP) and
** the definitions of all helper functions needed to make it work.
*********************************************************************/

#include "pch.h"
#include "wallHack.h"

//If the current entity's team number is equal to the local player's team number, it returns false so that the
//main function below knows that the entity is an enemy player. This function is only called if in a team-based match
//because team number for each entity is randomly 0 or 1 and doesn't do anything in a free-for-all mode.
bool currentMatch::isEnemyPlayer(ent* entity)
{
	if (localPlayer->team == entity->team)
		return false;
	else 
		return true;
}

//If the current entity's state is equal to 0, it means they are alive and we should draw around them. Otherwise, the
//entity is dead and we should stop drawing the box.
bool currentMatch::isAlive(ent* entity)
{
	if (entity->state == 0)
		return true;
	else
		return false;
}

//If the current matches gameMode number equals any of the team-based gamemodes' numbers below, returns true so
//that the main function knows to only draw rectangles around the enemy team.
//Gamemode numbers found here: https://github.com/assaultcube/AC/blob/master/source/src/protocol.h
bool currentMatch::isTeamBased()
{
	if (gameMode == 0 || gameMode == 4 || gameMode == 5 || gameMode == 7 || gameMode == 11 || gameMode == 13 || gameMode == 14 || gameMode == 16 || gameMode == 17 || gameMode == 20 || gameMode == 21)
		return true;
	else 
		return false;
}

//CITATION: The drawFilledRectangle() and drawBorderBox() functions are modified functions similar to functions found via GuidedHacking.com.

//The drawFilledRectangle() does the actual drawing. A RECT object is defined based on the dimensions (x, y, h, and w) passed into the function, 
//then FillRect (Windows.h function) is called to actually draw the filled in rectangle. Color is determined by the brush passed in.
void drawFilledRectangle(float x, float y, float w, float h, HBRUSH brush, HDC deviceContext)
{
	RECT rectangle = { long(x), long(y), long(x + w), long(y + h) };

	FillRect(deviceContext, &rectangle, brush);
}

//The drawBorderBox() function makes 4 calls to drawFilledRectangle() in order to draw 4 separate rectangles (1 for each side of the rectangle), that
//will make up the box that is drawn around an entity in our main function.
void drawBorderBox(float x, float y, float w, float h, float thickness, HBRUSH brush, HDC deviceContext)
{
	//Draws the bottom line of our rectangle.
	drawFilledRectangle(x, y, w, thickness, brush, deviceContext);

	//Draws the right line of our rectangle.
	drawFilledRectangle(x, y, thickness, h, brush, deviceContext);

	//Draws the left line of our rectangle.
	drawFilledRectangle(x + w, y, thickness, h, brush, deviceContext);

	//Draws the top line of our rectangle.
	drawFilledRectangle(x, y + h, w + thickness, thickness, brush, deviceContext);
}

//WorldToScreen function transforms a matrix (3D coordinates) into a position on the screen, 
//which will allow us to accurately draw a rectangle around each entity's position on the screen.
//CITATION: https://guidedhacking.com/threads/world2screen-direct3d-and-opengl-worldtoscreen-functions.8044/
bool WorldToScreen(Vec3 pos, Vec2 &screen, float matrix[16], int windowWidth, int windowHeight) // 3D to 2D
{
	Vec4 clipCoords;

	//clipCoords = world coordinates * viewMatrix
	clipCoords.x = pos.x*matrix[0] + pos.y*matrix[4] + pos.z*matrix[8] + matrix[12];
	clipCoords.y = pos.x*matrix[1] + pos.y*matrix[5] + pos.z*matrix[9] + matrix[13];
	clipCoords.z = pos.x*matrix[2] + pos.y*matrix[6] + pos.z*matrix[10] + matrix[14];
	clipCoords.w = pos.x*matrix[3] + pos.y*matrix[7] + pos.z*matrix[11] + matrix[15];

	if (clipCoords.w < float(0.1))
		return false;

	//Normalized device coordinates
	Vec3 NDC;

	//NDC = clipCoords.(x, y, z) / clipCoords.w
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	//Transform to window coordinates so that we can draw to an accurate position in the window.
	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

//Main function that will be used for injection.
DWORD WINAPI wallHackMain()
{
	//Declares a new currentMatch object so we can access the classes variables and functions.
	currentMatch esp;

	//HWND object holds the AssaultCube window.
	HWND windowAC = FindWindow(0, TEXT("AssaultCube"));

	//HDC object holds a device context (DC) object. This one holds the DC of the AssaultCube window. Used as part of the drawing functions.
	HDC digitalContextAC = GetDC(windowAC);

	//Holds the raw address of the first entity in the entity list.
	DWORD entityList = *(DWORD*)(0x0050F4F4 + 0x4);

	//Vec2 vectors that will be used to hold the entity's base and head positions on the screen (passed to WorldToScreen).
	Vec2 baseVec, headVec;

	//Loops through the entities (players) in the match.
	for (int i = 0; i < esp.amountOfPlayers; i++)
	{
		//eAddr holds the raw address of the current entity, while curEnt holds the current entity
		//as a ent object, which makes it easier to access its team variable.
		DWORD eAddr = *(DWORD*)(entityList + (0x4 * i));
		ent* curEnt = esp.entities->ents[i];

		//If it is a team based gamemode, only draw boxes around the enemy team. See else statement below for free-for-all gamemodes.
		if (esp.isTeamBased())
		{
			//Only draw the box around the entity if it is alive, valid, and not on localPlayer's team (i.e. only enemies).
			if (eAddr != NULL && esp.isEnemyPlayer(curEnt) && esp.isAlive(curEnt))
			{
				//Sets enemy's position points (by address) and add them to a Vec3 vector (passed to WorldToScreen). 
				float entX = *(float*)(eAddr + 0x34);
				float entY = *(float*)(eAddr + 0x38);
				float entZ = *(float*)(eAddr + 0x3C);
				Vec3 entPos = {entX, entY, entZ };

				//Sets enemy's head position points (by address) and add them to a Vec3 vector (passed to WorldToScreen).
				float entHeadX = *(float*)(eAddr + 0x4);
				float entHeadY = *(float*)(eAddr + 0x8);
				float entHeadZ = *(float*)(eAddr + 0xC);
				Vec3 entHeadPos = { entHeadX, entHeadY, entHeadZ };

				//If the enemy is on our screen (determined by return value of WorldToScreen()), draw a rectangle around it.
				if (WorldToScreen(entPos, baseVec, esp.viewMatrix, 1024, 768))
				{
					if (WorldToScreen(entHeadPos, headVec, esp.viewMatrix, 1024, 768))
					{
						//Entity's head position - base position = head height. Used to determine other rectangle properties below.
						float head = headVec.y - baseVec.y;
						//Head height / 2 = width of the box (how wide the box is horizontally)
						float width = head / 2;
						//Width / -2 = center of the box (centers the box over the entity)
						float center = width / -2;
						//Head height / -6 = extra space (changing this value affects the height of the box (i.e. removing / -6 just draws a line under the entity's feet)
						float vertSpace = head / -6;

						//Creates the brush that is passed to drawBorderBox and sets the color to red. Once drawBorderBox is called,
						//the actual rectangle is drawn around the enemy. The BRUSH and call to DeleteObject have to be done within
						//the for loop or unintended consequences happen (i.e. boxes start being drawn in white after a few seconds).
						HBRUSH Brush = CreateSolidBrush(RGB(252, 3, 3));
						drawBorderBox(baseVec.x + center, baseVec.y, width, head - vertSpace, 1., Brush, digitalContextAC);
						DeleteObject(Brush);
					}
				}
			}
		}
		//If it is a free-for-all gamemode, draw a box around every entity in the game.
		else
		{
			//Box is only drawn around the entity if it is valid and alive.
			if (eAddr != NULL && esp.isAlive(curEnt))
			{
				//Sets enemy's position points and add them to a Vec3 vector (passed to WorldToScreen).
				float entX = *(float*)(eAddr + 0x34);
				float entY = *(float*)(eAddr + 0x38);
				float entZ = *(float*)(eAddr + 0x3C);
				Vec3 entPosition = {entX, entY, entZ};

				//Sets enemy's head position points and add them to a Vec3 vector (passed to WorldToScreen).
				float entHeadX = *(float*)(eAddr + 0x4);
				float entHeadY = *(float*)(eAddr + 0x8);
				float entHeadZ = *(float*)(eAddr + 0xC);
				Vec3 entHeadPosition = {entHeadX, entHeadY, entHeadZ};

				//If the enemy is on our screen (determined by return value of WorldToScreen()), draw a rectangle around it.
				if (WorldToScreen(entPosition, baseVec, esp.viewMatrix, esp.windowSizeH, esp.windowSizeV))
				{
					if (WorldToScreen(entHeadPosition, headVec, esp.viewMatrix, esp.windowSizeH, esp.windowSizeV))
					{
						//Entity's head position - base position = head height. Used to determine other rectangle properties below.
						float head = headVec.y - baseVec.y;
						//Head height / 2 = width of the box (how wide the box is horizontally)
						float width = head / 2;
						//Width / -2 = center of the box (centers the box over the entity)
						float center = width / -2;
						//Head height / -6 = vertical space (changing this value affects the height of the box (i.e. removing / -6 just draws a line under the entity's feet)
						float vertSpace = head / -6;

						//Creates the brush that is passed to drawBorderBox and sets the color to red. Once drawBorderBox is called,
						//the actual rectangle is drawn around the enemy. The BRUSH and call to DeleteObject have to be done within
						//the for loop or unintended consequences happen (i.e. boxes start being drawn in white after a few seconds).
						HBRUSH Brush = CreateSolidBrush(RGB(252, 3, 3));
						drawBorderBox(baseVec.x + center, baseVec.y, width, head - vertSpace, 1., Brush, digitalContextAC);
						DeleteObject(Brush);
					}
				}
			}
		}
	}

	//Deallocate memory created by GetDC to avoid memory leaks.
	DeleteObject(digitalContextAC);

	return 0;
}