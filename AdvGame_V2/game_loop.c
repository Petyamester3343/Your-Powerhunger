#include "skeleton.c"

// prototype functions for the game loop and the main menu since they are intertwined
static void MainMenu(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList);
static void GameLoop(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList);

static void checkWinCondition(Player* p, FOE_DLL* foeList) {
    if (jimmyDefeated) {
        printf("The wanderer has finally slain Jimmy. You win!\n");
        saveData(p);
        exit(0);
    }
}

// When the player dies, a counter incrememnts
// Once the counter reaches 10, it's Game Over
// Until then, the Player is placed back at its starter position
static void GameOver(Player* p, FOE_DLL* foeList, OBJ_DLL* objList, char input, char map[15][15]) {

	printf("You died!\n");
	p->deathCount++;																			// Death count is incremented by 1
	Sleep(2000);
	emptyFoeList(foeList);																		// Both of the containers get emptied
	emptyObjList(objList);
	if (p->deathCount == 10) {																	// Player can die 10 times in total. After that, the game restarts.
		const char* msg = "GAME OVER";
		for (int i = 0; i < sizeof(msg); i++) {
			printf("%c ", msg[i]);
			Sleep(10000);
			MainMenu(input, map, p, objList, foeList);
		}
	}
	else {
		p->E.hp = PLAYER_MAX_HP;
		p->mp = PLAYER_MAX_MP;
		p->E.pos.col = p->E.pos.row = 7;
		p->E.dead = false;

		placeObjectsOnMap(objList, foeList, p, 5);
		breedFoes(p, objList, foeList);
	}
}

// Game initializes and the loop begins
static void NewGame(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList) {
	p = initPlayer();																			// Player is initialized

	objList = createObjList();																	// Object list is initialized here
	placeObjectsOnMap(objList, foeList, p, 5);													// and instances of objects are added

	foeList = createFoeList();																	// Same with the Foe list
	breedFoes(p, objList, foeList);																// and foes

	GameLoop(getc, map, p, objList, foeList);													// Everything's set. LET THE GAME BEGIN!!!
}

// Game initializes (while using a save file as source), and the loop begins
static void LoadGame(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList) {
	initPlayerMagic(p);																			// Since the game has already loaded the save, magic initialization is first

	objList = createObjList();																	// Object list are initialized for now
	placeObjectsOnMap(objList, foeList, p, 5);

	foeList = (FOE_DLL*)createFoeList();
	breedFoes(p, objList, foeList);

	GameLoop(getc, map, p, objList, foeList);
}

// Game loop
static void GameLoop(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList) {
	while (1) {
		removeFoeByStatus(foeList);																// In case there was a fight, the corpses need to be taken care of
		system("cls");																			// Clears the console for the new output
		drawMap(map, p, objList, foeList);														// Draws the map (player, foes, objects, unoccupied positions)
		showPlayerInfo(p);																		// A simple UI, which shows the current and max. stats of player, along with the money they possess
		narrate(map, p);																		// A desription of the territory the character is on (related buffs may be implemented later)

		bool acted = playerAction(getc, map, p, objList, foeList);								// This function determines if the player has "moved" in any way possible.
		if (acted) {
			moveFoes(foeList, objList, p);														// If so, the enemies move, too (but their movement is randomized, quasi-wandering).
			moveJimmy(jimmySummoned, foeList, p);												// If Jimmy is present, he moves towards the player (based on the absolute values of the hor. and vert. distance between them)

			if (getFoeCount(foeList) == 0) {													// Should the player clear the map, they are granted another chance to gain more power
				placeObjectsOnMap(objList, foeList, p, 5);
				breedFoes(p, objList, foeList);
			}
			if (p->E.hp < PLAYER_MAX_HP)
				placeObjectsOnMap(objList, foeList, p, 1);										// Loot rain for more power!
		}

		if (p->E.hp == 0) {
			GameOver(p, foeList, objList, getc, map);
		}

		checkWinCondition(p, foeList);

		Sleep(10);
	}
}

// The main menu and its mechanism
static void MainMenu(char getc, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList) {
	HANDLE hndl = GetStdHandle(STD_OUTPUT_HANDLE);

	char choice[3] = { '#', ' ', ' ' };

	while (1) {
		system("cls");
		SetConsoleTextAttribute(hndl, FOREGROUND_RED);
        const char* title = "POWERHUNGER\x1b[0m";
		printf("%s\n\t%c New Game\n\t%c Load Game\n\t%c \x1b[31mDelete Save File\n\n\x1b[0m", title, choice[0], choice[1], choice[2]);

		getc = getch();
		// the player may "move" the hashtag char by 1 unit fwd/bwd
		switch (getc) {
		case 'w':
			if (choice[0] == '#') {
				choice[2] = '#';
				choice[0] = ' ';
			}
			else if (choice[1] == '#') {
				choice[0] = '#';
				choice[1] = ' ';
			}
			else {
				choice[1] = '#';
				choice[2] = ' ';
			}
			break;
		case 's':
			if (choice[0] == '#') {
				choice[1] = '#';
				choice[0] = ' ';
			}
			else if (choice[1] == '#') {
				choice[2] = '#';
				choice[1] = ' ';
			}
			else {
				choice[0] = '#';
				choice[2] = ' ';
			}
			break;
		case 13: // Enter key
			if (choice[0] == '#') {
				NewGame(getc, map, p, objList, foeList);
			}

			else if (choice[1] == '#') {
				if (chooseAndLoadData(p) == -1) {															// Should a save file not exist
					NewGame(getc, map, p, objList, foeList);
				}
				else {																				// If there is a save file,
					LoadGame(getc, map, p, objList, foeList);
				}
			}
			else {
				chooseAndDeleteData();																        // Save file gets deleted
			}
			break;
		case 27: exit(1); break;
		}
	}
}
