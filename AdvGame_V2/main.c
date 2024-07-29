#include "game_loop.c"

int main(void)
{
    srand((unsigned)time(NULL));

    char map[MAP_SIZE][MAP_SIZE];
    initMap(map);

    char c = ' ';

    Player* p=(Player*)malloc(sizeof(Player));

    OBJ_DLL* objList = createObjList();
    FOE_DLL* foeList = createFoeList();

    MainMenu(c, map, p, objList, foeList);

    destroyFoeList(foeList);
    destroyObjList(objList);
    free(p);

    return MAX_PATH;
}
