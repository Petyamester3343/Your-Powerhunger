#include "game_data.c"
#include "battle_sys.c"
#include "shop.c"
#include "tictactoe.c"

#include <math.h>
#include <time.h>
#include <ctype.h>

// Draws the map
static inline void drawMap(char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // to manipulate the color of the output
	system("cls");

	for (unsigned int i = 0; i < MAP_SIZE; i++) {
		for (unsigned int j = 0; j < MAP_SIZE; j++) {

			// Draw Player
			if (i == p->E.pos.col && j == p->E.pos.row) {
				SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
				printf("X \x1b[0m");
				continue;
			}

			// Draw the Objects
			ObjNode* currObj = objList->head;
            while(currObj != NULL) {
                if (currObj->o->pos.col == i && currObj->o->pos.row == j) {
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
                    printf("%c \x1b[0m", currObj->o->sign);
                    break;
                }
                currObj = currObj->next;
            }

            if(currObj != NULL) continue;

			// Draw foes
			FoeNode* currFoe = foeList->head;
			while (currFoe != NULL) {
				if (currFoe->f->E.pos.col == i && currFoe->f->E.pos.row == j) {
                    if(strcmp(currFoe->f->E.name, "Jimmy") == 0) {
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE);
                        printf("J \x1b[0m");
                    }
                    else {
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
                        printf("! \x1b[0m");
                    }
                    break;
				}
				currFoe = currFoe->next;
			}

			if(currFoe != NULL) continue;


            switch (map[i][j]) {
			case 'T':
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
				printf("%c \x1b[0m", map[i][j]);
				break;
            case 'R':
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE);
                printf("%c \x1b[0m", map[i][j]);
                break;
			default:
				printf("%c \x1b[0m", map[i][j]);
				break;
			}

		}
		printf("\n");
	}
}

// Moves the boss of the game
static inline void moveJimmy(bool isPresent, FOE_DLL* foeList, Player* p, OBJ_DLL* ol) {
	if(isPresent) {
        FoeNode* jimmyNode = findFoeByName(foeList, "Jimmy");
        if (jimmyNode != NULL) {
            int dRow = abs((int)p->E.pos.row - (int)jimmyNode->f->E.pos.row);
            int dCol = abs((int)p->E.pos.col - (int)jimmyNode->f->E.pos.col);

            if (dRow > dCol) {
                if (p->E.pos.row > jimmyNode->f->E.pos.row) {
                    jimmyNode->f->E.pos.row++;
                }
                else {
                    jimmyNode->f->E.pos.row--;
                }
            }
            else {
                if (p->E.pos.col > jimmyNode->f->E.pos.col) {
                    jimmyNode->f->E.pos.col++;
                }
                else {
                    jimmyNode->f->E.pos.col--;
                }
            }
        }
        printf("Jimmy moved toward %s! (x: %d, y: %d)\n", p->E.name, jimmyNode->f->E.pos.col + 1, jimmyNode->f->E.pos.row + 1);
        if(jimmyNode->f->E.pos.col == p->E.pos.col && jimmyNode->f->E.pos.row == p->E.pos.row) {
            printf("%s has found you... Prepare for the BATTLE OF AEONS!!!\n", jimmyNode->f->E.name);
            SLEEP_MS(500);
            foeChoosesAction(p, jimmyNode->f, ol, foeList);
        }
        SLEEP_MS(500);
	}
}

