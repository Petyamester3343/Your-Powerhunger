#include <conio.h>

#include "DLL_container.c"
#include "xp_sys.c"
#include "definitions.h"

// prototype definition for the method playerChoosesAction (due to being intertwined with foeChoosesAction)
PWRHNGR_DEF playerChoosesAction(Player* p, Foe* f, OBJ_DLL* ol, FOE_DLL* fl);

// Determines if there is a Foe in the Player's surrounding
// The radius is 1 unit
//(not on the Player's coordinate)

PWRHNGR_BOOLDEF checkPlayerSurrounding(Foe* f, Player* p)
{
    return
        (f->E.pos.col == p->E.pos.col + PLAYER_R && f->E.pos.row == p->E.pos.row) ||
        (f->E.pos.col == p->E.pos.col - PLAYER_R && f->E.pos.row == p->E.pos.row) ||
        (f->E.pos.col == p->E.pos.col && f->E.pos.row == p->E.pos.row + PLAYER_R) ||
        (f->E.pos.col == p->E.pos.col && f->E.pos.row == p->E.pos.row - PLAYER_R) ||
        (f->E.pos.col == p->E.pos.col + PLAYER_R && f->E.pos.row == p->E.pos.row - PLAYER_R) ||
        (f->E.pos.col == p->E.pos.col - PLAYER_R && f->E.pos.row == p->E.pos.row + PLAYER_R) ||
        (f->E.pos.col == p->E.pos.col + PLAYER_R && f->E.pos.row == p->E.pos.row + PLAYER_R) ||
        (f->E.pos.col == p->E.pos.col - PLAYER_R && f->E.pos.row == p->E.pos.row - PLAYER_R);
}

// Player attacks Foe
PWRHNGR_DEF playerAttack(Player* p, Foe* f)
{
    bool obliterated = false;

    if (p->E.hp != 0 && f->E.hp != 0)
    {
        uint pDMG = (p->E.atk > f->E.def) ? (p->E.atk - f->E.def) : 0;
        uint multi = 2;
        uint pCrit_DMG = pDMG * multi;
        int rnd = rand() % 50;
        printf("%s attacks!\n", p->E.name);
        if (rnd % 2 == 0)
        {
            if (rnd % 10 == 0)
            {
                if (pCrit_DMG < f->E.hp)
                {
                    if (pCrit_DMG > 0)
                    {
                        f->E.hp -= pCrit_DMG;
                        printf("CRITICAL DAMAGE!\t(%s: %d\t|\t%s: %d)\n", p->E.name, p->E.hp, f->E.name, f->E.hp);
                        SLEEP_MS(LATENCY);
                    }
                    else
                    {
                        f->E.hp -= 3;
                        printf("%s tanked %s's attack!\t(%s: %d\t|\t%s: %d)\n", f->E.name, p->E.name, p->E.name, p->E.hp, f->E.name, f->E.hp);
                        SLEEP_MS(LATENCY);
                    }
                }
                else
                {
                    f->E.hp = 0;
                    f->E.dead = true;
                    printf("%s obliterated %s!\n", p->E.name, f->E.name);
                    obliterated = true;
                    SLEEP_MS(LATENCY);
                }
            }
            else
            {
                if (pDMG < f->E.hp)
                {
                    if (pDMG > 0)
                    {
                        f->E.hp -= pDMG;
                        printf("Attack successful!\t(%s: %d\t|\t%s: %d)\n", p->E.name, p->E.hp, f->E.name, f->E.hp);
                        SLEEP_MS(LATENCY);
                    }
                    else
                    {
                        f->E.hp -= 1;
                        printf("%s tanked %s's attack!\t(%s: %d\t|\t%s: %d)\n", f->E.name, p->E.name, p->E.name, p->E.hp, f->E.name, f->E.hp);
                        SLEEP_MS(LATENCY);
                    }
                }
                else
                {
                    f->E.hp = 0;
                    f->E.dead = true;
                    printf("%s smoked %s!\n", p->E.name, f->E.name);
                    SLEEP_MS(LATENCY);
                }
            }
        }
        else
        {
            printf("%s missed!\n", p->E.name);
            SLEEP_MS(LATENCY);
        }
    }

    if (f->E.dead || p->E.dead)
    {
        if (f->E.dead)
        {
            if(strcmp(f->E.name, "Jimmy") == 0)
                jimmyDefeated = true;
            uint xpReward = (obliterated) ? f->E.xp * 2 : f->E.xp;
            xp_gain(xpReward, p, true);
            uint lootReward = (obliterated) ? f->loot * 2 : f->loot;
            looting(p, lootReward);
            SLEEP_MS(LATENCY);
        }
        return;
    }
}

