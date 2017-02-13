// Monarch of Margeria
//
// by Matthew Meyn and Scott Williams
//
// 2016/6/6

#include "coord.h"
#include "gameActor.h"
#include "room.h"
#include "gameMap.h"
#include "game.h"
#include "loader.h"
#include "controller.h"
#include "params.h"
#include "interpreter.h"
#include "gameInputOutput.h"
#include "animation.h"
#include "gameActor.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#include "utilities/tokenizer.h"

using namespace std;

void setupRoom1( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	RoomExit* room1Door = new RoomExit( idGen->generateId(), -1, Coord( 1, 1 ), false );
	controller->insertInRoom( -1, Coord( 10, 3 ), room1Door );
	MoveableBox* room1Box = new MoveableBox( idGen->generateId(), "block" );
	room1Box->addDescriptor( "desk" );
	controller->insertInRoom( -1, Coord( 13, 4 ), room1Box );

	Window* cellWin = new Window( idGen->generateId(), -2, Coord( 1, 7 ) );
	cellWin->setInteractMessage(
			"Standing on your tiptoes, you still can't quite reach the tiny barred window up in the corner of the cell." );
	controller->insertInRoom( -1, Coord( 14, 8 ), cellWin );
}

void setupRoom2( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	RoomExit* lowWindow = new RoomExit( idGen->generateId(), -1, Coord( 13, 7 ), true );
	lowWindow->setTxtMap( "txtmapWindow.txt" );
	controller->insertInRoom( -2, Coord( 0, 7 ), (GameActor*) lowWindow );

}

void setupRoom3( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	MoveableBox* mb = new MoveableBox( idGen->generateId(), "box" );
	controller->insertInRoom( -3, Coord( 9, 1 ), mb );

	Bones* b1 = new Bones( idGen->generateId(), "bones" );
	controller->insertInRoom( -3, Coord( 10, 8 ), b1 );
}

void setupRoom4( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	Explosive* exp1 = new Explosive( idGen->generateId(), "bomb" );
	controller->insertInRoom( -4, Coord( 1, 9 ), exp1 );
	Explosive* exp2 = new Explosive( idGen->generateId(), "bomb" );
	controller->insertInRoom( -4, Coord( 12, 1 ), exp2 );

	vector<GameActor*>* torches = new vector<GameActor*>();

	PuzzleRoomExit* puzzleDoor = new PuzzleRoomExit( idGen->generateId(), -3, Coord( 13, 1 ), false,
			torches, "The room shudders as the great door slides through the floor." );

	puzzleDoor->setInteractMessage(
			"The imposing iron door bears the seal of the royal family above the motto, \"IN MARGARINE WE TRUST\"" );

	for (int i = 0; i < 9; i++) {
		torches->push_back(
				new PuzzleTorch( idGen->generateId(), "brazier", puzzleDoor, false, true ) );
	}

	static_cast<PuzzleTorch*>( torches->at( 1 ) )->setShouldBeLit( false );

	controller->insertInRoom( -4, Coord( 4, 2 ), torches->at( 0 ) );
	controller->insertInRoom( -4, Coord( 7, 2 ), torches->at( 1 ) );
	controller->insertInRoom( -4, Coord( 10, 2 ), torches->at( 2 ) );
	controller->insertInRoom( -4, Coord( 4, 5 ), torches->at( 3 ) );
	controller->insertInRoom( -4, Coord( 7, 5 ), torches->at( 4 ) );
	controller->insertInRoom( -4, Coord( 10, 5 ), torches->at( 5 ) );
	controller->insertInRoom( -4, Coord( 4, 8 ), torches->at( 6 ) );
	controller->insertInRoom( -4, Coord( 7, 8 ), torches->at( 7 ) );
	controller->insertInRoom( -4, Coord( 10, 8 ), torches->at( 8 ) );

	controller->insertInRoom( -4, Coord( 0, 1 ), puzzleDoor );

}

