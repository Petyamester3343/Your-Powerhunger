#include "game_data.c"
#include "battle_sys.c"
#include "shop.c"
#include "tictactoe.c"

#include <math.h>
#include <time.h>
#include <ctype.h>

// Map-drawing function with output color manipulation
PWRHNGR_DEF drawMap(char map[MAP_SIZE][MAP_SIZE], Player* p, OBJ_DLL* objList, FOE_DLL* foeList)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // to manipulate the color of the output
    system("cls");

    for (uint i = 0; i < MAP_SIZE; i++)
    {
        for (uint j = 0; j < MAP_SIZE; j++)
        {
            // Draw Player
            if (i == p->E.pos.col && j == p->E.pos.row)
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
                printf("X \x1b[0m");
                continue;
            }

            // Draw the Objects
            ObjNode* currObj = objList->head;
            while(currObj != NULL)
            {
                if (currObj->o->pos.col == i && currObj->o->pos.row == j)
                {
                    SetConsoleTextAttribute(hConsole, FOREGROUND_YELLOW);
                    printf("%c \x1b[0m", currObj->o->sign);
                    break;
                }
                currObj = currObj->next;
            }

            if(currObj != NULL) continue;

            // Draw Foes
            FoeNode* currFoe = foeList->head;
            while (currFoe != NULL)
            {
                if (currFoe->f->E.pos.col == i && currFoe->f->E.pos.row == j)
                {
                    if(strcmp(currFoe->f->E.name, BOSS_NAME) == 0)
                    {
                        SetConsoleTextAttribute(hConsole, FOREGROUND_PURPLE);
                        printf("B \x1b[0m");
                    }
                    else
                    {
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
                        printf("! \x1b[0m");
                    }
                    break;
                }
                currFoe = currFoe->next;
            }

            if(currFoe != NULL) continue;


            // The rest of the map (River is turquoise, Town is green)
            switch (map[i][j])
            {
            case 'R':
                SetConsoleTextAttribute(hConsole, FOREGROUND_CYAN);
                printf("%c \x1b[0m", map[i][j]);
                break;
            case 'T':
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
                printf("%c \x1b[0m", map[i][j]);
                break;
            default:
                SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
                printf("%c \x1b[0m", map[i][j]);
                break;
            }

        }
        printf("\n");
    }
}

// Moves the boss of the game, named "Jimmy"
// (based on the difference between the absolute values of hor. and vert. distances)
PWRHNGR_DEF moveBoss(bool isPresent, FOE_DLL* foeList, Player* p, OBJ_DLL* ol)
{
    if(isPresent)
    {
        FoeNode* bossNode = findFoeByName(foeList, BOSS_NAME);
        if (bossNode != NULL)
        {
            int dRow = abs((int)p->E.pos.row - (int)bossNode->f->E.pos.row);
            int dCol = abs((int)p->E.pos.col - (int)bossNode->f->E.pos.col);

            if (dRow > dCol)
            {
                if (p->E.pos.row > bossNode->f->E.pos.row)
                {
                    bossNode->f->E.pos.row++;
                }
                else
                {
                    bossNode->f->E.pos.row--;
                }
            }
            else
            {
                if (p->E.pos.col > bossNode->f->E.pos.col)
                {
                    bossNode->f->E.pos.col++;
                }
                else
                {
                    bossNode->f->E.pos.col--;
                }
            }
        }
        printf("%s moved toward %s! (x: %d, y: %d)\n", bossNode->f->E.name, p->E.name, bossNode->f->E.pos.col + 1, bossNode->f->E.pos.row + 1);
        if(bossNode->f->E.pos.col == p->E.pos.col && bossNode->f->E.pos.row == p->E.pos.row)
        {
            printf("%s has found you... Prepare for the BATTLE OF AEONS!!!\n", bossNode->f->E.name);
            SLEEP_MS(LATENCY);
            foeChoosesAction(p, bossNode->f, ol, foeList);
        }
        SLEEP_MS(LATENCY);
    }
}

