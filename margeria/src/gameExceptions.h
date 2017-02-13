#ifndef GAME_EXCEPTIONS_H
#define GAME_EXCEPTIONS_H

#include <exception>

class invalidCoordException : public std::exception {
private:
	const char* excMessage;
public:
	invalidCoordException(const char* message) {
		excMessage = message;
	}
	const char* what();
};

class unableToSpawnException : public std::exception {
private:
	const char* excMessage;
public:
	unableToSpawnException(const char* message) {
		excMessage = message;
	}
	const char* what();
};

class fileIoException : public std::exception {
private:
	const char* excMessage;
public:
	fileIoException(const char* message) {
		excMessage = message;
	}
	const char* what();
};

class idException : public std::exception {
private:
	const char* excMessage;
public:
	idException(const char* message) {
		excMessage = message;
	}
	const char* what();
};

class eventTypeException : public std::exception {
private:
	const char* excMessage;
public:
	eventTypeException(const char* message) {
		excMessage = message;
	}
	const char* what();
};

class displaySizeException : public std::exception {
private:
	const char* excMessage;
public:
	displaySizeException(const char* message) {
		excMessage = message;
	}
	const char* what();
};

#endif
