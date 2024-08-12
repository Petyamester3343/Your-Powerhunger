#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "definitions.h"

// A homemade struct for defining the position on a map
PWRHNGR_STRUCT Pos {
	unsigned int col;
	unsigned int row;
	bool occupied;
} Pos;

// The struct for casting magic on Player's behalf
PWRHNGR_STRUCT Magia {
	char* magName;
	unsigned int magATK;
	unsigned int magCost;
	unsigned int magLVReq;
	bool acquired;
} Magia;

// Objects which would permanently buff the player until their death
PWRHNGR_STRUCT Object {
	char sign;
	unsigned int eff;
	bool found;
	Pos pos;
} Object;

// The core struct of everything sentient in this project
PWRHNGR_STRUCT Entity {
	char* name;
	unsigned int hp;
	unsigned int atk;
	unsigned int def;
	unsigned int xp;
	bool dead;
	bool fled;
	Pos pos;
} Entity;

// The playable character's struct, having Entity "inherited,"
// and being able to cast magic, possess money, and level up
PWRHNGR_STRUCT Player {
	Entity E;
	Magia M[SKILL_ROSTER];
	unsigned int mp;
	unsigned int deathCount;
	unsigned int money;
	unsigned int lv;
	bool boughtMagic;
	bool boughtATK;
	bool boughtDEF;
	bool boughtVigor;
	unsigned int aegisPickedUp;
} Player;

// The enemy struct, having Entity "inherited,"
// and containing loot for the Player to pick up as Gold after it died
PWRHNGR_STRUCT Foe {
	Entity E;
	unsigned int loot;
} Foe;