// Allows the Foes (except for Jimmy) to move
PWRHNGR_DEF moveFoes(FOE_DLL* foeList, OBJ_DLL* objList, Player* p)
{
    FoeNode* current = foeList->head;
    while (current != NULL && strcmp(current->f->E.name, BOSS_NAME) != 0)
    {
        if(!current->f->E.dead)
        {
            bool moved = false;
            Pos newPos;
            Pos prevPos = current->f->E.pos;
            prevPos.occupied = false;
            do
            {
                newPos = prevPos;
                switch (rand() % 4)   // 0 = up, 1 = down, 2 = left, 3 = right
                {
                case 0: // Move up
                    if (newPos.col > 0)
                    {
                        newPos.occupied = false;
                        newPos.col -= 1;
                        newPos.occupied = true;
                    }
                    break;
                case 1: // Move down
                    if (newPos.col < 14)
                    {
                        newPos.occupied = false;
                        newPos.col += 1;
                        newPos.occupied = true;
                    }
                    break;
                case 2: // Move left
                    if (newPos.row > 0)
                    {
                        newPos.occupied = false;
                        newPos.row -= 1;
                        newPos.occupied = true;
                    }
                    break;
                case 3: // Move right
                    if (newPos.row < 14)
                    {
                        newPos.occupied = false;
                        newPos.row += 1;
                        newPos.occupied = true;
                    }
                    break;
                }
            }
            while ((findFoeByPos(foeList, newPos) != NULL) && findObjByPos(objList, newPos) != NULL && newPos.occupied);   // Retry if occupied

            // Move to new position if it's not occupied
            if (findFoeByPos(foeList, newPos) == NULL && findObjByPos(objList, newPos) == NULL)
            {
                current->f->E.pos = newPos;
                moved = true;
            }
            else
            {
                current->f->E.pos = prevPos;
                moved = false;
            }

            if (current->f->E.pos.col == p->E.pos.col && current->f->E.pos.row == p->E.pos.row && p->E.pos.occupied == current->f->E.pos.occupied)
                foeChoosesAction(p, current->f, objList, foeList);

            if (moved)
                printf("%s moved (X: %d, Y: %d)\n", current->f->E.name, current->f->E.pos.col + 1, current->f->E.pos.row + 1);
            else
                printf("%s passed this turn (X: %d, Y: %d)\n", current->f->E.name, current->f->E.pos.col + 1, current->f->E.pos.row + 1);
        }

        current = current->next;
        SLEEP_MS((LATENCY / 3) * 2);
    }
}

// Utility function for the greeting sequence
PWRHNGR_BOOLDEF unknownOne(myStr msg)
{
    return
        strcmp(msg, "Cassius") != 0 &&
        strcmp(msg, "Phantom") !=0 &&
        strcmp(msg, "Sakura") !=0 &&
        strcmp(msg, "Sasaki") !=0 &&
        strcmp(msg, "Thomas") !=0 &&
        strcmp(msg, "Minerva") !=0 &&
        strcmp(msg, "Lucius") !=0 &&
        strcmp(msg, "Hana") !=0 &&
        strcmp(msg, "Saki") !=0 &&
        strcmp(msg, "Aleister") !=0 &&
        strcmp(msg, "Nari") != 0 &&
        strcmp(msg, "Akari") !=0;
}

// Text printing methodized for the greeting sequence
PWRHNGR_DEF printText(myStr msg)
{
    for(uint i=0; i<strlen(msg); i++)
    {
        printf("\x1b[31m%c", toupper(msg[i]));
        SLEEP_MS(LATENCY);
    }
}

// Formatting name into a "Xyz" uniform
PWRHNGR_DEF formatName(char str[STR_SIZE])
{
    str[0] = toupper(str[0]);
    for(uint i=1; i<strlen(str); i++)
        str[i] = tolower(str[i]);
}

