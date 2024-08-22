#include "def_vals.h"
#include "entities.h"
#include "definitions.h"

#include <stdio.h>

// Allows the Player to level up
PWRHNGR_DEF level_up(Player* p)
{
    if (p->E.xp >= LV_UP_REQ(p->lv))
    {
        p->lv++;

        if(strcmp(p->E.name, "Saki")==0)
        {
            PLAYER_MAX_HP += 5;
            PLAYER_MAX_MP += 5;
            PLAYER_ATK += 5;
            PLAYER_DEF += 5;
        }
        else
        {
            PLAYER_MAX_MP += 10;
            PLAYER_MAX_HP += 10;
            PLAYER_ATK += 10;
            PLAYER_DEF += 10;
        }

        p->E.hp = PLAYER_MAX_HP;
        p->E.atk = PLAYER_ATK;
        p->E.def = PLAYER_DEF;
        p->mp = PLAYER_MAX_MP;

        printf("%s is now at LEVEL %d!\n", p->E.name, p->lv);

        if (p->lv == FIREBALL_LV_REQ && !p->M[0].acquired)
        {
            p->M[0].acquired = true;
            printf("%s has acquired %s!\n", p->E.name, FB);
        }
        if (p->lv == BLIZZARD_LV_REQ && !p->M[1].acquired)
        {
            p->M[1].acquired = true;
            printf("%s has acquired %s!\n", p->E.name, BLIZZ);
        }
        if (p->lv == LIGHTNING_LV_REQ && !p->M[2].acquired)
        {
            p->M[2].acquired = true;
            printf("%s has acquired %s!\n", p->E.name, RAI);
        }
        if (p->lv == GRAV_BOMB_LV_REQ && !p->M[3].acquired)
        {
            p->M[3].acquired = true;
            printf("%s has acquired %s!\n", p->E.name, GRAV);
        }
        if (p->lv == LOEWENHERZ_LV_REQ && !p->M[4].acquired)
        {
            p->M[4].acquired = true;
            printf("%s has acquired %s!\n", p->E.name, LIONHEART);
        }
        SLEEP_MS(500);
    }
}

// Grants the player experience points
// (Either from an Object or from slaying a Foe)
PWRHNGR_DEF xp_gain(uint experience, Player* p, bool fought)
{
    p->E.xp += experience;
    if (fought)
        printf("%s has gained %d XP from the battle! (Current: %d / %d)\n", p->E.name, experience, p->E.xp, LV_UP_REQ(p->lv));
    else
        printf("(Current: %d / %d)\n", p->E.xp, LV_UP_REQ(p->lv));

    while (p->E.xp >= LV_UP_REQ(p->lv))
        level_up(p);
}

// Grants the player GOLD from slaying a Foe
// GOLD can be used to buy permanent upgrades in Shop
PWRHNGR_DEF looting(Player* p, uint money)
{
    p->money += money;
    printf("%d GOLD gained!\n", money);
    SLEEP_MS(500);
}
