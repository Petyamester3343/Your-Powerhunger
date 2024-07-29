#include "game_data.c"
#include "battle_sys.c"
#include "shop.c"
#include "DLL_container.c"

#include <math.h>
#include <time.h>

// draws the map
static void drawMap(char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // to manipulate the color of the output
	system("cls");

	for (unsigned int i = 0; i < MAP_SIZE; i++) {
		for (unsigned int j = 0; j < MAP_SIZE; j++) {
			bool isDrawn = false;

			// Draw player
			if (i == p->E.pos.col && j == p->E.pos.row) {
				SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
				printf("X \x1b[0m");
				isDrawn = true;
			}

			// Draw objects
			ObjNode* currObj = objList->head;
			int count = 0;
			while (currObj != NULL) {
				if (currObj->o->pos.col == i && currObj->o->pos.row == j) {
					count++;
					if (count == 1 && !isDrawn) {
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
						printf("%c \x1b[0m", currObj->o->sign);
						isDrawn = true;
					}
				}
				currObj = currObj->next;
			}

			// Draw foes
			FoeNode* currFoe = foeList->head;
			while (currFoe != NULL) {
				if (currFoe->f->E.pos.col == i && currFoe->f->E.pos.row == j) {
					if (!isDrawn) {
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
						printf("! \x1b[0m");
						isDrawn = true;
					}
					else {
						// Foe cannot occupy this cell because it's already occupied
						isDrawn = false;
						break; // Exit the loop to prevent further checks
					}
				}
				currFoe = currFoe->next;
			}

			if (!isDrawn) {
				switch (map[i][j]) {
				case 'T':
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
					printf("%c \x1b[0m", map[i][j]);
					break;
				default:
					printf("%c \x1b[0m", map[i][j]);
				}
			}
		}
		printf("\n");
	}
}

static void moveJimmy(bool isPresent, FOE_DLL* foeList, Player* p) {
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
	}
}

static void moveFoes(FOE_DLL* foeList, OBJ_DLL* objList, Player* p) {
	FoeNode* current = foeList->head;
	while (current != NULL) {
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
			foeChoosesAction(p, current->f);

		if (moved)
			printf("%s moved (X: %d, Y: %d)\n", current->f->E.name, current->f->E.pos.col + 1, current->f->E.pos.row + 1);
		else
			printf("%s passed this turn (X: %d, Y: %d)\n", current->f->E.name, current->f->E.pos.col + 1, current->f->E.pos.row + 1);

		current = current->next;
		Sleep(166);
	}
}

// initializes the player and the spells
// in order to avoid access violation
static Player* initPlayer() {
	Player* p = (Player*)malloc(sizeof(Player));

	if (p != NULL) {
		char msg[100] = "";
		printf("Name your hero: ");
		scanf("%[^\n]s", msg);
		if(strcmp(_tolower(msg), "akari")==0)
            p->boughtMagic = true;
		p->E.name = strdup(msg);
		p->money = 0;
		defPlayerValRestore(p);
		initPlayerMagic(p);
	}

	return p;
}

