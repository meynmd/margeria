// a list of all available room objects in the game

#ifndef ROOMOBJECTS_H
#define ROOMOBJECTS_H

#include "gameActor.h"
#include "utilities/idGenerator.h"

#include <map>

using namespace std;

class RoomObjects {
private:
	static map<const char, GameActor*> objectMap;
	static IdGenerator* idGen;

	static RoomObjects* _roomObjects;
	static bool inst;

protected:
	RoomObjects();

public:
	static RoomObjects* RoomObjectsClass();
	~RoomObjects() { inst = false; }

	GameActor* findRoomObject(char symbol);
};



#endif
