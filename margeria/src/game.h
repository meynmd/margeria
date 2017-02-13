/***
 Game class

 This class is the heart of the game model and should
 encompass/contain all aspects of the game state
 ***/

#ifndef GAME_H
#define GAME_H

#include "gameMap.h"
#include "utilities/idGenerator.h"

#include <set>
#include <map>

// GameMode
// What phase of the game is the player in?
// Add more as needed
enum GameMode {
	Exploration, Conversation, Combat
};

/***
 class Game

 "Master" model class

 Fields:
	GameMode mode						enum value specifying current game mode
	bool IsGameAlive					true > playing game, false > game over
	PlayerCharacter* playerCharacter	pointer to the player character object
	GameMap* currentMap					pointer to the game map
	Coord currentRoom					pointer to room in map where player located
	Coord currentRoomCoord				coords to room in map where player located
	Coord playerLocationCoord			coords in current room where player located
	int idCounter						to make sure generated ID numbers unique
	std::vector<GameActor*>				list of all actors in game. Index is ID
 ***/

class Game {
private:
	GameMode mode;
	bool isGameAlive; 	// false == game over
	PlayerCharacter* playerCharacter;
	GameMap* currentMap;
	Room* currentRoom;
	Coord currentRoomCoord;
	Coord playerLocationCoord;
	int idCounter;
	set<GameActor*> gameActors;
	map<string, GameActor*> roomActorDictionary;
	set<string> roomActorDescriptors;
	IdGenerator* idGen;

	vector<Room*> levelRooms;

	void updateRoomActorDictionary();

public:
	Game( PlayerCharacter* p, GameMap* m );

	// move player character in direction d
	Event* movePlayer(Direction d, bool forward);

	// move GameActor at this coord in direction d if possible
	Event* moveActor(Coord from, Direction d);

	void setCurrentRoom(int rId);

	void setCurrentRoom( int rId, Coord pLoc );

	int generateId() {
		return idGen->generateId();
	}

	void addRoom(Room* r) {
		levelRooms.push_back(r);
	}

	// get/set methods
	bool getIsAlive() {
		return isGameAlive;
	}
	void setIsAlive(bool a) {
		isGameAlive = a;
	}
	PlayerCharacter* getPlayerCharacter() {
		return playerCharacter;
	}
	GameMode getMode() {
		return mode;
	}
	void setMode(GameMode m) {
		mode = m;
	}
	GameMap* getMap() {
		return currentMap;
	}
	void setMap(GameMap* m);

	Coord getCurrentRoomCoord() {
		return currentRoomCoord;
	}
	Coord getPlayerLocationCoord() {
		return playerLocationCoord;
	}
	Room* getCurrentRoom() {
		return currentRoom;
	}
	set<string> getRoomActorDescriptors() {
		return roomActorDescriptors;
	}
	GameActor* getRoomActorByDescriptor(string d) {
		try {
			return roomActorDictionary.at(d);
		}
		catch(exception e) {
			return 0;
		}
	}
	void insertInRoomActorDictionary(GameActor*);


};

#endif
