// class GameActor and the classes that inherit from GameActor

#ifndef GAMEACTOR_H
#define GAMEACTOR_H

#include "coord.h"
#include "event.h"
#include "action.h"
#include "animation.h"
#include "move.h"
#include "params.h"

#include <set>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>

using namespace std;

enum class ActorType {
	Base,
	Character,
	Container,
	Explosive,
	LightSource,
	PlayerCharacter,
	RoomExit,
	Treasure,
	RoomObject,
	Window
};

class Room;

/**************************************************************************************************
 GameActor (base class)
 **************************************************************************************************/

/***
 class GameActor

 base class for all objects and characters with which the player can interact in a room

 Fields:
 id			int						ID as assigned in constructor call
 name		string					name of the object
 type		ActorType				type of this GameActor
 actions	map<Action, Event*>		map associating available Actions with resulting Events

 Methods:
 void setActionMap(map<Action, Event*>)
 sets this GameActor's entire map of Actions to Events

 void mapAction(Action, Event*)
 maps an individual Action taken on this object to a resulting Event

 get/set methods:
 int getId()
 void setName(string name)
 string getName()
 ***/
class GameActor {
protected:
	const int id;
	ActorType type;
	string name;
	int roomId;
	Room* room;
	Coord location;
	set<string> descriptors;
	bool canInteract;
	bool solved;
	map<Action, Event*> actionEventMap;
	string interactMessage;
	char symbol;
	bool txtMapLoaded;
	char** txtMap;
	Animation* animation;
	Animation* moveLeft, *moveRight, *moveUp, *moveDown;

public:
	GameActor( int objId, string objName ) :
			id( objId ), name( objName ), symbol( ' ' ) {
		type = ActorType::Base;
		actionEventMap = map<Action, Event*>();
		descriptors = set<string>();
		canInteract = false;
		solved = false;
		interactMessage = "";
		roomId = 0;
		room = 0;
		location = Coord( 0, 0 );
		txtMapLoaded = false;
		txtMap = new char*[BLOCK_HEIGHT];
		for (int i = 0; i < BLOCK_HEIGHT; i++) {
			txtMap[i] = new char[1 + BLOCK_WIDTH];
		}
		animation = moveLeft = moveRight = moveUp = moveDown = 0;
	}

	~GameActor() {
		for (int i = 0; i < BLOCK_HEIGHT; i++) {
			delete[] txtMap[i];
		}
		delete[] txtMap;
	}

	void setTxtMapAt( int x, int y, char c );
	bool setTxtMap( string filename );
	void setTxtMap( char** byteArr );
	char** getTxtMap();

	bool setAnimation( string filename );
	Animation* getAnimation( Direction );

	int getId() {
		return id;
	}
	bool getIsSolved() {
		return solved;
	}
	void setName( string name ) {
		this->name = name;
	}
	string getName() {
		return name;
	}
	char getSymbol() {
		return symbol;
	}
	ActorType getType() {
		return type;
	}
	void setCanInteract(bool ia) {
		canInteract = ia;
	}
	bool getCanInteract() {
		return canInteract;
	}
	void setInteractMessage( string s ) {
		interactMessage = s;
	}
	string getInteractMessage() {
		return interactMessage;
	}
	void setRoomId( int rId ) {
		roomId = rId;
	}
	int getRoomId() {
		return roomId;
	}
	void setRoom( Room* r ) {
		room = r;
	}
	Room* getRoom() {
		return room;
	}
	void setLocationCoords( Coord xy ) {
		location = xy;
	}
	Coord getLocationCoords() {
		return location;
	}

	void addDescriptor( string );
	set<string> getDescriptors();
	bool hasDescriptor( string );
	void setActionMap( map<Action, Event*> );
	void mapAction( Action, Event* );
	virtual Event* getActionResult( Action, GameActor* );
	virtual Event* onCollision( GameActor* ) {
		return new Event( EventType::None, 0, 0 );
	}
	virtual Event* update() {
		return new Event( EventType::None, 0, 0 );
	}
};

/**************************************************************************************************
 Objects you'll see and maybe interact with in the game
 **************************************************************************************************/

/***
 class RoomObject

 Any inanimate object that is "part of" a Room. A RoomObject may not move or change any
 of its attributes during the game.
 ***/
