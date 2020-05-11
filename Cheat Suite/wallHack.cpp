/*********************************************************************
** This file contains the main function for the wall hack (ESP) and
** the definitions of all helper functions needed to make it work.
*********************************************************************/

#include "wallHack.h"

//If the current entity's team number is equal to the local player's team number, it returns false so that the
//main function below knows that the entity is an enemy player. This function is only called if in a team-based match
//because team number for each entity is randomly 0 or 1 and doesn't do anything in a free-for-all mode.
bool ESP::isEnemy(ent* e)
{
	if (localPlayer->team == e->team)
		return false;
	else 
		return true;
}

//If the current matches gameMode number equals any of the team-based gamemodes' numbers below, returns true so
//that the main function knows to only draw rectangles around the enemy team.
//Gamemode numbers found here: https://github.com/assaultcube/AC/blob/master/source/src/protocol.h
bool ESP::isTeamBased()
{
	if (gameMode == 0 || gameMode == 4 || gameMode == 5 || gameMode == 7 || gameMode == 11 || gameMode == 13 ||
		gameMode == 14 || gameMode == 16 || gameMode == 17 || gameMode == 20 || gameMode == 21)
		return true;
	else 
		return false;
}

//CITATION: The drawFilledRectangle() and drawBorderBox() functions are modified functions similar to functions found via GuidedHacking.com.
void drawFilledRectangle(int x, int y, int w, int h, HBRUSH brush, HDC deviceContext)
{
	RECT rectangle = { x, y, x + w, y + h };

	FillRect(deviceContext, &rectangle, brush);
}

void drawBorderBox(int x, int y, int w, int h, int thickness, HBRUSH brush, HDC deviceContext)
{
	drawFilledRectangle(x, y, w, thickness, brush, deviceContext);
	drawFilledRectangle(x, y, thickness, h, brush, deviceContext);
	drawFilledRectangle(x + w, y, thickness, h, brush, deviceContext);
	drawFilledRectangle(x, y + h, w + thickness, thickness, brush, deviceContext);
}

