// implementation of GameActor class

#include "gameActor.h"
#include "coord.h"
#include "gameMap.h"
#include "utilities/idGenerator.h"
#include "utilities/tokenizer.h"

void GameActor::setActionMap( map<Action, Event*> actionMap ) {
	actionEventMap = actionMap;
}

void GameActor::mapAction( Action a, Event* e ) {
	actionEventMap[a] = e;
}

void GameActor::addDescriptor( string d ) {
	descriptors.insert( d );
}

bool GameActor::hasDescriptor( string d ) {
	if (descriptors.find( d ) == descriptors.end()) {
		return false;
	} else {
		return true;
	}
}

set<string> GameActor::getDescriptors() {
	return descriptors;
}

Event* GameActor::getActionResult( Action a, GameActor* b ) {
	return new Event( EventType::Redraw, this, b,
			"You don't want to do that. Maybe you should move on..." );
}

bool GameActor::setTxtMap( string filename ) {
	string txtMapFilename = dataPath + filename;
	ifstream txtMapFile( txtMapFilename );
	int row = 0;
	if (txtMapFile) {
		char buf[1 + BLOCK_WIDTH];
		while (txtMapFile.getline( buf, 1 + BLOCK_WIDTH )) {
			strncpy( txtMap[row], buf, 1 + BLOCK_WIDTH );
			row++;
		}
		txtMapLoaded = true;
		return true;
	} else {
		return false;
	}
}

void GameActor::setTxtMap( char** byteArr ) {
	this->txtMap = byteArr;
}

void GameActor::setTxtMapAt( int x, int y, char c ) {
	txtMap[y][x] = c;
}

char** GameActor::getTxtMap() {
	if (txtMapLoaded) {
		return txtMap;
	} else {
		return 0;
	}
}

bool GameActor::setAnimation( string filename ) {
	if (filename.empty()) {
		this->animation = 0;
		return false;
	}
	ifstream animFile;
	animFile.open( (dataPath + filename).c_str() );
	animation = new Animation( animFile );
	if (animation) {
		return true;
	} else {
		return false;
	}
}

Animation* GameActor::getAnimation( Direction d ) {
	switch (d) {
	case Left:
		return moveLeft;
		break;
	case Right:
		return moveRight;
		break;
	case Up:
		return moveUp;
		break;
	case Down:
		return moveDown;
		break;
	default:
		return 0;
	}
}

RoomExit::RoomExit( int objId, int to, Coord spawn, bool unlocked ) :
		Lockable( objId, "exit" ) {
	GameActor::symbol = '=';
	GameActor::type = ActorType::RoomExit;
	GameActor::descriptors.insert( "door" );
	GameActor::canInteract = true;
	GameActor::interactMessage = "You encounter a door.";
	nextRoomId = to;
	spawnLocation = spawn;
	if (unlocked) {
		unlock();
	} else {
		lock();
	}
}

void RoomExit::lock() {
	isUnlocked = false;
	GameActor::interactMessage = "The door is locked.";
	GameActor::symbol = '#';
	GameActor::setTxtMap( "txtmapExitLocked.txt" );
}

void RoomExit::unlock() {
	isUnlocked = true;
	GameActor::interactMessage = "You encounter a door.";
	GameActor::symbol = '=';
	GameActor::setTxtMap( "txtmapExit.txt" );
}

Event* RoomExit::onCollision( GameActor* a ) {
	if (a->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, 0, 0 );
	} else {
		if (isUnlocked) {
			return new RelocatePlayerEvent( this, nextRoomId, &spawnLocation );
		} else {
			return new Event( EventType::OpenConsole, this, 0 );
		}
	}
}

Event* RoomExit::getActionResult( Action a, GameActor* b ) {
	if (a == Action::Open) {
		if (!isUnlocked) {
			Character* c = static_cast<Character*>( b );
			if (!key) {
				return new Event( EventType::Redraw, this, 0,
						"The door is locked, and you don't have a key for it." );
			}
			if (c->findInventoryItem( key->getId() )) {
				unlock();
				return new Event( EventType::Redraw, this, 0, "You unlock the door." );
			} else {
				return new Event( EventType::Redraw, this, 0,
						"You don't have the key to open this door." );
			}
		} else {
			unlock();
			return new Event( EventType::Redraw, this, 0 );
		}
	} else if (a == Action::Close) {
		lock();
		return new Event( EventType::Redraw, this, 0, "You lock the door." );
	} else {
		return new Event( EventType::Redraw, this, 0, "You're not sure how to do that." );
	}
}

Event* Stairs::getActionResult( Action a, GameActor* ga ) {
	if (!ga) {
		return new Event( EventType::None, 0, 0 );
	}
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, 0, 0 );
	}
	//PlayerCharacter* p = static_cast<PlayerCharacter*>( ga );
	if (a == Action::Climb || a == Action::Move) {
		return new RelocatePlayerEvent( this, nextRoomId, &spawnLocation );
	} else {
		return new Event( EventType::Redraw, this, 0,
				"You're not sure you understand what you want to do. Can you try phrasing it another way?" );
	}
}

