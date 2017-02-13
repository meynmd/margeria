// movement utility functions

#include "move.h"
#include <cstdlib>

// returns modified coordinates to implement requested move direction
Coord move( Coord s, Direction d ) {
	switch (d) {
	case Up:
		s.y -= 1;
		break;
	case Down:
		s.y += 1;
		break;
	case Left:
		s.x -= 1;
		break;
	case Right:
		s.x += 1;
		break;
	default:
		throw("Invalid Movement direction");
	}
	return s;
}

Direction reverse( Direction d ) {
	switch (d) {
	case Up:
		return Down;
		break;
	case Down:
		return Up;
		break;
	case Left:
		return Right;
		break;
	case Right:
		return Left;
		break;
	default:
		throw("Invalid Movement direction");
	}
}

Direction findDirection(Coord from, Coord to) {
	bool xDir;
	int dx = to.x - from.x;
	int dy = to.y - from.y;
	if(abs(dy) > abs(dx)) {
		xDir = false;
	} else {
		xDir = true;
	}
	if(xDir) {
		if(dx > 0)
			return Right;
		else
			return Left;
	} else {
		if(dy > 0)
			return Down;
		else
			return Up;
	}
}