//WorldToScreen function transforms a matrix (3D coordinates) into a position on the screen, 
//which will allow us to accurately draw a rectangle around each entity's position on the screen.
//CITATION: https://guidedhacking.com/threads/world2screen-direct3d-and-opengl-worldtoscreen-functions.8044/
bool WorldToScreen(Vec3 pos, Vec2 &screen, float matrix[16], int windowWidth, int windowHeight) // 3D to 2D
{
	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	Vec4 clipCoords;
	clipCoords.x = pos.x*matrix[0] + pos.y*matrix[4] + pos.z*matrix[8] + matrix[12];
	clipCoords.y = pos.x*matrix[1] + pos.y*matrix[5] + pos.z*matrix[9] + matrix[13];
	clipCoords.z = pos.x*matrix[2] + pos.y*matrix[6] + pos.z*matrix[10] + matrix[14];
	clipCoords.w = pos.x*matrix[3] + pos.y*matrix[7] + pos.z*matrix[11] + matrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	//perspective division, dividing by clip.W = Normalized Device Coordinates
	Vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	//Transform to window coordinates
	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

//Main function that will be used for injection.
DWORD WINAPI wallHackMain()
{
	//Declares a new ESP object so we can access the classes variables and functions.
	ESP esp;

	//HWND object holds the AssaultCube window.
	HWND windowAC = FindWindow(0, TEXT("AssaultCube"));

	//HDC object holds a device context (DC) object. This one holds the DC of the AssaultCube window. Used as part of the drawing functions.
	HDC digitalContextAC = GetDC(windowAC);

	//Holds the raw address of the first entity in the entity list.
	DWORD entityList = *(DWORD*)(0x0050F4F4 + 0x4);

	//Vec2 vectors that will be used to hold the entity's base and head positions on the screen (passed to WorldToScreen).
	Vec2 vecBase, vecHead;

	//Loops through the entities in the match.
	for (int i = 0; i < esp.amountOfPlayers; i++)
	{
		//eAddr holds the raw address of each entity, while e holds the entity
		//as a ent object, which makes it easier to access its team variable.
		DWORD eAddr = *(DWORD*)(entityList + 0x4 * i);
		ent* e = esp.entities->ents[i];

		//If it is a team based gamemode, only draw boxes around the enemy team. See else statement below for free-for-all gamemodes.
		if (esp.isTeamBased())
		{
			//Only draw the box around the entity if it is valid and not on localPlayer's team (i.e. only enemies).
			if (eAddr != NULL && esp.isEnemy(e))
			{
				//Sets enemy's position points and add them to a Vec3 vector (passed to WorldToScreen).
				float enemyX = *(float*)(eAddr + 0x34);
				float enemyY = *(float*)(eAddr + 0x38);
				float enemyZ = *(float*)(eAddr + 0x3C);
				Vec3 enemyPos = { enemyX, enemyY, enemyZ };

				//Sets enemy's head position points and add them to a Vec3 vector (passed to WorldToScreen).
				float enemyXHead = *(float*)(eAddr + 0x4);
				float enemyYHead = *(float*)(eAddr + 0x8);
				float enemyZHead = *(float*)(eAddr + 0xC);
				Vec3 enemyHeadPos = { enemyXHead, enemyYHead, enemyZHead };

				//If the enemy is on our screen (determined by return value of WorldToScreen(), draw a rectangle around it.
				if (WorldToScreen(enemyPos, vecBase, esp.viewMatrix, 1024, 768))
				{
					if (WorldToScreen(enemyHeadPos, vecHead, esp.viewMatrix, 1024, 768))
					{
						//Entity's head position - base position = head height. Used to determine other rectangle properties below.
						float head = vecHead.y - vecBase.y;
						//Head height / 2 = width of the box (how wide the box is horizontally)
						float width = head / 2;
						//Width / -2 = center of the box (centers the box over the entity)
						float center = width / -2;
						//Head height / -6 = extra space (changing this value affects the height of the box (i.e. removing / -6 just draws a line under the entity's feet)
						float extra = head / -6;

						//Creates the brush that is passed to drawBorderBox and sets the color to red. Once drawBorderBox is called,
						//the actual rectangle is drawn around the enemy. The BRUSH and call to DeleteObject have to be done within
						//the for loop or unintended consequences happen (i.e. boxes start being drawn in white after a few seconds).
						HBRUSH Brush = CreateSolidBrush(RGB(252, 3, 3));
						drawBorderBox(vecBase.x + center, vecBase.y, width, head - extra, 1, Brush, digitalContextAC);
						DeleteObject(Brush);
					}
				}
			}
		}
		//If it is a free-for-all gamemode, draw a box around every entity in the game.
		else
		{
		    //Draw a box around any entity that isn't NULL.
			if (eAddr != NULL)
			{
				//Sets enemy's position points and add them to a Vec3 vector (passed to WorldToScreen).
				float enemyX = *(float*)(eAddr + 0x34);
				float enemyY = *(float*)(eAddr + 0x38);
				float enemyZ = *(float*)(eAddr + 0x3C);
				Vec3 enemyPos = { enemyX, enemyY, enemyZ };

				//Sets enemy's head position points and add them to a Vec3 vector (passed to WorldToScreen).
				float enemyXHead = *(float*)(eAddr + 0x4);
				float enemyYHead = *(float*)(eAddr + 0x8);
				float enemyZHead = *(float*)(eAddr + 0xC);
				Vec3 enemyHeadPos = { enemyXHead, enemyYHead, enemyZHead };

                //If the enemy is on our screen (determined by return value of WorldToScreen(), draw a rectangle around it.
				if (WorldToScreen(enemyPos, vecBase, esp.viewMatrix, 1024, 768))
				{
					if (WorldToScreen(enemyHeadPos, vecHead, esp.viewMatrix, 1024, 768))
					{
						//Entity's head position - base position = head height. Used to determine other rectangle properties below.
						float head = vecHead.y - vecBase.y;
						//Head height / 2 = width of the box (how wide the box is horizontally)
						float width = head / 2;
						//Width / -2 = center of the box (centers the box over the entity)
						float center = width / -2;
						//Head height / -6 = extra space (changing this value affects the height of the box (i.e. removing / -6 just draws a line under the entity's feet)
						float extra = head / -6;

						//Creates the brush that is passed to drawBorderBox and sets the color to red. Once drawBorderBox is called,
						//the actual rectangle is drawn around the enemy. The BRUSH and call to DeleteObject have to be done within
						//the for loop or unintended consequences happen (i.e. boxes start being drawn in white after a few seconds).
						HBRUSH Brush = CreateSolidBrush(RGB(252, 3, 3));
						drawBorderBox(vecBase.x + center, vecBase.y, width, head - extra, 1, Brush, digitalContextAC);
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