class RoomObject: public GameActor {
private:

	// maybe some data about graphical representation?

public:
	RoomObject( int objId, string objName, char sym ) :
			GameActor( objId, objName ) {
		GameActor::symbol = sym;
		GameActor::type = ActorType::RoomObject;
		GameActor::canInteract = false;
	}
};

class Broom: public GameActor {
public:
	Broom( int objId, string objName ) :
			GameActor( objId, objName ) {
		GameActor::setTxtMap( "txtmapBroom.txt" );
		GameActor::canInteract = true;
		GameActor::addDescriptor( "broom" );
	}
	virtual Event* onCollision( GameActor* );

};

class Bones: public GameActor {
public:
	Bones( int objId, string objName ) :
			GameActor( objId, objName ) {
		GameActor::setTxtMap( "txtmapBones.txt" );
		GameActor::canInteract = true;
		GameActor::addDescriptor( "bones" );
	}
	virtual Event* onCollision( GameActor* );
};

class Table: public GameActor {
public:
	Table( int objId, string objName ) :
			GameActor( objId, objName ) {
		GameActor::setTxtMap( "txtmapTable.txt" );
		GameActor::canInteract = true;
		GameActor::addDescriptor( "table" );
	}
	virtual Event* onCollision( GameActor* );
};

class Chair: public GameActor {
public:
	Chair( int objId, string objName ) :
			GameActor( objId, objName ) {
		GameActor::setTxtMap( "txtmapChair.txt" );
		GameActor::canInteract = true;
		GameActor::addDescriptor( "chair" );
	}
	virtual Event* onCollision( GameActor* );
};

class Shelf: public GameActor {
public:
	Shelf( int objId, string objName ) :
			GameActor( objId, objName ) {
		GameActor::setTxtMap( "txtmapShelf.txt" );
		GameActor::canInteract = true;
		GameActor::addDescriptor( "shelf" );
		GameActor::interactMessage =
				"This shelf has mutliple strange ingredients on it. You notice some jars labelled"
				" \'Nightshade\', \'Eye of New\', \'Bezoar\', and \'Milk of the Poppy.\'";
	}
	virtual Event* onCollision( GameActor* );
	virtual Event* getActionResult( Action, GameActor* );
};

class Fish: public GameActor {
public:
	Fish( int objId, string objName ) :
			GameActor( objId, objName ) {
		GameActor::setTxtMap( "txtmapDeadFish.txt" );
		GameActor::canInteract = true;
		GameActor::addDescriptor( "fish" );
		GameActor::interactMessage = "You pass by a pile of foul-smelling fish.";
	}
	virtual Event* getActionResult( Action, GameActor* );
	virtual Event* onCollision( GameActor* );
};

class WineBarrel: public GameActor {
public:
	WineBarrel( int objId, string objName ) :
			GameActor( objId, objName ) {
		GameActor::canInteract = true;
		GameActor::addDescriptor( "wine barrel" );
		GameActor::addDescriptor( "wine" );
		GameActor::addDescriptor( "barrel" );
		GameActor::interactMessage =
				"This is a barrel from your personal collection of wines fit for a prince. You "
						"can't quite read the label in this light; maybe you'd better have a taste to see "
						"which one it is...";
		//GameActor::setTxtMap( "WineBarrelLeft" );
	}

	virtual Event* getActionResult( Action, GameActor* );
	virtual Event* onCollision( GameActor* );
};

class Pot: public GameActor {
public:
	Pot( int objId, string objName ) :
			GameActor( objId, objName ) {
		GameActor::setTxtMap( "txtmapPot.txt" );
		GameActor::canInteract = true;
		GameActor::addDescriptor( "pot" );
		GameActor::addDescriptor( "food" );
		GameActor::addDescriptor( "stew" );
		GameActor::interactMessage =
				"Hmm. It looks like someone has been making dinner here. Luckily they seem to have "
						"stepped out for a bit while their stew cooks over the fire.";
	}

	virtual Event* getActionResult( Action, GameActor* );
	virtual Event* onCollision( GameActor* );
};

class Food: public GameActor {
public:
	Food( int objId, string objName ) :
			GameActor( objId, objName ) {
		GameActor::canInteract = true;
		GameActor::addDescriptor( "food" );
	}

