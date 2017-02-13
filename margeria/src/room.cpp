// implementation of class Room

#include "room.h"
#include "gameExceptions.h"
#include <cstdio>

Room::Room( int roomId, Coord dim )/* :
		id(roomId)*/ {
	dimensions = dim;
	vector<Space> col(dim.y, Space());
	spaces = vector<vector<Space> >(dim.x, col);
	name = "";
	description = "";
	spawnPoint = Coord(0, 0);
	hasLightSource = true;
}

Room::Room( int roomId, Coord dim, Coord sPoint, string name,
		string description ) :
		id(roomId) {
	dimensions =  dim;
	vector<Space> col(dim.y, Space());
	spaces = vector<vector<Space> >(dim.x, col);
	this->name = name;
	this->description = description;
	hasLightSource = true;
	if (isInBounds(sPoint)) {
		spawnPoint = sPoint;
	} else {
		throw(invalidCoordException("location out of game map bounds"));
	}
}


bool Room::removeAt( Coord xy, GameActor* obj ) {
	if (!isInBounds(xy)) {
		throw("location out of game map bounds");
	}
	// if the object is in this location, remove it
	if (spaces[xy.x][xy.y].contents == obj) {
		spaces[xy.x][xy.y].contents = 0;
		return true;
	} else {
		return false;
	}
}

bool Room::insertAt( Coord xy, GameActor* obj ) {
	if (!isInBounds(xy)) {
		throw(invalidCoordException("location out of game map bounds"));
	}
	// if no object is in this location, insert the object
	if (spaces[xy.x][xy.y].contents == 0) {
		spaces[xy.x][xy.y].contents = obj;
		obj->setRoom(this);
		obj->setRoomId(this->id);
		obj->setLocationCoords(xy);
		return true;
	} else {
		return false;
	}
}

// for testing purposes
void Room::print() {
	printf("%s\n\n%s\n\n", name.c_str(), description.c_str());
	for (int j = 0; j < dimensions.y; j++) {
		for (int i = 0; i < dimensions.x; i++) {
			if (objectAt(Coord(i, j)) == 0) {
				printf("     ");
			} else {
				printf("  %c  ", objectAt(Coord(i, j))->getSymbol());
			}
		}
		printf("\n\n");
	}
}

bool Room::isInBounds(Coord xy) {
	if( xy.x >= 0 && xy.x < dimensions.x &&
		xy.y >= 0 && xy.y < dimensions.y ) {
		return true;
	} else {
		return false;
	}
}