static void initMap(char map[MAP_SIZE][MAP_SIZE]) {
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

static void placeObjectsOnMap(OBJ_DLL* list, FOE_DLL* fl, Player* p, const unsigned int num) {
	for (int i = 0; i < num; i++) {
		Object* o = (Object*)malloc(sizeof(Object));
		if (o != NULL) {
			switch (rand() % 7) {
			case 0:
				insertObjIntoList(list, fl, 'S', 5, p);
				break;
			case 1:
				insertObjIntoList(list, fl, 'A', 5, p);
				break;
			case 2:
				insertObjIntoList(list, fl, '+', 5, p);
				break;
			case 3:
				insertObjIntoList(list, fl, '/', 5, p);
				break;
			case 4:
				insertObjIntoList(list, fl, 'M', 5, p);
				break;
			case 5:
				insertObjIntoList(list, fl, '&', 10, p);
				break;
			case 6:
				insertObjIntoList(list, fl, '$', rand() % 30 + 10, p);
				break;
			}
		}
	}
}

static void playerSeeksObj(Player* p, OBJ_DLL* list) {
	static int mangaFound = 0;
	ObjNode* currObjNode = list->head;
	while (currObjNode != NULL) {
		if (currObjNode->o->pos.col == p->E.pos.col && currObjNode->o->pos.row == p->E.pos.row) {
			ObjNode* nextObjNode = currObjNode->next;

			// Found the object
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
				printf("%s found an Aegis Core! DEF +%d\n", p->E.name, currObjNode->o->eff);
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
					if (mangaFound < 3 && mangaFound != 0)
						printf("%s has found nothing in that chest...\nExcept for a manga with a topless girl and butterflies on its cover...\nOur dear wanderer left it there, for it was a bad omen.\n", p->E.name);
					if (mangaFound == 3) {
						printf("%s has found that cursed manga again.\nCuriosity finally took over his mind and read it...\nOur dear wanderer became enraged about its content...\nHow a poor girl had to suffer in such an inhumane way...\nATK +20! DEF -20!\n", p->E.name);
						mangaFound = 0;
						p->E.atk += 20;
						p->E.def -= 20;
					}
				}
				currObjNode->o->found = true;
				break;
			}

			Sleep(500);

			if (currObjNode->o->found == true) {
				if (currObjNode == list->head && currObjNode == list->tail) {
					// Case: Only one node in the list
					list->head = NULL;
					list->tail = NULL;
				}
				else if (currObjNode == list->head) {
					// Case: Deleting the head node
					list->head = currObjNode->next;
					list->head->prev = NULL;
				}
				else if (currObjNode == list->tail) {
					// Case: Deleting the tail node
					list->tail = currObjNode->prev;
					list->tail->next = NULL;
				}
				else {
					// Case: Deleting a node in between
					currObjNode->prev->next = currObjNode->next;
					currObjNode->next->prev = currObjNode->prev;
				}

				// Free the memory allocated to the node
				free(currObjNode);
			}
			currObjNode = nextObjNode;
		}
		else {
			currObjNode = currObjNode->next;
		}
	}
}

// The player takes action upon input (movement, attacking foes in its vicinity)
// It becomes true when they moved
static bool playerAction(char input, char map[15][15], Player* p, OBJ_DLL* objList, FOE_DLL* foeList) {
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
	case 'e': impendingDoom(p, foeList); break;
		// case 'l': placeObjectsOnMap(objList, foeList, p, 5); break; // debug
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
				if (exitChoice[0] == '>') exit(177013);
				else break;
			}
		}
	}

	if (moved) {
		printf("%s moved! (X: %d, Y: %d)\n", p->E.name, p->E.pos.col + 1, p->E.pos.row + 1);
		playerSeeksObj(p, objList);
	}

	return moved;
}