// Initializes the player and the spells in order to avoid access violation
// Also, a small greeting sequence is triggered under certain secret conditions
PWRHNGR_PLAYERDEF initPlayer()
{
    Player* p = (Player*)malloc(sizeof(Player));

    if (p != NULL)
    {
        char msg[STR_SIZE] = "";
        printf("Name your hero: ");
        fflush(stdin);
        scanf("%[^\n]s", msg);
        formatName(msg);
        p->E.name = strdup(msg);
        p->money = 0;
        p->E.hasMagiaResist = true;
        p->E.magiaResist = (strcmp(p->E.name, "Saki") != 0) ? 10 : 15;
        defPlayerValRestore(p);
        initPlayerMagic(p);

        if(!unknownOne(msg))
        {
            if(strcmp(msg, "Thomas") == 0)
            {
                printf("I am not afraid of your hounds... ");
                printText(strcat(msg, "!"));
                printf("\x1b[0m");
            }

            else if(strcmp(msg, "Hana") == 0)
            {
                // An unbeatable Tic-Tac-Toe... Just for prank...
                printf("\x1b[0mOh, little child... You are not suited for such realms at all...\n");
                SLEEP_MS(LATENCY * 4);
                printf("Let us play something else instead, shall we?\n");
                SLEEP_MS(LATENCY * 4);
                char yesno = ' ';
                while(yesno != 'n')
                {
                    system("cls");
                    gameMech_3T();
                    printf("Again? (Y/N) ");
                    fflush(stdin);
                    scanf("%c", &yesno);
                    if(toupper(yesno) == 'N')
                    {
                        free(p);
                        exit(265918);
                    }
                }
            }

            else if(strcmp(msg, "Sasaki") == 0)
            {
                printf("\x1b[31mTo think you can live with such secrets...\n");
                SLEEP_MS(LATENCY * 4);
                printf("\x1b[31mHave you ever wondered why she can see a glimpse of the future... ");
                printText(strcat(msg, "?"));
                printf("\x1b[0m\n");
            }

            else if(strcmp(msg, "Aleister") == 0 || strcmp(msg, "Cassius") == 0)
            {
                p->boughtVigor = true;
                PLAYER_MAX_HP += PERM_BUFF;
                PLAYER_MAX_MP += PERM_BUFF;
                p->E.hp = PLAYER_MAX_HP;
                p->mp = PLAYER_MAX_MP;

                printf("\x1b[31mMy death will never bring Her back... ");
                SLEEP_MS(LATENCY*4);
                printText(strcat(msg, "!"));
                printf("\x1b[0m\n");
            }

            else if(strcmp(msg, "Sakura") == 0)
            {
                printf("\x1b[31mAs if you can foresee my defeat... ");
                SLEEP_MS(LATENCY * 4);
                printText(strcat(msg, "..."));
                printf("\x1b[0m\n");
            }

            else if(strcmp(msg, "Phantom") == 0)
            {
                p->E.name = strdup("Phantom");
                p->lv = 10;
                p->boughtMagic = true;
                p->boughtATK = true;
                p->boughtDEF = true;
                p->boughtVigor = true;
                PLAYER_MAX_HP += (p->lv - 1) * LVUP_VAL + PERM_BUFF;
                PLAYER_MAX_MP += (p->lv - 1) * LVUP_VAL + PERM_BUFF;
                PLAYER_ATK += (p->lv - 1) * LVUP_VAL + PERM_BUFF;
                PLAYER_DEF += (p->lv - 1) * LVUP_VAL + PERM_BUFF;
                p->E.hp = PLAYER_MAX_HP;
                p->mp = PLAYER_MAX_MP;
                p->E.atk = PLAYER_ATK;
                p->E.def = PLAYER_DEF;

                printf("\x1b[31mGet psyched for your funeral... ");
                SLEEP_MS(LATENCY * 2);
                printText(strcat(msg, "!!!"));
                printf("\x1b[0m\n");
            }

            else if(strcmp(msg, "Nari") == 0)
            {
                p->E.name = strdup("Nari");
                p->boughtATK = true;
                p->boughtDEF = true;
                PLAYER_ATK += PERM_BUFF;
                PLAYER_DEF += PERM_BUFF;
                p->E.atk = PLAYER_ATK;
                p->E.def = PLAYER_DEF;

                printf("\x1b[31mAh, there you are...\n");
                SLEEP_MS(LATENCY * 4);
                printf("The failed \"materfamilias...\"\n");
                SLEEP_MS(LATENCY * 4);
                printf("How does it feel like to be in Hell for betraying your own, innocent kin...\n");
                SLEEP_MS(LATENCY * 4);
                printf("Your only flesh and blood... A victim of the chaos you inadvertently unleashed on her...\n");
                SLEEP_MS(LATENCY * 4);
                printf("Making her suffer and beg for her death with that disingenuous move of yours...\n");
                SLEEP_MS(LATENCY * 4);
                printf("Just because of a lie shattering your pride...\n");
                SLEEP_MS(LATENCY * 4);
                printf("Please do humor me... Do you have any ounce of remorse in your heart...\n");
                SLEEP_MS(LATENCY * 4);
                printf("Do you wish to send her soul to Heaven..., ");
                SLEEP_MS(LATENCY * 4);
                printText(strcat(msg, "...?"));
                printf("\x1b[0m\n");
            }

            else if(strcmp(msg, "Saki") == 0)
            {
                printf("\x1b[31mYou...\n\x1b[0m");
                SLEEP_MS(LATENCY * 4);
                printf("Why are you here?\n");
                SLEEP_MS(LATENCY * 4);
                printf("You should not be..., for you are not fit for this realm...\n");
                SLEEP_MS(LATENCY * 4);
                printf("... Yes... I pity you, broken one...\n");
                SLEEP_MS(LATENCY * 4);
                printf("Even though I am supposed to be your enemy...\n");
                SLEEP_MS(LATENCY * 4);
                printf("I cannot let you in...\n");
                SLEEP_MS(LATENCY * 4);
                printf("You have to understand...\n");
                SLEEP_MS(LATENCY * 4);

                char choice[2]= {' ', '>'};
                while(1)
                {
                    system("cls");
                    myStr msg1 = "I cannot let you do your evil either! ... I need to be free... I'm going in, no matter what!";
                    myStr msg2 = "I... I can understand... I'm not ready yet...";
                    printf("%c\t%s\n%c\t%s", choice[0], msg1, choice[1], msg2);
                    char input = getch();
                    switch(input)
                    {
                    case 'w':
                        if(choice[0]=='>')
                        {
                            choice[0]=' ';
                            choice[1]='>';
                        }
                        else
                        {
                            choice[1]=' ';
                            choice[0]='>';
                        }
                        break;
                    case 's':
                        if(choice[0]=='>')
                        {
                            choice[0]=' ';
                            choice[1]='>';
                        }
                        else
                        {
                            choice[1]=' ';
                            choice[0]='>';
                        }
                        break;
                    case 13:
                        if(choice[0] == '>')
                        {
                            printf("\n... I see... It is your choice, after all...");
                            SLEEP_MS(LATENCY * 4);
                            printf("\n\x1b[31mYet you do not know what you are dwelling yourself into... ");
                            SLEEP_MS(LATENCY * 4);
                            printText(strcat(msg, "..."));
                            printf("\x1b[0m\n");

                            PLAYER_ATK /= 2;
                            p->E.atk = PLAYER_ATK;
                            PLAYER_DEF /= 2;
                            p->E.def = PLAYER_DEF;
                            PLAYER_MAX_HP *= 2;
                            p->E.hp = PLAYER_MAX_HP;
                            PLAYER_MAX_MP *= 2;
                            p->mp = PLAYER_MAX_MP;
                        }
                        else
                        {
                            printf("\n\x1b[0mA wise decision...\n");
                            free(p);
                            exit(265918);
                        }
                    }
                }

            }

            else if(strcmp(msg, "Akari") == 0 || strcmp(msg, "Lucius") == 0 || strcmp(msg, "Minerva") == 0)
            {
                if(strcmp(msg, "Akari") == 0)
                    p->boughtMagic = true;

                printf("\x1b[31mSo, you have finally come for my head... ");
                SLEEP_MS(LATENCY*4);
                printText(strcat(msg, "!"));
                printf("\x1b[0m\n");
            }
        }

        else
        {
            printf("\x1b[31mWelcome to Hell... ");
            SLEEP_MS(LATENCY*4);
            printText(strcat(msg, "!"));
            printf("\x1b[0m\n");
        }
    }
    SLEEP_MS(LATENCY);

    return p;
}

