#include "pch.h"
#include "aimbot.h"


std::vector<playerent*> playerList;
std::vector<playerent*> targets;
std::vector<float> distanceToBots;

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
	// Calculate position vectors, index is bot number in currentEntity array
	std::vector<float> distanceFromPlayer;

	for (int i = 0; i < numPlayers - 1; i++)
	{
		playerent* player = (playerent*)local_player_addr;

		float deltaX = 0;
		float deltaY = 0;
		float deltaZ = 0;
		float botX = playerList[i]->bodyPos.x;			// Same as head x coordinate
		float botY = playerList[i]->bodyPos.y;			// Same as head y coordinate
		float botZ = playerList[i]->headPos.z;

		// The three components of the vector from the players head to the enemies head
		deltaX = botX - player->bodyPos.x;
		deltaY = botY - player->bodyPos.y;
		deltaZ = botZ - player->headPos.z;

		// Calculate the overall magnitude of the vector, store in distanceToBots array
		float magnitude = sqrt(pow(deltaX, 2) + pow(deltaY, 2) + pow(deltaZ, 2));
		distanceToBots.push_back(magnitude);
	}

	// if player no longer exists, or they are dead, or they are on my team
		//continue

	// if player isn't visible
		// continue

	//add player to "targets
}

void Aimbot::fillEntityArray()
{
	playerList.clear();
	uintptr_t entityListPtr = (uintptr_t)local_player_addr + 0x4;		// 10F4F8, location of where bots are stored
	uintptr_t firstEntity = *(uintptr_t*)entityListPtr + 0x4;		// Index 0 is empty, first bot stored at index 1

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