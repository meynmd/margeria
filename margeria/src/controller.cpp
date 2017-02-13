// implementation of class Controller

#include "controller.h"
#include "params.h"
#include "gameExceptions.h"

#include <iostream>
#include <unistd.h>

#define BUFSIZE 1024

Controller* Controller::_controller = 0;
bool Controller::init = false;
bool Controller::inst = false;

Controller::Controller() {
	game = 0;
	gameMap = 0;
	playerCharacter = 0;
	interpreter = 0;
	eventQueue = deque<Event*>();
	gameConsoleOutput = "";
	attachedActor = 0;
	lastConsoleInput = "";
	returnEvent = new Event( EventType::None, 0, 0 );
	initNcurses();
}

Controller* Controller::ControllerClass() {
	if (!inst) {
		_controller = new Controller();
		inst = true;
	}
	return _controller;
}

void Controller::createPlayerCharacter() {
	if (playerCharacter) {
		delete playerCharacter;
		playerCharacter = 0;
	}
	playerCharacter = new PlayerCharacter( string( "hero" ), 100, 100, 200 );
	playerCharacter->setTxtMap( "txtmapHero.txt" );
}

bool Controller::initGame() {
	gameMap = loader.getMap();
	if (!playerCharacter || !gameMap) {
		return false;
	}
	if (game) {
		delete game;
	}
	game = new Game( playerCharacter, gameMap );
	interpreter = new Interpreter( game );
	init = true;

	io->displaySpecialMessage(
			"Awake, O prince! Your destiny awaits...if you can escape these walls.", 2, 5, 1 );

	eventQueue.push_back(
			new RelocatePlayerEvent( (GameActor*) playerCharacter, game->getCurrentRoom()->getId(),
					&game->getCurrentRoom()->spawnPoint ) );
	eventQueue.push_back( new Event( EventType::Redraw, 0, 0 ) );
	return true;
}

void Controller::initNcurses() {
	io = GameInputOutput::GameInputOutputClass();
	try {
		io->initDisplay();
	} catch (displaySizeException& sizeExcept) {
		fprintf( stderr, "\nThis game requires a minimum console window size of %d x %d.\n"
				"You are currently at %d x %d. Please resize window and restart.\n\n",
				MIN_GAMEWIN_WIDTH + STATSWIN_WIDTH + ROOM_DISP_MARGIN,
				MIN_GAMEWIN_HEIGHT + MIN_CONSOLE_LINES + ROOM_DISP_MARGIN, COLS, LINES );
		exit( 1 );
	}
	io->initInput();
}

void Controller::releaseScreen() {
	io->releaseDisplay();
}

void Controller::showStartScreen() {
	io->startScreen();
}

void Controller::showOpeningMessage( string s ) {
	io->displaySpecialMessage( s, 1, 4, 1 );
}

void Controller::printCurrentRoom() {
	if (init) {
		game->getCurrentRoom()->print();
	}
}

void Controller::draw() {
	if (init) {
		Room* r;
		if (game->getCurrentRoom()->hasLightSource) {
			r = game->getCurrentRoom();
			io->drawRoom( r );
		} else {
			r = game->getCurrentRoom();
			Coord pcLoc = playerCharacter->getLocationCoords();
			int ulX = pcLoc.x - playerCharacter->getLightRadius();
			int ulY = pcLoc.y - playerCharacter->getLightRadius();
			if (ulX < 0) {
				ulX = 0;
			}
			if (ulY < 0) {
				ulY = 0;
			}
			int lrX = pcLoc.x + playerCharacter->getLightRadius();
			int lrY = pcLoc.y + playerCharacter->getLightRadius();
			if (lrX > game->getCurrentRoom()->dimensions.x - 1) {
				lrX = game->getCurrentRoom()->dimensions.x - 1;
			}
			if (lrY > game->getCurrentRoom()->dimensions.y - 1) {
				lrY = game->getCurrentRoom()->dimensions.y - 1;
			}
			io->drawRoom( r, Coord( ulX, ulY ), Coord( lrX, lrY ) );
		}
		io->drawStatsWin( playerCharacter->getInventory() );
	}
}

