#include "idGenerator.h"

bool IdGenerator::inst = false;
int IdGenerator::idCounter = 0;
IdGenerator* IdGenerator::_idGenerator = 0;

IdGenerator::IdGenerator(int startVal) {
	idCounter = startVal;
}

IdGenerator* IdGenerator::IdGeneratorClass(int startVal) {
	if(!inst) {
		_idGenerator = new IdGenerator(startVal);
		inst = true;
	}
	return _idGenerator;
}

int IdGenerator::generateId() {
	return idCounter++;
}