Event* Stairs::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::OpenConsole, this, 0 );
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}

PuzzleRoomExit::PuzzleRoomExit(
								int objId,
								int to,
								Coord spawn,
								bool unlocked,
								vector<GameActor*>* puzzleObj,
								string solved ) :
		RoomExit( objId, to, spawn, false ) {
	puzzleObjects = puzzleObj;
	solvedMessage = solved;
	animated = true;
	GameActor::setTxtMap( "txtmapPuzzleExit.txt" );
	GameActor::setAnimation( "animDoorSlide.txt" );
}

Event* PuzzleRoomExit::update() {
	bool isSolved = true;
	for (GameActor* g : *puzzleObjects) {
		isSolved &= g->getIsSolved();
	}
	if (isSolved) {
		unlock();
		setTxtMap( "txtmapPuzzleExit.txt" );
		if (animated) {
			AnimationEvent* ae = new AnimationEvent( this->animation, this->location, true,
					solvedMessage );
			ae->shakeRoom = true;
			ae->frames = 45;
			ae->jitterSize = 1;
			for (int i = 0; i < BLOCK_HEIGHT; i++) {
				strncpy( ae->txtmap[i], this->txtMap[i], 1 + BLOCK_WIDTH );
			}
			setTxtMap( "txtmapEmpty.txt" );
			return ae;
		} else {
			setTxtMap( "txtmapEmpty.txt" );
			return new Event( EventType::Redraw, this, 0, solvedMessage );
		}

		//return e;
	} else {
		return new Event( EventType::Redraw, this, 0 );
	}
}

Event* PuzzleRoomExit::getActionResult( Action a, GameActor* b ) {
	if (a == Action::Open) {
		return new Event( EventType::Redraw, this, 0,
				"The door is locked, and you don't have a key for it. In fact, there "
						"doesn't even seem to be a keyhole. Perhaps there's some other "
						"mechanism by which it operates." );
	} else {
		return new Event( EventType::Redraw, this, 0, "You're not sure how to do that." );
	}
}

Window::Window( int objId, int to, Coord spawn ) :
		RoomExit( objId, to, spawn, false ) {
	GameActor::type = ActorType::Window;
	GameActor::name = "window";
	GameActor::interactMessage = "You see a window.";
	GameActor::setTxtMap( "txtmapWindow.txt" );
}

Event* Window::getActionResult( Action a, GameActor* b ) {
	if (a == Action::Open) {
		return new Event( EventType::Redraw, this, 0, "You can't quite reach the window." );
	} else if (a == Action::Climb) {
		return new Event( EventType::Redraw, this, 0,
				"You try to climb the wall, but you can't get up to the window; there's nothing to grab onto." );
	} else {
		return new Event( EventType::Redraw, this, 0, "You're not sure how to do that." );
	}
}

LowWindow::LowWindow( int objId, int to, Coord spawn ) :
		RoomExit( objId, to, spawn, false ) {
	GameActor::type = ActorType::Window;
	GameActor::name = "window";
	GameActor::interactMessage = "You see a window.";
	GameActor::setTxtMap( "txtmapWindow.txt" );
}

Event* LowWindow::getActionResult( Action a, GameActor* b ) {
	if (a == Action::Open || a == Action::Climb) {
		return new Event( EventType::Redraw, this, 0, "You can't quite reach the window." );
	} else if (a == Action::Climb) {
		return new Event( EventType::Redraw, this, 0,
				"You try to climb the wall, but you can't get up to the window; there's nothing to grab onto." );
	} else {
		return new Event( EventType::Redraw, this, 0, "You're not sure how to do that." );
	}
}

Container::Container( int objId, string objName, bool unlocked ) :
		Lockable( objId, objName ) {
	if (unlocked) {
		unlock();
	} else {
		lock();
	}
	Lockable::key = 0;
	canInteract = true;
	contents = 0;
	GameActor::symbol = 'C';
	GameActor::type = ActorType::Container;
	GameActor::interactMessage = "This container is locked.";
	GameActor::setTxtMap( "txtmapChest.txt" );
}

void Container::put( GameActor* a ) {
	contents = a;
}

void Container::lock() {
	isUnlocked = false;
	GameActor::interactMessage = "The chest is locked.";
}

void Container::unlock() {
	isUnlocked = true;
	GameActor::interactMessage = "The chest is unlocked.";
}

GameActor* Container::getContents() {
	return contents;
}

void Container::removeContents() {
	contents = 0;
}

Event* Container::getActionResult( Action a, GameActor* b ) {
	if (!b) {
		return new Event( EventType::None, 0, 0 );
	}
	if (b->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, 0, 0 );
	}
	Character* c = static_cast<Character*>( b );
	if (a == Action::Open) {
		if (isUnlocked) {
			if (contents == 0) {
				return new Event( EventType::Redraw, this, 0, "The chest is empty." );
			}
			GameActor* tmp = contents;
			contents = 0;
			return new AcquireEvent( b, tmp, true );
		} else {
			if (key != 0) {
				if (c->findInventoryItem( key->getId() )) {
					unlock();
					return new Event( EventType::OpenConsole, this, 0, "You open the chest." );
				}
			}
			return new Event( EventType::Redraw, this, 0,
					"You don't have the key to open this chest." );
		}
	} else if (a == Action::Take) {
		if (isUnlocked) {
			GameActor* tmp = contents;
			contents = 0;
			return new AcquireEvent( b, tmp, true );
		} else {
			return new Event( EventType::OpenConsole, this, b, "The chest is still locked." );
		}
	} else {
		return new Event( EventType::OpenConsole, this, b, "You're not sure what you mean." );
	}
}

