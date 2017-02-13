/*
 * animation.h
 *
 * animation struct
 */

#ifndef ANIMATION_H
#define ANIMATION_H

#include <fstream>

#include "coord.h"

/***
struct Animation

simple data structure for animation data

fields:
	Coord dimensions		size of animation in game spaces
	int frames				how many frames in the animation
	int frameDelay			millisecond delay between frames
	char*** txtMap			array of array of strings representing text maps of frames
	bool loaded				has the animation been loaded

methods:
	bool load( std::ifstream& )		load the animation from animation file
	void print()					print each frame

***/

struct Animation {
	char*** txtMap;
	Coord dimensions;
	int frames;
	int frameDelay, startDelay, endDelay;
	bool loaded;

	Animation( std::ifstream& );

	bool load( std::ifstream& );
	void print();
};

#endif
