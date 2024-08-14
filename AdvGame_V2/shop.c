#include "entities.h"
#include "def_vals.h"
#include "definitions.h"

#include <stdio.h>
#include <conio.h>


// The shop system (practically an upgrade system where one must sacrifice GOLD to obtain more power)
PWRHNGR_DEF go2Shop(Player* p)
{
    char choice[5] = { '>', ' ', ' ', ' ', ' ' };
    char exitChoice[2] = { ' ', '>' };
    char input = ' ';

    // Shop loop
    while (1)
    {
        system("cls");
        printf("Welcome, weary traveler!\nFeel free to browse through my wares!\n");
        printf("\t%c %s\t->\t\t$%d\n", choice[0], AKARI, RSOJ_PRICE);
        printf("\t%c %s\t->\t\t$%d\n", choice[1], "Aegis Automatized Armor", ARMOR_PRICE);
        printf("\t%c %s\t\t->\t\t$%d\n", choice[2], "Plasma Railgun", PLASMA_GUN_PRICE);
        printf("\t%c %s\t\t->\t\t$%d\n", choice[3], "Vigor Enhancer", INVIGORATOR_PRICE);
        printf("\t%c %s\n\n", choice[4], "EXIT SHOP");

        bool leaving = false;
        input = getch();
        switch (input)
        {
        case 'w':
            if (choice[0] == '>')
            {
                choice[4] = '>';
                choice[0] = ' ';
            }
            else if (choice[1] == '>')
            {
                choice[0] = '>';
                choice[1] = ' ';
            }
            else if (choice[2] == '>')
            {
                choice[1] = '>';
                choice[2] = ' ';
            }
            else if (choice[3] == '>')
            {
                choice[2] = '>';
                choice[3] = ' ';
            }
            else if (choice[4] == '>')
            {
                choice[3] = '>';
                choice[4] = ' ';
            }
            break;
        case 's':
            if (choice[0] == '>')
            {
                choice[1] = '>';
                choice[0] = ' ';
            }
            else if (choice[1] == '>')
            {
                choice[2] = '>';
                choice[1] = ' ';
            }
            else if (choice[2] == '>')
            {
                choice[3] = '>';
                choice[2] = ' ';
            }
            else if (choice[3] == '>')
            {
                choice[4] = '>';
                choice[3] = ' ';
            }
            else if (choice[4] == '>')
            {
                choice[0] = '>';
                choice[4] = ' ';
            }
            break;
        case 13:
            // If Player has enough money, they can obtain the upgrade, and the fee will be deducted from their purse.
            // Alt. case I.: If the player has already acquired a certain upgrade, they may not acquire it again.
            // Alt. case II.: If the player hasn't got enough GOLD, they may not acquire the upgrade.
            if (choice[0] == '>')
            {
                if (p->money >= RSOJ_PRICE && !p->boughtMagic)
                {
                    p->money -= RSOJ_PRICE;
                    p->M[5].magName = strdup(AKARI);
                    p->M[5].magATK = RSOJ_ATK;
                    p->M[5].magCost = RSOJ_COST;
                    p->M[5].magLVReq = RSOJ_LV_REQ;
                    p->M[5].acquired = true;
                    p->boughtMagic = true;
                    printf("%s has successfully acquired the most powerful spell known to mankind!\n", p->E.name);
                    SLEEP_MS(LATENCY);
                    break;
                }
                else if(p->boughtMagic)
                {
                    printf("%s has already acquired the %s!\n", p->E.name, AKARI);
                    SLEEP_MS(LATENCY);
                }
                else if (p->money < RSOJ_PRICE)
                {
                    printf("Not enough money!\n");
                    SLEEP_MS(LATENCY);
                }
            }
            if (choice[1] == '>')
            {
                if (p->money >= ARMOR_PRICE && !p->boughtDEF)
                {
                    p->money -= ARMOR_PRICE;
                    p->E.def += PERM_BUFF;
                    p->boughtDEF = true;
                    printf("%s has purchased the Aegis Automatized Armor! DEF +30\n", p->E.name);
                    SLEEP_MS(LATENCY);
                    break;
                }
                else if (p->boughtDEF)
                {
                    printf("%s has already acquired this upgrade!\n", p->E.name);
                    SLEEP_MS(LATENCY);
                }
                else if (p->money < ARMOR_PRICE)
                {
                    printf("Not enough money!\n");
                    SLEEP_MS(LATENCY);
                }
            }
            if (choice[2] == '>')
            {
                if (p->money >= PLASMA_GUN_PRICE && !p->boughtATK)
                {
                    p->money -= PLASMA_GUN_PRICE;
                    p->E.atk += PERM_BUFF;
                    p->boughtATK = true;
                    printf("%s has purchased the Plasma Railgun! ATK +30\n", p->E.name);
                    SLEEP_MS(LATENCY);
                    break;
                }
                else if (p->boughtATK)
                {
                    printf("%s has already acquired this upgrade!\n", p->E.name);
                    SLEEP_MS(LATENCY);
                }
                else if (p->money < PLASMA_GUN_PRICE)
                {
                    printf("Not enough money!\n");
                    SLEEP_MS(LATENCY);
                }
            }
            if (choice[3] == '>')
            {
                if (p->money >= INVIGORATOR_PRICE && !p->boughtVigor)
                {
                    p->money -= INVIGORATOR_PRICE;
                    PLAYER_MAX_HP += PERM_BUFF;
                    p->E.hp = PLAYER_MAX_HP;
                    PLAYER_MAX_MP += PERM_BUFF;
                    p->mp = PLAYER_MAX_MP;
                    p->boughtVigor = true;
                    printf("%s has purchased the Vigor Enhancer! HP & MP +30 PERMANENTLY!\n", p->E.name);
                    SLEEP_MS(LATENCY);
                    break;
                }
                else if (p->boughtVigor)
                {
                    printf("%s has already acquired this upgrade!\n", p->E.name);
                    SLEEP_MS(LATENCY);
                }
                else if (p->money < INVIGORATOR_PRICE)
                {
                    printf("Not enough money!\n");
                    SLEEP_MS(LATENCY);
                }
            }
            if (choice[4] == '>')
            {
                while (!leaving)
                {
                    system("cls");
                    printf("Are you sure you want to leave the Shop?\n\t%c YES\n\t%c NO", exitChoice[0], exitChoice[1]);
                    input = getch();
                    switch (input)
                    {
                    case 'w':
                        if (exitChoice[1] == '>')
                        {
                            exitChoice[0] = '>';
                            exitChoice[1] = ' ';
                        }
                        else if (exitChoice[0] == '>')
                        {
                            exitChoice[1] = '>';
                            exitChoice[0] = ' ';
                        }
                        break;
                    case 's':
                        if (exitChoice[0] == '>')
                        {
                            exitChoice[1] = '>';
                            exitChoice[0] = ' ';
                        }
                        else if (exitChoice[1] == '>')
                        {
                            exitChoice[0] = '>';
                            exitChoice[1] = ' ';
                        }
                        break;
                    case 13:
                        if (exitChoice[0] == '>')
                        {
                            return;
                        }
                        if (exitChoice[1] == '>')
                        {
                            leaving = true;
                        }
                    }
                }
            }
        }
    }
}
