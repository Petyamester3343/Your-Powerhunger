#pragma once

#include <string.h>

#include "entities.h"
#include "definitions.h"

// Indicators of Jimmy having been defeated and summoned
static bool jimmyDefeated = false;
static bool jimmySummoned = false;

// Level up value, used to incrememnt the Player's numeral stats
const PWRHNGR_UINT LVUP_VAL = 10;


// PLAYER DEFAULT STATS
PWRHNGR_UINT PLAYER_MAX_HP = 100;
PWRHNGR_UINT PLAYER_MAX_MP = 50;
PWRHNGR_UINT PLAYER_ATK = 10;
PWRHNGR_UINT PLAYER_DEF = 10;
static Pos PLAYER_START_POS = { 7,7 };

// Initializes the Player struct
// OR returns it to the default values (after a game over)
PWRHNGR_DEF defPlayerValRestore(Player* p) {
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
	p->deathCount = 0;
}


// Enemies

// SLIME DEFAULT STATS
PWRHNGR_STR SLIME_NAME = "Slime";
PWRHNGR_UINT SLIME_HP = 15;
const PWRHNGR_UINT SLIME_ATK = 3;
PWRHNGR_UINT SLIME_DEF = 2;
const PWRHNGR_UINT SLIME_XP = 10;
const PWRHNGR_UINT SLIME_LOOT = 10;

// GOBLIN DEFAULT STATS
PWRHNGR_STR GOBLIN_NAME = "Goblin";
PWRHNGR_UINT GOBLIN_HP = 25;
const PWRHNGR_UINT GOBLIN_ATK = 7;
PWRHNGR_UINT GOBLIN_DEF = 6;
const PWRHNGR_UINT GOBLIN_XP = 20;
const PWRHNGR_UINT GOBLIN_LOOT = 20;

// ACOLYTE DEFAULT STATS
PWRHNGR_STR ACOLYTE_NAME = "Acolyte";
PWRHNGR_UINT ACOLYTE_HP = 40;
const PWRHNGR_UINT ACOLYTE_ATK = 12;
PWRHNGR_UINT ACOLYTE_DEF = 10;
const PWRHNGR_UINT ACOLYTE_XP = 40;
const PWRHNGR_UINT ACOLYTE_LOOT = 40;

// THWARTED SELF DEFAULT STATS
PWRHNGR_STR THWARTED_SELF_NAME = "Thwarted Self";
PWRHNGR_UINT THWARTED_SELF_HP = 70;
const PWRHNGR_UINT THWARTED_SELF_ATK = 20;
PWRHNGR_UINT THWARTED_SELF_DEF = 14;
const PWRHNGR_UINT THWARTED_SELF_XP = 80;
const PWRHNGR_UINT THWARTED_SELF_LOOT = 80;

// CARCASS GOLEM DEFAULT STATS
PWRHNGR_STR GOLEM_NAME = "Carcass Golem";
PWRHNGR_UINT GOLEM_HP = 90;
const PWRHNGR_UINT GOLEM_ATK = 30;
PWRHNGR_UINT GOLEM_DEF = 20;
const PWRHNGR_UINT GOLEM_XP = 160;
const PWRHNGR_UINT GOLEM_LOOT = 160;

// JIMMY DEFAULT STATS
PWRHNGR_STR BOSS_NAME = "Jimmy";
PWRHNGR_UINT BOSS_HP = 150;
const PWRHNGR_UINT BOSS_ATK = 50;
PWRHNGR_UINT BOSS_DEF = 40;


// Magic

// FIREBALL DEFAULT STATS (1.)
PWRHNGR_STR FB = "Homing Fireball";
const PWRHNGR_UINT FIREBALL_ATK = 10;
const PWRHNGR_UINT FIREBALL_COST = 5;
const PWRHNGR_UINT FIREBALL_LV_REQ = 2;

// BLIZZARD DEFAULT STATS (2.)
PWRHNGR_STR BLIZZ = "Blades of the Blizzard Queen";
const PWRHNGR_UINT BLIZZARD_ATK = 20;
const PWRHNGR_UINT BLIZZARD_COST = 10;
const PWRHNGR_UINT BLIZZARD_LV_REQ = 4;

// LIGHTNING DEFAULT STATS (3.)
PWRHNGR_STR RAI = "Raijin's Wrath";
const PWRHNGR_UINT LIGHTNING_ATK = 25;
const PWRHNGR_UINT LIGHTNING_COST = 15;
const PWRHNGR_UINT LIGHTNING_LV_REQ = 6;

// GRAVITY_BOMB DEFAULT STATS (4.)
PWRHNGR_STR GRAV = "Gravity Bomb";
const PWRHNGR_UINT GRAV_BOMB_ATK = 35;
const PWRHNGR_UINT GRAV_BOMB_COST = 25;
const PWRHNGR_UINT GRAV_BOMB_LV_REQ = 7;

// LOEWENHERZ DEFAULT STATS (5.)
PWRHNGR_STR LIONHEART = "Ray of the Lionhearted";
const PWRHNGR_UINT LOEWENHERZ_ATK = 40;
const PWRHNGR_UINT LOEWENHERZ_COST = 35;
const PWRHNGR_UINT LOEWENHERZ_LV_REQ = 9;

// RSoJ DEFAULT STATS (6.)
PWRHNGR_STR AKARI = "Raining Spears of Justice";
const PWRHNGR_UINT RSOJ_ATK = 60;
const PWRHNGR_UINT RSOJ_COST = 50;
const PWRHNGR_UINT RSOJ_LV_REQ = 0;
const PWRHNGR_UINT RSOJ_PRICE = 1250;

// Initializes the Player's Magia array
PWRHNGR_DEF initPlayerMagic(Player* p) {
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
