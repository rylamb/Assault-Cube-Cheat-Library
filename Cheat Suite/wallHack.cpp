/*********************************************************************
** This file contains the main function for the wall hack (ESP) and
** the definitions of all helper functions needed to make it work.
*********************************************************************/

#include "pch.h"
#include "wallHack.h"
#include "glDraw.h"

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
				if (WorldToScreen(entPos, baseVec, esp.viewMatrix, esp.windowSizeH, esp.windowSizeV))
				{
					if (WorldToScreen(entHeadPos, headVec, esp.viewMatrix, esp.windowSizeH, esp.windowSizeV))
					{
						//Entity's head position - base position = head height. Used to determine other rectangle properties below.
						float head = headVec.y - baseVec.y;
						//Head height / 1.5 = width of the box (how wide the box is horizontally)
						float width = head / 1.5f;
						//Width / -2 = center of the box (centers the box over the entity)
						float center = width / -2;
						//Head height / -2.5 = extra space (changing this value affects the height of the box (i.e. removing / -6 just draws a line under the entity's feet)
						float vertSpace = head / -2.5f;

						//Call to the OpenGL version of drawing a box. Function borrowed from Kitch's OpenGL GUI. See definition
						//in glDraw.cpp. This does the actual drawing of the box around the entities.
                        GL::DrawOutline((baseVec.x + center) * 1.17f, baseVec.y * 1.17f, width, head - vertSpace, 1, rgb::red);
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
						float width = head / 1.5f;
						//Width / -2 = center of the box (centers the box over the entity)
						float center = width / -2;
						//Head height / -6 = vertical space (changing this value affects the height of the box (i.e. removing / -6 just draws a line under the entity's feet)
						float vertSpace = head / -2.5f;

						//Call to the OpenGL version of drawing a box. Function borrowed from Kitch's OpenGL GUI. See definition
						//in glDraw.cpp. This does the actual drawing of the box around the entities.
                        GL::DrawOutline((baseVec.x + center) * 1.17f, baseVec.y * 1.17f, width, head - vertSpace, 1, rgb::red);
                    }
				}
			}
		}
	}

	return 0;
}