Event* Container::onCollision( GameActor* a ) {
	if (a->getType() != ActorType::Character && a->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, 0, 0 );
	}
	if (isUnlocked && contents == 0) {
		return new Event( EventType::None, 0, 0 );
	} else {
		if (a->getType() == ActorType::PlayerCharacter) {
			return new Event( EventType::OpenConsole, this, 0 );
		} else {
			return new Event( EventType::None, 0, 0 );
		}
	}
}

Key::Key( int objId, string objName, Lockable* keyTo ) :
		GameActor( objId, objName ) {
	GameActor::canInteract = true;
	GameActor::symbol = 'F';
	GameActor::addDescriptor( "key" );
	to = keyTo;
	if (to) {
		to->setKey( this );
	}
	GameActor::setTxtMap( "txtmapKey.txt" );
}

Event* Key::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		this->room->removeAt( this->getLocationCoords(), this );
		return new AcquireEvent( a, static_cast<GameActor*>( this ), true );
	} else {
		return new Event( EventType::None, this, 0 );
	}
}

MoveableBox::MoveableBox( int id, string name ) :
		GameActor( id, name ) {
	GameActor::canInteract = true;
	GameActor::symbol = 'B';
	GameActor::name = "large crate";
	GameActor::addDescriptor( "box" );
	GameActor::addDescriptor( "crate" );
	GameActor::interactMessage =
			"This box is pretty heavy, but you might be able to push it if you tried.";
	GameActor::setTxtMap( "txtmapBox.txt" );

	// load move animations
	ifstream animFile;
	animFile.open( (dataPath + "animPlayerBoxMoveRight.txt").c_str() );
	Animation* r = new Animation( animFile );
	if (r) {
		GameActor::moveRight = r;
	}
	animFile.close();
	animFile.open( (dataPath + "animPlayerBoxMoveUp.txt").c_str() );
	Animation* u = new Animation( animFile );
	if (u) {
		GameActor::moveUp = u;
	}
	animFile.close();
	animFile.open( (dataPath + "animPlayerBoxMoveLeft.txt").c_str() );
	Animation* l = new Animation( animFile );
	if (l) {
		GameActor::moveLeft = l;
	}
	animFile.close();
	animFile.open( (dataPath + "animPlayerBoxMoveDown.txt").c_str() );
	Animation* d = new Animation( animFile );
	if (d) {
		GameActor::moveDown = d;
	}
	animFile.close();
}

Event* MoveableBox::getActionResult( Action a, GameActor* ga ) {
	if (!ga) {
		return new Event( EventType::None, 0, 0 );
	}
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, 0, 0 );
	}
	if (a == Action::Move) {
		return new Event( EventType::Attach, this, ga,
				"Move to push or pull " + this->getName()
						+ ". Press x or move in another direction to let go." );
	} else if (a == Action::Climb) {
		Coord thisLoc = this->getLocationCoords();
		for (int j = thisLoc.y - 1; j <= thisLoc.y + 1; j++) {
			for (int i = thisLoc.x - 1; i <= thisLoc.x + 1; i++) {
				if (i < 0 || i > room->dimensions.x || j < 0 || j > room->dimensions.y) {
					continue;
				}
				if (room->spaces[i][j].contents) {
					GameActor* ga = room->spaces[i][j].contents;
					Window* w = 0;
					if (ga->getType() == ActorType::Window) {
						w = static_cast<Window*>( ga );
						Coord* spawnLoc = new Coord( w->getSpawnLocation().x,
								w->getSpawnLocation().y );
						return new RelocatePlayerEvent( this, w->getNextRoomId(), spawnLoc,
								"You climb on top of the crate. Straining, you grab hold of the "
										"bars and shake them. Finally, the crumbling bricks crack and you "
										"pull the bars out, leaving a space just large enough for you to "
										"just barely squeeze through." );
					}
				}
			}
		}
		return new Event( EventType::Redraw, this, ga,
				"You climb on top of the crate. There's not much to see or do up there, so you climb down again." );
	} else {
		return new Event( EventType::Redraw, this, ga, "You can't do that!" );
	}
}
Event* MoveableBox::onCollision( GameActor* a ) {
	if (a->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, this, 0 );
	} else {
		return new Event( EventType::OpenConsole, this, 0 );
	}
}

DestructibleWall::DestructibleWall( int id, string name, GameActor* under ) :
		GameActor( id, name ) {
	GameActor::canInteract = true;
	GameActor::symbol = '~';
	GameActor::addDescriptor( "wall" );
	GameActor::setInteractMessage( "This wall feels a little flimsy." );
	GameActor::setTxtMap( "txtmapBeforeBombWall.txt" );
	GameActor::setAnimation( "animExplosion01.txt" );
	underneath = under;
}