// Foe attacks Player
PWRHNGR_DEF foeAttack(Player* p, Foe* f)
{
    if (p->E.hp != 0 && f->E.hp != 0)
    {
        uint fDMG = (f->E.atk < p->E.def) ? 0 : (f->E.atk - p->E.def);
        int multi = 2;
        uint fCrit_DMG = fDMG * multi;

        int rnd = rand() % 50;
        printf("%s attacks!\n", f->E.name);
        if (rnd % 2 == 0)
        {
            if (rnd % 10 == 0)
            {
                if (fCrit_DMG < p->E.hp)
                {
                    if (fCrit_DMG > 0)
                    {
                        p->E.hp -= fCrit_DMG;
                        printf("CRITICAL DAMAGE!\t(%s: %d\t|\t%s: %d)\n", p->E.name, p->E.hp, f->E.name, f->E.hp);
                        SLEEP_MS(LATENCY);
                    }
                    else
                    {
                        p->E.hp -= 3;
                        printf("%s tanked %s's attack!\t(%s: %d\t|\t%s: %d)\n", p->E.name, f->E.name, p->E.name, p->E.hp, f->E.name, f->E.hp);
                        SLEEP_MS(LATENCY);
                    }
                }
                else
                {
                    p->E.hp = 0;
                    p->E.dead = true;
                    printf("%s roasted %s!\n", f->E.name, p->E.name);
                    SLEEP_MS(LATENCY);
                }
            }
            else
            {
                if (fDMG <= p->E.hp)
                {
                    if (fDMG > 0)
                    {
                        p->E.hp -= fDMG;
                        printf("Attack successful!\t(%s: %d\t|\t%s: %d)\n", p->E.name, p->E.hp, f->E.name, f->E.hp);
                        SLEEP_MS(LATENCY);
                    }
                    else
                    {
                        p->E.hp -= 1;
                        printf("%s tanked %s's attack!\t(%s: %d\t|\t%s: %d)\n", p->E.name, f->E.name, p->E.name, p->E.hp, f->E.name, f->E.hp);
                        SLEEP_MS(LATENCY);
                    }
                }
                else
                {
                    p->E.hp = 0;
                    p->E.dead = true;
                    printf("%s ate %s for breakfast!\n", f->E.name, p->E.name);
                    SLEEP_MS(LATENCY);
                }
            }
        }
        else
        {
            printf("%s missed!\n", f->E.name);
            SLEEP_MS(LATENCY);
        }
    }

    if (p->E.dead || f->E.dead)
    {
        return;
    }
}

