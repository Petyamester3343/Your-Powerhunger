#include "def_vals.h"

#ifdef _WIN32
    #include <windows.h> // for Sleep()
    #include <direct.h> // for _mkdir()
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000) // usleep takes microseconds
#endif // _WIN32

#include <conio.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAVE_DIR "saves/"
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

// Checks if the file has the extension .dat
static inline bool hasDatExtension(const char* fileName) {
    return (strrchr(fileName, '.') != NULL && strcmp(strrchr(fileName, '.'), ".dat") == 0);
}

// Creates the save folder
static inline void ensureSaveDir() {
    if(_mkdir(SAVE_DIR) != 0 && errno != EEXIST) {
        fprintf(stderr, "Cannot create main save directory!\n");
    }
}

// Checks if the file exists
static inline bool checkFileExists(const char* filePath) {
    return access(filePath, F_OK) != -1;
}

// Saves the Player's stats (actions included)
static inline void saveData(Player* p) {
    ensureSaveDir();

	char filePath[MAX_PATH];
	bzero(filePath, MAX_PATH);
	snprintf(filePath, sizeof(filePath), "%s%s_save.dat", SAVE_DIR, p->E.name);

	printf("Saving: %s\n", filePath);
	SLEEP_MS(500);

	if(checkFileExists(filePath)) {
        char choice;
        printf("File \"%s\" already exists. Overwrite? (Y/N): ", filePath);
        scanf(" %c", &choice);
        while(choice != 'Y' && choice != 'y') {
            if(choice != 'y' && choice != 'Y') {
                if(choice == 'n' || choice == 'N') {
                    printf("Aborting save.\n");
                    SLEEP_MS(500);
                    return;
                }
                printf("Invalid choice!\n");
            }
        }
	}

	FILE* fp = fopen(filePath, "wb");

	if (fp == NULL) {
		fprintf(stderr, "Cannot open file!\n");
		return;
	}

	fwrite(p, sizeof(Player), 1, fp);
	fwrite(p->E.name, strlen(p->E.name), 1, fp);

	fclose(fp);
	printf("Save data created successfully!\n");

	SLEEP_MS(500);
}

// Lists the existing save files
static inline void listSaveFiles(char saveFiles[][MAX_PATH], int* fileCount) {
    DIR* dir;
    struct dirent* entry;
    int count = 0;

    dir = opendir(SAVE_DIR);
    if(dir == NULL) {
        fprintf(stderr, "Cannot open save directory!\n");
        return;
    }

    printf("Directory opened successfully!\n");

    while((entry = readdir(dir))!=NULL) {
        printf("Entry found: %s\n", entry->d_name);
        if(hasDatExtension(entry->d_name)) {
            printf("Save file found: %s\n", entry->d_name);
            bzero(saveFiles[count], MAX_PATH);
            strncpy(saveFiles[count], entry->d_name, MAX_PATH - 1);
            count++;
        }
    }

    closedir(dir);
    *fileCount = count;

    printf("Files found: %d\n", *fileCount);
}

// Allows the user to choose from the available save datas and load one of them
static inline int chooseAndLoadData(Player* p) {
	char saveFiles[SAVE_SIZE][MAX_PATH];
	bzero(saveFiles, sizeof(saveFiles));
	int fileCount = 0;

	listSaveFiles(saveFiles, &fileCount);

	if(fileCount == 0) {
        printf("No save files found!\n");
        return -1;
	}

	printf("Available save files:\n");
	for(int i=0; i<fileCount; i++) {
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
		SLEEP_MS(1000);
		return -1;
	}

	if (fseek(fp, 0, SEEK_END) < 0) {
		fclose(fp);
		return -1;
	}

	long val = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	val -= sizeof(Player);

	fread(p, sizeof(Player), 1, fp);
	p->E.name = (char*)calloc((size_t)val + 1, sizeof(char));
	if (p->E.name != NULL) {
		fread(p->E.name, (size_t)val, 1, fp);
	}

	if (p->lv > 1) {
		PLAYER_MAX_HP += (p->lv - 1) * LVUP_VAL;
		PLAYER_MAX_MP += (p->lv - 1) * LVUP_VAL;
		PLAYER_ATK += (p->lv - 1) * LVUP_VAL;
		PLAYER_DEF += (p->lv - 1) * LVUP_VAL;
	}

	p->E.dead = false;
	p->E.fled = false;

	if(p->boughtATK) PLAYER_ATK += 30;
	if(p->boughtDEF) PLAYER_DEF += 30;
	if(p->boughtVigor) {
        PLAYER_MAX_HP += 30;
        PLAYER_MAX_MP += 30;
	}

	fclose(fp);

	printf("Data loaded successfully!\n");
	SLEEP_MS(1000);

	return 0;
}

// Allows the user to choose from the available save datas and delete one of them
static inline void chooseAndDeleteData() {
	char saveFiles[10][MAX_PATH];
	int fileCount = 0;

	listSaveFiles(saveFiles, &fileCount);

	if(fileCount == 0) {
        printf("No save files found to delete!\n");
        return;
	}

	printf("Available:\n");
	for(unsigned int i = 0; i<fileCount; i++) {
        printf("%d: %s\n", i, saveFiles[i]);
	}

	int choice = -1;
	printf("Enter the number of the save file to delete: ");
	while(scanf("%d", &choice) != 1 || choice < 0 || choice >= fileCount) {
        printf("Invalid choice. Enter a valid number: ");
        while(getchar()!='\n');
	}

	char filePath[PATH_MAX];
	snprintf(filePath, sizeof(filePath), "%s%s", SAVE_DIR, saveFiles[choice]);

	if(remove(filePath)==0) {
        printf("Deletion successful!\n");
	}
	else {
        perror("FILE DELETION ERROR");
        printf("Error num: %d\n", errno);
	}

    SLEEP_MS(1000);
}
