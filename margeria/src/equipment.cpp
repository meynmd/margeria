// Item and subclasses implementation

#include "gameActor.h"

void Armor::setProtectFactor( float factor )
{
	if (factor < 0.01)
		protectFactor = 0.f;
	if (factor > 0.99)
		protectFactor = 1.f;
	else
		protectFactor = factor;
}
