// general parameters for the game

#ifndef PARAMS_H
#define PARAMS_H

#include <string>
#include <ncurses.h>

//#define DEBUG 1
#define BUF_SIZE 1024

// movement keys
#define MVLEFT_1 'a'
#define MVLEFT_2 'h'
#define MVLEFT_3 KEY_LEFT
#define MVRIGHT_1 'd'
#define MVRIGHT_2 'l'
#define MVRIGHT_3 KEY_RIGHT
#define MVUP_1 'w'
#define MVUP_2 'k'
#define MVUP_3 KEY_UP
#define MVDOWN_1 's'
#define MVDOWN_2 'j'
#define MVDOWN_3 KEY_DOWN
#define RELEASE_OBJ_1 'x'
#define RELEASE_OBJ_2 'm'
#define OPEN_CONSOLE '\t'
#define REPEAT '\n'
#define QUIT 'q'


// display params
#define BLOCK_WIDTH 5
#define BLOCK_HEIGHT 3
#define MAX_CONSOLE_LINES 10
#define MIN_CONSOLE_LINES 10
#define MIN_GAMEWIN_HEIGHT 36
#define MIN_GAMEWIN_WIDTH 92
#define STATSWIN_WIDTH 32
#define ROOM_DISP_MARGIN 1
#define CONSOLE_LR_MARGIN 4
#define CONSOLE_TB_MARGIN 1

#define SEE_OBJECT_RADIUS 1

static std::string dataPath = "data/";
static const std::string PROMPT = "> ";
static const std::string GAME_TITLE = "Monarch of Margeria";

#endif

