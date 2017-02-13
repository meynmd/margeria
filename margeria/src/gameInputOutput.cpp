#include "gameInputOutput.h"
#include "params.h"
#include "gameExceptions.h"

#include <cstdio>
#include <cstring>
#include <unistd.h>

using namespace std;

GameInputOutput* GameInputOutput::_gameInputOutput = 0;
bool GameInputOutput::inst = false;

GameInputOutput::GameInputOutput() {
	consoleLines = vector<string>();
	maxConsoleLines = MAX_CONSOLE_LINES;
	currentConsoleLine = 0;
	lastChar = '\0';
	lastConsoleInput = "";
}

GameInputOutput* GameInputOutput::GameInputOutputClass() {
	if (!inst) {
		_gameInputOutput = new GameInputOutput();
		inst = true;
	}
	return _gameInputOutput;
}

void GameInputOutput::initDisplay() {
	initscr();
	noecho();
	curs_set( FALSE );

	getmaxyx( stdscr, windowHeight, windowWidth );
	int minHeight = MIN_GAMEWIN_HEIGHT + MIN_CONSOLE_LINES + ROOM_DISP_MARGIN;
	int minWidth = MIN_GAMEWIN_WIDTH + STATSWIN_WIDTH + ROOM_DISP_MARGIN;
	if (windowHeight < minHeight || windowWidth < minWidth) {
		releaseDisplay();
		throw(displaySizeException( "The display is too small." ));
	}

	statsWindowWidth = windowWidth - MIN_GAMEWIN_WIDTH - 2 * ROOM_DISP_MARGIN;
	gameWindowWidth = MIN_GAMEWIN_WIDTH;
	gameWindowHeight = MIN_GAMEWIN_HEIGHT;
	statsWindowHeight = gameWindowHeight;
	consoleWindowHeight = windowHeight - gameWindowHeight - ROOM_DISP_MARGIN;
	consoleWindowWidth = windowWidth - 2 * ROOM_DISP_MARGIN;

	maxConsoleLines = consoleWindowHeight - 2 * CONSOLE_TB_MARGIN - 2;

	initConsoleWindow( consoleWindowWidth, consoleWindowHeight, ROOM_DISP_MARGIN,
			ROOM_DISP_MARGIN + gameWindowHeight );
	initGameWindow( gameWindowWidth, gameWindowHeight, 0, 0 );
	initStatsWindow( statsWindowWidth, statsWindowHeight, gameWindowWidth + ROOM_DISP_MARGIN, 0 );
}

void GameInputOutput::initGameWindow( int xDim, int yDim, int startX, int startY ) {
	gameWindow = newwin( yDim, xDim, startY, startX );	// (rows, cols, begin_y, begin_x)
}

void GameInputOutput::initConsoleWindow( int xDim, int yDim, int startX, int startY ) {
	consoleWindow = newwin( yDim, xDim, startY, startX );	// (rows, cols, begin_y, begin_x)
	clearConsoleWindow();
	//box( consoleWindow, 0, 0 );
	wrefresh( consoleWindow );
}

void GameInputOutput::initStatsWindow( int xDim, int yDim, int startX, int startY ) {
	statsWindow = newwin( yDim, xDim, startY, startX );
	//box( statsWindow, 0, 0 );
	wrefresh( statsWindow );
}

void GameInputOutput::initInput() {
	//keypad( consoleWindow, TRUE );
	keypad( stdscr, TRUE );
	//halfdelay( 1 );
	nodelay( consoleWindow, false );
}

void GameInputOutput::shakeRoom( Room* r, int frames ) {
	shakeRoom( r, frames, 1 );
}

void GameInputOutput::shakeRoom( Room* r, int frames, int jitterSize ) {
	for (int i = 0; i < frames; i++) {
		int offsetX = rand() % (1 + 2 * jitterSize) - jitterSize;
		int offsetY = rand() % (1 + 2 * jitterSize) - jitterSize;
		drawRoom( r, Coord( offsetX, offsetY ) );
		usleep( 25000 );
	}
	drawRoom( r );
}

void GameInputOutput::drawRoom( Room* r ) {
	drawRoom( r, Coord( 0, 0 ) );
}

void GameInputOutput::drawRoom( Room* r, Coord offset ) {
	if (!r) {
		drawRoom( r, Coord( 0, 0 ), Coord( 0, 0 ), offset );
	} else {
		drawRoom( r, Coord( 0, 0 ), Coord( r->dimensions.x - 1, r->dimensions.y - 1 ), offset );
	}
}

void GameInputOutput::drawRoom( Room* r, Coord ul, Coord lr ) {
	drawRoom( r, ul, lr, Coord( 0, 0 ) );
}

