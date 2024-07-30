#include "game_loop.c"

int main(void)
{
    // time seed
    srand((unsigned)time(NULL));

    // 2D character array acting as a map
    char map[MAP_SIZE][MAP_SIZE];
    initMap(map);

    // input for the user (it'd turn into getch())
    char c = ' ';

    // Instance of the Player struct
    // The user may control it
    Player* p=(Player*)malloc(sizeof(Player));

    // Doubly linked list for storing Objects and Foes
    OBJ_DLL* objList = createObjList();
    FOE_DLL* foeList = createFoeList();

    // Main Menu shows up
    MainMenu(c, map, p, objList, foeList);

    return 0;
}