Event* DestructibleWall::getActionResult( Action act, GameActor* b ) {
	if (!b) {
		return new Event( EventType::None, 0, 0 );
	}
	if (b->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, 0, 0 );
	}

	if (act == Action::Destroy) {
		Character* c = static_cast<Character*>( b );
		GameActor* item;
		if ((item = c->findInventoryItemByType( ActorType::Explosive ))) {
			AnimationEvent* ae =
					new AnimationEvent( this->animation, this->location, true,
							"In a thunderous explosion, the wall crumbles into a heap of dust and stone fragments." );
			ae->shakeRoom = true;
			ae->frames = 15;
			ae->jitterSize = 2;
			for (int i = 0; i < BLOCK_HEIGHT; i++) {
				strncpy( ae->txtmap[i], this->txtMap[i], 1 + BLOCK_WIDTH );
			}
			Coord thisLoc = this->getLocationCoords();
			room->removeAt( thisLoc, this );
			if (underneath) {
				room->insertAt( thisLoc, underneath );
			}
			c->removeInventoryItem( item );
			return ae;
		} else {
			return new Event( EventType::Redraw, this, b,
					"You don't have anything you can use for that." );
		}
	} else {
		return new Event( EventType::Redraw, this, b,
				"Why don't you try doing something else to the wall?" );
	}
}

Event* DestructibleWall::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::OpenConsole, this, 0 );
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}

Explosive::Explosive( int objId, string objName ) :
		GameActor( objId, objName ) {
	GameActor::canInteract = true;
	GameActor::symbol = 'X';
	GameActor::name = objName;
	GameActor::type = ActorType::Explosive;
	GameActor::setTxtMap( "txtmapBomb.txt" );
}

Event* Explosive::getActionResult( Action act, GameActor* b ) {
	if (!b) {
		return new Event( EventType::None, 0, 0 );
	}
	if (b->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, 0, 0 );
	}
	if (act == Action::Take) {
		this->room->removeAt( this->getLocationCoords(), this );
		return new AcquireEvent( b, static_cast<GameActor*>( this ), true );
	}
	return new Event( EventType::Redraw, this, b, "What do you want to use the bomb for?" );
}

Event* Explosive::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		this->room->removeAt( this->getLocationCoords(), this );
		return new AcquireEvent( a, static_cast<GameActor*>( this ), true );
	} else {
		return new Event( EventType::None, this, 0 );
	}
}

Event* Broom::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::Redraw, this, 0,
				"This is just a broom. It probably won't help you very much." );
	} else
		return new Event( EventType::None, 0, 0 );
}

Event* Bones::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::Redraw, this, 0,
				"It looks like a pile of bones. Maybe best you didn't touch that." );
	} else
		return new Event( EventType::None, 0, 0 );
}

Event* Chair::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::Redraw, this, 0,
				"This is a chair. It's used for sitting, but now is not the time for sitting!" );
	} else
		return new Event( EventType::None, 0, 0 );
}

Event* Table::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::Redraw, this, 0,
				"This is a table. You don't notice anything interesting about it." );
	} else
		return new Event( EventType::None, 0, 0 );
}

Event* Shelf::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		//return new Event( EventType::Redraw, this, 0, "This shelf has mutliple strange ingredients on it. You notice some jars labelled \'Nightshade\', \'Eye of New\', \'Bezoar\', and \'Milk of the Poppy.\'" );
		if (!canInteract) {
			return new Event( EventType::Redraw, this, 0, interactMessage );
		} else {
			return new Event( EventType::OpenConsole, this, 0 );
		}
	} else
		return new Event( EventType::None, 0, 0 );
}

Event* Shelf::getActionResult( Action a, GameActor* ga ) {
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, this, 0 );
	}
	return new Event( EventType::Redraw, this, 0,
			"There doesn't seem to be anything here that can help you.");
}

Event* Fish::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::OpenConsole, this, 0 );
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}

Event* Fish::getActionResult( Action a, GameActor* ga ) {
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, this, 0 );
	}
	if (a == Action::Consume) {
		return new Event( EventType::Redraw, this, 0,
				"You choke down a mouthful of the rotting fish. And promptly choke it back up again." );
	} else {
		return new Event( EventType::Redraw, this, 0,
				"The fish smells quite awful. Maybe you should move on..." );
	}
}

Event* WineBarrel::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::OpenConsole, this, 0 );
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}

Event* WineBarrel::getActionResult( Action a, GameActor* ga ) {
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, this, 0 );
	}
	PlayerCharacter* pc = static_cast<PlayerCharacter*>( ga );
	if (a == Action::Consume) {
		float newDelay = 1.5 * pc->getMoveAnimationSpeed();
		pc->setMoveAnimationSpeed( (int) newDelay );
		return new Event( EventType::Redraw, this, 0,
				"You take a nice long drink out of the barrel." );
	} else {
		return new Event( EventType::Redraw, this, 0, "This is a fine wine! Don't do that to it." );
	}
}

