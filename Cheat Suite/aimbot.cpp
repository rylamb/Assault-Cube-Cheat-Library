#include "pch.h"
#include "aimbot.h"


std::vector<Player> playerList;
std::vector<Player> targets;

uintptr_t module_base = (uintptr_t)GetModuleHandle(L"ac_client.exe");
uintptr_t* local_player_addr = (uintptr_t*)(module_base + 0x10F4F4);
uintptr_t* numPlayersAddr = (uintptr_t*)0x50F500;					// Location of # of players in game
int numPlayers = *numPlayersAddr;

float Vector3::vectorMagnitude()
{
	return sqrt(x * x + y * y + z * z);
}

void Aimbot::run()
{
	fillEntityArray();
	getTargets();
	calcPlayerDistances();

	// sort targets by distances

	// if targets isn't empty
		// se my yawPitchAngle to move to target
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
	for (unsigned int i = 0; i < playerList.size(); i++)
	{
		playerent* player = (playerent*)local_player_addr;

		float deltaX = 0;
		float deltaY = 0;
		float deltaZ = 0;
		float botX = playerList[i].entAddr->headPos.x;			// Same as head x coordinate
		float botY = playerList[i].entAddr->headPos.y;			// Same as head y coordinate
		float botZ = playerList[i].entAddr->headPos.z;

		// The three components of the vector from the players head to the enemies head
		deltaX = botX - player->headPos.x;
		deltaY = botY - player->headPos.y;
		deltaZ = botZ - player->headPos.z;

		// Calculate the overall magnitude of the vector, store in distanceToBots array
		float magnitude = sqrt(pow(deltaX, 2) + pow(deltaY, 2) + pow(deltaZ, 2));
		playerList[i].distanceToPlayer = magnitude;
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