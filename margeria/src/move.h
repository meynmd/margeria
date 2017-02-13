// Direction enum and utility function to help with movement

#ifndef MOVE_H
#define MOVE_H

#include "coord.h"

enum Direction {
	Up, Right, Down, Left
};

Direction reverse(Direction);
Direction findDirection(Coord from, Coord to);
Coord move( Coord s, Direction d );

#endif