Event* Pot::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::OpenConsole, this, 0 );
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}

Event* Pot::getActionResult( Action a, GameActor* ga ) {
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, this, 0 );
	}
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );
	PlayerCharacter* pc = static_cast<PlayerCharacter*>( ga );
	if (a == Action::Activate) {
		return new Event( EventType::Redraw, this, 0, "You stir the pot." );
	} else if (a == Action::Take) {
		pc->insertInventoryItem( new Food( idGen->generateId(), "stew" ) );
		return new Event( EventType::Redraw, this, ga, "You put some stew into a jar." );
	} else if (a == Action::Consume) {
		return new Event( EventType::Redraw, this, 0,
				"The stew's too hot to eat straight out of the pot. You grab a jar from "
						"the shelf. If you want, put some stew in there." );
	} else {
		return new Event( EventType::Redraw, this, 0, "You're not sure how to do that." );
	}
}

Event* Food::getActionResult( Action a, GameActor* ga ) {
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, this, 0 );
	}
	PlayerCharacter* pc = static_cast<PlayerCharacter*>( ga );
	if (a == Action::Consume) {
		pc->resetMoveAnimationSpeed();
		pc->removeInventoryItem( this );
		return new Event( EventType::Redraw, this, 0,
				"You open the jar and eat the stew. You feel reinvigorated." );
	} else {
		return new Event( EventType::Redraw, this, 0,
				"You're not sure whether you want to do that." );
	}
}

Candle::Candle( int objId, string objName, bool lit ) :
		Lightable( objId, objName, lit ) {
	GameActor::type = ActorType::LightSource;
	GameActor::canInteract = true;
	GameActor::symbol = 'i';
	GameActor::addDescriptor( "candle" );
	GameActor::setTxtMap( "txtmapCandle.txt" );
}

Event* Candle::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		this->room->removeAt( this->getLocationCoords(), this );
		PlayerCharacter* pc = static_cast<PlayerCharacter*>( a );
		if (pc->getLightRadius() < 1)
			pc->setLightRadius( 1 );
		return new AcquireEvent( a, static_cast<GameActor*>( this ), true );
	} else {
		return new Event( EventType::None, this, 0 );
	}
}

Torch::Torch( int objId, string objName, bool lit ) :
		Lightable( objId, objName, lit ) {
	GameActor::type = ActorType::LightSource;
	GameActor::canInteract = true;
	GameActor::symbol = 'T';
	GameActor::addDescriptor( "torch" );
	GameActor::setTxtMap( "txtmapTorch.txt" );
}

Event* Torch::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		this->room->removeAt( this->getLocationCoords(), this );
		AcquireEvent* e = new AcquireEvent( a, static_cast<GameActor*>( this ), true );
		e->description = "Hey! What's this? You have stumbled upon a torch lying on the ground. "
				"It feels a little wet, and it smells pretty bad, but you just might be able to "
				"LIGHT it if you had something to light it with...";
		return e;
	} else {
		return new Event( EventType::None, this, 0 );
	}
}

void Torch::light() {
	isLit = true;
	GameActor::interactMessage = "The torch casts a flickering orange glow on your surroundings.";
}

Event* Torch::getActionResult( Action a, GameActor* sender ) {
	if (!sender) {
		return new Event( EventType::None, 0, 0 );
	}
	if (sender->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, 0, 0 );
	}
	PlayerCharacter* p = static_cast<PlayerCharacter*>( sender );
	Lightable* l = 0;
	if (a == Action::Activate) {
		if ((l = static_cast<Lightable*>( p->findInventoryItemByType( ActorType::LightSource ) ))) {
			if (!l->getIsLit()) {
				return new Event( EventType::Redraw, this, 0,
						"The " + l->getName() + " is not lit. Try to find something already lit." );
			}
			light();
			p->setHasLight( true );
			p->setLightRadius( 4 );
			return new Event( EventType::Redraw, this, 0,
					"You light the torch with the " + l->getName() );
		} else {
			return new Event( EventType::Redraw, this, 0,
					"You don't have anything to use to light the torch, and you never learned in "
							"princely school how to light a fire. If you could just find something that's "
							"already lit..." );
		}
	} else if (a == Action::Take) {
		return new Event( EventType::Redraw, this, 0,
				"You think you see an object that looks like a torch lying on the ground just a "
						"few feet away." );
	} else {
		return new Event( EventType::Redraw, this, 0, "You want to do what??" );
	}
}

PuzzleTorch::PuzzleTorch(
							int objId,
							string objName,
							PuzzleRoomExit* controlObj,
							bool lit,
							bool solvedLit ) :
		GameActor( objId, objName ) {
	shouldBeLit = solvedLit;
	controlObject = controlObj;
	GameActor::canInteract = true;
	GameActor::symbol = 'T';
	GameActor::addDescriptor( "torch" );
	GameActor::addDescriptor( "brazier" );
	if (lit) {
		light();
	} else {
		unlight();
	}
	if (isLit == shouldBeLit) {
		GameActor::solved = true;
	} else {
		GameActor::solved = false;
	}
}

