#include "loader.h"
#include "gameActor.h"
#include "roomObjects.h"
#include "params.h"
#include "utilities/idGenerator.h"
#include "utilities/tokenizer.h"

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

void Loader::loadMap( ifstream& mapFile, string path ) {
	srand(time(0));
	gameMap = new GameMap();
	char line[BUF_SIZE];
	int rId, roomCount = 0;
	mapFile.seekg( ios::beg );

	// each line should contain unique negative ID# and filename separated by tab
	while (mapFile.getline( line, BUF_SIZE, '\t' )) {
		rId = atoi( line );
		if (rId >= 0) {
			throw(idException( "Room ID must be negative" ));
		}
		mapFile.getline( line, BUF_SIZE );
		string rFilename = path + line;
		ifstream rFile;
		rFile.open( rFilename.c_str() );
		Room* r = loadRoom( rFile );
		if (r) {
			gameMap->insertRoom( rId, r );
			roomCount++;
		}
		// first line should contain id and filename of start room
		if (roomCount == 1) {
			gameMap->setStartRoom( rId );
		}
	}
}

Room* Loader::loadRoom( ifstream& roomFile ) {
	if (!roomFile) {
		throw(fileIoException( "Error reading room file\n" ));
	}

	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );
	RoomObjects* roomObjects = RoomObjects::RoomObjectsClass();
	char line[BUF_SIZE];
	int xDim, yDim;
	roomFile.seekg( ios::beg );

	// first line: room ID
	roomFile.getline( line, BUF_SIZE );
	int rId = atoi( line );
	if (rId >= 0) {
		throw(fileIoException( "Map file incorrectly formatted\n" ));
	}

	// second line should be x, y dimension
	if ('[' != roomFile.get()) {
		throw(fileIoException( "Map file incorrectly formatted\n" ));
	}
	roomFile.getline( line, BUF_SIZE, ',' );
	xDim = atoi( line );
	roomFile.getline( line, BUF_SIZE );
	yDim = atoi( line );

	// next header line is starting room coords
	if ('[' != roomFile.get()) {
		throw(fileIoException( "Map file incorrectly formatted\n" ));
	}
	roomFile.getline( line, BUF_SIZE, ',' );
	int startX = atoi( line );
	roomFile.getline( line, BUF_SIZE );
	int startY = atoi( line );

	// lit or unlit
	bool lit = false;
	roomFile.getline(line, BUF_SIZE);
	if(line[0] == 'L') {
		lit = true;
	}

	// next lines should be name, then description
	roomFile.getline( line, BUF_SIZE );
	string rName( line );
	roomFile.getline( line, BUF_SIZE );
	string rDesc( line );

	Room* room = new Room( rId, Coord( xDim, yDim ), Coord( startX, startY ), rName, rDesc );
	room->hasLightSource = lit;

	// following lines should be a representation of the room
	int row = 0;
	while (roomFile.getline( line, BUF_SIZE )) {
		// read a "row" of room spaces
		int col = 0;
		vector<string> tokens = tokenize( string( line ), set<char>( { '\t' } ), false );
		for (auto t = tokens.begin(); t != tokens.end(); t++) {
			if (*t == "n") {
			} else if(*t == "MB") {
				room->insertAt(Coord(col,row), new MoveableBox(idGen->generateId(), "box"));
			} else {
				room->insertAt( Coord( col, row ), roomObjects->findRoomObject( t->at( 0 ) ) );
			}
			col++;
		}
		row++;
	}
	return room;
}

