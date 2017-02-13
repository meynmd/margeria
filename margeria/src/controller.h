/***
Controller class

This class implements the mechanics of the game. It should be used to create a new game, to
monitor the game state, and to prompt, accept and process the player's input
***/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "game.h"
#include "event.h"
#include "gameInputOutput.h"
#include "loader.h"
#include "interpreter.h"

#include <fstream>
#include <deque>
#include <vector>
#include <unistd.h>

using namespace std;

/***
class Controller

This class interfaces with the Game class to create the mechanics of the game.

fields:
	Game* game							the game being controlled
	GameMap* gameMap					the game map
	PlayerCharacter* playerCharacter	the player character
	deque<Event*> eventQueue			queue of events this controller needs to process
	vector<string> outputBuffer			list of strings to be output to the console
	GameInputOutput* io					reference to GameInputOutput class
	Loader loader						Loader class to load data from files
	Direction lastMove					direction of player's last move
	GameActor* attachedActor			if an actor is attached to player, this points to it

	string gameConsoleOutput			output to the debugging log

methods:
	static Controller* ControllerClass()		Returns a reference to the instance of the
												singleton Controller class

	bool loadMap(ifstream& mapFile)				Loads the map contained in the mapFile
	void createPlayerCharacter()				Creates a player character
	bool initGame()								Must be called AFTER loading the map and creating
												the player character
	void cycle()								Runs one cycle of the game loop
	Event* processCollision(Event*)				Determine what to do given collision event
	Event* getInput()							Get input from keyboard, return an input event
	Event* getConsoleInput()					Open a console and get text input
	Event* processKeyInput(char)				Return appropriate MoveEvent based on key passed in
	Event* enterCombat(Character* enemy)		Enter into combat mode, return the result of combat
	Event* processConsoleInput(Event*)			Uses text interpreter to translate command to Event
	void printCurrentRoom()						for testing, print a representation of current room

	bool loadContainers(ifstream&)				load containers for level from file
	bool loadExits(ifstream&)					load exits for level from file
	void initNcurses()							initialize the ncurses input/output
	bool insertInCurrentRoom(Coord, GameActor*)	places a GameActor in the current room
	Event* popEventQueue()						returns the event at top of queue or type None
	void draw()									calls the draw module to redraw screen
	void printOutputBuffer()					send output buffer to console
	string getGameConsoleOut()					returns gamaConsoleOutput
	Event* relocatePlayer(RelocatePlayerEvent*)	change current room
	Event* giveItem(AcquireEvent* e)			put an item in the character's inventory

***/
class Controller {
private:
	Game* game;
	GameMap* gameMap;
	PlayerCharacter* playerCharacter;
	deque<Event*> eventQueue;
	vector<string> outputBuffer;
	GameInputOutput* io;
	Loader loader;
	Interpreter* interpreter;
	GameActor* attachedActor;
	Direction lastMove;
	//clock_t lastInputClock;
	Event* returnEvent;
	string lastConsoleInput;
	string gameConsoleOutput;

	static Controller* _controller;
	static bool inst;
	static bool init;

	Controller();

	Event* moveAttached(Direction d);
	Event* discoverActors(DiscoverEvent*);

public:
	static Controller* ControllerClass();
	~Controller() {inst = false;}

	// in this implementation, a map file will be just a list of room file names
	bool loadMap(ifstream&, string);
	bool loadContainers(ifstream&);
	bool loadExits(ifstream&);
	void createPlayerCharacter();
	bool initGame();
	void initNcurses();
	void quitGame();
	bool insertInCurrentRoom(Coord, GameActor*);
	bool insertInRoom(int, Coord, GameActor*);
	bool removeFromRoom(int rId, Coord xy, GameActor* a);
	GameActor* objectAt(int rId, Coord xy);
	Event* popEventQueue();
	Event* cycle();
	Event* processCollision(Event*);
	Event* getInput();
	Event* getConsoleInput(GameActor*);
	Event* processConsoleInput(Event*);
	Event* processKeyInput(char);
	Event* enterCombat(Character*);
	void draw();
	void showOpeningMessage(string);
	void printCurrentRoom();
	void printOutputBuffer();
	string getGameConsoleOut() { return gameConsoleOutput; }
	Event* relocatePlayer(RelocatePlayerEvent*);
	Event* giveItem(AcquireEvent* e);
	Event* playAnimation(AnimationEvent*);
	Event* swapActor(SwapEvent* e);
	void showStartScreen();
	Event* finishGame();
	void releaseScreen();

	PlayerCharacter* getPlayerCharacter() {return playerCharacter;}
};

#endif
