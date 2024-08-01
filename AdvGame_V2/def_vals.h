#pragma once

#include <string.h>

#include "entities.h"

// DEFAULT MAP SIZE
#define MAP_SIZE 15

// DEFAULT NORMAL OBJECT BUFF VALUE
#define BUFF 5

// Indicators of Jimmy having been defeated and summoned
static bool jimmyDefeated = false;
static bool jimmySummoned = false;

// Level up value, used to incrememnt the Player's numeral stats
const static unsigned int LVUP_VAL = 10;



// PLAYER DEFAULT STATS
static unsigned int PLAYER_MAX_HP = 100;
static unsigned int PLAYER_MAX_MP = 50;
static unsigned int PLAYER_ATK = 10;
static unsigned int PLAYER_DEF = 10;
static Pos PLAYER_START_POS = { 7,7 };

static void defPlayerValRestore(Player* p) {
	PLAYER_MAX_HP = 100;
	PLAYER_MAX_MP = 50;
	PLAYER_ATK = 10;
	PLAYER_DEF = 10;

	p->E.hp = PLAYER_MAX_HP;
	p->E.atk = PLAYER_ATK;
	p->E.def = PLAYER_DEF;
	p->E.xp = 0;
	p->E.pos = PLAYER_START_POS;
	p->E.dead = false;

	p->lv = 1;
	p->mp = PLAYER_MAX_MP;
	p->boughtMagic = false;
	p->boughtATK = false;
	p->boughtDEF = false;
	p->boughtVigor = false;

	p->aegisPickedUp = 0;
}



// Enemies

// SLIME DEFAULT STATS
static const char* SLIME_NAME = "Slime";
static unsigned int SLIME_HP = 15;
static const unsigned int SLIME_ATK = 3;
static unsigned int SLIME_DEF = 2;
static const unsigned int SLIME_XP = 10;
static const unsigned int SLIME_LOOT = 10;

// GOBLIN DEFAULT STATS
static const char* GOBLIN_NAME = "Goblin";
static unsigned int GOBLIN_HP = 25;
static const unsigned int GOBLIN_ATK = 7;
static unsigned int GOBLIN_DEF = 6;
static const unsigned int GOBLIN_XP = 20;
static const unsigned int GOBLIN_LOOT = 20;

// ACOLYTE DEFAULT STATS
static const char* ACOLYTE_NAME = "Acolyte";
static unsigned int ACOLYTE_HP = 40;
static const unsigned int ACOLYTE_ATK = 12;
static unsigned int ACOLYTE_DEF = 10;
static const unsigned int ACOLYTE_XP = 40;
static const unsigned int ACOLYTE_LOOT = 40;

// THWARTED SELF DEFAULT STATS
static const char* THWARTED_SELF_NAME = "Thwarted Self";
static unsigned int THWARTED_SELF_HP = 70;
static const unsigned int THWARTED_SELF_ATK = 20;
static unsigned int THWARTED_SELF_DEF = 14;
static const unsigned int THWARTED_SELF_XP = 80;
static const unsigned int THWARTED_SELF_LOOT = 80;

// CARCASS GOLEM DEFAULT STATS
static const char* GOLEM_NAME = "Carcass Golem";
static unsigned int GOLEM_HP = 90;
static const unsigned int GOLEM_ATK = 30;
static unsigned int GOLEM_DEF = 20;
static const unsigned int GOLEM_XP = 160;
static const unsigned int GOLEM_LOOT = 160;

// JIMMY DEFAULT STATS
static const char* BOSS_NAME = "Jimmy";
static unsigned int JIMMY_HP = 150;
static const unsigned int JIMMY_ATK = 50;
static unsigned int JIMMY_DEF = 40;



// Magic

// FIREBALL DEFAULT STATS (1.)
static const char* FB = "Homing Fireball";
static unsigned int FIREBALL_ATK = 10;
static unsigned int FIREBALL_COST = 5;
static unsigned int FIREBALL_LV_REQ = 2;