// Initializes the map
PWRHNGR_DEF initMap(char map[MAP_SIZE][MAP_SIZE])
{
    bzero(map, sizeof(char) * MAP_SIZE * MAP_SIZE);
    FILE* mapFP = fopen("map.txt", "r");

    if (mapFP == NULL)
    {
        perror("ERROR OPENING FILE");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAP_SIZE; i++)
    {
        for (int j = 0; j < MAP_SIZE; j++)
        {
            int c = fgetc(mapFP);
            if (c == EOF)
            {
                fprintf(stderr, "Unexpected end of file!\n");
                fclose(mapFP);
                exit(EXIT_FAILURE);
            }
            map[i][j] = (char)c;
        }
        fgetc(mapFP);
    }

    fclose(mapFP);
}

// Places a random Object on the map, which buffs the Player (until the next level up, that is)
// Depending on the situation, the iteration can be controlled
PWRHNGR_DEF placeObjectsOnMap(OBJ_DLL* list, FOE_DLL* fl, Player* p, const uint num)
{
    for (int i = 0; i < num; i++)
    {
        switch (rand() % 7)
        {
        case 0:
            insertObjIntoList(list, fl, 'S', OBJ_DEF_BUFF, p);
            break;
        case 1:
            insertObjIntoList(list, fl, 'A', OBJ_DEF_BUFF, p);
            break;
        case 2:
            insertObjIntoList(list, fl, '+', OBJ_DEF_BUFF, p);
            break;
        case 3:
            insertObjIntoList(list, fl, '/', OBJ_DEF_BUFF, p);
            break;
        case 4:
            insertObjIntoList(list, fl, 'M', OBJ_DEF_BUFF, p);
            break;
        case 5:
            insertObjIntoList(list, fl, '&', OBJ_DEF_BUFF*2, p);
            break;
        case 6:
            insertObjIntoList(list, fl, '$', rand() % 30 + 10, p);
            break;
        }
    }
}

