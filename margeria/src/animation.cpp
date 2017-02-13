#include "animation.h"
#include "params.h"
#include "gameExceptions.h"

#include <fstream>
#include <cstring>

using namespace std;

Animation::Animation(ifstream& animFile) {
	txtMap = 0;
	frames = 0;
	frameDelay = 0;
	startDelay = 0;
	endDelay = 0;
	loaded = load(animFile);
}

bool Animation::load( ifstream& animFile ) {
	if (!animFile) {
		return false;
	}

	// first line should be number of frames
	char line[BUF_SIZE];
	animFile.getline( line, BUF_SIZE );
	frames = atoi( line );
	if (frames < 1) {
		return false;
	}

	// second line: initial delay before playing animation
	animFile.getline( line, BUF_SIZE );
	startDelay = 1000 * atoi(line);

	// third line: delay between frames in mseconds
	animFile.getline( line, BUF_SIZE );
	frameDelay = 1000 * atoi(line);

	// fourth line: delay after playing animation
	animFile.getline( line, BUF_SIZE );
	endDelay = 1000 * atoi(line);


	// 5th line: dimensions in game tiles
	if ('[' != animFile.get()) {
		throw(fileIoException( "Map file incorrectly formatted\n" ));
	}
	animFile.getline( line, BUF_SIZE, ',' );
	int xDim = atoi( line );
	animFile.getline( line, BUF_SIZE );
	int yDim = atoi( line );
	dimensions = Coord( xDim, yDim );

	// set up text map
	txtMap = new char**[frames];
	for(int j = 0; j < frames; j++) {
		txtMap[j] = new char*[yDim * BLOCK_HEIGHT];
		for(int i = 0; i < yDim * BLOCK_HEIGHT; i++) {
			txtMap[j][i] = new char[1 + xDim * BLOCK_WIDTH];
		}
	}

	// read each frame from file
	for(int i = 0; i < frames; i++) {
		int row = 0;
		animFile.getline(line, BUF_SIZE);
		while(line[0] != '\0') {
			strncpy(txtMap[i][row], line, 1 + xDim * BLOCK_WIDTH);
			row++;
			animFile.getline(line, BUF_SIZE);
		}
	}
	return true;
}



void Animation::print() {
	if(!loaded) {
		return;
	}
	for(int j = 0; j < frames; j++) {
		printf("FRAME %d\n\n", j);
		for(int i = 0; i < dimensions.y * BLOCK_HEIGHT; i++) {
			printf("%s\n", txtMap[j][i]);
		}
		printf("\n");
	}
}
