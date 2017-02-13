cFlags = -Wall -c -g -std=c++11 -DDEBUG=0
lFlags = -Wall -g -std=c++11

all: margeria

margeria: main.o gameInputOutput.o animation.o gameActor.o roomObjects.o room.o gameMap.o game.o equipment.o loader.o controller.o event.o interpreter.o move.o gameExceptions.o idGenerator.o tokenizer.o params.h
	g++ $(lFlags) -o ../build/margeria main.o gameInputOutput.o animation.o gameActor.o roomObjects.o room.o gameMap.o game.o equipment.o loader.o controller.o event.o interpreter.o move.o gameExceptions.o idGenerator.o tokenizer.o -lncurses

gameInputOutput.o: gameInputOutput.h gameInputOutput.cpp params.h
	g++ $(cFlags) gameInputOutput.cpp

animation.o: animation.cpp animation.h params.h
	g++ $(cFlags) animation.cpp

gameActor.o: gameActor.cpp gameActor.h params.h
	g++ $(cFlags) gameActor.cpp
	
roomObjects.o: roomObjects.cpp roomObjects.h params.h
	g++ $(cFlags) roomObjects.cpp
	
room.o: room.cpp room.h params.h
	g++ $(cFlags) room.cpp

gameMap.o: gameMap.cpp gameMap.h params.h
	g++ $(cFlags) gameMap.cpp

game.o: game.cpp game.h params.h
	g++ $(cFlags) game.cpp

equipment.o: equipment.cpp equipment.h params.h
	g++ $(cFlags) equipment.cpp
	
move.o: move.h move.cpp params.h
	g++ $(cFlags) move.cpp
	
loader.o: loader.h loader.cpp params.h
	g++ $(cFlags) loader.cpp
	
controller.o: controller.h controller.cpp params.h
	g++ $(cFlags) controller.cpp
	
event.o: event.cpp event.h params.h
	g++ $(cFlags) event.cpp
	
interpreter.o: interpreter.cpp interpreter.h params.h
	g++ $(cFlags) interpreter.cpp
	
main.o: main.cpp game.h params.h
	g++ $(cFlags) main.cpp
	
gameExceptions.o: gameExceptions.h gameExceptions.cpp params.h
	g++ $(cFlags) gameExceptions.cpp
	
idGenerator.o: utilities/idGenerator.h utilities/idGenerator.cpp params.h
	g++ $(cFlags) utilities/idGenerator.cpp
	
tokenizer.o:  utilities/tokenizer.h  utilities/tokenizer.cpp params.h
	g++ $(cFlags) utilities/tokenizer.cpp
	
clean:
	rm *.o