void setupRoom5( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	RoomExit* secretExit1 = new RoomExit( idGen->generateId(), -6, Coord( 1, 4 ), true );
	secretExit1->setTxtMap( "txtmapAfterBombWall.txt" );
	DestructibleWall* dw = new DestructibleWall( idGen->generateId(), "flimsy wall", secretExit1 );
	controller->insertInRoom( -5, Coord( 14, 4 ), dw );

	WineBarrel* wbLeft = new WineBarrel( idGen->generateId(), "Chateau Stinquee, 1747" );
	wbLeft->setTxtMap( "txtmapWineLeft.txt" );
	controller->insertInRoom( -5, Coord( 10, 2 ), wbLeft );
	WineBarrel* wbRight = new WineBarrel( idGen->generateId(), "Blortwaug Estates, 1802" );
	wbRight->setTxtMap( "txtmapWineRight.txt" );
	controller->insertInRoom( -5, Coord( 11, 2 ), wbRight );
}

void setupRoom6( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	Bones* b = new Bones( idGen->generateId(), "bones" );
	controller->insertInRoom( -6, Coord( 2, 1 ), b );

}

void setupRoom7( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	vector<Coord> l( {
			Coord( 9, 3 ),
			Coord( 10, 3 ),
			Coord( 9, 4 ),
			Coord( 10, 4 ),
			Coord( 9, 5 ),
			Coord( 10, 5 ) } );
	Bridge* b1 = new Bridge( idGen->generateId(), "bridge" );
	Bridge* b2 = new Bridge( idGen->generateId(), "bridge" );
	b1->setTxtMap( "txtmapBridgeUpper.txt" );
	// lower is default image
	vector<GameActor*> ga( {
			(GameActor*) b1,
			(GameActor*) b1,
			(GameActor*) 0,
			(GameActor*) 0,
			(GameActor*) b2,
			(GameActor*) b2 } );
	vector<string> t(
			{ string( "Margarine: Production and Uses" ), string(
					"A Comprehensive History of Food Oils" ), string(
					"From Butter to Better: Bridging the Gap" ) } );

	Bookshelf* b = new Bookshelf( idGen->generateId(), "bookshelf", t, 2, "Here's a bookshelf", -10,
			l, ga );
	controller->insertInRoom( -7, Coord( 1, 1 ), b );

	Shelf* shelf1 = new Shelf( idGen->generateId(), "shelf" );
	Shelf* shelf2 = new Shelf( idGen->generateId(), "shelf" );
	Shelf* shelf3 = new Shelf( idGen->generateId(), "shelf" );
	Shelf* shelf4 = new Shelf( idGen->generateId(), "shelf" );
	Shelf* shelf5 = new Shelf( idGen->generateId(), "shelf" );
	shelf1->setInteractMessage(
			"The bookshelf is crammed with ancient crumbling books. You open one,"
					" whose pages promptly turn to dust as you touch them." );
	shelf2->setInteractMessage(
			"The books on this shelf are badly damaged. You don't think there's "
					"anything worth reading here. Then again, you never think anything's worth reading. "
					"So much for princely education." );
	shelf3->setInteractMessage(
			"The bookshelf is crammed with ancient crumbling books. You open one,"
					" whose pages promptly turn to dust as you touch them." );
	shelf4->setInteractMessage(
			"The books on this shelf are badly damaged. You don't think there's "
					"anything worth reading here. Then again, you never think anything's worth reading. "
					"So much for princely education." );
	shelf5->setInteractMessage(
			"You pick up a book and strike what you imagine to be an intellectual"
					" pose. But as you begin to run your eyes over the tiny lines, you start to feel"
					" extremely sleepy. And foolish, since there's nobody here to see you anyway." );
	controller->insertInRoom( -7, Coord( 10, 8 ), shelf1 );
	controller->insertInRoom( -7, Coord( 2, 4 ), shelf1 );
	controller->insertInRoom( -7, Coord( 1, 7 ), shelf2 );
	controller->insertInRoom( -7, Coord( 11, 3 ), shelf2 );
	controller->insertInRoom( -7, Coord( 5, 2 ), shelf3 );
	controller->insertInRoom( -7, Coord( 5, 5 ), shelf4 );
	controller->insertInRoom( -7, Coord( 13, 7 ), shelf5 );
}