// Allows the Player to pick Objects up and consume them
PWRHNGR_DEF playerSeeksObj(Player* p, OBJ_DLL* list)
{
    static int mangaFound = 0;
    ObjNode* currObjNode = list->head;
    while (currObjNode != NULL)
    {
        if (currObjNode->o->pos.col == p->E.pos.col && currObjNode->o->pos.row == p->E.pos.row)
        {
            // Player has found an Object on the given position of the map
            switch (currObjNode->o->sign)
            {
            case '+':
                printf("%s found some Elixir! HP +%d\n", p->E.name, currObjNode->o->eff);
                if ((currObjNode->o->eff + p->E.hp) >= PLAYER_MAX_HP)
                {
                    printf("Health overcharge!\n");
                    p->E.hp += currObjNode->o->eff;
                }
                else
                {
                    printf("You are healed!\n");
                    p->E.hp = PLAYER_MAX_HP;
                }
                currObjNode->o->found = true;
                break;
            case 'S':
                printf("%s found a sturdy and sharp blade! ATK +%d\n", p->E.name, currObjNode->o->eff);
                p->E.atk += currObjNode->o->eff;
                currObjNode->o->found = true;
                break;
            case 'A':
                p->aegisPickedUp++;
                printf("%s found an Aegis Core! DEF +%d\nAegis Cores in %s's possession: %d\n",
                       p->E.name, currObjNode->o->eff, p->E.name, p->aegisPickedUp);
                p->E.def += currObjNode->o->eff;
                currObjNode->o->found = true;
                break;
            case '/':
                printf("%s found an Arcane Tome! XP +%d ", p->E.name, currObjNode->o->eff);
                xp_gain(currObjNode->o->eff, p, false);
                currObjNode->o->found = true;
                break;
            case 'M':
                printf("%s found some Blue Spike! MP +%d\n", p->E.name, currObjNode->o->eff);
                if ((currObjNode->o->eff + p->mp) >= PLAYER_MAX_MP)
                {
                    printf("Mana overcharge!\n");
                    p->mp += currObjNode->o->eff;
                }
                else
                {
                    printf("Your mana has been replenished!\n");
                    p->mp = PLAYER_MAX_MP;
                }
                currObjNode->o->found = true;
                break;
            case '$':
                printf("%s has found %d GOLD! Finders keepers, they say...\n", p->E.name, currObjNode->o->eff);
                p->money += currObjNode->o->eff;
                currObjNode->o->found = true;
                break;
            case '&':
                if (rand() % 2 == 0)
                {
                    printf("%s has found a bowl of Mac & Cheese with some Chicken Nuggets! Its best before date is still yet to expire! HP +%d\n",
                           p->E.name, currObjNode->o->eff);
                    p->E.hp += currObjNode->o->eff;
                }
                else if (rand() % 3 == 0)
                {
                    printf("%s has found a Titanium Blade! ATK +%d\n", p->E.name, currObjNode->o->eff);
                    p->E.atk += currObjNode->o->eff;
                }
                else if (rand() % 5 == 0)
                {
                    printf("%s has found an Antaresian Shield! DEF +%d\n", p->E.name, currObjNode->o->eff);
                    p->E.def += currObjNode->o->eff;
                }
                else if (rand() % 7 == 0)
                {
                    printf("%s has found an Ancient Diary! XP +%d ", p->E.name, currObjNode->o->eff);
                    xp_gain(currObjNode->o->eff, p, false);
                }
                else if (rand() % 11 == 0)
                {
                    printf("%s has found a Mana Potion! Its best before date is still yet to expire! MP +%d\n", p->E.name, currObjNode->o->eff);
                    p->mp += currObjNode->o->eff;
                }
                else if (rand() % 13 == 0)
                {
                    uint itemVal = rand() % 100 + 5 * currObjNode->o->eff;
                    printf("%s has found a valuable item! Lucky bastard... GOLD +%d\n", p->E.name, itemVal);
                    p->money += itemVal;
                }
                else
                {
                    mangaFound++;
                    if (mangaFound < 3)
                    {
                        printf("%s has found nothing in that chest...\nExcept for a manga with a topless girl and butterflies on its cover...\n",
                               p->E.name);
                        printf("Our dear wanderer left it there, for it was a bad omen.\n");
                    }
                    if (mangaFound == 3)
                    {
                        printf("%s has found that cursed manga again.\nCuriosity finally took over his mind and read it...\n", p->E.name);
                        printf("Our dear wanderer became enraged about its content...\nHow a poor girl had to suffer in such an inhumane way...\n");
                        printf("ATK +20! DEF -20!\n");
                        mangaFound = 0;
                        p->E.atk += 20;
                        p->E.def -= 20;
                    }
                }
                currObjNode->o->found = true;
                break;
            }

            SLEEP_MS(500);

            if(currObjNode->o->found)
            {
                removeObjNode(list, currObjNode);
            }

            currObjNode = currObjNode->next;
        }
        else
        {
            currObjNode = currObjNode->next;
        }
    }
}