void Loader::loadContainers( ifstream& containerFile ) {
	if (!containerFile) {
		throw(fileIoException( "Error reading room file\n" ));
	}
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );
	char line[BUF_SIZE];
	containerFile.seekg( ios::beg );

	while (containerFile.getline( line, BUF_SIZE )) {
		vector<string> tokens = tokenize( string( line ), set<char>( { '\t' } ), false );
		bool unlocked;
		if (tokens.at( 3 ) == "L") {
			unlocked = false;
		} else {
			unlocked = true;
		}
		int containerRoomId = atoi( tokens.at( 1 ).c_str() );
		int keyRoomId;
		if (!unlocked) {
			keyRoomId = atoi( tokens.at( 4 ).c_str() );
		}
		vector<string> containerCoordTok = tokenize( tokens.at( 2 ), set<char>( { '(', ')', ',' } ),
				false );
		Coord containerCoord( atoi( containerCoordTok.at( 0 ).c_str() ),
				atoi( containerCoordTok.at( 1 ).c_str() ) );
		Coord keyCoord;
		if (!unlocked) {
			vector<string> keyCoordTok = tokenize( tokens.at( 5 ), set<char>( { '(', ')', ',' } ),
					false );
			keyCoord = Coord( atoi( keyCoordTok.at( 0 ).c_str() ),
					atoi( keyCoordTok.at( 1 ).c_str() ) );
		}

		// place container and key in appropriate room(s)
		Room* cr = gameMap->getRoomById( containerRoomId );
		Container* con = new Container( idGen->generateId(), tokens.at( 0 ), unlocked );
		/*
		if(rand() % 3 != 0) {
			con->put(generateRandomEquipment());
		}
		*/
		con->put(new Gold(idGen->generateId(), "gold bar"));
		cr->insertAt( containerCoord, con );
		if (!unlocked) {
			Room* kr = gameMap->getRoomById( keyRoomId );
			kr->insertAt( keyCoord,
					new Key( idGen->generateId(), "key to " + tokens.at( 0 ), con ) );
		}
	}
}

void Loader::loadExits( ifstream& doorFile ) {
	if (!doorFile) {
		throw(fileIoException( "Error reading room file\n" ));
	}
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );
	char line[BUF_SIZE];
	doorFile.seekg( ios::beg );

	while (doorFile.getline( line, BUF_SIZE )) {
		vector<string> tokens = tokenize( string( line ), set<char>( { '\t' } ), false );
		bool unlocked;
		if (tokens.at( 5 ) == "L") {
			unlocked = false;
		} else {
			unlocked = true;
		}
		int roomId = atoi( tokens.at( 1 ).c_str() );
		int destRoomId = atoi( tokens.at( 3 ).c_str() );
		vector<string> roomCoordTok = tokenize( tokens.at( 2 ), set<char>( { '(', ')', ',' } ),
				false );
		Coord roomCoord( atoi( roomCoordTok.at( 0 ).c_str() ),
				atoi( roomCoordTok.at( 1 ).c_str() ) );
		vector<string> destRoomCoordTok = tokenize( tokens.at( 4 ), set<char>( { '(', ')', ',' } ),
				false );
		Coord destRoomCoord( atoi( destRoomCoordTok.at( 0 ).c_str() ),
				atoi( destRoomCoordTok.at( 1 ).c_str() ) );
		int keyRoomId;
		Coord keyCoord;
		if (!unlocked) {
			keyRoomId = atoi( tokens.at( 6 ).c_str() );
			vector<string> keyCoordTok = tokenize( tokens.at( 7 ), set<char>( { '(', ')', ',' } ),
					false );
			keyCoord = Coord( atoi( keyCoordTok.at( 0 ).c_str() ),
					atoi( keyCoordTok.at( 1 ).c_str() ) );
		}
		// place exit and key, if necessary
		Room* r = gameMap->getRoomById( roomId );
		RoomExit* re = new RoomExit( idGen->generateId(), destRoomId, destRoomCoord, unlocked );
		GameActor* gRe = static_cast<GameActor*>( re );
		gRe->setName( tokens[0] );
		r->insertAt( roomCoord, gRe );
		if (!unlocked) {
			Room* kr = gameMap->getRoomById( keyRoomId );
			kr->insertAt( keyCoord,
					new Key( idGen->generateId(), "key to " + tokens.at( 0 ), (Lockable*) re ) );
		}
	}
}

GameActor* Loader::generateRandomEquipment() {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );
	GameActor* item = 0;
	int whatType = rand() % 2;
	if (whatType == 0) {
		item = static_cast<GameActor*>( new Explosive(idGen->generateId(), "bomb"));
	} else {
		item = static_cast<GameActor*>( new Food( idGen->generateId(), "beef jerkey"));
	}
	return item;
}

