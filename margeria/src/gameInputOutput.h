#ifndef GAME_IO_H
#define GAME_IO_H

#include "room.h"
#include "animation.h"

#include <string>
#include <vector>
#include <ncurses.h>
#include <ctime>

class GameInputOutput {
private:
	std::vector<std::string> consoleLines;

	WINDOW* consoleWindow;
	WINDOW* gameWindow;
	WINDOW* statsWindow;

	int windowHeight, windowWidth, gameWindowWidth, statsWindowWidth;
	int consoleWindowWidth, consoleWindowHeight, gameWindowHeight, statsWindowHeight;
	int maxConsoleLines, currentConsoleLine;
	char lastChar;
	string lastConsoleInput;

	static GameInputOutput* _gameInputOutput;
	static bool inst;

	GameInputOutput();

	void initGameWindow( int, int, int, int );
	void initConsoleWindow( int, int, int, int );
	void initStatsWindow( int, int, int, int );

	void printConsoleLines();
	void printBlock( int, int, GameActor* );
	string popConsole();
	void clearConsoleWindow();
	void closeWindow( WINDOW* );
	void printString( string, WINDOW* );

public:
	static GameInputOutput* GameInputOutputClass();
	~GameInputOutput() {
		inst = false;
	}

	void initDisplay();
	void initInput();
	void drawRoom( Room* );
	void drawRoom( Room*, Coord offset );
	void drawRoom( Room*, Coord ul, Coord lr, Coord offset );
	void drawRoom( Room*, Coord ul, Coord lr );
	void shakeRoom(Room*, int);
	void shakeRoom(Room*, int, int);
	void drawStatsWin(set<GameActor*>);
	void displayOpeningMessage(string, int);
	void displaySpecialMessage(string, int, int, int);
	void alert();
	void closeConsole( WINDOW* );
	void writeConsole( string );
	void clearConsole();
	char getKeyInput();
	std::string getConsoleInput();
	Event* playAnimation(Animation* a, Coord loc);
	Event* playAnimationReverse(Animation* a, Coord loc);
	void startScreen();
	void releaseDisplay();
};

#endif