// Player casts Magia on a Foe in exchange for n MP
// (n is varying, for it is based on the Magia the Player may cast)
PWRHNGR_DEF playerCastsMagic(Player* p, Foe* f, Magia m)
{
    bool obliterated = false;

    if (p->E.hp != 0 && f->E.hp != 0)
    {
        uint multi = 2;
        uint pCrit_MAG_DMG = m.magATK * multi;
        int rnd = rand() % 50;
        if (p->mp >= m.magCost)
        {
            p->mp -= m.magCost;
            printf("%s sacrifices %d MP to cast %s on %s! (MP: %d)\n", p->E.name, m.magCost, m.magName, f->E.name, p->mp);
            if (rnd % 10 == 0)
            {
                if (pCrit_MAG_DMG > f->E.def)
                {
                    if (pCrit_MAG_DMG < f->E.hp)
                    {
                        f->E.hp -= pCrit_MAG_DMG;
                        printf("CRITICAL DAMAGE!\t(%s: %d\t|\t%s: %d)\n", p->E.name, p->E.hp, f->E.name, f->E.hp);
                        SLEEP_MS(LATENCY);
                    }
                    else
                    {
                        f->E.hp = 0;
                        f->E.dead = true;
                        obliterated = true;
                        printf("%s obliterated %s!\n", p->E.name, f->E.name);
                        SLEEP_MS(LATENCY);
                    }
                }
                else
                {
                    f->E.hp -= 10;
                    printf("%s tanked %s's attack!\t(%s: %d\t|\t%s: %d)\n", f->E.name, p->E.name, p->E.name, p->E.hp, f->E.name, f->E.hp);
                    SLEEP_MS(LATENCY);
                }
            }
            else
            {
                if (m.magATK > f->E.def)
                {
                    if (m.magATK < f->E.hp)
                    {
                        f->E.hp -= m.magATK;
                        printf("Attack successful!\t(%s: %d\t|\t%s: %d)\n", p->E.name, p->E.hp, f->E.name, f->E.hp);
                        SLEEP_MS(LATENCY);
                    }
                    else
                    {
                        f->E.hp = 0;
                        f->E.dead = true;
                        printf("%s smoked %s!\n", p->E.name, f->E.name);
                        SLEEP_MS(LATENCY);
                    }
                }
                else
                {
                    f->E.hp -= 5;
                    printf("%s tanked %s's attack!\t(%s: %d\t|\t%s: %d)\n", f->E.name, p->E.name, p->E.name, p->E.hp, f->E.name, f->E.hp);
                    SLEEP_MS(LATENCY);
                }
            }
        }
        else
        {
            printf("Not enough MP!\n");
            SLEEP_MS(LATENCY);
        }
    }

    if (f->E.dead)
    {
        uint xpReward = (obliterated) ? f->E.xp * 2 : f->E.xp;
        xp_gain(xpReward, p, true);
        uint lootReward = (obliterated) ? f->loot : f->loot / 2;
        looting(p, lootReward);
        return;
    }
}

// Allows the Player to choose from their acquired spells
PWRHNGR_DEF playerChoosesMagic(Player* p, Foe* f)
{
    char magChoice[7] = { '>', ' ', ' ', ' ', ' ', ' ', ' ' };

    while (!p->E.fled && !p->E.dead)
    {
        system("cls");
        printf("Choose your spell:\n");
        if (p->M[0].acquired)
            printf("\t%c %s (%d MP)\n", magChoice[0], p->M[0].magName, p->M[0].magCost);
        else
            printf("\t%c -\n", magChoice[0]);

        if (p->M[1].acquired)
            printf("\t%c %s (%d MP)\n", magChoice[1], p->M[1].magName, p->M[1].magCost);
        else
            printf("\t%c -\n", magChoice[1]);

        if (p->M[2].acquired)
            printf("\t%c %s (%d MP)\n", magChoice[2], p->M[2].magName, p->M[2].magCost);
        else
            printf("\t%c -\n", magChoice[2]);

        if (p->M[3].acquired)
            printf("\t%c %s (%d MP)\n", magChoice[3], p->M[3].magName, p->M[3].magCost);
        else
            printf("\t%c -\n", magChoice[3]);

        if (p->M[4].acquired)
            printf("\t%c %s (%d MP)\n", magChoice[4], p->M[4].magName, p->M[4].magCost);
        else
            printf("\t%c -\n", magChoice[4]);

        if (p->M[5].acquired && strcmp(AKARI, p->M[5].magName) == 0)
            printf("\t%c %s (%d MP)\n", magChoice[5], p->M[5].magName, p->M[5].magCost);
        else
            printf("\t%c -\n", magChoice[5]);

        printf("\t%c EXIT\n\n", magChoice[6]);
        char in = getch();
        switch (in)
        {
        case 'w':
            if (magChoice[0] == '>')
            {
                magChoice[6] = '>';
                magChoice[0] = ' ';
            }
            else if (magChoice[1] == '>')
            {
                magChoice[0] = '>';
                magChoice[1] = ' ';
            }
            else if (magChoice[2] == '>')
            {
                magChoice[1] = '>';
                magChoice[2] = ' ';
            }
            else if (magChoice[3] == '>')
            {
                magChoice[2] = '>';
                magChoice[3] = ' ';
            }
            else if (magChoice[4] == '>')
            {
                magChoice[3] = '>';
                magChoice[4] = ' ';
            }
            else if (magChoice[5] == '>')
            {
                magChoice[4] = '>';
                magChoice[5] = ' ';
            }
            else
            {
                magChoice[5] = '>';
                magChoice[6] = ' ';
            }
            break;
        case 's':
            if (magChoice[0] == '>')
            {
                magChoice[1] = '>';
                magChoice[0] = ' ';
            }
            else if (magChoice[1] == '>')
            {
                magChoice[2] = '>';
                magChoice[1] = ' ';
            }
            else if (magChoice[2] == '>')
            {
                magChoice[3] = '>';
                magChoice[2] = ' ';
            }
            else if (magChoice[3] == '>')
            {
                magChoice[4] = '>';
                magChoice[3] = ' ';
            }
            else if (magChoice[4] == '>')
            {
                magChoice[5] = '>';
                magChoice[4] = ' ';
            }
            else if (magChoice[5] == '>')
            {
                magChoice[6] = '>';
                magChoice[5] = ' ';
            }
            else
            {
                magChoice[0] = '>';
                magChoice[6] = ' ';
            }
            break;
        case 13:
            if (magChoice[0] == '>')
            {
                if (p->M[0].acquired)
                {
                    playerCastsMagic(p, f, p->M[0]);
                    return;
                }
                else
                {
                    printf("%s hasn't unlocked this spell yet!\n", p->E.name);
                }
            }
            else if (magChoice[1] == '>')
            {
                if (p->M[1].acquired)
                {
                    playerCastsMagic(p, f, p->M[1]);
                    return;
                }
                else
                {
                    printf("%s hasn't unlocked this spell yet!\n", p->E.name);
                }
            }
            else if (magChoice[2] == '>')
            {
                if (p->M[2].acquired)
                {
                    playerCastsMagic(p, f, p->M[2]);
                    return;
                }
                else
                {
                    printf("%s hasn't unlocked this spell yet!\n", p->E.name);
                }
            }
            else if (magChoice[3] == '>')
            {
                if (p->M[3].acquired)
                {
                    playerCastsMagic(p, f, p->M[3]);
                    return;
                }
                else
                {
                    printf("%s hasn't unlocked this spell yet!\n", p->E.name);
                }
            }
            else if (magChoice[4] == '>')
            {
                if (p->M[4].acquired)
                {
                    playerCastsMagic(p, f, p->M[4]);
                    return;
                }
                else
                {
                    printf("%s hasn't unlocked this spell yet!\n", p->E.name);
                }
            }
            else if (magChoice[5] == '>')
            {
                if (p->M[5].acquired)
                {
                    playerCastsMagic(p, f, p->M[5]);
                    return;
                }
                else
                {
                    printf("%s hasn't purchased this spell yet!\n", p->E.name);
                }
            }
            else return;
        }
    }
}

