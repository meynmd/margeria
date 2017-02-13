// class to load game data

#ifndef LOADER_H
#define LOADER_H

#include "gameMap.h"
#include "roomObjects.h"
#include "gameExceptions.h"

#include <iostream>

using namespace std;

/***
class Loader

Loader for maps and rooms

GameMap* loadMap(ifstream& mapFile)
	returns a GameMap* resulting from loading data in mapFile

Room* loadRoom(ifstream& roomFile);
	Room loader loads room from data file roomFile. See roomDataFormat.txt
***/
class Loader {
private:
	GameMap* gameMap;

	GameActor* generateRandomEquipment();

public:
	Loader() { gameMap = 0; }

	void loadMap(ifstream&, string);

	// Room loader loads room from data file of symbols. See roomDataFormat.txt
	Room* loadRoom(ifstream&);

	void loadContainers(ifstream&);

	void loadExits(ifstream&);

	GameMap* getMap() { return gameMap; }
};

#endif
