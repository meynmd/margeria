// Interpreter class

#include "interpreter.h"
#include "utilities/tokenizer.h"

#include <cctype>
#include <algorithm>

map<string, Action> Interpreter::verbs = map<string, Action>( {
		{ "touch", Action::Activate },
		{ "use", Action::Activate },
		{ "activate", Action::Activate },
		{ "flip", Action::Activate },
		{ "turn on", Action::Activate },
		{ "light", Action::Activate },
		{ "stir", Action::Activate },
		{ "turn off", Action::Deactivate },
		{ "put out", Action::Deactivate },
		{ "put", Action::Deactivate },
		{ "deactivate", Action::Deactivate },
		{ "climb", Action::Climb },
		{ "mount", Action::Climb },
		{ "on", Action::Climb },
		{ "up", Action::Climb },
		{ "step", Action::Climb },
		{ "walk", Action::Climb },
		{ "eat", Action::Consume },
		{ "drink", Action::Consume },
		{ "taste", Action::Consume },
		{ "blow", Action::Destroy },
		{ "bomb", Action::Destroy },
		{ "destroy", Action::Destroy },
		{ "explode", Action::Destroy },
		{ "open", Action::Open },
		{ "unlock", Action::Open },
		{ "close", Action::Close },
		{ "lock", Action::Close },
		{ "move", Action::Move },
		{ "push", Action::Move },
		{ "pull", Action::Move },
		{ "rotate", Action::Move },
		{ "shift", Action::Move },
		{ "turn", Action::Move },
		{ "give", Action::Give },
		{ "smell", Action::Observe },
		{ "look", Action::Observe },
		{ "see", Action::Observe },
		{ "put", Action::Take },
		{ "take", Action::Take },
		{ "get", Action::Take },
		{ "find", Action::Take },
		{ "scoop", Action::Take },
		{ "grab", Action::Take },
		{ "ladle", Action::Take },
		{ "read", Action::Take },
		{ "quit", Action::Quit } } );

set<char> Interpreter::separators = set<char>( { ' ', '.', '!', ',', '(', ')', ':', '\'', '\"', '\t', '\n' } );

Interpreter::Interpreter(Game* g) {
	userText = "";
	game = g;
	context = 0;
	action = Action::None;
	target = 0;
	recognizedName = "";
}

Action Interpreter::interpret( InputEvent* ie ) {
	userText = "";
	action = Action::None;
	target = 0;
	recognizedName = "";
	userText = ie->text;
	context = ie->subject;
	this->game = game;
	tokens = vector<string>();
	action = Action::None;
	tokens = tokenize( userText, separators, true );
	parse();
	if (!target) {
		target = ie->subject;
	}
	if (action == Action::Quit) {
		return action;
	}
	// if there's an action and a target, see if there's a corresponding event
	if (action == Action::None || target == 0) {
		return Action::None;
	} else {
		return action;
	}
}

Event* Interpreter::execute() {
	if (action == Action::Quit) {
		return new Event( EventType::CloseConsole, 0, 0 );
	}
	if (target && action != Action::None) {
		return target->getActionResult( action, game->getPlayerCharacter() );
	} else if (target) {
		return new Event( EventType::Redraw, 0, 0, "You don't know how to do that." );
	} else {
		if (recognizedName.empty()) {
			return new Event( EventType::Redraw, 0, 0,
					"You don't understand what you are trying to do." );
		} else {
			return new Event( EventType::Redraw, 0, 0,
					"You think you see a "  + recognizedName + " over there, but you need to get "
					"closer to it." );
		}
	}
}

void Interpreter::parse() {
	// try to find the verb in this text
	if(tokens.empty()) {
		return;
	}
	auto tok = tokens.begin();
	string token = *tok;
	if (token == "quit") {
		action = Action::Quit;
		return;
	}

	set<string> contextDescriptors;
	if(context) {
		contextDescriptors = context->getDescriptors();
	}
	bool haveVerb = false;
	//bool haveTarget = false;

	while (tok != tokens.end()) {
		if (!haveVerb) {
			if (verbs.find( *tok ) != verbs.end()) {
				action = verbs[*tok];
				haveVerb = true;
			}
		} else {
			// if word matches context object, target found
			if(contextDescriptors.find(*tok) != contextDescriptors.end()) {
				target = context;
				break;
			}
			// look for a target object nearby
			target = game->getRoomActorByDescriptor( *tok );
			if (target) {
				//haveTarget = true;
				break;
			}
			// if not found, look in inventory
			target = game->getPlayerCharacter()->findInventoryItemByName( *tok );
			if (target) {
				//haveTarget = true;
				break;
			}
			// can't find target, but do we at least recognize something in the room?
			if (game->getRoomActorDescriptors().count( *tok )) {
				recognizedName = *tok;
			} else {
				recognizedName = "";
			}

		}
		tok++;
	}
}