void setupRoom8( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	vector<GameActor*>* statues = new vector<GameActor*>();
	PuzzleRoomExit* puzzleDoor = new PuzzleRoomExit( idGen->generateId(), -12, Coord( 7, 8 ), false,
			statues, "The room shudders as the great door slides through the floor." );

	statues->push_back(
			new RotatingStatue( idGen->generateId(), "LeftStatue", puzzleDoor, Left, Right ) );
	statues->push_back(
			new RotatingStatue( idGen->generateId(), "RightStatue", puzzleDoor, Right, Left ) );
	statues->push_back(
			new RotatingStatue( idGen->generateId(), "UpStatue", puzzleDoor, Up, Down ) );
	statues->push_back(
			new RotatingStatue( idGen->generateId(), "DownStatue", puzzleDoor, Down, Up ) );

	controller->insertInRoom( -8, Coord( 5, 4 ), statues->at( 0 ) );
	controller->insertInRoom( -8, Coord( 9, 4 ), statues->at( 1 ) );
	controller->insertInRoom( -8, Coord( 7, 2 ), statues->at( 2 ) );
	controller->insertInRoom( -8, Coord( 7, 6 ), statues->at( 3 ) );

	controller->insertInRoom( -8, Coord( 7, 0 ), puzzleDoor );
	RoomExit* secretExit2 = new RoomExit( idGen->generateId(), -7, Coord( 13, 4 ), true );
	secretExit2->setTxtMap( "txtmapAfterBombWall.txt" );
	DestructibleWall* dw2 = new DestructibleWall( idGen->generateId(), "second flimsy wall",
			secretExit2 );
	controller->insertInRoom( -8, Coord( 0, 4 ), dw2 );

}

void setupRoom9( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	Shelf* sh = new Shelf( idGen->generateId(), "shelf" );
	controller->insertInRoom( -9, Coord( 12, 8 ), sh );

	Table* tb = new Table( idGen->generateId(), "table" );
	controller->insertInRoom( -9, Coord( 2, 2 ), tb );

	Chair* ch1 = new Chair( idGen->generateId(), "chair" );
	Chair* ch2 = new Chair( idGen->generateId(), "chair" );
	controller->insertInRoom( -9, Coord( 2, 1 ), ch1 );
	controller->insertInRoom( -9, Coord( 3, 1 ), ch2 );

	Pot* p = new Pot( idGen->generateId(), "pot" );
	controller->insertInRoom( -9, Coord( 10, 7 ), p );
}

void setupRoom10( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	Bones* b = new Bones( idGen->generateId(), "bones" );
	controller->insertInRoom( -10, Coord( 2, 1 ), b );

}

void setupRoom11( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	Stairs* st = new Stairs( idGen->generateId(), -15, Coord( 7, 8 ) );
	st->addDescriptor( "staircase" );
	st->setInteractMessage( "A long narrow winding staircase leads up to the main castle." );
	controller->insertInRoom( -11, Coord( 7, 0 ), st );

	MoveableBox* mb1 = new MoveableBox( idGen->generateId(), "box" );
	MoveableBox* mb2 = new MoveableBox( idGen->generateId(), "box" );
	MoveableBox* mb3 = new MoveableBox( idGen->generateId(), "box" );
	controller->insertInRoom( -11, Coord( 4, 8 ), mb1 );
	controller->insertInRoom( -11, Coord( 7, 6 ), mb2 );
	controller->insertInRoom( -11, Coord( 12, 3 ), mb3 );
}

void setupRoom12( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	Broom* b1 = new Broom( idGen->generateId(), "broom" );
	controller->insertInRoom( -12, Coord( 9, 8 ), b1 );
	Broom* b2 = new Broom( idGen->generateId(), "broom" );
	controller->insertInRoom( -12, Coord( 9, 7 ), b2 );
}