Event* PuzzleTorch::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::OpenConsole, this, 0 );
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}
Event* PuzzleTorch::getActionResult( Action a, GameActor* ga ) {
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, this, 0 );
	}
	PlayerCharacter* p = static_cast<PlayerCharacter*>( ga );
	Event* e;
	Lightable* l = 0;
	if (a == Action::Activate) {
		if ((l = static_cast<Lightable*>( p->findInventoryItemByType( ActorType::LightSource ) ))) {
			if (!l->getIsLit()) {
				return new Event( EventType::Redraw, this, 0,
						"The " + l->getName() + " is not lit. Try to find something already lit." );
			} else {
				light();
			}
		} else {
			return new Event( EventType::Redraw, this, 0,
					"You don't have anything to use to light the torch." );
		}
	} else if (a == Action::Deactivate) {
		unlight();
	} else {
		return new Event( EventType::Redraw, this, 0,
				"You're not sure what you mean. Can you try putting it a different way?" );
	}
	if (isLit == shouldBeLit) {
		GameActor::solved = true;
		e = controlObject->update();
	} else {
		GameActor::solved = false;
		e = new Event( EventType::Redraw, this, 0 );
	}
	if (e->description.empty()) {
		e->description = isLit ? "You light the " + this->name + "." : "You put out the " + this->name + ".";
	}
	return e;
}

void PuzzleTorch::light() {
	isLit = true;
	GameActor::interactMessage = "This torch is lit.";
	GameActor::setTxtMap( "txtmapLitTorch.txt" );
}

void PuzzleTorch::unlight() {
	isLit = false;
	GameActor::interactMessage = "This torch is not lit.";
	GameActor::setTxtMap( "txtmapUnlitTorch.txt" );
}

void PuzzleTorch::setShouldBeLit( bool l ) {
	shouldBeLit = l;
	if (isLit == shouldBeLit) {
		GameActor::solved = true;
	}
}

Pit::Pit( int objId, string objName ) :
		GameActor( objId, objName ) {
	for (int y = 0; y < BLOCK_HEIGHT; y++) {
		for (int x = 0; x < BLOCK_WIDTH; x++) {
			this->setTxtMapAt( x, y, 178 );
		}
		setTxtMapAt( BLOCK_WIDTH, y, '\0' );
	}
	GameActor::setTxtMap( "txtmapPit_3.txt" );
	GameActor::canInteract = true;
}

//	RelocatePlayerEvent( GameActor* s, int rId, Coord* rc, string text ) :

Event* Pit::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		if (a->getRoomId() < -11) {
			return new RelocatePlayerEvent( a, -11, new Coord( 7, 1 ),
					"You fall through the crumbling floor in a cloud of splinters. After tumbling some distance, you land with a thud on the floor below." );
		}
		return new Event( EventType::EndGame, 0, 0,
				"OH, NOOOOOOOO! You fell through the crumbling floor into a bottomless pit. Someone really needs to fix the floor in this game! (Sorry...)" );
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}

Direction RotatingStatue::orientations[] = { Up, Right, Down, Left };

string RotatingStatue::txtMaps[] = {
		"txtmapStatueUp.txt",
		"txtmapStatueRight.txt",
		"txtmapStatueDown.txt",
		"txtmapStatueLeft.txt" };

RotatingStatue::RotatingStatue(
								int objId,
								string objName,
								PuzzleRoomExit* controlObj,
								Direction initRotation,
								Direction solvedRotation ) :
		GameActor( objId, objName ) {
	orientation = initRotation;
	solvedOrientation = solvedRotation;
	controlObject = controlObj;
	if (orientation == solvedOrientation) {
		GameActor::solved = true;
	} else {
		GameActor::solved = false;
	}
	GameActor::canInteract = true;
	GameActor::symbol = '&';
	GameActor::addDescriptor( "statue" );
	GameActor::setInteractMessage(
			"The statue rests on a round pedestal. It creakily rotates on its base when you touch it." );
	GameActor::setTxtMap( txtMaps[orientation] );
}

Event* RotatingStatue::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::OpenConsole, this, 0 );
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}

Event* RotatingStatue::getActionResult( Action a, GameActor* ga ) {
	Event* e;
	if (a == Action::Move) {
		rotateRight();
		if (orientation == solvedOrientation) {
			GameActor::solved = true;
			e = controlObject->update();
		} else {
			GameActor::solved = false;
			e =
					new Event( EventType::Redraw, this, 0,
							"You strain to pull the statue around in its socket. Eventually, it clicks into place." );
		}
		if (e->description.empty()) {
			e->description =
					"You strain to pull the statue around in its socket. Eventually, it clicks into place.";
		}
		return e;
	} else {
		return new Event( EventType::Redraw, 0, 0, "You can't do that!" );
	}
}

void RotatingStatue::rotateRight() {
	int o = (1 + orientation) % 4;
	orientation = orientations[o];
	setTxtMap( txtMaps[o] );
}

void RotatingStatue::rotateLeft() {
	int o = (3 + orientation) % 4;
	orientation = orientations[o];
	setTxtMap( txtMaps[o] );
}

