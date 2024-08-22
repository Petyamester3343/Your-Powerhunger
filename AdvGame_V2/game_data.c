#include "def_vals.h"

#include <conio.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"
#include "DLL_container.h"

// Check if the file has a .dat extension
PWRHNGR_BOOLDEF hasDatExtension(myStr fileName) {
    return (strrchr(fileName, '.') != NULL && strcmp(strrchr(fileName, '.'), ".dat") == 0);
}

// Create the save folder if it doesn't exist
PWRHNGR_DEF ensureSaveDir() {
    if (_mkdir(SAVE_DIR) != 0 && errno != EEXIST) {
        fprintf(stderr, "Cannot create main save directory!\n");
    }
}

// Check if a file exists
PWRHNGR_BOOLDEF checkFileExists(myStr filePath) {
    return access(filePath, F_OK) != -1;
}

// Save Player data, including Objects and Foes
PWRHNGR_DEF saveData(Player* p, OBJ_DLL* ol, FOE_DLL* fl) {
    ensureSaveDir();

    char filePath[MAX_PATH];
    snprintf(filePath, sizeof(filePath), "%s%s_save.dat", SAVE_DIR, p->E.name);

    printf("Saving: %s\n", filePath);

    if (checkFileExists(filePath)) {
        char choice;
        printf("File \"%s\" already exists. Overwrite? (Y/N): ", filePath);
        scanf(" %c", &choice);
        if (choice != 'Y' && choice != 'y') {
            printf("Aborting save.\n");
            return;
        }
    }

    FILE* fp = fopen(filePath, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open file!\n");
        return;
    }

    // Save Player data
    fwrite(p, sizeof(Player), 1, fp);

    // Save Player name
    size_t nameLen = strlen(p->E.name);
    fwrite(&nameLen, sizeof(size_t), 1, fp);
    fwrite(p->E.name, nameLen, 1, fp);

    // Save number of objects
    ObjNode* currO = ol->head;
    uint oC = 0;
    while (currO != NULL) {
        oC++;
        currO = currO->next;
    }
    fwrite(&oC, sizeof(uint), 1, fp);

    // Save each object
    currO = ol->head;
    while (currO != NULL) {
        fwrite(currO->o, sizeof(Object), 1, fp);
        currO = currO->next;
    }

    // Save number of foes
    FoeNode* currF = fl->head;
    uint fC = 0;
    while (currF != NULL) {
        fC++;
        currF = currF->next;
    }
    fwrite(&fC, sizeof(uint), 1, fp);

    // Save each foe
    currF = fl->head;
    while (currF != NULL) {
        fwrite(currF->f, sizeof(Foe), 1, fp);
        size_t nameLength = strlen(currF->f->E.name);
        fwrite(&nameLength, sizeof(size_t), 1, fp); // Save name length
        fwrite(currF->f->E.name, nameLength, 1, fp); // Save name itself
        currF = currF->next;
    }

    fclose(fp);
    printf("Save data created successfully!\n");
}

// List the save files in the save directory
PWRHNGR_DEF listSaveFiles(char saveFiles[][MAX_PATH], int* fileCount) {
    DIR* dir = opendir(SAVE_DIR);
    if (dir == NULL) {
        fprintf(stderr, "Cannot open save directory!\n");
        return;
    }

    struct dirent* entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (hasDatExtension(entry->d_name)) {
            strncpy(saveFiles[count], entry->d_name, MAX_PATH - 1);
            count++;
        }
    }

    closedir(dir);
    *fileCount = count;
}

