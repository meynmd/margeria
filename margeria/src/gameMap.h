#ifndef GAMEMAP_H
#define GAMEMAP_H
/***
 class GameMap

 contains all the rooms in the game

 Fields:
 dimensions	Coord						how many rooms in X and Y dimensions
 rooms		vector< vector<Room*> >		grid of rooms
 startRoom	Coord						on entering map, player should be
 spawned into room at these coords
 ***/

#include "room.h"

#include <map>

using namespace std;

class GameMap {
	Coord startRoomCoord; // default player spawn point

	map<int, Room*> roomMap;
	map<int, Container*> containers;
	int startRoom;

public:
	GameMap();

	//void addRoom(int roomId, Room* r);

	void insertRoom( int roomId, Room* r );

	void setStartRoom(int roomId) {
		startRoom = roomId;
	}

	Room* getRoomById(int rId) {
		return roomMap.at(rId);
	}

	Room* getStartRoom() {
		return roomMap.at(startRoom);
	}

	int getStartRoomId() {
		return startRoom;
	}

	void print();
};

#endif