Book::Book(
			int objId,
			string objName,
			int targetRoomId,
			vector<Coord> loc,
			vector<GameActor*> obj,
			string message ) :
		Switch( objId, objName, false, targetRoomId, loc, obj, message ) {
}

Event* Book::getActionResult( Action a, GameActor* ga ) {
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, this, 0 );
	}
	if (a == Action::Take) {
		if (!activated) {
			int rId = targetRoomId;
			SwapEvent* e = new SwapEvent( rId, insertLoc, insertObj, activateMessage );
			e->shakeRoom = true;
			e->frames = 20;
			e->jitterSize = 1;
			return e;
			//return new SwapEvent( rId, insertLoc, insertObj, activateMessage );
		} else {
			return new Event( EventType::Redraw, 0, 0, "You already looked at this." );
		}
	} else {
		return new Event( EventType::Redraw, 0, 0,
				"Sorry, what did you want to do with this book?" );
	}
}

Switch::Switch(
				int objId,
				string objName,
				bool on,
				int targetRoomId,
				vector<Coord> loc,
				vector<GameActor*> obj,
				string message ) :
		GameActor( objId, objName ) {
	this->targetRoomId = targetRoomId;
	activated = on;
	insertObj = obj;
	insertLoc = loc;
	activateMessage = message;
	GameActor::canInteract = true;
	GameActor::setTxtMap( "txtmapSwitch.txt" );
	GameActor::interactMessage =
			"What does this switch do, you wonder? Only one way to find out...";
}

Event* Switch::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::OpenConsole, this, 0 );
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}

Event* Switch::getActionResult( Action a, GameActor* ga ) {
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, this, 0 );
	}
	if (a == Action::Activate || a == Action::Deactivate || a == Action::Move) {
		if (!activated) {
			int rId = targetRoomId;
			SwapEvent* e = new SwapEvent( rId, insertLoc, insertObj, activateMessage );
			e->shakeRoom = true;
			e->frames = 20;
			e->jitterSize = 1;
			//return new SwapEvent( rId, insertLoc, insertObj, activateMessage );
			return e;
		} else {
			return new Event( EventType::Redraw, 0, 0, "Nothing seems to happen." );
		}
	} else {
		return new Event( EventType::Redraw, 0, 0,
				"Can you explain what you want to do in another way?" );
	}
}

Bookshelf::Bookshelf(
						int objId,
						string objName,
						vector<string> titles,
						int solutionTitle,
						string message,
						int targetRoomId,
						vector<Coord> loc,
						vector<GameActor*> obj ) :
		GameActor( objId, objName ) {
	bookTitles = titles;
	if (solutionTitle < 0 || solutionTitle >= titles.size()) {
		solutionTitle = -1;
	}
	solutionIdx = solutionTitle;
	locations = loc;
	objects = obj;
	this->targetRoomId = targetRoomId;
	GameActor::addDescriptor( "shelf" );
	GameActor::addDescriptor( "bookcase" );
	GameActor::addDescriptor( "book" );
	GameActor::addDescriptor( "books" );
	GameActor::setTxtMap( "txtmapBookshelf2.txt" );
	GameActor::canInteract = true;
}

Event* Bookshelf::onCollision( GameActor* a ) {
	IdGenerator* idGen = IdGenerator::IdGeneratorClass( 1 );
	if (a->getType() == ActorType::PlayerCharacter) {
		vector<GameActor*> books;
		for (int i = 0; i < bookTitles.size(); i++) {
			if (i == solutionIdx) {
				Book* book = new Book( idGen->generateId(), this->bookTitles.at( i ), targetRoomId,
						locations, objects, "You pull on the book, but it won't come off "
								"the shelf. You do hear a distant rumbling sound, though." );
				for (string s : tokenize( bookTitles.at( i ), set<char>( { ' ' } ), false )) {
					book->addDescriptor( s );
				}
				books.push_back( book );
			} else {
				vector<GameActor*> emptyActors;
				vector<Coord> emptyCoords;

				Book* book = new Book( idGen->generateId(), this->bookTitles.at( i ), targetRoomId,
						emptyCoords, emptyActors, "You pull out the book, leaf through a "
								"few pages, yawn, and throw it on the floor. You were never much "
								"one for intellectual pursuits." );
				for (string s : tokenize( bookTitles.at( i ), set<char>( { ' ', ':', ',' } ), true )) {
					book->addDescriptor( s );
				}
				books.push_back( book );
			}
		}
		DiscoverEvent* e = new DiscoverEvent( books, true );
		e->description = "This shelf is full of interesting titles, to name a few: ";
		for (int i = 0; i < bookTitles.size(); i++) {
			e->description += bookTitles.at( i );
			if (i < bookTitles.size() - 1)
				e->description += ", ";
			else
				e->description += ".";
		}
		return e;
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}

Event* Bookshelf::getActionResult( Action a, GameActor* ga ) {
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, this, 0 );
	}
	if (a == Action::Observe || a == Action::Activate) {
		return new Event( EventType::Collision, ga, this );
	} else {
		return new Event( EventType::Redraw, this, 0,
				"What do you want to do with the bookshelf, again?" );
	}
}


