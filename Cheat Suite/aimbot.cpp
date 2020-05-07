#include "pch.h"
#include "aimbot.h"


std::vector<playerent*> playerList;
std::vector<playerent*> targets;

uintptr_t module_base = (uintptr_t)GetModuleHandle(L"ac_client.exe");
uintptr_t* local_player_addr = (uintptr_t*)(module_base + 0x10F4F4);

float Vector3::vectorMagnitude()
{
	return sqrt(x * x + y * y + z * z);
}

void Aimbot::run()
{
	// if player no longer exists, or they are dead, or they are on my team
		//continue

	// if player isn't visible
		// continue

	//add player to "targets
}

void Aimbot::fillEntityArray()
{
	playerList.clear();
	uintptr_t* numPlayersAddr = (uintptr_t*)0x50F500;					// Location of # of players in game
	uintptr_t entityListPtr = (uintptr_t)local_player_addr + 0x4;		// 10F4F8, location of where bots are stored
	uintptr_t firstEntity = *(uintptr_t*)entityListPtr + 0x4;		// Index 0 is empty, first bot stored at index 1
	int numPlayers = *numPlayersAddr;

	for (int i = 0; i < numPlayers - 1; i++)
	{
		// Increment memory address, storing dynamically allocated bot memory locations into playerList
		uintptr_t currentEntity = *(uintptr_t*)(firstEntity + 4*i);
		if (currentEntity != 0)
		{
			if (*(uintptr_t*)currentEntity == 0x4E4A98 || *(uintptr_t*)currentEntity == 0x4E4AC0) {
				playerList.push_back((playerent*)currentEntity);
			}
		}
	}
}


// Remove later
void Aimbot::printEntityArray()
{
	for (unsigned int i = 0; i < playerList.size(); i++)
	{
		std::cout << "Head X: " << playerList[i]->headPos.x << std::endl;
		std::cout << "Head Y: " << playerList[i]->headPos.y << std::endl;
		std::cout << "Head Z: " << playerList[i]->headPos.z << std::endl;
		std::cout << "Body X: " << playerList[i]->bodyPos.x << std::endl;
		std::cout << "Body Y: " << playerList[i]->bodyPos.y << std::endl;
		std::cout << "Body Z: " << playerList[i]->bodyPos.z << std::endl;
		std::cout << "Yaw: " << playerList[i]->yawPitchRoll.x << std::endl;
		std::cout << "Pitch: " << playerList[i]->yawPitchRoll.y << std::endl;
		std::cout << "Roll: " << playerList[i]->yawPitchRoll.z << std::endl;
		std::cout << "Health: " << playerList[i]->playerHealth << std::endl;
		std::cout << "Armor: " << playerList[i]->playerArmor << std::endl;
		std::cout << "Team: " << playerList[i]->team << std::endl;
		std::cout << "Player State: " << playerList[i]->team << std::endl << std::endl;
	}
}