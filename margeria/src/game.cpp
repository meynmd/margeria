// Game class implemented

#include "game.h"
#include "gameExceptions.h"
//#include "gameState.h"

Game::Game( PlayerCharacter* p, GameMap* m ) {
	mode = Exploration;
	isGameAlive = true;
	playerCharacter = p;
	gameActors = std::set<GameActor*>();
	gameActors.insert( playerCharacter );
	currentRoom = 0;
	setMap( m );
	idCounter = 1;
	levelRooms = vector<Room*>();
	idGen = IdGenerator::IdGeneratorClass( 1 );
}

void Game::setCurrentRoom( int rId ) {
	if (currentRoom) {
		currentRoom->removeAt( playerLocationCoord, (GameActor*) playerCharacter );
	}
	currentRoom = currentMap->getRoomById( rId );
	setCurrentRoom( rId, currentRoom->spawnPoint );
}

void Game::setCurrentRoom( int rId, Coord pLoc ) {
	if (currentRoom) {
		currentRoom->removeAt( playerLocationCoord, (GameActor*) playerCharacter );
	}
	currentRoom = currentMap->getRoomById( rId );
	playerLocationCoord = pLoc;
	if (!currentRoom->insertAt( playerLocationCoord, playerCharacter )) {
		throw(unableToSpawnException( "Can't insert player character at spawn point" ));
	}

	// populate the list of GameActor descriptors in current room, and map descriptors to objects
	roomActorDescriptors = set<string>();
	roomActorDictionary = map<string, GameActor*>();
	for (auto i = currentRoom->spaces.begin(); i != currentRoom->spaces.end(); i++) {
		for (auto j = i->begin(); j != i->end(); j++) {
			if (j->contents != 0) {
				// add GameActor's name
				roomActorDescriptors.insert( j->contents->getName() );
				// add each descriptor for this GameActor
				for (string s : j->contents->getDescriptors()) {
					roomActorDescriptors.insert( s );
				}
			}
		}
	}
}

void Game::insertInRoomActorDictionary( GameActor* a ) {
	string n = a->getName();
	for (int i = 0; i < n.length(); i++) {
		n[i] = tolower( n[i] );
	}
	roomActorDictionary[n] = a;
	for (string s : a->getDescriptors()) {
		for (int i = 0; i < s.length(); i++) {
			s[i] = tolower( s[i] );
		}
		roomActorDictionary[s] = a;
		roomActorDescriptors.insert(s);
	}

}

void Game::updateRoomActorDictionary() {
	roomActorDictionary = map<string, GameActor*>();
	// define a "box" to search for objects to add to the descriptors map
	int firstRow, lastRow, firstCol, lastCol, r = SEE_OBJECT_RADIUS;
	if (playerLocationCoord.y >= r) {
		firstRow = playerLocationCoord.y - r;
	} else {
		firstRow = 0;
	}
	if (playerLocationCoord.y < currentRoom->dimensions.y - r - 1) {
		lastRow = playerLocationCoord.y + r;
	} else {
		lastRow = currentRoom->dimensions.y - 1;
	}
	if (playerLocationCoord.x >= r) {
		firstCol = playerLocationCoord.x - r;
	} else {
		firstCol = 0;
	}
	if (playerLocationCoord.x < currentRoom->dimensions.x -r - 1) {
		lastCol = playerLocationCoord.x + r;
	} else {
		lastCol = currentRoom->dimensions.x - 1;
	}

	// scan that box for objects
	for (int j = firstRow; j <= lastRow; j++) {
		for (int i = firstCol; i <= lastCol; i++) {
			//Space sp = currentRoom->spaces.at(j).at(i);
			Space sp = currentRoom->spaces[i][j];

			if (sp.contents != 0) {
				// add GameActor
				roomActorDictionary[sp.contents->getName()] = sp.contents;
				// add each descriptor for this GameActor
				for (string st : sp.contents->getDescriptors()) {
					roomActorDictionary[st] = sp.contents;
				}
			}
		}
	}
}

void Game::setMap( GameMap* m ) {
	currentMap = m;
	setCurrentRoom( currentMap->getStartRoomId() );
}

// attempt to move the actor (located at from) in Direction d
Event* Game::moveActor( Coord from, Direction d ) {
	if (!currentRoom->isInBounds( from )) {
		return new Event( EventType::None, 0, 0 );
	}
	GameActor* actor = currentRoom->objectAt( from );
	Coord target = move( from, d );
	if (!currentRoom->isInBounds( target )) {
		return new Event( EventType::None, actor, 0 );
	}
	if (currentRoom->objectAt( target ) == 0) {
		currentRoom->removeAt( from, actor );
		currentRoom->insertAt( target, actor );
		actor->setLocationCoords( target );
		return new Event( EventType::Redraw, actor, 0 );
	} else {
		return new Event( EventType::Collision, actor, currentRoom->objectAt( target ) );
	}
}

Event* Game::movePlayer( Direction d, bool forward ) {
	Coord oldPlayerLocation = playerLocationCoord;
	Event* r = moveActor( playerLocationCoord, d );
	// if move is okay, will return Redraw event
	if (r->eventType == EventType::Redraw) {
		playerLocationCoord = move( playerLocationCoord, d );
		updateRoomActorDictionary();

		Animation* anim = 0;
		if (playerCharacter->getAttachedActor() == 0) {
			if ((anim = playerCharacter->getAnimation( d ))) {
				if (d == Right || d == Down) {
					return new AnimationEvent( anim, oldPlayerLocation, true, "" );
				} else {
					return new AnimationEvent( anim, playerLocationCoord, true, "" );
				}
			} else {
				return r;
			}
		} else {
			// player is moving an object around
			GameActor* aa = playerCharacter->getAttachedActor();
			if (forward) {
				if ((anim = aa->getAnimation( d ))) {
					if (d == Right || d == Down) {
						return new AnimationEvent( anim, oldPlayerLocation, true, "" );
					} else {
						return new AnimationEvent( anim, aa->getLocationCoords(), true, "" );
					}
				} else
					return r;
			} else {
				// moving backward, pulling object
				if ((anim = aa->getAnimation( reverse( d ) ))) {
					if (d == Right || d == Down) {
						return new AnimationEvent( anim, aa->getLocationCoords(), false, "" );
					} else {
						return new AnimationEvent( anim, playerCharacter->getLocationCoords(), false, "" );
					}
				}
			}
		}

	}
	return r;
}