void Controller::printOutputBuffer() {
	for (string s : outputBuffer) {
		io->writeConsole( s );
		io->writeConsole( "" );
	}
	outputBuffer.clear();
}

// in this implementation, a map file will be just a list of int IDs mapped to room file names
bool Controller::loadMap( ifstream& mapFile, string path ) {
	if (!mapFile) {
		throw(fileIoException( "Error reading map file\n" ));
	}
	loader = Loader();
	loader.loadMap( mapFile, path );
	if (gameMap == 0) {
		return false;
	} else {
		return true;
	}
}

bool Controller::loadContainers( ifstream& containerFile ) {
	if (!containerFile) {
		return false;
	}
	loader.loadContainers( containerFile );
	return true;
}

bool Controller::loadExits( ifstream& exitFile ) {
	if (!exitFile) {
		return false;
	}
	loader.loadExits( exitFile );
	return true;
}

Event* Controller::popEventQueue() {
	if (eventQueue.empty()) {
		// if there is no event waiting to be handled, get input
		if (DEBUG) {
			gameConsoleOutput = "Event Queue Empty\n";
		}
		return new Event( EventType::None, 0, 0 );
	}

	Event* topEvent = eventQueue.front();
	eventQueue.pop_front();
	if (!topEvent->description.empty()) {
		outputBuffer.push_back( topEvent->description + "\n" );
	}
	if (DEBUG) {
		gameConsoleOutput = topEvent->toString();
	}
	return topEvent;
}

// one cycle of game loop
Event* Controller::cycle() {
	Event* topEvent = popEventQueue();
	if (!game->getIsAlive()) {
		if (topEvent->eventType == EventType::Input) {
			InputEvent* e = static_cast<InputEvent*>( topEvent );
			e->key = tolower(e->key);
			switch (e->key) {
			case 'y':
				if (DEBUG) {
					game->setIsAlive( true );
					returnEvent->eventType = EventType::None;
				} else {
					returnEvent->eventType = EventType::Restart;
				}
				break;
			case 'n':
				io->releaseDisplay();
				returnEvent->eventType = EventType::Quit;
				break;
			default:
				returnEvent->eventType = EventType::None;
				break;
			}
		} else {
			eventQueue.push_back( getInput() );
		}
	} else {
		switch (topEvent->eventType) {

		case EventType::Restart:
			return topEvent;

		case EventType::Acquire:
			eventQueue.push_back( giveItem( static_cast<AcquireEvent*>( topEvent ) ) );
			//eventQueue.push_back( new Event( EventType::Redraw, 0, 0 ) );
			if (topEvent->subject->getType() == ActorType::PlayerCharacter) {
				eventQueue.push_back( new MoveEvent( playerCharacter, lastMove ) );
			}
			break;

		case EventType::Attach:
			attachedActor = topEvent->sender;
			playerCharacter->setAttachedActor( topEvent->sender );
			if (!topEvent->sender || !topEvent->subject) {
				break;
			}
			// undo if the player is not in directly front of the object
			if (topEvent->sender->getLocationCoords().x != topEvent->subject->getLocationCoords().x
					&& topEvent->sender->getLocationCoords().y
							!= topEvent->subject->getLocationCoords().y) {
				playerCharacter->setAttachedActor( 0 );
				eventQueue.push_back(
						new Event( EventType::Redraw, 0, 0, "You need to get closer." ) );
			}
			break;

		case EventType::CloseConsole:
			// this event may come in useful if we need to close the console
			break;

		case EventType::Collision:
			eventQueue.push_back( processCollision( topEvent ) );
			break;

		case EventType::Discover:
			eventQueue.push_back( discoverActors( static_cast<DiscoverEvent*>( topEvent ) ) );
			break;

		case EventType::EndGame:
			game->setIsAlive( false );
			outputBuffer.push_back( "GAME OVER" );
			if (DEBUG) {
				outputBuffer.push_back(
						"DEBUG set, so you can cheat! Do you want to continue? (y or n)" );
			} else {
				outputBuffer.push_back( "Do you want to play again? (y or n)" );
			}
			printOutputBuffer();
			eventQueue.push_back( getInput() );
			break;

		case EventType::Input:
			// single-key input (that is, arrow key) directed to processKeyInput()
			if ((static_cast<InputEvent*>( topEvent ))->inputType == InputType::Key) {
				eventQueue.push_back(
						processKeyInput( (static_cast<InputEvent*>( topEvent ))->key ) );
			}
			// console input
			else {
				eventQueue.push_back( processConsoleInput( static_cast<InputEvent*>( topEvent ) ) );
			}
			break;

		case EventType::Move:
			// Move event: cast event to Move type and pass movePlayer() the direction
			if (playerCharacter->getAttachedActor()) {
				eventQueue.push_back(
						moveAttached( static_cast<MoveEvent*>( topEvent )->direction ) );
				break;
			} else {
				lastMove = static_cast<MoveEvent*>( topEvent )->direction;
				eventQueue.push_back(
						game->movePlayer( (static_cast<MoveEvent*>( topEvent ))->direction,
								true ) );
				break;
			}

		case EventType::OpenConsole:
			if (topEvent->sender) {
				io->writeConsole( topEvent->sender->getInteractMessage() );
			}
			io->writeConsole( "What do you want to do?" );
			//eventQueue.push_back( new Event( EventType::Redraw, 0, 0 ) );
			eventQueue.push_back( getConsoleInput( topEvent->sender ) );
			break;

		case EventType::Redraw:
			draw();
			break;

		case EventType::PlayAnimation:
			eventQueue.push_back( playAnimation( static_cast<AnimationEvent*>( topEvent ) ) );
			break;

		case EventType::RelocatePlayer:
			// move the player to next room
			eventQueue.push_back( relocatePlayer( (RelocatePlayerEvent*) topEvent ) );
			break;

		case EventType::SwapActor:
			swapActor( static_cast<SwapEvent*>( topEvent ) );
			break;

		default:
			eventQueue.push_back( getInput() );
			break;
		}
	}
	if (topEvent) {
		delete topEvent;
	}
	printOutputBuffer();
	return returnEvent;
}

