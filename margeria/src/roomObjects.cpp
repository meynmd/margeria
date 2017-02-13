#include "roomObjects.h"

bool RoomObjects::inst = false;
RoomObjects* RoomObjects::_roomObjects = 0;
IdGenerator* RoomObjects::idGen = IdGenerator::IdGeneratorClass(1);

map<const char, GameActor*> RoomObjects::objectMap = map<const char, GameActor*>( {
		{ '|', new RoomObject(idGen->generateId(), "WallH", '|') },
		{ '-', new RoomObject(idGen->generateId(), "WallV", '-') },
		{ '\\', new RoomObject(idGen->generateId(), "WallLR", '\\') },
		{ '/', new RoomObject(idGen->generateId(), "WallRL", '/') },

		{ ';', new RoomObject(idGen->generateId(), "WallUL", '/') },
		{ '\'', new RoomObject(idGen->generateId(), "WallUR", '\\') },
		{ ',', new RoomObject(idGen->generateId(), "WallLL", '\\') },
		{ '.', new RoomObject(idGen->generateId(), "WallLR", '/') },

		{ 'L', new RoomObject(idGen->generateId(), "WaterL", 'L') },
		{ 'R', new RoomObject(idGen->generateId(), "WaterR", 'R') },
		{ '2', new RoomObject(idGen->generateId(), "Box2", '2') },
		{ '3', new RoomObject(idGen->generateId(), "Box3", '3') },
		{ '4', new RoomObject(idGen->generateId(), "Box4", '4') },
		{ '<', new RoomObject(idGen->generateId(), "WaterTL", '<') },
		{ '>', new RoomObject(idGen->generateId(), "WaterTR", '>') },
		{ ')', new RoomObject(idGen->generateId(), "WaterBL", ')') },
		{ '(', new RoomObject(idGen->generateId(), "WaterBR", '(') },

		{ 'F', new Fish(idGen->generateId(), "Fish") },
		{ 'C', new Candle(idGen->generateId(), "Candle", true) },
		{ 'T', new Torch(idGen->generateId(), "torch", false) },
		{ 'H', new Pit(idGen->generateId(), "pit") },
		{ 'h', new Pit(idGen->generateId(), "pit") },
		{ 'P', new Pot(idGen->generateId(), "pot") },
		{ 'l', new RoomObject(idGen->generateId(), "tlBed", 'l') },
		{ 'r', new RoomObject(idGen->generateId(), "trBed", 'r') },
		{ 'b', new RoomObject(idGen->generateId(), "brBed", 'b') },
		{ 'y', new RoomObject(idGen->generateId(), "blBed", 'y') },
		{ 'm', new RoomObject(idGen->generateId(), "hedge", 'm') },
		
		
		
		
		
} );

RoomObjects::RoomObjects() {
	objectMap[';']->setTxtMap("txtmapWallUL.txt");
	objectMap['\'']->setTxtMap("txtmapWallUR.txt");
	objectMap[',']->setTxtMap("txtmapWallLL.txt");
	objectMap['.']->setTxtMap("txtmapWallLR.txt");
	objectMap['|']->setTxtMap("txtmapWallV.txt");
	objectMap['-']->setTxtMap("txtmapWallH.txt");
	objectMap['\\']->setTxtMap("txtmapLRwall.txt");
	objectMap['/']->setTxtMap("txtmapRLwall.txt");
	objectMap['L']->setTxtMap("txtmapLWater.txt");
	objectMap['R']->setTxtMap("txtmapRWater.txt");
	objectMap['2']->setTxtMap("txtmapBox2.txt");
	objectMap['3']->setTxtMap("txtmapBox3.txt");
	objectMap['4']->setTxtMap("txtmapBox4.txt");
	objectMap['<']->setTxtMap("txtmapSewerWaterTL.txt");
	objectMap['>']->setTxtMap("txtmapSewerWaterTR.txt");
	objectMap[')']->setTxtMap("txtmapSewerWaterBL.txt");
	objectMap['(']->setTxtMap("txtmapSewerWaterBR.txt");
	//objectMap['W']->setTxtMap("txtmapWineLeft.txt");
	//objectMap['B']->setTxtMap("txtmapWineRight.txt");
	objectMap['F']->setTxtMap("txtmapDeadFish.txt");
	objectMap['C']->setTxtMap("txtmapCandle_2.txt");
	objectMap['h']->setTxtMap("txtmapPit_2.txt");
	objectMap['l']->setTxtMap("txtmapBedTopLeft.txt");
	objectMap['r']->setTxtMap("txtmapBedTopRight.txt");
	objectMap['b']->setTxtMap("txtmapBedBotLeft.txt");
	objectMap['y']->setTxtMap("txtmapBedBotRight.txt");
	objectMap['m']->setTxtMap("txtmapHedge2.txt");

}

RoomObjects* RoomObjects::RoomObjectsClass() {
	if(!inst) {
		_roomObjects = new RoomObjects();
		inst = true;
	}
	return _roomObjects;
}

GameActor* RoomObjects::findRoomObject(char symbol) {
	return objectMap.at(symbol);
}