void GameInputOutput::drawRoom( Room* r, Coord ul, Coord lr, Coord offset ) {
	wclear( gameWindow );

	if (!r) {
		wrefresh( gameWindow );
		return;
	}

	if (DEBUG) {
		char rNum[16];
		sprintf( rNum, "Room %d", abs( r->id ) );
		mvwprintw( gameWindow, 0, gameWindowWidth - 8, rNum );
	}

	int xDim = r->dimensions.x;
	int yDim = r->dimensions.y;
	if (ul.x < 0 || ul.x > xDim - 1 || ul.y < 0 || ul.y > yDim - 1 || lr.x < 0 || lr.x > xDim - 1
			|| lr.y < 0 || lr.y > yDim - 1) {
		wrefresh( gameWindow );
		return;
	}

	// print room contents
	int startX = ROOM_DISP_MARGIN + offset.x;
	int startY = offset.y;
	//int startY = ROOM_DISP_MARGIN + offset.y;
	char symbol;
	for (int y = ul.y; y <= lr.y; y++) {
		for (int x = ul.x; x <= lr.x; x++) {
			GameActor* go = r->objectAt( Coord( x, y ) );
			if (go) {
				symbol = go->getSymbol();
			} else {
				symbol = ' ';
			}
			if (go) {
				if (go->getTxtMap()) {
					printBlock( startX + BLOCK_WIDTH * x, startY + BLOCK_HEIGHT * y, go );
				} else {
					mvwaddch( gameWindow, startY + BLOCK_HEIGHT * y, startX + BLOCK_WIDTH * x,
							symbol );
				}
			}
		}
	}
	wrefresh( gameWindow );
}

void GameInputOutput::drawStatsWin( set<GameActor*> inv ) {
	const int leftMargin = 4;
	wclear( statsWindow );
	map<string, int> itemCounts;
	for (GameActor* item : inv) {
		string name = item->getName();
		if (itemCounts.find( name ) != itemCounts.end()) {
			itemCounts[name]++;
		} else {
			itemCounts[name] = 1;
		}
	}

	int row = 0;
	
	// print top border
	mvwaddch( statsWindow, 0, 0, ACS_ULCORNER );
	for (int i = 1; i < statsWindowWidth - 1; i++) {
		waddch( statsWindow, ACS_HLINE );
	}
	waddch( statsWindow, ACS_URCORNER );

	// print side and bottom borders
	for (int i = 1; i < statsWindowHeight - 1; i++) {
		mvwaddch( statsWindow, i, 0, ACS_VLINE );
		mvwaddch( statsWindow, i, statsWindowWidth - 1, ACS_VLINE );
	}
	mvwaddch( statsWindow, statsWindowHeight - 1, 0, ACS_LLCORNER );
	for (int i = 1; i < statsWindowWidth - 1; i++) {
		waddch( statsWindow, ACS_HLINE );
	}
	waddch( statsWindow, ACS_LRCORNER );
	
	
	// print inventory list
	mvwprintw( statsWindow, row++, statsWindowWidth / 2 - 5, "Inventory" );
	for (auto i : itemCounts) {
		row++;
		char buf[16];
		sprintf( buf, "%s x%d", i.first.c_str(), i.second );
		buf[15] = '\0';
		mvwprintw( statsWindow, row, leftMargin, buf );
	}
	//box(statsWindow, 0, 0);
	wrefresh( statsWindow );
}

void GameInputOutput::alert() {
	beep();
}

void GameInputOutput::printBlock( int x, int y, GameActor* a ) {
	char** block = a->getTxtMap();
	for (int j = 0; j < BLOCK_HEIGHT; j++) {
		for (int i = 0; i < BLOCK_WIDTH; i++) {
			switch (block[j][i]) {
			case '1':
				mvwaddch( gameWindow, y + j, x + i, ACS_CKBOARD );
				break;
			case '2':
				mvwaddch( gameWindow, y+j, x+i, ACS_DIAMOND );
				break;
			default:
				mvwaddch( gameWindow, y + j, x + i, block[j][i] );
				break;
			}
		}
	}
}