	virtual Event* getActionResult( Action, GameActor* );
};

class Lockable;

class Key: public GameActor {
private:
	Lockable* to;

public:
	Key( int objId, string objName, Lockable* keyTo );
	virtual Event* onCollision( GameActor* );
};

class Lockable: public GameActor {
protected:
	bool isUnlocked;
	Key* key;

public:
	Lockable( int objId, string objName ) :
			GameActor( objId, objName ) {
	}

	void setKey( Key* k ) {
		key = k;
	}
	Key* getKey() {
		return key;
	}
	virtual void lock() {
		isUnlocked = false;
	}
	virtual void unlock() {
		isUnlocked = true;
	}
};

class RoomExit: public Lockable {
protected:
	int nextRoomId;
	Coord spawnLocation;

public:
	RoomExit( int objId, int to, Coord spawn, bool unlocked );
	Coord getSpawnLocation() {
		return spawnLocation;
	}
	int getNextRoomId() {
		return nextRoomId;
	}
	bool getIsUnlocked() {
		return isUnlocked;
	}
	// void setIsUnlocked( bool ul );
	virtual void lock();
	virtual void unlock();
	virtual Event* getActionResult( Action, GameActor* );
	virtual Event* onCollision( GameActor* );
};

class Stairs: public RoomExit {
public:
	Stairs( int objId, int to, Coord spawn ) :
			RoomExit( objId, to, spawn, true ) {
		GameActor::setTxtMap( "txtmapStairs.txt" );
		GameActor::name = "stairs";
		GameActor::interactMessage = "You encounter stairs.";
	}

	virtual Event* getActionResult( Action, GameActor* );
	virtual Event* onCollision( GameActor* );
};

class PuzzleRoomExit: public RoomExit {
private:
	vector<GameActor*>* puzzleObjects;
	string solvedMessage;
	bool animated;

public:
	PuzzleRoomExit(
					int objId,
					int to,
					Coord spawn,
					bool unlocked,
					vector<GameActor*>* puzzleObj,
					string solved );

	bool getAnimated() {
		return animated;
	}
	void setAnimated( bool a ) {
		animated = a;
	}
	virtual Event* update();
	virtual Event* getActionResult( Action a, GameActor* b );
};

class Window: public RoomExit {
public:
	Window( int objId, int to, Coord spawn );

	virtual Event* getActionResult( Action, GameActor* );
};

class LowWindow: public RoomExit {
public:
	LowWindow( int objId, int to, Coord spawn );

	virtual Event* getActionResult( Action, GameActor* );
};

class Container: public Lockable {
private:
	GameActor* contents;

public:
	Container( int objId, string objName, bool unlocked );

	virtual void lock();
	virtual void unlock();
	void put( GameActor* );
	GameActor* getContents();
	void removeContents();
	virtual Event* getActionResult( Action, GameActor* );
	virtual Event* onCollision( GameActor* );
};

class MoveableBox: public GameActor {
public:
	MoveableBox( int id, string name );

	virtual Event* getActionResult( Action, GameActor* );
	virtual Event* onCollision( GameActor* );
};

class DestructibleWall: public GameActor {
private:
	GameActor* underneath;

public:
	DestructibleWall( int id, string name, GameActor* under );

	virtual Event* getActionResult( Action, GameActor* );
	virtual Event* onCollision( GameActor* );
};

class Explosive: public GameActor {
public:
	Explosive( int objId, string objName );

	virtual Event* getActionResult( Action, GameActor* );
	virtual Event* onCollision( GameActor* );
};

class Lightable: public GameActor {
protected:
	bool isLit;

public:
	Lightable( int objId, string objName, bool lit ) :
			GameActor( objId, objName ) {
		isLit = lit;
	}

	bool getIsLit() {
		return isLit;
	}
};

class Candle: public Lightable {
private:
	bool isLit;
public:
	Candle( int objId, string objName, bool lit );

	virtual Event* onCollision( GameActor* );
};

class Torch: public Lightable {
private:
	bool isLit;

public:
	Torch( int objId, string objName, bool lit );

	virtual Event* onCollision( GameActor* );
	virtual Event* getActionResult( Action, GameActor* );
	bool getIsLit() {
		return isLit;
	}
	void light();
};