void setupRoom13( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	vector<GameActor*>* statues = new vector<GameActor*>();
	PuzzleRoomExit* puzzleDoor = new PuzzleRoomExit( idGen->generateId(), -20, Coord( 1, 4 ), false,
			statues, "You hear a distant rumbling." );
	puzzleDoor->setAnimation( "" );
	puzzleDoor->setInteractMessage("The heavy iron door has no keyhole. It bears only the "
			"mysterious inscription: \"Konami\". You return to exploring the castle, pondering "
			"upon what black magic this strange word might conjure.");
	statues->push_back(
			new RotatingStatue( idGen->generateId(), "Left Statue", puzzleDoor, Down, Up ) );
	statues->push_back(
			new RotatingStatue( idGen->generateId(), "LeftCenter Statue", puzzleDoor, Right, Up ) );
	statues->push_back(
			new RotatingStatue( idGen->generateId(), "RightCenter Statue", puzzleDoor, Left,
					Down ) );
	statues->push_back(
			new RotatingStatue( idGen->generateId(), "Right Statue", puzzleDoor, Up, Down ) );

	controller->insertInRoom( -13, Coord( 2, 6 ), statues->at( 0 ) );
	controller->insertInRoom( -13, Coord( 5, 6 ), statues->at( 1 ) );
	controller->insertInRoom( -13, Coord( 9, 6 ), statues->at( 2 ) );
	controller->insertInRoom( -13, Coord( 12, 6 ), statues->at( 3 ) );

	controller->insertInRoom( -19, Coord( 14, 4 ), puzzleDoor );

}

void setupRoom14( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	MoveableBox* mb1 = new MoveableBox( idGen->generateId(), "box" );
	controller->insertInRoom( -14, Coord( 6, 3 ), mb1 );
	MoveableBox* mb2 = new MoveableBox( idGen->generateId(), "box" );
	controller->insertInRoom( -14, Coord( 7, 3 ), mb2 );

}

void setupRoom15( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	Stairs* st = new Stairs( idGen->generateId(), -18, Coord( 7, 8 ) );
	st->addDescriptor( "staircase" );
	st->setInteractMessage( "A grand staircase leads up to the next floor of the castle." );
	controller->insertInRoom( -15, Coord( 7, 0 ), st );

	Chair* c1 = new Chair( idGen->generateId(), "chair" );
	Chair* c2 = new Chair( idGen->generateId(), "chair" );
	Chair* c3 = new Chair( idGen->generateId(), "chair" );
	Chair* c4 = new Chair( idGen->generateId(), "chair" );
	Chair* c5 = new Chair( idGen->generateId(), "chair" );
	Chair* c6 = new Chair( idGen->generateId(), "chair" );
	Chair* c7 = new Chair( idGen->generateId(), "chair" );
	Chair* c8 = new Chair( idGen->generateId(), "chair" );

	controller->insertInRoom( -15, Coord( 2, 2 ), c1 );
	controller->insertInRoom( -15, Coord( 5, 2 ), c2 );
	controller->insertInRoom( -15, Coord( 9, 2 ), c3 );
	controller->insertInRoom( -15, Coord( 12, 2 ), c4 );
	controller->insertInRoom( -15, Coord( 2, 6 ), c5 );
	controller->insertInRoom( -15, Coord( 5, 6 ), c6 );
	controller->insertInRoom( -15, Coord( 9, 6 ), c7 );
	controller->insertInRoom( -15, Coord( 12, 6 ), c8 );

}

