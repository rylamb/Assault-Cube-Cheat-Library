#include "pch.h"
#include "aimbot.h"
#include <cmath>
#include <algorithm>

#define PI 3.14159265f

std::vector<Player> playerList;
std::vector<Player> targets;

uintptr_t module_base = (uintptr_t)GetModuleHandle(L"ac_client.exe");
uintptr_t* local_player_addr = (uintptr_t*)(module_base + 0x10F4F4);
playerent* localPlayer = (playerent*)*local_player_addr;
uintptr_t* numPlayersAddr = (uintptr_t*)0x50F500;					// Location of # of players in game
int numPlayers = *numPlayersAddr;

float Aimbot::vectorMagnitude(Vector3 vect)
{
	return sqrt(vect.x * vect.x + vect.y * vect.y + vect.z * vect.z);
}

float Aimbot::distanceBetweenTwoPoints(Vector3 point1, Vector3 point2)
{
	Vector3 deltaVector;
	deltaVector.x = (point2.x - point1.x);
	deltaVector.y = (point2.y - point1.y);
	deltaVector.z = (point2.z - point1.z);
	
	return vectorMagnitude(deltaVector);
}

Vector3 Aimbot::findAngle(Vector3 point1, Vector3 point2)
{
	// Calculates the angles between a vector defined by 2 points and the x,y,z axis
	Vector3 resultantAngles;
	float arcTanAngleX = ((-atan2f(point2.x - point1.x, point2.y - point1.y) * (180 / PI)) + 180);
	float arcTanAngleY = asinf(point2.z - point1.z) / distanceBetweenTwoPoints(point1, point2) * (180 / PI);

	std::cout << "arcTanAngleX: " << arcTanAngleX << std::endl;
	std::cout << "arcTanAngleY: " << arcTanAngleY << std::endl;
	
	resultantAngles.x = arcTanAngleX;
	resultantAngles.y = arcTanAngleY;
	resultantAngles.z = 0.0;
	return resultantAngles;
}

void Aimbot::run()
{
	fillEntityArray();
	getTargets();

	for (unsigned int i = 0; i < targets.size(); i++)
	{
		targets[i].targetAngles = findAngle(localPlayer->headPos, targets[i].entAddr->headPos);
	}
	
	calcPlayerDistances();

	std::sort(targets.begin(), targets.end(), comparator);

	if (!targets.empty())
	{
		localPlayer->yawPitchRoll.x = targets[0].targetAngles.x;
		localPlayer->yawPitchRoll.y = targets[0].targetAngles.y;
		localPlayer->yawPitchRoll.z = targets[0].targetAngles.z;
	}
}

bool Aimbot::comparator(Player p1, Player p2)
{
	return p1.distanceToPlayer < p2.distanceToPlayer;
}


void Aimbot::fillEntityArray()
{
	uintptr_t entityListPtr = (uintptr_t)local_player_addr + 0x4;		// 10F4F8, location of where bots are stored
	uintptr_t firstEntity = *(uintptr_t*)entityListPtr + 0x4;		// Index 0 is empty, first bot stored at index 1

	playerList.clear();
	for (int i = 0; i < numPlayers - 1; i++)
	{
		// Increment memory address, storing dynamically allocated bot memory locations into playerList
		uintptr_t currentEntity = *(uintptr_t*)(firstEntity + 4*i);
		if (currentEntity != 0)
		{
			if (*(uintptr_t*)currentEntity == 0x4E4A98 || *(uintptr_t*)currentEntity == 0x4E4AC0) {
				struct Player playerEnt;
				playerEnt.entAddr = (playerent*)currentEntity;
				playerList.push_back(playerEnt);
			}
		}
	}
}

void Aimbot::getTargets()
{
	targets.clear();
	for (unsigned int i = 0; i < playerList.size(); i++)
	{
		// TODO: Implement team validation to avoid team killing
		if (playerList[i].entAddr != nullptr && playerList[i].entAddr->state == 0)
		{
			// TODO: Implement visibility checking
			targets.push_back(playerList[i]);
		}
	}
}


void Aimbot::calcPlayerDistances()
{
	for (unsigned int i = 0; i < targets.size(); i++)
	{
		float deltaX = 0;
		float deltaY = 0;
		float deltaZ = 0;
		float botX = targets[i].entAddr->headPos.x;			// Same as head x coordinate
		float botY = targets[i].entAddr->headPos.y;			// Same as head y coordinate
		float botZ = targets[i].entAddr->headPos.z;

		// The three components of the vector from the players head to the enemies head
		deltaX = botX - localPlayer->headPos.x;
		deltaY = botY - localPlayer->headPos.y;
		deltaZ = botZ - localPlayer->headPos.z;

		// Calculate the overall magnitude of the vector, store in distanceToBots array
		float magnitude = sqrt(pow(deltaX, 2) + pow(deltaY, 2) + pow(deltaZ, 2));
		targets[i].distanceToPlayer = magnitude;
	}
}

void Aimbot::printEntityArray()
{
	for (unsigned int i = 0; i < playerList.size(); i++)
	{
		std::cout << "Head X: " << playerList[i].entAddr->headPos.x << std::endl;
		std::cout << "Head Y: " << playerList[i].entAddr->headPos.y << std::endl;
		std::cout << "Head Z: " << playerList[i].entAddr->headPos.z << std::endl;
		std::cout << "Body X: " << playerList[i].entAddr->bodyPos.x << std::endl;
		std::cout << "Body Y: " << playerList[i].entAddr->bodyPos.y << std::endl;
		std::cout << "Body Z: " << playerList[i].entAddr->bodyPos.z << std::endl;
		std::cout << "Yaw: " << playerList[i].entAddr->yawPitchRoll.x << std::endl;
		std::cout << "Pitch: " << playerList[i].entAddr->yawPitchRoll.y << std::endl;
		std::cout << "Roll: " << playerList[i].entAddr->yawPitchRoll.z << std::endl;
		std::cout << "Health: " << playerList[i].entAddr->playerHealth << std::endl;
		std::cout << "Armor: " << playerList[i].entAddr->playerArmor << std::endl;
		std::cout << "Team: " << playerList[i].entAddr->team << std::endl;
		std::cout << "Player State: " << playerList[i].entAddr->state << std::endl;
		std::cout << "Distance from Player: " << playerList[i].distanceToPlayer << std::endl << std::endl;
	}
}

void Aimbot::printTargetList()
{
	for (unsigned int i = 0; i < targets.size(); i++)
	{
		std::cout << "Health: " << targets[i].entAddr->playerHealth << std::endl;
		std::cout << "Armor: " << targets[i].entAddr->playerArmor << std::endl;
		std::cout << "Team: " << targets[i].entAddr->team << std::endl;
		std::cout << "Player State: " << targets[i].entAddr->state << std::endl;
		std::cout << "Distance from Player: " << targets[i].distanceToPlayer << std::endl << std::endl;
	}
}