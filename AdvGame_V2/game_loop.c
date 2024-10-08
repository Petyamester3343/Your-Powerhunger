#include "skeleton.c"

// prototype functions for the game loop and the main menu since they are intertwined
PWRHNGR_DEF MainMenu(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList);
PWRHNGR_DEF GameLoop(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList);

PWRHNGR_DEF checkWinCondition(Player* p, OBJ_DLL* objList, FOE_DLL* foeList)
{
    if (bossDefeated)
    {
        printf("%s, you have finally slain Jimmy. You win!\n", p->E.name);
        saveData(p, objList, foeList);
        exit(0);
    }
}

// When the player dies, a counter incrememnts
// Once the counter reaches 10, it's Game Over
// Until then, the Player is placed back at its starter position
PWRHNGR_DEF GameOver(Player* p, FOE_DLL* foeList, OBJ_DLL* objList, char input, char map[15][15])
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    printf("You died!\n");
    p->deathCount++;																			// Death count is incremented by 1
    SLEEP_MS(2000);
    emptyFoeList(foeList);																		// Both of the containers get emptied
    emptyObjList(objList);
    if(p->deathCount >= 7 && 10-p->deathCount!=1)
        printf("Only %d chances left...\n",10-p->deathCount);

    else if(10-p->deathCount == 1)
    {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        printf("Only one shot left...\nBetter make it count...\x1b[0m\n");
    }

    else if (p->deathCount == 10)  																// Player can die 10 times in total. After that, the game restarts.
    {
        myStr msg = "GAME OVER!";
        p->deathCount = 0;
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        for (int i = 0; i < sizeof(msg); i++)
        {
            printf("%c", msg[i]);
            SLEEP_MS(400);
        }
        printf("\x1b[0m\n");
        SLEEP_MS(5000);
        MainMenu(input, map, p, objList, foeList);
    }
    else
    {
        p->E.hp = PLAYER_MAX_HP;
        p->mp = PLAYER_MAX_MP;
        p->E.pos.col = p->E.pos.row = 7;
        p->E.dead = false;

        placeObjectsOnMap(objList, foeList, p, 5);
        breedFoes(p, objList, foeList);
    }
}

// Game initializes and the loop begins
PWRHNGR_DEF NewGame(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList)
{
    p = initPlayer();																			// Player is initialized

    objList = createObjList();																	// Object list is initialized
    placeObjectsOnMap(objList, foeList, p, 5);													// and instances of objects are added

    foeList = createFoeList();																	// Same sitch with the Foe list
    breedFoes(p, objList, foeList);																// and Foes

    initMap(map);                                                                               // Initializing map from a .txt file (15*15 char)
    initPlayerMagic(p);                                                                         // Initializing Player's Magia

    GameLoop(getc, map, p, objList, foeList);													// Everything's set. LET THE GAME BEGIN!!!
}

// Game initializes (while using a save file as source), and the loop begins
PWRHNGR_DEF LoadGame(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList)
{
    initPlayerMagic(p);																			// Since the game has already loaded the save, re-initialization of Player's Magia is first
    initMap(map);                                                                               // Initializing map from a .txt file (15*15 char)
    GameLoop(getc, map, p, objList, foeList);                                                   // Everything's set. LET THE GAME CONTINUE!
}