void setupRoom17( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	vector<GameActor*>* torches = new vector<GameActor*>();
	PuzzleRoomExit* puzzleDoor = new PuzzleRoomExit( idGen->generateId(), -19, Coord( 7, 8 ), false,
			torches, "You hear a distant rumbling." );
	puzzleDoor->setAnimation( "" );
	puzzleDoor->setInteractMessage(
			"The large iron door has no keyhole. It bears an inscription, though: \"We have brought light and truth to the four corners of the kingdom!\"" );

	torches->push_back( new PuzzleTorch( idGen->generateId(), "torch", puzzleDoor, false, true ) );
	torches->push_back( new PuzzleTorch( idGen->generateId(), "torch", puzzleDoor, false, true ) );
	torches->push_back( new PuzzleTorch( idGen->generateId(), "torch", puzzleDoor, false, true ) );
	torches->push_back( new PuzzleTorch( idGen->generateId(), "torch", puzzleDoor, false, true ) );

	controller->insertInRoom( -17, Coord( 1, 1 ), torches->at( 0 ) );
	controller->insertInRoom( -17, Coord( 13, 1 ), torches->at( 1 ) );
	controller->insertInRoom( -17, Coord( 1, 8 ), torches->at( 2 ) );
	controller->insertInRoom( -17, Coord( 13, 8 ), torches->at( 3 ) );

	controller->insertInRoom( -18, Coord( 7, 0 ), puzzleDoor );

}

void setupRoom18( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	MoveableBox* b1 = new MoveableBox( idGen->generateId(), "box" );
	controller->insertInRoom( -18, Coord( 11, 6 ), b1 );
}

void setupRoom19( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	Chair* c1 = new Chair( idGen->generateId(), "chair" );
	Chair* c2 = new Chair( idGen->generateId(), "chair" );
	Chair* c3 = new Chair( idGen->generateId(), "chair" );

	controller->insertInRoom( -19, Coord( 8, 3 ), c1 );
	controller->insertInRoom( -19, Coord( 9, 3 ), c2 );
	controller->insertInRoom( -19, Coord( 10, 3 ), c3 );

}

void setupRoom20( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	MoveableBox* mb1 = new MoveableBox( idGen->generateId(), "box" );
	MoveableBox* mb2 = new MoveableBox( idGen->generateId(), "box" );
	MoveableBox* mb3 = new MoveableBox( idGen->generateId(), "box" );
	MoveableBox* mb4 = new MoveableBox( idGen->generateId(), "box" );
	MoveableBox* mb5 = new MoveableBox( idGen->generateId(), "box" );
	MoveableBox* mb6 = new MoveableBox( idGen->generateId(), "box" );

	controller->insertInRoom( -20, Coord( 6, 3 ), mb1 );
	controller->insertInRoom( -20, Coord( 7, 3 ), mb2 );
	controller->insertInRoom( -20, Coord( 8, 3 ), mb3 );
	controller->insertInRoom( -20, Coord( 6, 5 ), mb4 );
	controller->insertInRoom( -20, Coord( 7, 5 ), mb5 );
	controller->insertInRoom( -20, Coord( 8, 5 ), mb6 );

	RoomExit* ex = new RoomExit( idGen->generateId(), -21, Coord( 1, 1 ), false );
	controller->insertInRoom( -20, Coord( 14, 8 ), (GameActor*) ex );

	Key* keyToExit = new Key( idGen->generateId(), "key", ex );

	// treasure_chest	-20	(7,4)	L	-16	(5,3)
	Container* c = new Container( idGen->generateId(), "chest", false );
	controller->insertInRoom( -20, Coord( 7, 4 ), (GameActor*) c );
	c->put( keyToExit );

	Key* keyToContainer = new Key( idGen->generateId(), "key", c );
	controller->insertInRoom( -16, Coord( 5, 3 ), keyToContainer );
}

void setupRoom21( Controller* controller ) {

}

