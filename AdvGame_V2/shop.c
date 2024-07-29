#include "entities.h"
#include "def_vals.h"

#include <stdio.h>
#include <windows.h>
#include <conio.h>

// Prices of the permanent stat boosters
static const int armor_price = 1000;
static const int plasma_gun_price = 1000;
static const int invigorator_price = 750;

// The shop system (practically an upgrade system where one must sacrifice GOLD to obtain more power)
static void go2Shop(Player* p) {
	char choice[5] = { '>', ' ', ' ', ' ', ' ' };
	char exitChoice[2] = { ' ', '>' };
	char input = ' ';

	// Shop loop
	while (1) {
		system("cls");
		printf("Welcome, weary traveler!\nFeel free to browse through my wares!\n");
		printf("\t%c %s\t->\t\t$%d\n", choice[0], AKARI, RSOJ_PRICE);
		printf("\t%c %s\t->\t\t$%d\n", choice[1], "Aegis Automatized Armor", armor_price);
		printf("\t%c %s\t\t->\t\t$%d\n", choice[2], "Plasma Railgun", plasma_gun_price);
		printf("\t%c %s\t\t->\t\t$%d\n", choice[3], "Vigor Enhancer", invigorator_price);
		printf("\t%c %s\n\n", choice[4], "EXIT SHOP");

		bool staying = false;
		input = getch();
		switch (input) {
		case 'w':
			if (choice[0] == '>') {
				choice[4] = '>';
				choice[0] = ' ';
			}
			else if (choice[1] == '>') {
				choice[0] = '>';
				choice[1] = ' ';
			}
			else if (choice[2] == '>') {
				choice[1] = '>';
				choice[2] = ' ';
			}
			else if (choice[3] == '>') {
				choice[2] = '>';
				choice[3] = ' ';
			}
			else if (choice[4] == '>'){
				choice[3] = '>';
				choice[4] = ' ';
			}
			break;
		case 's':
			if (choice[0] == '>') {
				choice[1] = '>';
				choice[0] = ' ';
			}
			else if (choice[1] == '>') {
				choice[2] = '>';
				choice[1] = ' ';
			}
			else if (choice[2] == '>') {
				choice[3] = '>';
				choice[2] = ' ';
			}
			else if (choice[3] == '>') {
				choice[4] = '>';
				choice[3] = ' ';
			}
			else if (choice[4] == '>') {
				choice[0] = '>';
				choice[4] = ' ';
			}
			break;
		case 13:
			if (choice[0] == '>') {
				if (p->money >= RSOJ_PRICE && !p->boughtMagic) {
					p->money -= RSOJ_PRICE;
					p->M[5].magName = strdup(AKARI);
					p->M[5].magATK = RSOJ_ATK;
					p->M[5].magCost = RSOJ_COST;
					p->M[5].magLVReq = RSOJ_LV_REQ;
					p->M[5].acquired = true;
					p->boughtMagic = true;
					printf("%s has successfully acquired the most powerful spell known to mankind!\n", p->E.name);
					Sleep(500);
					break;
				}
				else if(p->boughtMagic){
					printf("%s has already acquired the %s!\n", p->E.name, AKARI);
					Sleep(500);
				}
				else {
                    printf("Not enough money!\n");
					Sleep(500);
				}
			}
			if (choice[1] == '>') {
				if (p->money >= armor_price && !p->boughtDEF) {
					p->money -= armor_price;
					p->E.def += 30;
					p->boughtDEF = true;
					printf("%s has purchased the Aegis Automatized Armor! DEF +30\n", p->E.name);
					Sleep(500);
					break;
				}
				else if(p->boughtDEF) {
                    printf("%s has already acquired this upgrade!\n", p->E.name);
                    Sleep(500);
				}
				else {
					printf("Not enough money!\n");
					Sleep(500);
				}
			}
			if (choice[2] == '>') {
				if (p->money >= plasma_gun_price && !p->boughtATK) {
					p->money -= plasma_gun_price;
					p->E.atk += 30;
					p->boughtATK = true;
					printf("%s has purchased the Plasma Railgun! ATK +30\n", p->E.name);
					Sleep(500);
					break;
				}
				else if(p->boughtATK) {
                    printf("%s has already acquired this upgrade!\n", p->E.name);
                    Sleep(500);
				}
				else {
					printf("Not enough money!\n");
					Sleep(500);
				}
			}
			if (choice[3] == '>') {
				if (p->money >= invigorator_price && !p->boughtVigor) {
					p->money -= invigorator_price;
					PLAYER_MAX_HP += 30;
					p->E.hp = PLAYER_MAX_HP;
					PLAYER_MAX_MP += 30;
					p->mp = PLAYER_MAX_MP;
					p->boughtVigor = true;
					printf("%s has purchased the Vigor Enhancer! HP & MP +30 PERMANENTLY!\n", p->E.name);
					Sleep(500);
					break;
				}
				else if(p->boughtVigor) {
                    printf("%s has already acquired this upgrade!\n", p->E.name);
                    Sleep(500);
				}
				else {
					printf("Not enough money!\n");
					Sleep(500);
				}
			}
			if (choice[4] == '>') {
				while (!staying) {
					system("cls");
					printf("Are you sure you want to leave the Shop?\n\t%c YES\n\t%c NO", exitChoice[0], exitChoice[1]);
					input = getch();
					switch (input) {
					case 'w':
						if (exitChoice[1] == '>') {
							exitChoice[0] = '>';
							exitChoice[1] = ' ';
						}
						else if (exitChoice[0] == '>') {
							exitChoice[1] = '>';
							exitChoice[0] = ' ';
						}
						break;
					case 's':
						if (exitChoice[0] == '>') {
							exitChoice[1] = '>';
							exitChoice[0] = ' ';
						}
						else if (exitChoice[1] == '>') {
							exitChoice[0] = '>';
							exitChoice[1] = ' ';
						}
						break;
					case 13:
						if (exitChoice[0] == '>') {
							return;
						}
						if (exitChoice[1] == '>') {
							staying = true;
						}
					}
				}
			}
		}
	}
}