// Foes may weaken the Player
PWRHNGR_DEF weakenPlayer(Player* p, Foe* f)
{
    printf("%s screams at %s. It tries to make the wanderer afraid.\n", f->E.name, p->E.name);
    if(rand()%50 > 25)
    {
        p->E.def -= 5;
        printf("The wanderer stepped back. DEF \x1b[31m-5!\x1b[0m\n");
    }
    else printf("The wanderer made a deadstare at the %s, causing the entity to silence itself.\n", f->E.name);
}

// Foe acts
PWRHNGR_DEF foeChoosesAction(Player* p, Foe* f, OBJ_DLL* ol, FOE_DLL* fl)
{
    f->E.fled = false;
    bool defChanged = false;

    /*
    1. elem.: ATTACK
    2. elem.: DEFEND
    3. elem.: SUMMON (only Acolyte) / SCREAM (weakening)
    4. elem.: FLEE
    */

    while ((!f->E.fled && !p->E.fled && !p->E.dead && !f->E.dead)
            && (f->E.hp != 0 && p->E.hp != 0))
    {
        switch (rand() % 100)
        {
        default:
            // Defaulting to attack Player
            if (!f->E.fled && !p->E.fled)
                foeAttack(p, f);
            break;
        case 10:
            // Case of Foe "hardening its skin"
            f->E.def += 5;
            defChanged = true;
            printf("%s hardened its skin!\n(+5 enemy DEF!)\n", f->E.name);
            SLEEP_MS(LATENCY);
            break;
        case 25:
            if(strcmp(f->E.name, ACOLYTE_NAME) == 0)
            {
                if(rand() % 2 == 0)
                {
                    switch(rand() % 8)
                    {
                    default:
                        insertFoeIntoList(p, ol, fl, SLIME_NAME, SLIME_HP, SLIME_ATK, SLIME_DEF, SLIME_XP, SLIME_LOOT);
                        printf("The Acolyte summoned a Slime on the map!\n");
                        break;
                    case 5:
                        insertFoeIntoList(p, ol, fl, GOBLIN_NAME, GOBLIN_HP, GOBLIN_ATK, GOBLIN_DEF, GOBLIN_XP, GOBLIN_LOOT);
                        printf("The Acolyte summoned a Goblin on the map!\n");
                        break;
                    case 6:
                        insertFoeIntoList(p, ol, fl, ACOLYTE_NAME, ACOLYTE_HP, ACOLYTE_ATK, ACOLYTE_DEF, ACOLYTE_XP, ACOLYTE_LOOT);
                        printf("The Acolyte summoned one of his brethren on the map!\n");
                        break;
                    case 7:
                        insertFoeIntoList(p, ol, fl, THWARTED_SELF_NAME, THWARTED_SELF_HP, THWARTED_SELF_ATK, THWARTED_SELF_DEF, THWARTED_SELF_XP, THWARTED_SELF_LOOT);
                        printf("The Acolyte summoned your Thwarted Self on the map!\n");
                        break;
                    case 8:
                        insertFoeIntoList(p, ol, fl, GOLEM_NAME, GOLEM_HP, GOLEM_ATK, GOLEM_DEF, GOLEM_XP, GOLEM_LOOT);
                        printf("The Acolyte summoned a Carcass Golem on the map!\n");
                        break;
                    }
                }
                else weakenPlayer(p, f);
            }
            else weakenPlayer(p, f);

            SLEEP_MS(LATENCY);
            break;
        case 50:
            // Case of Foe fleeing from the Player's divine wrath
            if(f->E.hp <= 25 && strcmp(f->E.name, BOSS_NAME) != 0)
            {
                f->E.fled = true;
                break;
            }
        }

        // Player's DEF returns to original after each turn
        if (p->E.def != PLAYER_DEF)
        {
            p->E.def = PLAYER_DEF;
        }

        // Foe's DEF returns to original
        if (defChanged)
        {
            f->E.def -= 5;
        }

        // In case the Foes is still there
        if (!f->E.fled)
        {
            playerChoosesAction(p, f, ol, fl);
        }
        else
        {
            printf("%s has fled the scene!\n", f->E.name);
            SLEEP_MS(LATENCY*2);
            return;
        }
    }
}