// Game loop
PWRHNGR_DEF GameLoop(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList)
{
    bool divineHelpGained = false;

    while (true)
    {
        system("cls");																			// Clear the console for the new output
        drawMap(map, p, objList, foeList);														// Draw the map (player, foes, objects, unoccupied positions)
        showPlayerInfo(p);																		// Draw a simple UI, which shows the current and max. stats of player, along with the money and lives they possess
        narrate(map, p);																		// A desription of the territory the character is on (related buffs may be implemented later)

        if (playerAction(getc, map, p, objList, foeList))                                       // This function determines if the player has acted in any way possible
        {
            if (getFoeCount(foeList) == 0)  													// Should the player clear the map, they are granted another chance to gain more power
            {
                placeObjectsOnMap(objList, foeList, p, 5);                                      // Loot rain after clearing the map.
                breedFoes(p, objList, foeList);                                                 // New Foes are summoned.
                divineHelpGained = false;                                                       // The Divine Help flag gets reset.
            }

            moveFoes(foeList, objList, p);														// After initialization, the enemies move, too (but their movement is randomized, quasi-wandering)
            moveBoss(bossSummoned, foeList, p, objList);										// If Jimmy is present, he moves towards the player (based on the absolute values of the hor. and vert. distance between them)

            if (p->E.hp < PLAYER_MAX_HP && !divineHelpGained)                                   // Upon getting hurt
            {
                placeObjectsOnMap(objList, foeList, p, 1);										// A guardian angel gives the Player a random object...
                divineHelpGained = true;                                                        // ... and refuses to elaborate further
            }
        }

        removeFoeByStatus(foeList);	                                                            // In case there was a fight, the Foes' corpses need to be taken care of

        if (p->E.hp == 0)                                                                       // In case Player's HP would reach 0
        {
            GameOver(p, foeList, objList, getc, map);                                           // Game over
            divineHelpGained = false;                                                           // Your guardian angel comes back
        }

        checkWinCondition(p, objList, foeList);                                                 // Your sole goal is to defeat Jimmy once you get strong enough (LV 10 or above)
    }
}

// The main menu and its mechanism
PWRHNGR_DEF MainMenu(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList)
{
    HANDLE hndl = GetStdHandle(STD_OUTPUT_HANDLE);
    char choice[3] = { '#', ' ', ' ' };

    bool exitChose = false;
    char exitChoice[2] = {' ', '#'};

    while (1)
    {
        system("cls");
        SetConsoleTextAttribute(hndl, FOREGROUND_RED);
        myStr title = "POWERHUNGER\x1b[0m";
        printf("%s\n\t%c New Game\n\t%c Load Game\n\t%c \x1b[31mDelete Save File\n\n\x1b[0m",
               title, choice[0], choice[1], choice[2]);

        getc = getch();                                                                         // The player may traverse the main menu by 1 unit forward/backward

        switch (getc)                                                                           // The same principle is applied to all the other selection menus
        {
        case 'w':
            if (choice[0] == '#')
            {
                choice[2] = '#';
                choice[0] = ' ';
            }
            else if (choice[1] == '#')
            {
                choice[0] = '#';
                choice[1] = ' ';
            }
            else
            {
                choice[1] = '#';
                choice[2] = ' ';
            }
            break;
        case 's':
            if (choice[0] == '#')
            {
                choice[1] = '#';
                choice[0] = ' ';
            }
            else if (choice[1] == '#')
            {
                choice[2] = '#';
                choice[1] = ' ';
            }
            else
            {
                choice[0] = '#';
                choice[2] = ' ';
            }
            break;
        case 13: // Enter key
            if (choice[0] == '#')
            {
                NewGame(getc, map, p, objList, foeList);
            }

            else if (choice[1] == '#')
            {
                if (chooseAndLoadData(p, objList, foeList) == -1)  								 // Should a save file not exist
                {
                    NewGame(getc, map, p, objList, foeList);
                }
                else  																			 // If there is a save file,
                {
                    LoadGame(getc, map, p, objList, foeList);
                }
            }
            else
            {
                chooseAndDeleteData();															 // Declared save file gets deleted
            }
            break;
        case 27: // Esc key
            while (!exitChose)
            {
                system("cls");
                printf("Are you sure you want to exit? (Any unsaved data will be lost.)\n\t%c YES\n\t%c NO",
                       exitChoice[0], exitChoice[1]);
                char exitInput = getch();
                switch (exitInput)
                {
                case 'w':
                    if (exitChoice[0] == '#')
                    {
                        exitChoice[0] = ' ';
                        exitChoice[1] = '#';
                    }
                    else
                    {
                        exitChoice[0] = '#';
                        exitChoice[1] = ' ';
                    }
                    break;
                case 's':
                    if (exitChoice[1] == '#')
                    {
                        exitChoice[0] = '#';
                        exitChoice[1] = ' ';
                    }
                    else
                    {
                        exitChoice[0] = ' ';
                        exitChoice[1] = '#';
                    }
                    break;
                case 13:
                    exitChose = true;
                    if (exitChoice[0] == '#')
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
    }
}