// Allows the Foes (except for Jimmy) to move
static inline void moveFoes(FOE_DLL* foeList, OBJ_DLL* objList, Player* p) {
	FoeNode* current = foeList->head;
	while (current != NULL && strcmp(current->f->E.name, "Jimmy") != 0) {
		if(!current->f->E.dead) {
            bool moved = false;
            Pos newPos;
            Pos prevPos = current->f->E.pos;
            prevPos.occupied = false;
            do {
                newPos = prevPos;
                switch (rand() % 4) { // 0 = up, 1 = down, 2 = left, 3 = right
                case 0: // Move up
                    if (newPos.col > 0) {
                        newPos.occupied = false;
                        newPos.col -= 1;
                        newPos.occupied = true;
                    }
                    break;
                case 1: // Move down
                    if (newPos.col < 14) {
                        newPos.occupied = false;
                        newPos.col += 1;
                        newPos.occupied = true;
                    }
                    break;
                case 2: // Move left
                    if (newPos.row > 0) {
                        newPos.occupied = false;
                        newPos.row -= 1;
                        newPos.occupied = true;
                    }
                    break;
                case 3: // Move right
                    if (newPos.row < 14) {
                        newPos.occupied = false;
                        newPos.row += 1;
                        newPos.occupied = true;
                    }
                    break;
                }
            } while ((findFoeByPos(foeList, newPos) != NULL) && findObjByPos(objList, newPos) != NULL && newPos.occupied); // Retry if occupied

            // Move to new position if it's not occupied
            if (findFoeByPos(foeList, newPos) == NULL && findObjByPos(objList, newPos) == NULL) {
                current->f->E.pos = newPos;
                moved = true;
            }
            else {
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
		SLEEP_MS(166);
	}
}

static inline bool unknownOne(const char* msg) {
    return
        strcmp(msg, "Phantom")!=0 &&
        strcmp(msg, "Sakura")!=0 &&
        strcmp(msg, "Akari")!=0 &&
        strcmp(msg, "Sasaki")!=0 &&
        strcmp(msg, "Hana")!=0 &&
        strcmp(msg, "Thomas")!=0 &&
        strcmp(msg, "Minerva")!=0 &&
        strcmp(msg, "Lucius")!=0 &&
        strcmp(msg, "Aleister")!=0 &&
        strcmp(msg, "Saki")!=0 &&
        strcmp(msg, "Nari")!=0;
}

// Initializes the player and the spells in order to avoid access violation
// Also, a small greeting under certain conditions
static inline Player* initPlayer() {
	Player* p = (Player*)malloc(sizeof(Player));

	if (p != NULL) {
		char msg[100] = "";
		printf("Name your hero: ");
		scanf("%[^\n]s", msg);
		p->E.name = strdup(msg);
		p->money = 0;
		defPlayerValRestore(p);
		initPlayerMagic(p);

		if(!unknownOne(msg)) {
            if(strcmp(msg, "Thomas") == 0) {
                printf("I'm not afraid of your hounds... ");
                for(int i=0; i<strlen(msg); i++) {
                    printf("\x1b[31m%c", toupper(msg[i]));
                }
                printf("\x1b[31m!\x1b[0m");
            }

            if(strcmp(msg, "Hana") == 0) {
                // Tic-Tac-Toe... just for prank...
                printf("Oh, little child... You're not suited for such realms at all...\n");
                SLEEP_MS(500);
                printf("Let's play something else instead, shall we?\n");
                SLEEP_MS(500);
                char yesno = ' ';
                while(yesno != 'n') {
                    system("cls");
                    gameMech_3T();
                    printf("Again? (Y/N) ");
                    scanf( "%c", &yesno);
                }
            }

            if(strcmp(msg, "Sasaki") == 0) {
                printf("\x1b[31mTo think you can live with such secrets... ");
                SLEEP_MS(500);
                printf("\x1b[31mHave you ever wondered why could she see a glimpse of her future... ");
                for(int i=0; i<strlen(msg); i++) {
                    printf("\x1b[31m%c", toupper(msg[i]));
                    SLEEP_MS(1000/3);
                }
                printf("\x1b[31m?\x1b[0m\n");
            }

            if(strcmp(msg, "Akari") == 0 || strcmp(msg, "Lucius") == 0 || strcmp(msg, "Minerva") == 0) {
                p->M[5].acquired = true;
                if(strcmp(msg, "Akari")==0) {
                    p->M[5].magName = strdup(AKARI);
                    p->M[5].magATK = RSOJ_ATK;
                    p->M[5].magCost = RSOJ_COST;
                    p->boughtMagic = true;
                }
                printf("\x1b[31mSo, you have finally come for my head... ");
                for(int i=0; i<strlen(msg); i++) {
                    printf("\x1b[31m%c", toupper(msg[i]));
                    SLEEP_MS(1000/3);
                }
                printf("\x1b[31m!\x1b[0m\n");
            }

            if(strcmp(msg, "Aleister") == 0 || strcmp(msg, "Al") == 0) {
                p->M[4].acquired = true;
                p->boughtVigor = true;
                PLAYER_MAX_HP += (p->lv-1)*LVUP_VAL + 30;
                PLAYER_MAX_MP += (p->lv-1)*LVUP_VAL + 30;
                p->E.hp = PLAYER_MAX_HP;
                p->mp = PLAYER_MAX_MP;
                printf("\x1b[31mMy death will never bring Her back... ");
                for(int i=0; i<strlen(msg); i++) {
                    printf("\x1b[31m%c", toupper(msg[i]));
                    SLEEP_MS(1000/3);
                }
                printf("\x1b[31m!\x1b[0m\n");
            }

            if(strcmp(msg, "Sakura") == 0) {
                p->M[2].acquired = true;
                printf("\x1b[31mAs if you can defeat me... ");
                for(int i=0; i<strlen(msg); i++) {
                    printf("\x1b[31m%c", toupper(msg[i]));
                    SLEEP_MS(1000/3);
                }
                printf("\x1b[31m...\x1b[0m\n");
            }

            if(strcmp(msg, "Phantom") == 0) {
                p->lv = 10;

                for(int i=0; i<SKILL_ROSTER; i++) {
                    p->M[i].acquired = true;
                }

                p->M[5].magName = strdup(AKARI);
                p->M[5].magATK = RSOJ_ATK;
                p->M[5].magCost = RSOJ_COST;
                p->boughtMagic = true;

                p->boughtATK = true;
                p->boughtDEF = true;
                p->boughtVigor = true;
                PLAYER_MAX_HP += (p->lv-1)*LVUP_VAL + 30;
                PLAYER_MAX_MP += (p->lv-1)*LVUP_VAL + 30;
                PLAYER_ATK += (p->lv-1)*LVUP_VAL + 30;
                PLAYER_DEF += (p->lv-1)*LVUP_VAL + 30;
                p->E.hp = PLAYER_MAX_HP;
                p->mp = PLAYER_MAX_MP;
                p->E.atk = PLAYER_ATK;
                p->E.def = PLAYER_DEF;

                printf("\x1b[31mGet psyched for your funeral... ");
                for(int i=0; i<strlen(msg); i++) {
                    printf("\x1b[31m%c", toupper(msg[i]));
                    SLEEP_MS(1000/3);
                }
                printf("\x1b[31m!!!\x1b[0m\n");
            }

            if(strcmp(msg, "Nari") == 0) {
                p->M[1].acquired = true;
                p->boughtATK = true;
                p->boughtDEF = true;
                PLAYER_ATK += (p->lv-1)*LVUP_VAL + 30;
                PLAYER_DEF += (p->lv-1)*LVUP_VAL + 30;
                p->E.atk = PLAYER_ATK;
                p->E.def = PLAYER_DEF;

                printf("\x1b[31mThe failed materfamilias...\n");
                SLEEP_MS(500);
                printf("\x1b[31mHow does it feel like to be in Hell for betraying your own, innocent blood...\n");
                SLEEP_MS(500);
                printf("\x1b[31mEven killing her with that disingenuous move of yours...\n");
                SLEEP_MS(500);
                printf("\x1b[31mJust because of a lie shattering your pride... ");
                for(int i=0; i<strlen(msg); i++) {
                    printf("\x1b[31m%c", toupper(msg[i]));
                    SLEEP_MS(1000/3);
                }
                printf("\x1b[31m...\x1b[0m\n");
            }

            if(strcmp(msg, "Saki") == 0) {
                printf("\x1b[31mYou...\n");
                SLEEP_MS(500);
                printf("\x1b[0mYou're not fit for this realm...\n");
                SLEEP_MS(500);
                printf("\x1b[0mI pity you...\n");
                SLEEP_MS(500);
                printf("\x1b[0mEven though I'm supposed to be your enemy...\n");
                SLEEP_MS(500);
                printf("\x1b[0mI cannot let you in...");
                SLEEP_MS(2000);

                char choice[2]={' ', '>'};
                while(1) {
                    system("cls");
                    printf("%c\tI cannot let you do your evil either! ... I need to be free... I'm going in, no matter what!\n%c\tI... I can understand... I'm not ready yet...",
                           choice[0], choice[1]);
                    char input = getch();
                    switch(input) {
                    case 'w':
                        if(choice[0]=='>') {
                            choice[0]=' ';
                            choice[1]='>';
                        }
                        else {
                            choice[1]=' ';
                            choice[0]='>';
                        }
                        break;
                    case 's':
                        if(choice[0]=='>') {
                            choice[0]=' ';
                            choice[1]='>';
                        }
                        else {
                            choice[1]=' ';
                            choice[0]='>';
                        }
                        break;
                    case 13:
                        if(choice[0]=='>') {
                            printf("\nYou don't know what you're dwelling yourself into... ");
                            for(int i=0; i<strlen(msg)+3; i++) {
                                printf("\x1b[31m%c", toupper(msg[i]));
                                SLEEP_MS(1000/3);
                                if(i>strlen(msg)) {
                                    printf("\x1b[31m.");
                                    SLEEP_MS(1000/3);
                                }
                            }
                            printf("\x1b[0m\n");

                            PLAYER_ATK /= 2;
                            p->E.atk = PLAYER_ATK;
                            PLAYER_DEF /= 2;
                            p->E.def = PLAYER_DEF;
                            PLAYER_MAX_HP *= 2;
                            p->E.hp = PLAYER_MAX_HP;
                            PLAYER_MAX_MP *= 2;
                            p->mp = PLAYER_MAX_MP;

                            p->M[3].acquired = true;

                            return p;
                        }
                        else {
                            printf("\nA wise decision...\n");
                            exit(265918);
                        }
                    }
                }

            }
		}

        else {
            printf("\x1b[31mWelcome to Hell... ");
            SLEEP_MS(500);
            for(int i=0;i<strlen(msg); i++) {
                printf("\x1b[31m%c", toupper(msg[i]));
                SLEEP_MS(1000/3);
            }
            printf("\x1b[31m!\x1b[0m\n");
        }
	}
	SLEEP_MS(2000);

	return p;
}

// Initializes the map
static inline void initMap(char map[MAP_SIZE][MAP_SIZE]) {
	memset(map, '\0', sizeof(char) * MAP_SIZE * MAP_SIZE);
	FILE* mapFP = fopen("map.txt", "r");

	if (mapFP == NULL) {
		perror("ERROR OPENING FILE");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < MAP_SIZE; i++) {
		for (int j = 0; j < MAP_SIZE; j++) {
			int c = fgetc(mapFP);
			if (c == EOF) {
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
static inline void placeObjectsOnMap(OBJ_DLL* list, FOE_DLL* fl, Player* p, const unsigned int num) {
	for (int i = 0; i < num; i++) {
        switch (rand() % 7) {
            case 0:
                insertObjIntoList(list, fl, 'S', BUFF, p);
                break;
            case 1:
                insertObjIntoList(list, fl, 'A', BUFF, p);
                break;
            case 2:
                insertObjIntoList(list, fl, '+', BUFF, p);
                break;
            case 3:
                insertObjIntoList(list, fl, '/', BUFF, p);
                break;
            case 4:
                insertObjIntoList(list, fl, 'M', BUFF, p);
                break;
            case 5:
                insertObjIntoList(list, fl, '&', BUFF*2, p);
                break;
            case 6:
                insertObjIntoList(list, fl, '$', rand() % 30 + 10, p);
                break;
        }
	}
}

// Allows the Player to pick Objects up and consume them
static inline void playerSeeksObj(Player* p, OBJ_DLL* list) {
	static int mangaFound = 0;
	ObjNode* currObjNode = list->head;
	while (currObjNode != NULL) {
		if (currObjNode->o->pos.col == p->E.pos.col && currObjNode->o->pos.row == p->E.pos.row) {
			// Player has found an Object
			switch (currObjNode->o->sign) {
			case '+':
				printf("%s found some Elixir! HP +%d\n", p->E.name, currObjNode->o->eff);
				if ((currObjNode->o->eff + p->E.hp) >= PLAYER_MAX_HP) {
					printf("Health overcharge!\n");
					p->E.hp += currObjNode->o->eff;
				}
				else {
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
				printf("%s found an Aegis Core! DEF +%d\nAegis Cores in %s's possession: %d\n", p->E.name, currObjNode->o->eff, p->E.name, p->aegisPickedUp);
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
				if ((currObjNode->o->eff + p->mp) >= PLAYER_MAX_MP) {
					printf("Mana overcharge!\n");
					p->mp += currObjNode->o->eff;
				}
				else {
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
				if (rand() % 2 == 0) {
					printf("%s has found a bowl of Mac & Cheese with some Chicken Nuggets! Its best before date is still yet to expire! HP +%d\n", p->E.name, currObjNode->o->eff);
					p->E.hp += currObjNode->o->eff;
				}
				else if (rand() % 3 == 0) {
                    printf("%s has found a Titanium Blade! ATK +%d\n", p->E.name, currObjNode->o->eff);
                    p->E.atk += currObjNode->o->eff;
                }
                else if (rand() % 5 == 0) {
					printf("%s has found an Antaresian Shield! DEF +%d\n", p->E.name, currObjNode->o->eff);
					p->E.def += currObjNode->o->eff;
                }
                else if (rand() % 7 == 0) {
                    printf("%s has found an Ancient Diary! XP +%d ", p->E.name, currObjNode->o->eff);
                    xp_gain(currObjNode->o->eff, p, false);
                }
                else if (rand() % 11 == 0) {
					printf("%s has found a Mana Potion! Its best before date is still yet to expire! MP +%d\n", p->E.name, currObjNode->o->eff);
					p->mp += currObjNode->o->eff;
                }
                else if (rand() % 13 == 0) {
                    unsigned int itemVal = rand() % 100 + 5 * currObjNode->o->eff;
					printf("%s has found a valuable item! Lucky bastard... GOLD +%d\n", p->E.name, itemVal);
					p->money += itemVal;
                }
				else {
					mangaFound++;
					if (mangaFound < 3) {
						printf("%s has found nothing in that chest...\nExcept for a manga with a topless girl and butterflies on its cover...\n", p->E.name);
						printf("Our dear wanderer left it there, for it was a bad omen.\n");
					}
					if (mangaFound == 3) {
						printf("%s has found that cursed manga again.\nCuriosity finally took over his mind and read it...\n", p->E.name);
						printf("Our dear wanderer became enraged about its content...\nHow a poor girl had to suffer in such an inhumane way...\nATK +20! DEF -20!\n");
						mangaFound = 0;
						p->E.atk += 20;
						p->E.def -= 20;
					}
				}
				currObjNode->o->found = true;
				break;
			}

			SLEEP_MS(500);

			if(currObjNode->o->found) {
                removeObjNode(list, currObjNode);
			}

			currObjNode = currObjNode->next;
		}
		else {
			currObjNode = currObjNode->next;
		}
	}
}

// The player takes action upon input (movement, attacking foes in its vicinity)
// It becomes true when they moved
static inline bool playerAction(char input, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList) {
	bool moved = false;
	bool isShopThreatened = false;
	unsigned int foesThreateningShop = 0;

	FoeNode* curr = foeList->head;

	char exitChoice[2] = { ' ', '>' };
	bool exitChose = false;

	// keyboard input
	input = getch();
	switch (input) {
	case 'w':
		foesThreateningShop = 0;
		if (p->E.pos.col > 0) {
			p->E.pos.occupied = false;
			p->E.pos.col--;
			p->E.pos.occupied = true;
			moved = true;
		}
		else printf("%s bumped into a wall! (X: %d, Y: %d)\n", p->E.name, p->E.pos.col + 1, p->E.pos.row + 1);
		break;
	case 's':
		if (p->E.pos.col < 14) {
			p->E.pos.occupied = false;
			p->E.pos.col++;
			p->E.pos.occupied = true;
			moved = true;
		}
		else printf("%s bumped into a wall! (X: %d, Y: %d)\n", p->E.name, p->E.pos.col + 1, p->E.pos.row + 1);
		break;
	case 'a':
		if (p->E.pos.row > 0) {
			p->E.pos.occupied = false;
			p->E.pos.row--;
			p->E.pos.occupied = true;
			moved = true;
		}
		else printf("%s bumped into a wall! (X: %d, Y: %d)\n", p->E.name, p->E.pos.col + 1, p->E.pos.row + 1);
		break;
	case 'd':
		if (p->E.pos.row < 14) {
			p->E.pos.occupied = false;
			p->E.pos.row++;
			p->E.pos.occupied = true;
			moved = true;
		}
		else printf("%s bumped into a wall! (X: %d, Y: %d)\n", p->E.name, p->E.pos.col + 1, p->E.pos.row + 1);
		break;
	case 'e': impendingDoom(p, foeList, objList); break;
    //case 'l': placeObjectsOnMap(objList, foeList, p, 5); break; // debug, floods the map with object to check for appending mistakes
	case '0': saveData(p); break;
	case 32:
		while (curr != NULL) {
			if (map[curr->f->E.pos.col][curr->f->E.pos.row] == 'T') {
				isShopThreatened = true;
				foesThreateningShop++;
			}
			curr = curr->next;
		}

		if (map[p->E.pos.col][p->E.pos.row] == 'T' && !isShopThreatened)
			go2Shop(p);
		else if (isShopThreatened)
			printf("%s needs to slay %d monsters at Town to access the shop!\n", p->E.name, foesThreateningShop);
		else if (map[p->E.pos.col][p->E.pos.row] != 'T')
			printf("There is no one who could sell the hero anything here...\n");
		break;
	case 27:
		while (!exitChose) {
			system("cls");
			printf("Are you sure you want to exit? (Any unsaved data will be lost.)\n\t%c YES\n\t%c NO", exitChoice[0], exitChoice[1]);
			char exitInput = getch();
			switch (exitInput) {
			case 'w':
				if (exitChoice[0] == '>') {
					exitChoice[0] = ' ';
					exitChoice[1] = '>';
				}
				else {
					exitChoice[0] = '>';
					exitChoice[1] = ' ';
				}
				break;
			case 's':
				if (exitChoice[1] == '>') {
					exitChoice[0] = '>';
					exitChoice[1] = ' ';
				}
				else {
					exitChoice[0] = ' ';
					exitChoice[1] = '>';
				}
				break;
			case 13:
				exitChose = true;
				if (exitChoice[0] == '>') {
                    destroyFoeList(foeList);
                    destroyObjList(objList);
                    free(p);
                    exit(177013);
				}
				else break;
			}
		}
	}

	if (moved) {
		printf("%s moved! (X: %d, Y: %d)\n", p->E.name, p->E.pos.col + 1, p->E.pos.row + 1);
		playerSeeksObj(p, objList); // After moving one unit in any direction, Player checks if "there is something on the ground"
		curr = foeList->head;
		while(curr != NULL) {
            if(curr->f->E.pos.col == p->E.pos.col && curr->f->E.pos.row == p->E.pos.row) {
                int rndGen = rand()%2;
                switch(rndGen) {
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

// Creates Foes based on the Player's level
// Adjusted not to flood the map
static inline void breedFoes(Player* p, OBJ_DLL* objList, FOE_DLL* foeList) {
    for(unsigned int i = 0; i < 5; i++) {
        if (p->lv <= 3) {
            if (getFoeCountByName(foeList, SLIME_NAME) < 2) { // Limit number of Slimes
                insertFoeIntoList(p, objList, foeList, SLIME_NAME, SLIME_HP, SLIME_ATK, SLIME_DEF, SLIME_XP, SLIME_LOOT);
                printf("Slime summoned!\n");
            }
        }

        if (p->lv >= 4) {
            if (getFoeCountByName(foeList, GOBLIN_NAME) < 2 && rand() % 2 == 0) { // Limit number of Goblins
                insertFoeIntoList(p, objList, foeList, GOBLIN_NAME, GOBLIN_HP, GOBLIN_ATK, GOBLIN_DEF, GOBLIN_XP, GOBLIN_LOOT);
                printf("Goblin summoned!\n");
            } else if (getFoeCountByName(foeList, SLIME_NAME) < 2) {
                insertFoeIntoList(p, objList, foeList, SLIME_NAME, SLIME_HP, SLIME_ATK, SLIME_DEF, SLIME_XP, SLIME_LOOT);
                printf("Slime summoned!\n");
            }
        }

        if (p->lv >= 6) {
            if (getFoeCountByName(foeList, ACOLYTE_NAME) < 2 && rand() % 2 == 0) { // Limit number of Acolytes
                insertFoeIntoList(p, objList, foeList, ACOLYTE_NAME, ACOLYTE_HP, ACOLYTE_ATK, ACOLYTE_DEF, ACOLYTE_XP, ACOLYTE_LOOT);
                printf("Acolyte summoned!\n");
            } else if (getFoeCountByName(foeList, GOBLIN_NAME) < 2) {
                insertFoeIntoList(p, objList, foeList, GOBLIN_NAME, GOBLIN_HP, GOBLIN_ATK, GOBLIN_DEF, GOBLIN_XP, GOBLIN_LOOT);
                printf("Goblin summoned!\n");
            }
        }

        if (p->lv >= 8) {
            if (getFoeCountByName(foeList, THWARTED_SELF_NAME) < 1 && rand() % 2 == 0) { // Limit number of Thwarted Selves
                insertFoeIntoList(p, objList, foeList, THWARTED_SELF_NAME, THWARTED_SELF_HP, THWARTED_SELF_ATK, THWARTED_SELF_DEF, THWARTED_SELF_XP, THWARTED_SELF_LOOT);
                printf("Your Thwarted Self summoned!\n");
            } else if (getFoeCountByName(foeList, ACOLYTE_NAME) < 2) {
                insertFoeIntoList(p, objList, foeList, ACOLYTE_NAME, ACOLYTE_HP, ACOLYTE_ATK, ACOLYTE_DEF, ACOLYTE_XP, ACOLYTE_LOOT);
                printf("Acolyte summoned!\n");
            }
        }

        if (p->lv >= 9) {
            if (getFoeCountByName(foeList, GOLEM_NAME) < 1 && rand() % 2 == 0) { // Limit number of Carcass Golems
                insertFoeIntoList(p, objList, foeList, GOLEM_NAME, GOLEM_HP, GOLEM_ATK, GOLEM_DEF, GOLEM_XP, GOLEM_LOOT);
                printf("Carcass Golem summoned!\n");
            } else if (getFoeCountByName(foeList, THWARTED_SELF_NAME) < 1) {
                insertFoeIntoList(p, objList, foeList, THWARTED_SELF_NAME, THWARTED_SELF_HP, THWARTED_SELF_ATK, THWARTED_SELF_DEF, THWARTED_SELF_XP, THWARTED_SELF_LOOT);
                printf("Your Thwarted Self summoned!\n");
            }
        }
    }

	if (p->lv >= 10 && !jimmySummoned) {
		insertFoeIntoList(p, objList, foeList, BOSS_NAME, BOSS_HP, BOSS_ATK, BOSS_DEF, 0, 0);
		foeList->tail->f->E.pos.col = foeList->tail->f->E.pos.row = 14;
		jimmySummoned = true;
	}
}

// A simple non-interactive UI
static inline void showPlayerInfo(Player* p) {
	printf("HP:\t%d/%d\n", p->E.hp, PLAYER_MAX_HP);
	printf("MP:\t%d/%d\n", p->mp, PLAYER_MAX_MP);
	printf("ATK:\t%d/%d\n", p->E.atk, PLAYER_ATK);
	printf("DEF:\t%d/%d\n", p->E.def, PLAYER_DEF);
	printf("LV:\t%d (XP:\t%d/%d)\n", p->lv, p->E.xp, LV_UP_REQ(p->lv));
	printf("GOLD:\t%d\n", p->money);
	printf("\nLives:\t%d\n", 10-p->deathCount);
}

// The Player's current location on the map (under the UI)
static inline void narrate(char map[15][15], Player* p) {
	printf("Current location: ");
	switch (map[p->E.pos.col][p->E.pos.row]) {
	case 'F': printf("Forest\n"); break;
	case 'R': printf("Riverside\n"); break;
	case 'T': printf("Town\n"); break;
	case 'V': printf("Valley\n"); break;
	case 'D': printf("Desert\n"); break;
	}
}