// Choose and load Player data from a save file
PWRHNGR_IDEF chooseAndLoadData(Player* p, OBJ_DLL* ol, FOE_DLL* fl) {
    char saveFiles[SAVE_SIZE][MAX_PATH];
    int fileCount = 0;

    listSaveFiles(saveFiles, &fileCount);

    if (fileCount == 0) {
        printf("No save files found!\n");
        return -1;
    }

    printf("Available save files:\n");
    for (int i = 0; i < fileCount; i++) {
        printf("%d: %s\n", i, saveFiles[i]);
    }

    int choice = -1;
    printf("Enter the number of the save file to load: ");
    while (scanf("%d", &choice) != 1 || choice < 0 || choice >= fileCount) {
        printf("Invalid choice. Enter a valid number: ");
        while (getchar() != '\n'); // Clear input buffer
    }

    char filePath[MAX_PATH];
    snprintf(filePath, sizeof(filePath), "%s%s", SAVE_DIR, saveFiles[choice]);

    FILE* fp = fopen(filePath, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open file!\nStarting new game...\n");
        return -1;
    }

    fread(p, sizeof(Player), 1, fp);

    size_t nameLen;
    fread(&nameLen, sizeof(size_t), 1, fp);
    p->E.name = (char*)calloc(nameLen + 1, sizeof(char));
    fread(p->E.name, nameLen, 1, fp);

    // Load objects
    uint oC = 0;
    fread(&oC, sizeof(uint), 1, fp);
    for (uint i = 0; i < oC; i++) {
        Object* o = (Object*)malloc(sizeof(Object));
        fread(o, sizeof(Object), 1, fp);
        ObjNode* oNode = (ObjNode*)malloc(sizeof(ObjNode));
        oNode->o = o;
        oNode->next = NULL;
        oNode->prev = ol->tail;
        if (ol->tail != NULL) {
            ol->tail->next = oNode;
        } else {
            ol->head = oNode;
        }
        ol->tail = oNode;
    }

    // Load foes
    uint fC = 0;
    fread(&fC, sizeof(uint), 1, fp);
    for (uint i = 0; i < fC; i++) {
        Foe* f = (Foe*)malloc(sizeof(Foe));
        fread(f, sizeof(Foe), 1, fp);
        size_t nameLength;
        fread(&nameLength, sizeof(size_t), 1, fp);
        f->E.name = (char*)calloc(nameLength + 1, sizeof(char));
        fread(f->E.name, nameLength, 1, fp);
        FoeNode* fNode = (FoeNode*)malloc(sizeof(FoeNode));
        fNode->f = f;
        fNode->next = NULL;
        fNode->prev = fl->tail;
        if (fl->tail != NULL) {
            fl->tail->next = fNode;
        } else {
            fl->head = fNode;
        }
        fl->tail = fNode;
    }

    fclose(fp);

    // Restore player stats
    if (p->lv > 1) {
        PLAYER_MAX_HP += (p->lv - 1) * LVUP_VAL;
        PLAYER_MAX_MP += (p->lv - 1) * LVUP_VAL;
        PLAYER_ATK += (p->lv - 1) * LVUP_VAL;
        PLAYER_DEF += (p->lv - 1) * LVUP_VAL;
    }

    if (p->boughtATK) PLAYER_ATK += PERM_BUFF;
    if (p->boughtDEF) PLAYER_DEF += PERM_BUFF;
    if (p->boughtVigor) {
        PLAYER_MAX_HP += PERM_BUFF;
        PLAYER_MAX_MP += PERM_BUFF;
    }

    p->E.dead = false;
    p->E.fled = false;

    printf("Data loaded successfully!\n");

    return 0;
}

// Choose and delete a save file
PWRHNGR_DEF chooseAndDeleteData() {
    char saveFiles[10][MAX_PATH];
    int fileCount = 0;

    listSaveFiles(saveFiles, &fileCount);

    if (fileCount == 0) {
        printf("No save files found to delete!\n");
        return;
    }

    printf("Available:\n");
    for (int i = 0; i < fileCount; i++) {
        printf("%d: %s\n", i, saveFiles[i]);
    }

    int choice = -1;
    printf("Enter the number of the save file to delete: ");
    while (scanf("%d", &choice) != 1 || choice < 0 || choice >= fileCount) {
        printf("Invalid choice. Enter a valid number: ");
        while (getchar() != '\n');
    }

    char filePath[MAX_PATH];
    snprintf(filePath, sizeof(filePath), "%s%s", SAVE_DIR, saveFiles[choice]);

    if (remove(filePath) == 0) {
        printf("Deletion successful!\n");
    } else {
        perror("FILE DELETION ERROR");
    }
}
