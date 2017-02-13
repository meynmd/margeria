// Action class
// defines actions that player can take on a GameActor

#ifndef ACTION_H
#define ACTION_H

enum class Action {
	None,
	Activate,
	Climb,
	Deactivate,
	Destroy,
	Open,
	Close,
	Move,
	Observe,
	Give,
	Take,
	Talk,
	Consume,
	Attack,
	Quit
};

#endif