class PuzzleTorch: public GameActor {
private:
	bool isLit;
	bool shouldBeLit;
	PuzzleRoomExit* controlObject;

public:
	PuzzleTorch( int objId, string objName, PuzzleRoomExit* controlObj, bool lit, bool solvedLit );

	virtual Event* onCollision( GameActor* );
	virtual Event* getActionResult( Action, GameActor* );
	bool getIsLit() {
		return isLit;
	}
	void light();
	void unlight();
	void setShouldBeLit( bool l );
	bool getShouldBeLit() {
		return shouldBeLit;
	}
};

class Pit: public GameActor {
public:
	Pit( int objId, string objName );

	virtual Event* onCollision( GameActor* );
};

class Bridge: public GameActor {
public:
	Bridge( int objId, string objName ) :
			GameActor( objId, objName ) {
		GameActor::setTxtMap( "txtmapBridgeLower.txt" );
	}
};

class Switch: public GameActor {
protected:
	bool activated;
	int targetRoomId;
	string activateMessage;
	vector<Coord> insertLoc;
	vector<GameActor*> insertObj;

public:
	Switch(
			int objId,
			string objName,
			bool on,
			int targetRoomId,
			vector<Coord> loc,
			vector<GameActor*> obj,
			string message );

	virtual Event* onCollision( GameActor* );
	virtual Event* getActionResult( Action, GameActor* );
};

/*
 class Bookshelf: Switch {
 private:
 vector<string> bookTitles;
 int solutionIdx;

 public:
 Bookshelf(
 int objId,
 string objName,
 int targetRoomId,
 vector<Coord> loc,
 vector<GameActor*> obj,
 vector<string> titles,
 int solutionTitle,
 string message );

 virtual Event* onCollision( GameActor* );
 virtual Event* getActionResult( Action, GameActor* );
 };
 */

class Bookshelf: public GameActor {
private:
	vector<string> bookTitles;
	int solutionIdx;
	vector<Coord> locations;
	vector<GameActor*> objects;
	int targetRoomId;

public:
	Bookshelf(
				int objId,
				string objName,
				vector<string> titles,
				int solutionTitle,
				string message,
				int targetRoomId,
				vector<Coord> loc,
				vector<GameActor*> obj );

	virtual Event* onCollision( GameActor* );
	virtual Event* getActionResult( Action, GameActor* );
};

class Book: public Switch {
public:
	Book(
			int objId,
			string objName,
			int targetRoomId,
			vector<Coord> loc,
			vector<GameActor*> obj,
			string message );

	virtual Event* getActionResult( Action, GameActor* );
};

class RotatingStatue: public GameActor {
private:
	Direction orientation;
	Direction solvedOrientation;
	PuzzleRoomExit* controlObject;
	static Direction orientations[];
	static string txtMaps[4];

public:
	RotatingStatue(
					int objId,
					string objName,
					PuzzleRoomExit* controlObj,
					Direction initRotation,
					Direction solvedRotation );
	void rotateRight();
	void rotateLeft();
	virtual Event* onCollision( GameActor* );
	virtual Event* getActionResult( Action, GameActor* );
};

class Goal: public PuzzleRoomExit {
public:
	Goal(
			int objId,
			int to,
			Coord spawn,
			bool unlocked,
			vector<GameActor*>* puzzleObj,
			string solved );

	virtual Event* onCollision( GameActor* );
	virtual Event* getActionResult( Action, GameActor* );
};

class Gold: public GameActor {
public:
	Gold( int objId, string objName ) :
			GameActor( objId, objName ) {
		GameActor::type = ActorType::Treasure;
		GameActor::canInteract = true;
		GameActor::addDescriptor( "gold" );
		GameActor::addDescriptor( "bar" );
	}
};

/**************************************************************************************************
 Character and PlayerCharacter
 **************************************************************************************************/

/***
 class Character

 game object with capabilities for interaction, fighting, etc.
 Each Character may have unique attribute values, so Characters
 must be placed in the game individually, not encoded into a room.

 Fields:
 level			int			level achieved by character
 health			int			amount of health the character has
 magic			int			amount of magic the character has

 Methods:
 int alterMagic( int amount )
 add amount points to character's magic

 int alterHealth( int amount )
 add amount points to character health

 int takeDamage( int damage )
 subtract amount points from character health

 bool isAlive()
 returns true if character health greater than 0, false otherwise

 getter/setter methods:
 int getHealth()
 int getMagic()
 int getLevel()
 void setHealth(int h)
 void setMagic( int m )
 void setLevel( int l )

 ***/