// Helper method for checking the headcount of Foes lurking in Town
PWRHNGR_UIDEF foeTerrorizingShop(char map[15][15], FOE_DLL* fl)
{
    FoeNode* curr = fl->head;
    int temp = 0;
    while (curr!=NULL)
    {
        if(map[curr->f->E.pos.col][curr->f->E.pos.row] == 'T')
        {
            temp++;
        }
        curr = curr->next;
    }
    return temp;
}

// The player takes action upon input (movement, attacking Foes in its vicinity)
// It becomes true when they moved
PWRHNGR_BOOLDEF playerAction(char input, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList)
{
    bool moved = false;

    char exitChoice[2] = { ' ', '>' };
    bool exitChose = false;

    // keyboard input
    input = getch();
    if(input == 0 || input == 224) {
        input = getch();
    }

    switch (input)
    {
    case 'w':
        if (p->E.pos.col > 0)
        {
            p->E.pos.occupied = false;
            p->E.pos.col--;
            p->E.pos.occupied = true;
            moved = true;
        }
        else printf("%s bumped into a wall! (X: %d, Y: %d)\n", p->E.name, p->E.pos.col + 1, p->E.pos.row + 1);
        break;
    case 's':
        if (p->E.pos.col < 14)
        {
            p->E.pos.occupied = false;
            p->E.pos.col++;
            p->E.pos.occupied = true;
            moved = true;
        }
        else printf("%s bumped into a wall! (X: %d, Y: %d)\n", p->E.name, p->E.pos.col + 1, p->E.pos.row + 1);
        break;
    case 'a':
        if (p->E.pos.row > 0)
        {
            p->E.pos.occupied = false;
            p->E.pos.row--;
            p->E.pos.occupied = true;
            moved = true;
        }
        else printf("%s bumped into a wall! (X: %d, Y: %d)\n", p->E.name, p->E.pos.col + 1, p->E.pos.row + 1);
        break;
    case 'd':
        if (p->E.pos.row < 14)
        {
            p->E.pos.occupied = false;
            p->E.pos.row++;
            p->E.pos.occupied = true;
            moved = true;
        }
        else printf("%s bumped into a wall! (X: %d, Y: %d)\n", p->E.name, p->E.pos.col + 1, p->E.pos.row + 1);
        break;
    case 'e':
        impendingDoom(p, foeList, objList);
        break;
    case '0':
        saveData(p, objList, foeList);
        break;
    case 32:
        if (map[p->E.pos.col][p->E.pos.row] == 'T' && foeTerrorizingShop(map, foeList) == 0)
            go2Shop(p);
        else if (foeTerrorizingShop(map, foeList) > 0)
            printf("%s needs to slay %d Foes at Town to access the shop!\n", p->E.name, foeTerrorizingShop(map, foeList));
        else if (map[p->E.pos.col][p->E.pos.row] != 'T')
            printf("There is no one who could sell the hero anything here.\n");
        SLEEP_MS(LATENCY);
        break;
    case 27:
        while (!exitChose)
        {
            system("cls");
            printf("Are you sure you want to exit? (Any unsaved data will be lost.)\n\t%c YES\n\t%c NO", exitChoice[0], exitChoice[1]);
            char exitInput = getch();
            switch (exitInput)
            {
            case 'w':
                if (exitChoice[0] == '>')
                {
                    exitChoice[0] = ' ';
                    exitChoice[1] = '>';
                }
                else
                {
                    exitChoice[0] = '>';
                    exitChoice[1] = ' ';
                }
                break;
            case 's':
                if (exitChoice[1] == '>')
                {
                    exitChoice[0] = '>';
                    exitChoice[1] = ' ';
                }
                else
                {
                    exitChoice[0] = ' ';
                    exitChoice[1] = '>';
                }
                break;
            case 13:
                exitChose = true;
                if (exitChoice[0] == '>')
                {
                    destroyFoeList(foeList);
                    destroyObjList(objList);
                    free(p);
                    exit(177013);
                }
                else break;
            }
        }
    }

    if (moved)
    {
        printf("%s moved! (X: %d, Y: %d)\n", p->E.name, p->E.pos.col + 1, p->E.pos.row + 1);
        // After moving one unit in any direction, Player checks if "there is something on the ground"
        playerSeeksObj(p, objList);

        // Player is now checking if there is any monster on the position they're about to occupy
        FoeNode* curr = foeList->head;
        while(curr != NULL)
        {
            if(curr->f->E.pos.col == p->E.pos.col && curr->f->E.pos.row == p->E.pos.row)
            {
                int rndGen = rand()%2;
                switch(rndGen)
                {
                case 0:
                    printf("%s successfully approached %s! %s gets to make the first move!\n", p->E.name, curr->f->E.name, p->E.name);
                    SLEEP_MS(500);
                    playerChoosesAction(p, curr->f, objList, foeList);
                    break;
                case 1:
                    printf("%s unfortunately noticed %s! %s gets to act first!\n", curr->f->E.name, p->E.name, curr->f->E.name);
                    SLEEP_MS(500);
                    foeChoosesAction(p, curr->f, objList, foeList);
                    break;
                }
            }
            curr = curr->next;
        }
    }

    return moved;
}