Goal::Goal(
			int objId,
			int to,
			Coord spawn,
			bool unlocked,
			vector<GameActor*>* puzzleObj,
			string solved ) :
		PuzzleRoomExit( objId, to, spawn, unlocked, puzzleObj, solved ) {
	GameActor::interactMessage = "This is it! Only this door stands between you and your escape.";
}

Event* Goal::onCollision( GameActor* a ) {
	if (a->getType() == ActorType::PlayerCharacter) {
		return new Event( EventType::OpenConsole, this, 0 );
	} else {
		return new Event( EventType::None, 0, 0 );
	}
}

Event* Goal::getActionResult( Action a, GameActor* ga ) {
	if (ga->getType() != ActorType::PlayerCharacter) {
		return new Event( EventType::None, this, 0 );
	}
	if (a == Action::Open || a == Action::Activate) {
		return new Event( EventType::EndGame, this, ga );
	} else {
		return new Event( EventType::Redraw, this, 0, "You didn't understand what you just said." );
	}
}

Character::Character(
						int objId,
						string objName,
						int initHealth,
						int initStrength,
						int initMagic,
						bool friendly ) :
		GameActor( objId, objName ) {
	level = 1;
	health = initHealth;
	strength = initStrength;
	magic = initMagic;
	isFriendly = friendly;
	moveLeft = moveRight = moveUp = moveDown = 0;
	moveLeft = 0;

	inventory = set<GameActor*>();
	GameActor::symbol = 'C';
	GameActor::type = ActorType::Character;
	GameActor::canInteract = true;
}

int Character::alterHealth( int amount ) {
	if (health + amount >= 0) {
		health += amount;
		return amount;
	} else {
		int change = health;
		health = 0;
		return change;
	}
}

int Character::alterMagic( int amount ) {
	if (magic + amount >= 0) {
		magic += amount;
		return amount;
	} else {
		int change = magic;
		magic = 0;
		return change;
	}
}

int Character::countInventoryItemByType( ActorType t ) {
	int count = 0;
	for (auto i : inventory) {
		if (i->getType() == t) {
			count++;
		}
	}
	return count;
}

GameActor* Character::findInventoryItemByType( ActorType t ) {
	for (auto i : inventory) {
		if (i->getType() == t) {
			return i;
		}
	}
	return 0;
}

GameActor* Character::findInventoryItemByName( string s ) {
	for (auto i : inventory) {
		if (i->getName() == s) {
			return i;
		}
	}
	return 0;
}

GameActor* Character::findInventoryItem( int itemId ) {
	for (auto i : inventory) {
		if (i->getId() == itemId) {
			return i;
		}
	}
	return 0;
}

GameActor* Character::removeInventoryItem( GameActor* item ) {
	auto iItem = inventory.find( item );
	if (iItem == inventory.end()) {
		return 0;
	} else {
		GameActor* pItem = *iItem;
		inventory.erase( iItem );
		return pItem;
	}
}

PlayerCharacter::PlayerCharacter(
									string playerName,
									int initPlayerHealth,
									int initPlayerStrength,
									int initPlayerMagic ) :
		Character( 0, playerName, initPlayerHealth, initPlayerHealth, initPlayerMagic, true ) {
	GameActor::symbol = 'P';
	GameActor::type = ActorType::PlayerCharacter;
	attachedActor = 0;
	hasLight = false;
	lightRadius = 0;

	// load move animations
	ifstream animFile;
	animFile.open( (dataPath + "animHeroMoveRight4.txt").c_str() );
	Animation* r = new Animation( animFile );
	if (r) {
		GameActor::moveRight = r;
	}
	animFile.close();
	animFile.open( (dataPath + "animHeroMoveUp.txt").c_str() );
	Animation* u = new Animation( animFile );
	if (u) {
		GameActor::moveUp = u;
	}
	animFile.close();
	animFile.open( (dataPath + "animHeroMoveLeft4.txt").c_str() );
	Animation* l = new Animation( animFile );
	if (l) {
		GameActor::moveLeft = l;
	}
	animFile.close();
	animFile.open( (dataPath + "animHeroMoveDown.txt").c_str() );
	Animation* d = new Animation( animFile );
	if (d) {
		GameActor::moveDown = d;
	}
	animFile.close();
	defaultMoveAnimDelay = moveAnimDelay = GameActor::moveRight->frameDelay;
}

void PlayerCharacter::setMoveAnimationSpeed( int delay ) {
	if (delay >= 0 && delay <= 250000) {
		moveLeft->frameDelay = delay;
		moveRight->frameDelay = delay;
		moveUp->frameDelay = 2 * delay;
		moveDown->frameDelay = 2 * delay;
		moveAnimDelay = delay;
	}
}

void PlayerCharacter::resetMoveAnimationSpeed() {
	moveLeft->frameDelay = defaultMoveAnimDelay;
	moveRight->frameDelay = defaultMoveAnimDelay;
	moveUp->frameDelay = 2 * defaultMoveAnimDelay;
	moveDown->frameDelay = 2 * defaultMoveAnimDelay;
	moveAnimDelay = defaultMoveAnimDelay;
}