class Character: public GameActor {
protected:
	bool isFriendly;
	int level;
	int health;
	int strength;
	int magic;
	set<GameActor*> inventory;

public:
	Character(
				int objId,
				string objName,
				int initHealth,
				int initStrength,
				int initMagic,
				bool friendly );

	virtual ~Character() {
	}

	void setHealth( int h ) {
		health = h;
	}
	void setMagic( int m ) {
		magic = m;
	}
	void setLevel( int l ) {
		level = l;
	}
	void setFriendly( bool f ) {
		isFriendly = f;
	}
	int getHealth() {
		return health;
	}
	int getMagic() {
		return magic;
	}
	int getLevel() {
		return level;
	}
	bool getIsFriendly() {
		return isFriendly;
	}
	set<GameActor*> getInventory() {
		return inventory;
	}
	bool isAlive() {
		return health > 0;
	}

	int countInventoryItemByType( ActorType );

	GameActor* findInventoryItemByType( ActorType );

	GameActor* findInventoryItemByName( string );

	GameActor* findInventoryItem( int );

	GameActor* removeInventoryItem( GameActor* );

	void insertInventoryItem( GameActor* item ) {
		inventory.insert( item );
	}

	// add amount to character magic
	int alterMagic( int amount );

	// add amount to character health
	int alterHealth( int amount );

	// if character's health is damaged, subtract damage amount
	int takeDamage( int damage ) {
		return alterHealth( -damage );
	}
};

/***
 class PlayerCharacter

 the player character
 ***/
class PlayerCharacter: public Character {
private:
	GameActor* attachedActor;
	bool hasLight;
	int lightRadius;
	int moveAnimDelay, defaultMoveAnimDelay;

public:
	PlayerCharacter(
						string playerName,
						int initPlayerHealth,
						int initPlayerStrength,
						int initPlayerMagic );

	void setAttachedActor( GameActor* a ) {
		attachedActor = a;
	}
	GameActor* getAttachedActor() {
		return attachedActor;
	}
	void setHasLight( bool l ) {
		hasLight = l;
	}
	bool getHasLight() {
		return hasLight;
	}
	void setLightRadius( int r ) {
		lightRadius = r;
	}
	int getLightRadius() {
		return lightRadius;
	}
	void setMoveAnimationSpeed( int );
	int getMoveAnimationSpeed() {
		return moveAnimDelay;
	}
	void resetMoveAnimationSpeed();
};

class Equipment: public GameActor {
private:
	string description;
	int weight;
	int value;

public:
	Equipment( int itemId, string itemName ) :
			GameActor( itemId, itemName ) {
	}

	Equipment( int itemId, string itemName, int itemWeight, int itemValue ) :
			GameActor( itemId, itemName ) {
		weight = itemWeight;
		value = itemValue;
	}

	int getId() {
		return id;
	}
	void setName( string n ) {
		name = n;
	}
	string getName() {
		return name;
	}
	void setDescription( string d ) {
		description = d;
	}
	string getDescription() {
		return description;
	}
	void setWeight( int w ) {
		weight = w;
	}
	int getWeight() {
		return weight;
	}
	void setValue( int v ) {
		value = v;
	}
	int getValue() {
		return value;
	}
};

class Armor: public Equipment {
private:
	float protectFactor;

public:
	Armor( int itemId, string itemName, float armorFactor ) :
			Equipment( itemId, itemName ) {
		setProtectFactor( armorFactor );
	}

	void setProtectFactor( float factor );

	float getProtectFactor() {
		return protectFactor;
	}
};

class Weapon: public Equipment {
private:
	float damageMultiplier;		// default damage caused by weapon

public:
	Weapon( int itemId, string itemName, int weaponDamage ) :
			Equipment( itemId, itemName ) {
		damageMultiplier = weaponDamage;
	}

	int getDamage() {
		return damageMultiplier;
	}
	void setDamage( int amount ) {
		damageMultiplier = amount;
	}
};

#endif