void setupRoom22( Controller* controller ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );

	vector<GameActor*>* statues = new vector<GameActor*>();

	PuzzleRoomExit* endGoal = new PuzzleRoomExit( idGen->generateId(), -999, Coord( 0, 0 ), false,
			statues, "The ground shakes as the heavy doors slides open." );

	statues->push_back( new RotatingStatue( idGen->generateId(), "statue", endGoal, Left, Right ) );
	statues->push_back(
			new RotatingStatue( idGen->generateId(), "statue", endGoal, Right, Right ) );
	statues->push_back( new RotatingStatue( idGen->generateId(), "statue", endGoal, Up, Right ) );

	for (int i = 0; i < statues->size(); i++) {
		statues->at( i )->setInteractMessage(
				"The statue bears an inscription: \'Turn toward what you seek.\'" );
	}

	controller->insertInRoom( -22, Coord( 2, 2 ), statues->at( 0 ) );
	controller->insertInRoom( -22, Coord( 7, 6 ), statues->at( 1 ) );
	controller->insertInRoom( -22, Coord( 12, 9 ), statues->at( 2 ) );

	controller->insertInRoom( -22, Coord( 15, 4 ), endGoal );
	controller->insertInRoom( -22, Coord( 14, 4 ), endGoal );

	vector<Coord> l( {
			Coord( 9, 3 ),
			Coord( 10, 3 ),
			Coord( 9, 4 ),
			Coord( 10, 4 ),
			Coord( 9, 5 ),
			Coord( 10, 5 ) } );
	Bridge* b1 = new Bridge( idGen->generateId(), "bridge" );
	Bridge* b2 = new Bridge( idGen->generateId(), "bridge" );
	b1->setTxtMap( "txtmapBridgeUpper.txt" );
	// lower is default image
	vector<GameActor*> ga( {
			(GameActor*) b1,
			(GameActor*) b1,
			(GameActor*) 0,
			(GameActor*) 0,
			(GameActor*) b2,
			(GameActor*) b2 } );

	Switch* bridgeSwitch = new Switch( idGen->generateId(), "switch", false, -22, l, ga,
			"You hear a clanking sound from the direction of the gate." );
	controller->insertInRoom( -21, Coord( 14, 2 ), bridgeSwitch );

}

void setupGame( Controller* controller ) {
	controller->initNcurses();
	
	// load rooms, containers and doors
	string mapFilename = dataPath + "map.txt";
	ifstream mapFile;
	mapFile.open( mapFilename.c_str() );
	controller->createPlayerCharacter();
	controller->loadMap( mapFile, dataPath );
	ifstream cFile;
	string cFilename = dataPath + "containers.txt";
	cFile.open( cFilename.c_str() );
	controller->loadContainers( cFile );
	ifstream eFile;
	string eFilename = dataPath + "doors.txt";
	eFile.open( eFilename );
	controller->loadExits( eFile );
	controller->initGame();

	// add "special" objects
	setupRoom1( controller );
	setupRoom2( controller );
	setupRoom3( controller );
	setupRoom4( controller );
	setupRoom5( controller );
	setupRoom6( controller );
	setupRoom7( controller );
	setupRoom8( controller );
	setupRoom9( controller );
	setupRoom10( controller );
	setupRoom11( controller );
	setupRoom12( controller );
	setupRoom13( controller );
	setupRoom14( controller );
	setupRoom15( controller );
	setupRoom17( controller );
	setupRoom18( controller );
	setupRoom19( controller );
	setupRoom20( controller );
	setupRoom21( controller );
	setupRoom22( controller );
}

bool startScreen() {
	Controller* controller = Controller::ControllerClass();
	controller->showStartScreen();

	char c;
	do {
		c = getch();
	} while (c != '\n' && c != 'q');
	if (c == '\n') {
		return true;
	} else {
		return false;
	}
}

void playGame() {
	Controller* c = Controller::ControllerClass();
	Event startEvent( EventType::Restart, 0, 0 );
	Event* e = &startEvent;

	while (true) {
		switch (e->eventType) {
		case EventType::Restart:
			if (!startScreen()) {
				c->releaseScreen();
				exit( 0 );
			}
			setupGame( c );
			e->eventType = EventType::None;
			break;
		case EventType::Quit:
			return;
			break;
		default:
			e = c->cycle();
			break;
		}
	}
}

int main( int argc, char** argv ) {
	if (argc > 1) {
		dataPath = argv[1];
		cout << "Data Folder: " << dataPath << endl;
	}

	playGame();

	echo();
	curs_set( TRUE );
	endwin();
}