Event* Controller::discoverActors( DiscoverEvent* e ) {
	vector<GameActor*> toAdd = e->ActorsToAdd;
	for (GameActor* a : toAdd) {
		game->insertInRoomActorDictionary( a );
	}
	if (e->triggerConsole) {
		return new Event( EventType::OpenConsole, 0, 0 );
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}

Event* Controller::moveAttached( Direction d ) {
	Direction toObject = findDirection( playerCharacter->getLocationCoords(),
			playerCharacter->getAttachedActor()->getLocationCoords() );
	if (d == toObject) {
		// moving in same direction == pushing box; move box first
		if (game->moveActor( playerCharacter->getAttachedActor()->getLocationCoords(), d )->eventType
				!= EventType::Collision) {
			return game->movePlayer( d, true );
			//return new Event( EventType::Redraw, 0, 0 );
		} else {
			// if collision, detach object from player
			return new Event( EventType::Attach, 0, 0 );
		}
	} else if (d == reverse( toObject )) {
		// moving in opposite direction == pulling box; move player first
		Event* e = game->movePlayer( d, false );
		if (e->eventType != EventType::Collision) {
			game->moveActor( playerCharacter->getAttachedActor()->getLocationCoords(), d );
			return e;
			//return new Event( EventType::Redraw, 0, 0 );
		} else {
			// if collision, detach from player
			return new Event( EventType::Attach, 0, 0 );
		}
	} else {
		// can't pull box sideways; detach from player
		eventQueue.push_back( new Event( EventType::Attach, 0, 0 ) );
		return new MoveEvent( playerCharacter, d );
	}
}

Event* Controller::giveItem( AcquireEvent* e ) {
	if (e->item == 0) {
		return new Event( EventType::None, 0, 0 );
	}
	GameActor* item = e->item;
	if (e->subject->getType() != ActorType::Character
			&& e->subject->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::Redraw, 0, 0 );
	}
	Character* owner = static_cast<Character*>( e->subject );

	// give or take away the item
	if (e->acquiring) {
		owner->insertInventoryItem( item );
		Event* done = new Event( EventType::Redraw, 0, 0 );
		done->description = "*** " + item->getName() + " acquired." + " ***";
		return done;
	} else {
		if (owner->removeInventoryItem( item ) == 0) {
			return new Event( EventType::Redraw, 0, 0 );
		} else {
			Event* done = new Event( EventType::Redraw, 0, 0 );
			done->description = item->getName() + " removed.";
			return done;
		}
	}
}

