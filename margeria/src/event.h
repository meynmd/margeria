/***
 struct Event and derived classes

 These structs are used by the Controller to determine what to do next
 ***/

#ifndef EVENT_H
#define EVENT_H

#include "animation.h"
#include "move.h"
#include "params.h"
#include <string>
#include <vector>

using namespace std;

class GameActor;
class Equipment;

/***
 enum class EventType

 the events that may be raised in the control loop

 Value:		Meaning:

 None		Empty event; may be ignored
 Input		Input received, needs to be processed. See InputEvent
 Move		Attempt to move an actor. See MoveEvent
 Collision	Moving GameActor has collided with GameActor subject
 EndGame	The game is over
 ***/

enum class EventType {
	None,			// no change in game state required
	Acquire,		// give/take item to/from character
	Attach,			// enter player-moving-object mode
	CloseConsole,	// should close the console
	Collision,		// collision detected between sender and subject
	Destroy,		// destroy sender
	Discover,		// add object to list of objects in room
	EndGame,		// game over
	Failure,		// indicates an attempted player action has failed
	Input,			// there's input to process
	Move,			// move sender in direction
	PlayAnimation,	// play an animation
	OpenConsole,	// need to interact with console
	Redraw,			// redraw screen
	RelocatePlayer,	// move the player character to another location
	Replace,		// destroy sender, replace with subject
	Restart,
	Success,		// indicates an attempted action has succeeded
	SwapActor,		// swap out whatever is in a space for a specific object
	Quit
};

/***
 enum class InputType

 Input is either a Key, which would be an arrow key indicating movement,
 or Console input, which needs to be interpreted
 ***/

enum class InputType {
	Key, Console
};

/***
 struct Event

 fields:
 EventType event			What type of event happened
 GameActor* sender		Who initiated the Event
 GameActor* subject		Other actor involved, if any

 ***/

struct Event {
public:
	EventType eventType;
	GameActor* sender;
	GameActor* subject;
	string description;
	bool shakeRoom;
	int frames;
	int jitterSize;

	Event( EventType e, GameActor* send, GameActor* subj ) :
			eventType( e ), sender( send ), subject( subj ), shakeRoom( false ), frames( 0 ), jitterSize(0) {
		description = "";
	}

	Event( EventType e, GameActor* send, GameActor* subj, string desc ) :
			eventType( e ), sender( send ), subject( subj ), description( desc ), shakeRoom(
					false ), frames( 0 ), jitterSize(0) {}

	string toString();
};

struct DiscoverEvent: public Event {
	vector<GameActor*> ActorsToAdd;
	bool triggerConsole;

	DiscoverEvent( vector<GameActor*> toAdd, bool console ) :
			Event( EventType::Discover, 0, 0 ) {
		ActorsToAdd = toAdd;
		triggerConsole = console;
	}
};

struct InputEvent: public Event {
	InputType inputType;
	char key;
	string text;

	// movement input constructor
	InputEvent( GameActor* s, char k ) :
			Event( EventType::Input, s, 0 ) {
		inputType = InputType::Key;
		key = k;
		text = "";
	}

	// console text input constructor
	InputEvent( GameActor* send, GameActor* subj, string t ) :
			Event( EventType::Input, send, subj ) {
		inputType = InputType::Console;
		key = ' ';
		text = t;
	}
};

struct MoveEvent: public Event {
	Direction direction;

	MoveEvent( GameActor* s, Direction d ) :
			Event( EventType::Move, s, 0 ) {
		direction = d;
	}
};

struct RelocatePlayerEvent: public Event {
	int roomId;
	Coord* roomCoords;

	RelocatePlayerEvent( GameActor* s, int rId, Coord* rc ) :
			Event( EventType::RelocatePlayer, s, 0 ) {
		roomId = rId;
		roomCoords = rc;
	}

	RelocatePlayerEvent( GameActor* s, int rId, Coord* rc, string text ) :
			Event( EventType::RelocatePlayer, s, 0 ) {
		roomId = rId;
		roomCoords = rc;
		Event::description = text;
	}
};

// character acquiring or losing item. The character is the subject of this event
struct AcquireEvent: public Event {
	GameActor* item;
	bool acquiring;    // false if item being taken away

	AcquireEvent( GameActor* owner, GameActor* obj, bool acq ) :
			Event( EventType::Acquire, 0, owner ) {
		item = obj;
		acquiring = acq;
	}
};

struct AnimationEvent: public Event {
	Animation* animation;
	Coord location;
	bool forward;
	char** txtmap;

	AnimationEvent( Animation* a, Coord l, bool f, string text ) :
			Event( EventType::PlayAnimation, 0, 0, text ) {
		animation = a;
		location = l;
		forward = f;
		shakeRoom = false;
		txtmap = new char*[BLOCK_HEIGHT];
		for (int i = 0; i < BLOCK_HEIGHT; i++) {
			txtmap[i] = new char[1 + BLOCK_WIDTH];
		}
	}
};

struct SwapEvent: public Event {
	vector<Coord> locations;
	vector<GameActor*> actors;
	int roomId;

	SwapEvent( int rId, vector<Coord> l, vector<GameActor*> a, string text ) :
			Event( EventType::SwapActor, 0, 0, text ) {
		roomId = rId;
		locations = l;
		actors = a;
	}
};

#endif

