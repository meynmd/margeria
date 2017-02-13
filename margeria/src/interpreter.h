/***
Text Interpreter
***/

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "event.h"
#include "game.h"
#include "action.h"

#include <string>
#include <set>

using namespace std;

/***
class Interpreter

the game's text interpreter

fields
	userText	string					the text being processed
	verbs		map<string, Action>		dictionary of verbs
***/
class Interpreter {
private:
	string userText;
	Game* game;
	GameActor* context;
	vector<string> tokens;
	Action action;
	GameActor* target;
	string recognizedName;

	static map<string, Action> verbs;
	static set<char> separators;

	void parse();

public:
	Interpreter(Game*);
	~Interpreter() {}



	Action interpret(InputEvent* ie);
	Event* execute();
};

#endif
