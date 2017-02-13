#ifndef ROOM_H
#define ROOM_H

#include "gameActor.h"
#include "event.h"

#include <fstream>
#include <string>
#include <vector>

using namespace std;

/***
 struct Space
 One space in a room grid. Contains a pointer to a gameActor (may be null).

 Fields:
 contents	gameActor*		what is in this space
 ***/
struct Space {
	GameActor* contents;

	// other fields?

};

/***
 struct Room

 A room in the game. Has an integer ID as specified by constuctor call.
 call objectAt(x, y) to get a pointer to game object at (x, y)

 Fields:
 id			integer ID of room, should be negative
 dimensions	Coord struct with the x and y dimensions
 spaces		2D vector of all spaces in room

 Methods:
 GameActor* objectAt( int x, int y )
 Returns pointer to the object at room grid location (x, y), or null pointer
 if there is no such object

 getter/setter functions:
 void setName( string name )
 void setDescription( string desc )
 string getName()
 string getDescription()
 ***/
struct Room {
	int id;			// should be negative
	string name;
	string description;
	Coord dimensions;
	Coord spawnPoint;
	vector< vector<Space> > spaces;
	bool hasLightSource;

	Room( int roomId, Coord dim );
	Room( int roomId, Coord dim, Coord sPoint, string name, string description );


	int getId() {
		return id;
	}

	//Room* load(istream& roomFile);

	void setName( string name ) {
		this->name = name;
	}
	void setDescription( string desc ) {
		description = desc;
	}
	string getName() {
		return name;
	}
	string getDescription() {
		return description;
	}
	Coord getDimensions() {
		return dimensions;
	}
	GameActor* objectAt( Coord xy ) {
		return spaces.at(xy.x).at(xy.y).contents;
	}

	bool isInBounds(Coord xy);

	bool removeAt( Coord xy, GameActor* obj );
	bool insertAt( Coord, GameActor* );

	void print();
};

#endif
