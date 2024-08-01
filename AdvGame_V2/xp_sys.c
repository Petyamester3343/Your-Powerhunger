#include "def_vals.h"
#include "entities.h"

#include <stdio.h>

// Mathematical formula for the Player
// Once reached, they will ascend to the next level
#define LV_UP_REQ(lv) ((lv)*(lv)*25)

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms)*1000)
#endif // _WIN32

#define PWRHNGRDEFINE static inline

// Allows the character to level up
PWRHNGRDEFINE void level_up(Player* p) {
	if (p->E.xp >= LV_UP_REQ(p->lv)) {
		p->lv++;

		PLAYER_MAX_HP += 10;
		PLAYER_ATK += 10;
		PLAYER_DEF += 10;
		PLAYER_MAX_MP += 10;

		p->E.hp = PLAYER_MAX_HP;
		p->E.atk = PLAYER_ATK;
		p->E.def = PLAYER_DEF;
		p->mp = PLAYER_MAX_MP;

		printf("%s is now at LEVEL %d!\n", p->E.name, p->lv);

		if (p->lv == FIREBALL_LV_REQ && !p->M[0].acquired) {
			p->M[0].acquired = true;
			printf("%s has acquired %s!\n", p->E.name, FB);
		}
		if (p->lv == BLIZZARD_LV_REQ && !p->M[1].acquired) {
			p->M[1].acquired = true;
			printf("%s has acquired %s!\n", p->E.name, BLIZZ);
		}
		if (p->lv == LIGHTNING_LV_REQ && !p->M[2].acquired) {
			p->M[2].acquired = true;
			printf("%s has acquired %s!\n", p->E.name, RAI);
		}
		if (p->lv == GRAV_BOMB_LV_REQ && !p->M[3].acquired) {
			p->M[3].acquired = true;
			printf("%s has acquired %s!\n", p->E.name, GRAV);
		}
		if (p->lv == LOEWENHERZ_LV_REQ && !p->M[4].acquired) {
			p->M[4].acquired = true;
			printf("%s has acquired %s!\n", p->E.name, LIONHEART);
		}
		SLEEP_MS(500);
	}
}

// Grants the player experience points
// (Either from an Arcane Tome or from slaying a Foe)
PWRHNGRDEFINE void xp_gain(unsigned int experience, Player* p, bool fought) {
	p->E.xp += experience;
	if (fought)
		printf("%s has gained %d XP from the battle! (Current: %d / %d)\n",
			p->E.name, experience, p->E.xp, LV_UP_REQ(p->lv));
	else
		printf("(Current: %d / %d)\n", p->E.xp, LV_UP_REQ(p->lv));
	while (p->E.xp >= LV_UP_REQ(p->lv)) {
		level_up(p);
	}
}

// Grants the player GOLD from slaying a Foe
// The first can be used to buy upgrades in Shop
PWRHNGRDEFINE void looting(Player* p, unsigned int money) {
	p->money += money;
	printf("%d GOLD gained!\n", money);
	SLEEP_MS(500);
}
