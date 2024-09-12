#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "definitions.h"

// A homemade struct for defining the position on a map
PWRHNGR_STRUCT Pos
{
    uint col;
    uint row;
    bool occupied;
} Pos;

// The struct for casting magic on Player's behalf
PWRHNGR_STRUCT Magia
{
    char* magName;
    uint magATK;
    uint magCost;
    uint magLVReq;
    bool acquired;
} Magia;

// Objects which would permanently buff the player until their death
PWRHNGR_STRUCT Object
{
    char sign;
    uint eff;
    bool found;
    Pos pos;
} Object;

// The core struct of everything sentient in this project
PWRHNGR_STRUCT Entity
{
    char* name;
    uint hp;
    uint atk;
    uint def;
    uint xp;
    bool dead;
    bool fled;
    Pos pos;
    bool hasMagiaResist;
    uint magiaResist;
} Entity;

// The playable character's struct, having Entity "inherited,"
// being able to cast magic, possess money, buy stuff, and level up,
// and also count the picked up Aegis cores
PWRHNGR_STRUCT Player
{
    Entity E;
    Magia M[SKILL_ROSTER];
    uint mp;
    uint deathCount;
    uint money;
    uint lv;
    bool boughtMagic;
    bool boughtATK;
    bool boughtDEF;
    bool boughtVigor;
    uint aegisPickedUp;
} Player;

// The enemy struct, having Entity "inherited,"
// and containing loot for the Player to pick up as Gold after it died
PWRHNGR_STRUCT Foe
{
    Entity E;
    uint loot;
} Foe;