// Allows the Player to initiate attack when they have a Foe in their vicinity
PWRHNGR_DEF impendingDoom(Player* p, FOE_DLL* foeList, OBJ_DLL* ol)
{
    FoeNode* curr = foeList->head;
    while (curr != NULL)
    {
        if (checkPlayerSurrounding(curr->f, p))
        {
            printf("%s noticed an instance of %s skulking around him (X: %d, Y: %d)\n", p->E.name, curr->f->E.name, curr->f->E.pos.col + 1, curr->f->E.pos.row + 1);
            int randGen = rand()%2;
            switch(randGen)
            {
            case 0:
                printf("%s was bamboozled by %s cathching it off guard!\n", curr->f->E.name, p->E.name);
                SLEEP_MS(LATENCY);
                playerChoosesAction(p, curr->f, ol, foeList);
                break;
            case 1:
                printf("%s has already noticed %s. The monster outsmarted the wanderer's outsmarting!\n", curr->f->E.name, p->E.name);
                SLEEP_MS(LATENCY);
                foeChoosesAction(p, curr->f, ol, foeList);
                break;
            }
        }
        curr = curr->next;
    }
}

// Player chooses an action
PWRHNGR_DEF playerChoosesAction(Player* p, Foe* f, OBJ_DLL* ol, FOE_DLL* fl)
{
    char in = ' ';
    char actChoice[5] = { '>', ' ', ' ', ' ', ' ' };
    p->E.fled = false;
    bool chose = false;

    uint enhDEF = 0;

    /*
    1. elem.: ATTACK
    2. elem.: DEFEND
    3. elem.: MAGIC
    4. elem.: HEAL
    5. elem.: FLEE
    */

    // The choosing segment
    while (!p->E.fled && !p->E.dead && !f->E.dead && !f->E.fled)
    {
        system("cls");
        printf("%s's HP: %d\n%s's MP: %d\n\n", p->E.name, p->E.hp, p->E.name, p->mp);
        printf("%s's HP: %d\n\n", f->E.name, f->E.hp);
        printf("Choose an action:\n\t%c ATTACK\n\t%c DEFEND\n\t%c MAGIC\n\t%c HEAL\n\t%c FLEE\n\n", actChoice[0], actChoice[1], actChoice[2], actChoice[3], actChoice[4]);

        in = getch();
        switch (in)
        {
        case 'w':
            if (actChoice[0] == '>')
            {
                actChoice[4] = '>';
                actChoice[0] = ' ';
            }
            else if (actChoice[1] == '>')
            {
                actChoice[0] = '>';
                actChoice[1] = ' ';
            }
            else if (actChoice[2] == '>')
            {
                actChoice[1] = '>';
                actChoice[2] = ' ';
            }
            else if (actChoice[3] == '>')
            {
                actChoice[2] = '>';
                actChoice[3] = ' ';
            }
            else
            {
                actChoice[3] = '>';
                actChoice[4] = ' ';
            }
            break;
        case 's':
            if (actChoice[0] == '>')
            {
                actChoice[1] = '>';
                actChoice[0] = ' ';
            }
            else if (actChoice[1] == '>')
            {
                actChoice[2] = '>';
                actChoice[1] = ' ';
            }
            else if (actChoice[2] == '>')
            {
                actChoice[3] = '>';
                actChoice[2] = ' ';
            }
            else if (actChoice[3] == '>')
            {
                actChoice[4] = '>';
                actChoice[3] = ' ';
            }
            else
            {
                actChoice[0] = '>';
                actChoice[4] = ' ';
            }
            break;
        case 13:
            chose = true;
            // If the Player chooses to attack the Foe
            if (actChoice[0] == '>')
            {
                playerAttack(p, f);
            }
            // If the Player chooses to initiate his defenses
            // I'd make it dependent on the Aegis Cores the Player picked up
            else if (actChoice[1] == '>')
            {
                if(p->aegisPickedUp > 1)
                {
                    enhDEF = p->E.def + (OBJ_DEF_BUFF * p->aegisPickedUp);
                    p->E.def = enhDEF;
                    printf("%d Aegis Cores acivated. %s's DEF is now strengthtened! (DEF: %d)", p->aegisPickedUp, p->E.name, p->E.def);
                }
                else if (p->aegisPickedUp == 1)
                {
                    enhDEF = p->E.def + OBJ_DEF_BUFF;
                    p->E.def = enhDEF;
                    printf("Aegis Core activated. %s's DEF is slightly strengthened! (DEF: %d)", p->E.name, p->E.def);

                }
                else
                {
                    enhDEF = p->E.def + (OBJ_DEF_BUFF-3);
                    p->E.def = enhDEF;
                    printf("%s is foucsing on the defensive with a blocking stance. (DEF: %d)", p->E.name, p->E.def);
                }
                SLEEP_MS(LATENCY);
            }
            // If the Player chooses to cast Magia on the Foe
            else if (actChoice[2] == '>')
            {
                bool hasMagic = false;
                for(uint i = 0; i<SKILL_ROSTER; i++)
                {
                    if(p->M[i].acquired)
                    {
                        hasMagic = true;
                    }
                }
                if (hasMagic)
                {
                    uint mana = p->mp;
                    uint prevMana = mana;
                    playerChoosesMagic(p, f);
                    mana = p->mp;
                    if (mana == prevMana)
                        chose = false;
                }
                else
                {
                    printf("You haven't unlocked any spells yet!\n");
                    chose = false;
                    SLEEP_MS(LATENCY);
                }
            }
            // If the Player decides to heal himself
            else if (actChoice[3] == '>')
            {
                printf("%s ate some bread! HP +20", p->E.name);
                if (p->E.hp < PLAYER_MAX_HP)
                    p->E.hp += 20;
                else
                    p->E.hp = PLAYER_MAX_HP;
                SLEEP_MS(LATENCY);
            }
            // If the player flees the scene
            else
            {
                p->E.fled = true;
                chose = false;
            }
            break;
        }
        system("cls");
        if (!chose) continue;

        if (p->E.fled)
        {
            printf("%s has fled the scene!\n", p->E.name);
            SLEEP_MS(LATENCY);
            return;
        }

        // In case both the quarries are alive and has not fled yet
        if (!f->E.dead && (!f->E.fled || !p->E.fled) && chose)
        {
            system("cls");
            foeChoosesAction(p, f, ol, fl);
        }
    }

    return;
}