Event* Controller::relocatePlayer( RelocatePlayerEvent* e ) {
	if (e->roomId >= 0) {
		if (DEBUG) {
			fprintf( stderr, "Error: RoomExit has bad ID: %d\n", e->roomId );
		}
		return new Event( EventType::None, 0, 0 );
	}
	//io->drawRoom(0);
	//usleep(500000);
	if (e->roomId == -999) {
		return finishGame();
	}
	Room* nextRoom = gameMap->getRoomById( e->roomId );
	if (e->roomCoords == 0) {
		game->setCurrentRoom( e->roomId, Coord( nextRoom->spawnPoint.x, nextRoom->spawnPoint.y ) );
	} else {
		game->setCurrentRoom( e->roomId, *(e->roomCoords) );
	}

	if (e->description.empty()) {
		io->clearConsole();
		outputBuffer.clear();
	}
	outputBuffer.push_back( nextRoom->getDescription() );
	return new Event( EventType::Redraw, 0, 0 );
}

Event* Controller::finishGame() {
	io->displaySpecialMessage(
			"Freedom at last! You escape into the countryside, hiding among your supporters in the surrounding mountains.",
			1, 5, 1 );
	int goldCount = playerCharacter->countInventoryItemByType( ActorType::Treasure );
	if (goldCount >= 2) {
		io->displaySpecialMessage(
				"With the gold you've reclaimed, you raise an army. In time, you return to the capital and reclaim your throne!",
				0, 7, 1 );
	} else {
		io->displaySpecialMessage(
				"Your few loyal supporters eventually grow weary and desert you. You flee to the neighboring kingdom of "
						"Lardonia and spend your remaining days in exile. If only you had managed to get more of your gold...",
				1, 7, 1 );
	}
	return new Event( EventType::Restart, 0, 0 );
}

// returns resulting Event from collision Event
Event* Controller::processCollision( Event* collision ) {
	Character* movingActor = static_cast<Character*>( collision->sender );
	GameActor* otherActor = collision->subject;

	// if the player character is not involved, no event required
	if (movingActor != playerCharacter && otherActor != playerCharacter) {
		return new Event( EventType::None, 0, 0 );
	}

	// if there is no interaction with this object, no event required
	if (!otherActor->getCanInteract()) {
		return new Event( EventType::None, 0, 0 );
	}

	// check whether combat is in order
	if (otherActor->getType() == ActorType::Character) {
		Character* otherCharacter = static_cast<Character*>( otherActor );
		if (!otherCharacter->getIsFriendly()) {
			return enterCombat( otherCharacter );
		}
	}

	return otherActor->onCollision( playerCharacter );
}

Event* Controller::enterCombat( Character* enemy ) {
	return new Event( EventType::None, 0, 0 );
}

// this method should return an InputEvent* (Key or Console, depending on what key was pressed)
Event* Controller::getInput() {
	char c = io->getKeyInput();
	if (c == ERR) {
		return new Event( EventType::None, 0, 0 );
	}
	return new InputEvent( static_cast<GameActor*>( game->getPlayerCharacter() ), c );
}

// get text input from the player
Event* Controller::getConsoleInput( GameActor* subj ) {
	string conIn = io->getConsoleInput();
	lastConsoleInput = conIn;
	return new InputEvent( static_cast<GameActor*>( game->getPlayerCharacter() ), subj, conIn );
}

Event* Controller::processConsoleInput( Event* ci ) {
	if (ci->eventType != EventType::Input) {
		throw(eventTypeException( "processConsoleInput expects an InputEvent\n" ));
	}
	InputEvent* inputEv = static_cast<InputEvent*>( ci );

	//Interpreter interpreter;
	interpreter->interpret( inputEv );
	return interpreter->execute();
}