// Utility method for summoning the Boss
PWRHNGR_DEF deployBoss(Player* p, OBJ_DLL* ol, FOE_DLL* fl)
{
    if (p->lv >= 10 && !jimmySummoned)
    {
        insertFoeIntoList(p, ol, fl,
                          BOSS_NAME, BOSS_HP, BOSS_ATK, BOSS_DEF, 0, 0);
        fl->tail->f->E.pos.col = fl->tail->f->E.pos.row = 14;
        jimmySummoned = true;
    }
}

// Creates Foes based on the Player's level
// Adjusted not to flood the map
PWRHNGR_DEF breedFoes(Player* p, OBJ_DLL* objList, FOE_DLL* foeList)
{
    for(uint i = 0; i < 5; i++)
    {
        if (p->lv <= 3)
        {
            if (getFoeCountByName(foeList, SLIME_NAME) < MAX_SLIME)   // Limit number of Slimes
            {
                insertFoeIntoList(p, objList, foeList,
                                  SLIME_NAME, SLIME_HP, SLIME_ATK, SLIME_DEF, SLIME_XP, SLIME_LOOT);
                printf("Slime summoned!\n");
            }
        }

        if (p->lv >= 4)
        {
            if (getFoeCountByName(foeList, GOBLIN_NAME) < MAX_GOBLIN && rand() % 2 == 0)   // Limit number of Goblins
            {
                insertFoeIntoList(p, objList, foeList,
                                  GOBLIN_NAME, GOBLIN_HP, GOBLIN_ATK, GOBLIN_DEF, GOBLIN_XP, GOBLIN_LOOT);
                printf("Goblin summoned!\n");
            }
            else if (getFoeCountByName(foeList, SLIME_NAME) < MAX_SLIME)
            {
                insertFoeIntoList(p, objList, foeList,
                                  SLIME_NAME, SLIME_HP, SLIME_ATK, SLIME_DEF, SLIME_XP, SLIME_LOOT);
                printf("Slime summoned!\n");
            }
        }

        if (p->lv >= 6)
        {
            if (getFoeCountByName(foeList, ACOLYTE_NAME) < MAX_ACOLYTE && rand() % 2 == 0)   // Limit number of Acolytes
            {
                insertFoeIntoList(p, objList, foeList,
                                  ACOLYTE_NAME, ACOLYTE_HP, ACOLYTE_ATK, ACOLYTE_DEF, ACOLYTE_XP, ACOLYTE_LOOT);
                printf("Acolyte summoned!\n");
            }
            else if (getFoeCountByName(foeList, GOBLIN_NAME) < MAX_GOBLIN)
            {
                insertFoeIntoList(p, objList, foeList,
                                  GOBLIN_NAME, GOBLIN_HP, GOBLIN_ATK, GOBLIN_DEF, GOBLIN_XP, GOBLIN_LOOT);
                printf("Goblin summoned!\n");
            }
        }

        if (p->lv >= 8)
        {
            if (getFoeCountByName(foeList, THWARTED_SELF_NAME) < MAX_THW_SELF && rand() % 2 == 0)   // Limit number of Thwarted Selves
            {
                insertFoeIntoList(p, objList, foeList,
                                  THWARTED_SELF_NAME, THWARTED_SELF_HP, THWARTED_SELF_ATK, THWARTED_SELF_DEF, THWARTED_SELF_XP, THWARTED_SELF_LOOT);
                printf("Your Thwarted Self summoned!\n");
            }
            else if (getFoeCountByName(foeList, ACOLYTE_NAME) < MAX_ACOLYTE)
            {
                insertFoeIntoList(p, objList, foeList,
                                  ACOLYTE_NAME, ACOLYTE_HP, ACOLYTE_ATK, ACOLYTE_DEF, ACOLYTE_XP, ACOLYTE_LOOT);
                printf("Acolyte summoned!\n");
            }
        }

        if (p->lv >= 9)
        {
            if (getFoeCountByName(foeList, GOLEM_NAME) < MAX_GOLEM && rand() % 2 == 0)   // Limit number of Carcass Golems
            {
                insertFoeIntoList(p, objList, foeList,
                                  GOLEM_NAME, GOLEM_HP, GOLEM_ATK, GOLEM_DEF, GOLEM_XP, GOLEM_LOOT);
                printf("Carcass Golem summoned!\n");
            }
            else if (getFoeCountByName(foeList, THWARTED_SELF_NAME) < MAX_THW_SELF)
            {
                insertFoeIntoList(p, objList, foeList,
                                  THWARTED_SELF_NAME, THWARTED_SELF_HP, THWARTED_SELF_ATK, THWARTED_SELF_DEF, THWARTED_SELF_XP, THWARTED_SELF_LOOT);
                printf("Your Thwarted Self summoned!\n");
            }
        }
    }

    deployBoss(p, objList, foeList);
}

