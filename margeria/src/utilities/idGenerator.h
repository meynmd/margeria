// integer ID generator

#ifndef ID_GENERATOR_H
#define ID_GENERATOR_H

class IdGenerator {
private:
	static int idCounter;
	static IdGenerator* _idGenerator;
	static bool inst;

	IdGenerator(int);

public:
	static IdGenerator* IdGeneratorClass(int);
	~IdGenerator() { inst = false; }

	int generateId();
};

#endif