void GameInputOutput::writeConsole( string s ) {
	if (!consoleWindow) {
		initDisplay();
	}
	int lines = 1;
	// if the line is longer than console width, wrap it into more lines
	while (s.length() > windowWidth - 2 * CONSOLE_LR_MARGIN - 2) {
		size_t endLine = s.rfind( ' ', windowWidth - 2 * CONSOLE_LR_MARGIN - 2 );
		string curLine = s.substr( 0, endLine );
		s = s.substr( 1 + endLine );
		consoleLines.push_back( curLine );
		lines++;
	}
	consoleLines.push_back( s );

	if (currentConsoleLine + lines > maxConsoleLines) {
		printConsoleLines();
	} else {
		for (int i = consoleLines.size() - lines; i < consoleLines.size(); i++) {
			string curLine = consoleLines.at( i );
			mvwaddch( consoleWindow, currentConsoleLine, 0, ACS_VLINE );
			mvwprintw( consoleWindow, currentConsoleLine, CONSOLE_LR_MARGIN, curLine.c_str() );
			mvwaddch( consoleWindow, currentConsoleLine, consoleWindowWidth - 1, ACS_VLINE );
			currentConsoleLine++;
		}
		wrefresh( consoleWindow );
	}
}

string GameInputOutput::popConsole() {
	string r = consoleLines.back();
	consoleLines.pop_back();
	return r;
}

void GameInputOutput::clearConsoleWindow() {
	wclear( consoleWindow );

	// print top border
	mvwaddch( consoleWindow, 0, 0, ACS_ULCORNER );
	for (int i = 1; i < consoleWindowWidth - 1; i++) {
		waddch( consoleWindow, ACS_HLINE );
	}
	waddch( consoleWindow, ACS_URCORNER );

	mvwprintw( consoleWindow, 0, consoleWindowWidth / 2 - GAME_TITLE.length() / 2,
			GAME_TITLE.c_str() );

	// print side and bottom borders
	for (int i = 1; i < consoleWindowHeight - 1; i++) {
		mvwaddch( consoleWindow, i, 0, ACS_VLINE );
		mvwaddch( consoleWindow, i, consoleWindowWidth - 1, ACS_VLINE );
	}
	mvwaddch( consoleWindow, consoleWindowHeight - 1, 0, ACS_LLCORNER );
	for (int i = 1; i < consoleWindowWidth - 1; i++) {
		waddch( consoleWindow, ACS_HLINE );
	}
	waddch( consoleWindow, ACS_LRCORNER );

	wrefresh( consoleWindow );
	currentConsoleLine = 1 + CONSOLE_TB_MARGIN;
}

void GameInputOutput::clearConsole() {
	consoleLines.clear();
	clearConsoleWindow();
}

void GameInputOutput::printConsoleLines() {
	clearConsoleWindow();

	const int xMargin = CONSOLE_LR_MARGIN; //, yMargin = CONSOLE_TB_MARGIN;
	int firstLine;
	if (maxConsoleLines < consoleLines.size()) {
		firstLine = consoleLines.size() - maxConsoleLines;
	} else {
		firstLine = 0;
	}

	// print each line
	for (int i = firstLine; i < consoleLines.size(); i++) {
		string line = consoleLines.at( i );
		mvwprintw( consoleWindow, currentConsoleLine, xMargin, line.c_str() );
		mvwaddch( consoleWindow, currentConsoleLine, consoleWindowWidth - 1, ACS_VLINE );
		currentConsoleLine++;
	}

	wrefresh( consoleWindow );
}

void GameInputOutput::closeWindow( WINDOW* w ) {
	wrefresh( w );
	delwin( w );
}

void GameInputOutput::displayOpeningMessage( string mes, int sec ) {
	WINDOW* fullscreen = newwin( windowHeight, windowWidth, 0, 0 );
	wclear( fullscreen );
	wrefresh( fullscreen );
	sleep( 1 );
	mvwprintw( fullscreen, windowHeight / 2, windowWidth / 2 - mes.length() / 2, mes.c_str() );
	wrefresh( fullscreen );
	sleep( sec );
	wclear( fullscreen );
	wrefresh( fullscreen );
	sleep( 1 );
	wclear( fullscreen );
	wrefresh( fullscreen );
}

void GameInputOutput::displaySpecialMessage(
												string mes,
												int secBefore,
												int secLength,
												int secAfter ) {
	clear();
	refresh();
	if (secBefore > 0)
		sleep( secBefore );

	int margin = 5;
	vector<string> message;

	// if the line is longer than console width, wrap it into more lines
	int lines = 1;
	while (mes.length() > windowWidth - 2 * margin) {
		size_t endLine = mes.rfind( ' ', windowWidth - 2 * margin );
		string curLine = mes.substr( 0, endLine );
		mes = mes.substr( 1 + endLine );
		message.push_back( curLine );
		lines++;
	}
	message.push_back( mes );
	int startLine = windowHeight / 2 - lines;
	if (startLine < 0)
		startLine = 0;

	if (message.size() == 1) {
		mvprintw( windowHeight / 2, windowWidth / 2 - mes.length() / 2, mes.c_str() );
	} else {
		for (int i = 0; i < lines; i++) {
			mvprintw( startLine + i, margin, message.at( i ).c_str() );
		}
	}

	refresh();
	sleep( secLength );
	clear();
	refresh();
	if (secAfter > 0)
		sleep( secAfter );
	clear();
	refresh();
}