// A simple non-interactive UI
PWRHNGR_DEF showPlayerInfo(Player* p)
{
    //printf("Player name:\t%s\n", p->E.name); // debug to test the save/load function
    printf("HP:\t%d/%d\n", p->E.hp, PLAYER_MAX_HP);
    printf("MP:\t%d/%d\n", p->mp, PLAYER_MAX_MP);
    printf("ATK:\t%d/%d\n", p->E.atk, PLAYER_ATK);
    printf("DEF:\t%d/%d\n", p->E.def, PLAYER_DEF);
    printf("LV:\t%d (XP:\t%d/%d)\n", p->lv, p->E.xp, LV_UP_REQ(p->lv));
    printf("GOLD:\t%d\n", p->money);
    printf("\nLives:\t%d\n", 10-p->deathCount);
}

// The Player's current location on the map (under the UI)
PWRHNGR_DEF narrate(char map[15][15], Player* p)
{
    printf("Current location: ");
    switch (map[p->E.pos.col][p->E.pos.row])
    {
    case 'F':
        printf("Forest\n");
        break;
    case 'R':
        printf("River\n");
        break;
    case 'T':
        printf("Town\n");
        break;
    case 'V':
        printf("Valley\n");
        break;
    case 'D':
        printf("Desert\n");
        break;
    }
}