Event* Controller::processKeyInput( char c ) {
	c = tolower(c);
	Direction d;
	switch (c) {
	case MVLEFT_1:
	case MVLEFT_2:
		d = Left;
		break;
	case MVRIGHT_1:
	case MVRIGHT_2:
		d = Right;
		break;
	case MVUP_1:
	case MVUP_2:
		d = Up;
		break;
	case MVDOWN_1:
	case MVDOWN_2:
		d = Down;
		break;
	case RELEASE_OBJ_1:
	case RELEASE_OBJ_2:
		return new Event( EventType::Attach, 0, 0 );
	case OPEN_CONSOLE:
		return new Event( EventType::OpenConsole, 0, 0 );
	case REPEAT:
		return new InputEvent( static_cast<GameActor*>( game->getPlayerCharacter() ), 0,
				lastConsoleInput );
	case QUIT:
		quitGame();
		break;
	default:
		return new Event( EventType::None, 0, 0 );
	}
	return new MoveEvent( static_cast<GameActor*>( game->getPlayerCharacter() ), d );
}

void Controller::quitGame() {
	outputBuffer.push_back( "Do you really want to exit? (Press y or n.)" );
	printOutputBuffer();
	InputEvent* e;
	do {
		e = (InputEvent*) getInput();
		e->key = tolower(e->key);
	} while (e->key != 'y' && e->key != 'n');
	if (e->key == 'y') {
		io->releaseDisplay();
		returnEvent->eventType = EventType::Quit;
	}
}

bool Controller::insertInCurrentRoom( Coord c, GameActor* a ) {
	return game->getCurrentRoom()->insertAt( c, a );
}

bool Controller::insertInRoom( int rId, Coord c, GameActor* a ) {
	if (!a) {
		return false;
	}
	return gameMap->getRoomById( rId )->insertAt( c, a );
}

bool Controller::removeFromRoom( int rId, Coord xy, GameActor* a ) {
	return gameMap->getRoomById( rId )->removeAt( xy, a );
}

GameActor* Controller::objectAt( int rId, Coord xy ) {
	return gameMap->getRoomById( rId )->objectAt( xy );
}

Event* Controller::playAnimation( AnimationEvent* e ) {
	Animation* a = e->animation;
	if (!a) {
		if (e->shakeRoom) {
			io->shakeRoom( game->getCurrentRoom(), e->frames, e->jitterSize );
		}
		return new Event( EventType::Redraw, 0, 0 );
	}
	Coord xy = e->location;
	bool forward = e->forward;
	GameActor* atXy = 0;
	if (e->shakeRoom) {
		atXy = game->getCurrentRoom()->objectAt( xy );
		char** tm = atXy->getTxtMap();
		if (e->txtmap) {
			atXy->setTxtMap( e->txtmap );
		} else {
			atXy->setTxtMap( "txtmapEmpty.txt" );
		}
		//game->getCurrentRoom()->removeAt(xy, atXy);
		io->shakeRoom( game->getCurrentRoom(), e->frames, e->jitterSize );
		//game->getCurrentRoom()->insertAt(xy, atXy);
		atXy->setTxtMap( tm );
	}
	if (forward) {
		return io->playAnimation( a, xy );
	} else {
		return io->playAnimationReverse( a, xy );
	}
}

Event* Controller::swapActor( SwapEvent* e ) {
	vector<Coord> coords = e->locations;
	vector<GameActor*> objects = e->actors;
	if (coords.size() != objects.size()) {
		return new Event( EventType::Failure, 0, 0,
				"Error: object count and location count don't match" );
	}
	for (int i = 0; i < coords.size(); i++) {
		GameActor* ga = objectAt( e->roomId, coords.at( i ) );
		if (ga)
			removeFromRoom( e->roomId, coords.at( i ), ga );
		insertInRoom( e->roomId, coords.at( i ), objects.at( i ) );
	}
	return new Event( EventType::Redraw, 0, 0 );
}