// BLIZZARD DEFAULT STATS (2.)
static const char* BLIZZ = "Blades of the Blizzard Queen";
static unsigned int BLIZZARD_ATK = 20;
static unsigned int BLIZZARD_COST = 10;
static unsigned int BLIZZARD_LV_REQ = 4;

// LIGHTNING DEFAULT STATS (3.)
static const char* RAI = "Raijin's Wrath";
static unsigned int LIGHTNING_ATK = 25;
static unsigned int LIGHTNING_COST = 15;
static unsigned int LIGHTNING_LV_REQ = 6;

// GRAVITY_BOMB DEFAULT STATS (4.)
static const char* GRAV = "Gravity Bomb";
static unsigned int GRAV_BOMB_ATK = 35;
static unsigned int GRAV_BOMB_COST = 25;
static unsigned int GRAV_BOMB_LV_REQ = 7;

// LOEWENHERZ DEFAULT STATS (5.)
static const char* LIONHEART = "Ray of the Lionhearted";
static unsigned int LOEWENHERZ_ATK = 40;
static unsigned int LOEWENHERZ_COST = 35;
static unsigned int LOEWENHERZ_LV_REQ = 9;

// RSoJ DEFAULT STATS (6.)
static const char* AKARI = "Raining Spears of Justice";
static unsigned int RSOJ_ATK = 60;
static unsigned int RSOJ_COST = 50;
static unsigned int RSOJ_LV_REQ = 0;
static unsigned int RSOJ_PRICE = 1250;


// Initializes the Player's Magia roster
static void initPlayerMagic(Player* p) {
	// initializing magic
	for (unsigned int i = 0; i < 6; i++) {
		switch (i) {
		case 0:
			p->M[i].magName = strdup(FB);
			p->M[i].magATK = FIREBALL_ATK;
			p->M[i].magCost = FIREBALL_COST;
			p->M[i].magLVReq = FIREBALL_LV_REQ;
			p->M[i].acquired = (p->lv >= FIREBALL_LV_REQ) ? true : false;
			break;
		case 1:
			p->M[i].magName = strdup(BLIZZ);
			p->M[i].magATK = BLIZZARD_ATK;
			p->M[i].magCost = BLIZZARD_COST;
			p->M[i].magLVReq = BLIZZARD_LV_REQ;
			p->M[i].acquired = (p->lv >= BLIZZARD_LV_REQ) ? true : false;
			break;
		case 2:
			p->M[i].magName = strdup(RAI);
			p->M[i].magATK = LIGHTNING_ATK;
			p->M[i].magCost = LIGHTNING_COST;
			p->M[i].magLVReq = LIGHTNING_LV_REQ;
			p->M[i].acquired = (p->lv >= LIGHTNING_LV_REQ) ? true : false;
			break;
		case 3:
			p->M[i].magName = strdup(GRAV);
			p->M[i].magATK = GRAV_BOMB_ATK;
			p->M[i].magCost = GRAV_BOMB_COST;
			p->M[i].magLVReq = GRAV_BOMB_LV_REQ;
			p->M[i].acquired = (p->lv >= GRAV_BOMB_LV_REQ) ? true : false;
			break;
		case 4:
			p->M[i].magName = strdup(LIONHEART);
			p->M[i].magATK = LOEWENHERZ_ATK;
			p->M[i].magCost = LOEWENHERZ_COST;
			p->M[i].magLVReq = LOEWENHERZ_LV_REQ;
			p->M[i].acquired = (p->lv >= LOEWENHERZ_LV_REQ) ? true : false;;
			break;
		case 5:
			p->M[i].magName = (!p->boughtMagic) ? strdup("PLACEHOLDER TEXT") : strdup(AKARI);
			p->M[i].magATK = (!p->boughtMagic) ? 0 : RSOJ_ATK;
			p->M[i].magCost = (!p->boughtMagic) ? 0 : RSOJ_COST;
			p->M[i].magLVReq = 0;
			p->M[i].acquired = (!p->boughtMagic) ? false : true;
		}
	}
}
