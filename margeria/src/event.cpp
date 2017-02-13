#include "event.h"
#include "gameActor.h"

string Event::toString() {
	string eTypeString;

	switch (eventType) {
	case EventType::None:
		eTypeString = "None";
		break;

	case EventType::Input:
		eTypeString = "Input";
		break;

	case EventType::OpenConsole:
		eTypeString = "Open Console";
		break;

	case EventType::Move:
		eTypeString = "Move";
		break;

	case EventType::Destroy:
		eTypeString = "Destroy";
		break;

	case EventType::Replace:
		eTypeString = "Replace";
		break;

	case EventType::Redraw:
		eTypeString = "Redraw";
		break;

	case EventType::Collision:
		eTypeString = "Collision";
		break;

	case EventType::EndGame:
		eTypeString = "EndGame";
		break;

	case EventType::Acquire:
		eTypeString = "Acquire";
		break;

	case EventType::Attach:
		eTypeString = "Attach";
		break;

	case EventType::Failure:
		eTypeString = "Failure";
		break;


	default:
		eTypeString = "Unknown";
		break;
	}

	string s = "Event type " + eTypeString + "\n";
	if (sender) {
		s += "Sender:\n\tId: " + sender->getId();
		s += "\n\tName: " + sender->getName();
	}
	if (subject) {
		s += "\nSubject:\n\tId: " + subject->getId();
		s += "\n\tName: " + subject->getName();
	}
	s += "\n";

	return s;
}
