#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "gameActor.h"

#include <string>

using namespace std;

/***
class Equipment

an object that can be possessed by a character

Fields:
	int id			ID number supplied in constructor
	string name		item name
	description		item description
	int weight		item weight
	int value		how many coins the item is worth
***/

class Equipment : public GameActor {
private:
	string description;
	int weight;
	int value;

public:
	Equipment( int itemId, string itemName ) : GameActor(itemId, itemName)
	{
	}

	Equipment( int itemId, string itemName, int itemWeight, int itemValue )
		: GameActor(itemId, itemName)
	{
		weight = itemWeight;
		value = itemValue;
	}

	int getId() { return id; }
	void setName( string n ) {name = n; }
	string getName() { return name; }
	void setDescription( string d ) {description = d; }
	string getDescription() { return description; }
	void setWeight( int w ) { weight = w; }
	int getWeight() { return weight; }
	void setValue( int v ) { value = v; }
	int getValue() { return value; }

};

class Armor : public Equipment {
private:
	float protectFactor;

public:
	Armor( int itemId, string itemName, float armorFactor ) : Equipment( itemId, itemName )
	{
		setProtectFactor(armorFactor);
	}

	void setProtectFactor( float factor );

	float getProtectFactor()
	{
		return protectFactor;
	}
};

class Weapon : public Equipment {
private:
	float damageMultiplier;		// default damage caused by weapon

public:
	Weapon( int itemId, string itemName, int weaponDamage ) : Equipment(itemId, itemName)
	{
		damageMultiplier = weaponDamage;
	}

	int getDamage()
	{
		return damageMultiplier;
	}
	void setDamage(int amount)
	{
		damageMultiplier = amount;
	}
};

#endif