// A method which creates Foes based on the Player's level
static void breedFoes(Player* p, OBJ_DLL* objList, FOE_DLL* foeList) {
    for(unsigned int i=0; i<5; i++) {
        if (p->lv <= 3) {
            if (getFoeCountByName(foeList, SLIME_NAME) < 2) { // Limit number of Slimes
                insertFoeIntoList(p, objList, foeList, SLIME_NAME, SLIME_HP, SLIME_ATK, SLIME_DEF, SLIME_XP, 10);
                printf("Slime summoned!\n");
            }
        }

        if (p->lv >= 4) {
            if (getFoeCountByName(foeList, GOBLIN_NAME) < 2 && rand() % 2 == 0) { // Limit number of Goblins
                insertFoeIntoList(p, objList, foeList, GOBLIN_NAME, GOBLIN_HP, GOBLIN_ATK, GOBLIN_DEF, GOBLIN_XP, 20);
                printf("Goblin summoned!\n");
            } else if (getFoeCountByName(foeList, SLIME_NAME) < 2) {
                insertFoeIntoList(p, objList, foeList, SLIME_NAME, SLIME_HP, SLIME_ATK, SLIME_DEF, SLIME_XP, 10);
                printf("Slime summoned!\n");
            }
        }

        if (p->lv >= 6) {
            if (getFoeCountByName(foeList, ACOLYTE_NAME) < 2 && rand() % 2 == 0) { // Limit number of Acolytes
                insertFoeIntoList(p, objList, foeList, ACOLYTE_NAME, ACOLYTE_HP, ACOLYTE_ATK, ACOLYTE_DEF, ACOLYTE_XP, 40);
                printf("Acolyte summoned!\n");
            } else if (getFoeCountByName(foeList, GOBLIN_NAME) < 2) {
                insertFoeIntoList(p, objList, foeList, GOBLIN_NAME, GOBLIN_HP, GOBLIN_ATK, GOBLIN_DEF, GOBLIN_XP, 20);
                printf("Goblin summoned!\n");
            }
        }

        if (p->lv >= 8) {
            if (getFoeCountByName(foeList, THWARTED_SELF_NAME) < 1 && rand() % 2 == 0) { // Limit number of Thwarted Selves
                insertFoeIntoList(p, objList, foeList, THWARTED_SELF_NAME, THWARTED_SELF_HP, THWARTED_SELF_ATK, THWARTED_SELF_DEF, THWARTED_SELF_XP, 80);
                printf("Your Thwarted Self summoned!\n");
            } else if (getFoeCountByName(foeList, ACOLYTE_NAME) < 2) {
                insertFoeIntoList(p, objList, foeList, ACOLYTE_NAME, ACOLYTE_HP, ACOLYTE_ATK, ACOLYTE_DEF, ACOLYTE_XP, 40);
                printf("Acolyte summoned!\n");
            }
        }

        if (p->lv >= 9) {
            if (getFoeCountByName(foeList, GOLEM_NAME) < 1 && rand() % 2 == 0) { // Limit number of Carcass Golems
                insertFoeIntoList(p, objList, foeList, GOLEM_NAME, GOLEM_HP, GOLEM_ATK, GOLEM_DEF, GOLEM_XP, 160);
                printf("Carcass Golem summoned!\n");
            } else if (getFoeCountByName(foeList, THWARTED_SELF_NAME) < 1) {
                insertFoeIntoList(p, objList, foeList, THWARTED_SELF_NAME, THWARTED_SELF_HP, THWARTED_SELF_ATK, THWARTED_SELF_DEF, THWARTED_SELF_XP, 80);
                printf("Your Thwarted Self summoned!\n");
            }
        }
    }

	if (p->lv == 10 && !jimmySummoned) {
		insertFoeIntoList(p, objList, foeList, BOSS_NAME, JIMMY_HP, JIMMY_ATK, JIMMY_DEF, 0, 0);
		foeList->tail->f->E.pos.col = foeList->tail->f->E.pos.row = 14;
		jimmySummoned = true;
	}
}

// debug feature
static void showPlayerInfo(Player* p) {
	printf("HP:\t%d/%d\n", p->E.hp, PLAYER_MAX_HP);
	printf("MP:\t%d/%d\n", p->mp, PLAYER_MAX_MP);
	printf("ATK:\t%d/%d\n", p->E.atk, PLAYER_ATK);
	printf("DEF:\t%d/%d\n", p->E.def, PLAYER_DEF);
	printf("LV:\t%d (XP:\t%d/%d)\n", p->lv, p->E.xp, LV_UP_REQ(p->lv));
	printf("GOLD:\t%d\n", p->money);
}

// debug feature
static void narrate(char map[15][15], Player* p) {
	printf("Current location: ");
	switch (map[p->E.pos.col][p->E.pos.row]) {
	case 'F': printf("Forest\n"); break;
	case 'R': printf("Riverside\n"); break;
	case 'T': printf("Town\n"); break;
	case 'V': printf("Valley\n"); break;
	case 'D': printf("Desert\n"); break;
	}
}