char GameInputOutput::getKeyInput() {
	keypad( consoleWindow, TRUE );
	lastChar = wgetch( consoleWindow );
	flushinp();
	return lastChar;
}

std::string GameInputOutput::getConsoleInput() {
	if (!consoleWindow) {
		initDisplay();
	}
	//writeConsole( PROMPT );
	mvwprintw( consoleWindow, currentConsoleLine, CONSOLE_LR_MARGIN, PROMPT.c_str() );
	echo();
	curs_set( TRUE );
	char buf[BUF_SIZE];
	wgetstr( consoleWindow, buf );

	std::string inputLine( buf );
	if (inputLine.empty()) {
		inputLine = lastConsoleInput;
	}

	writeConsole( PROMPT + inputLine );
	lastConsoleInput = inputLine;
	noecho();
	curs_set( FALSE );
	return inputLine;
}

void GameInputOutput::startScreen() {
	clear();
	refresh();
	int windowHeight, windowWidth;
	getmaxyx( stdscr, windowHeight, windowWidth );
	string title = "MONARCH OF MARGERIA";
	int firstLine = windowHeight / 2 - 5;
	mvprintw( firstLine, windowWidth / 2 - title.length() / 2, title.c_str() );
	mvprintw( firstLine + 8, windowWidth / 2 - 9, "Press ENTER to start" );
	mvprintw( firstLine + 10, windowWidth / 2 - 9, "Press q to quit" );
	refresh();
}

void GameInputOutput::releaseDisplay() {
	echo();
	curs_set( TRUE );
	endwin();
}

Event* GameInputOutput::playAnimation( Animation* a, Coord loc ) {
	if (!a) {
		return new Event( EventType::None, 0, 0, "playAnimation: no animation to play" );
	}
	int startX = ROOM_DISP_MARGIN + BLOCK_WIDTH * loc.x;
	int startY = BLOCK_HEIGHT * loc.y;
	if (startX + BLOCK_WIDTH * a->dimensions.x > gameWindowWidth
			|| startY + BLOCK_HEIGHT * a->dimensions.y > gameWindowHeight) {
		return new Event( EventType::None, 0, 0,
				"playAnimation: animation will not fit in this location" );
	}

	// display the animation
	usleep( a->startDelay );
	for (int f = 0; f < a->frames; f++) {
		for (int r = 0; r < BLOCK_HEIGHT * a->dimensions.y; r++) {
			if (strlen( a->txtMap[f][r] ) == BLOCK_WIDTH * a->dimensions.x) {
				mvwprintw( gameWindow, startY + r, startX, a->txtMap[f][r] );
			} else {
				return new Event( EventType::None, 0, 0,
						"playAnimation: animation not properly formatted" );
			}
		}
		wrefresh( gameWindow );
		usleep( a->frameDelay );
	}
	usleep( a->endDelay );
	return new Event( EventType::Redraw, 0, 0 );
}

Event* GameInputOutput::playAnimationReverse( Animation* a, Coord loc ) {
	if (!a) {
		return new Event( EventType::None, 0, 0, "playAnimation: no animation to play" );
	}
	int startX = ROOM_DISP_MARGIN + BLOCK_WIDTH * loc.x;
	int startY = BLOCK_HEIGHT * loc.y;
	if (startX + BLOCK_WIDTH * a->dimensions.x > gameWindowWidth
			|| startY + BLOCK_HEIGHT * a->dimensions.y > gameWindowHeight) {
		return new Event( EventType::None, 0, 0,
				"playAnimation: animation will not fit in this location" );
	}

	// display the animation
	usleep( a->startDelay );
	for (int f = a->frames - 1; f >= 0; f--) {
		for (int r = 0; r < BLOCK_HEIGHT * a->dimensions.y; r++) {
			if (strlen( a->txtMap[f][r] ) == BLOCK_WIDTH * a->dimensions.x) {
				mvwprintw( gameWindow, startY + r, startX, a->txtMap[f][r] );
			} else {
				return new Event( EventType::None, 0, 0,
						"playAnimation: animation not properly formatted" );
			}
		}
		wrefresh( gameWindow );
		usleep( a->frameDelay );
	}
	usleep( a->endDelay );
	return new Event( EventType::Redraw, 0, 0 );
}
