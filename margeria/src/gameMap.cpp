#include "gameMap.h"
#include "gameExceptions.h"
#include <cstdio>

GameMap::GameMap() {
	//dimensions = xy;
	//vector<Room*> col(dimensions.y, 0);
	//rooms = vector<vector<Room*> >(dimensions.x, col);
	//startRoomCoord = Coord(0, 0);

	roomMap = map<int, Room*>();
}

void GameMap::insertRoom( int roomId, Room* r ) {
	roomMap[roomId] = r;
}


// for testing purposes
void GameMap::print()
{
	printf("Map\n\n");
	for(auto i = roomMap.begin(); i != roomMap.end(); i++) {
		printf("%d\t%s\n", i->first, i->second->name.c_str());
	}
	printf("\n");
}

/*
void GameMap::addRoom(int roomId, Room* r) {
	roomMap[roomId] = r;
}
*/
