#define PWRHNGR_DEF static inline void
#define PWRHNGR_POSDEF static inline Pos
#define PWRHNGR_OLLDEF static inline OBJ_DLL*
#define PWRHNGR_OBJDEF static inline ObjNode*
#define PWRHNGR_BOOLDEF static inline bool
#define PWRHNGR_PLAYERDEF static inline Player*
#define PWRHNGR_IDEF static inline int

#define PWRHNGR_UINT static unsigned int
#define PWRHNGR_STR static const char*

#define PWRHNGR_STRUCT typedef struct


#define T3_DECL_CHAR static inline char
#define T3_DECL_INT static inline int
#define T3_DECL_VOID static inline void


// Mathematical formula for the Player
// Once reached, they will ascend to the next level
#define LV_UP_REQ(lv) ((lv)*(lv)*25)

#ifdef _WIN32
    #include <windows.h> // for Sleep()
    #include <direct.h> // for _mkdir()
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000) // usleep takes microseconds
#endif // _WIN32

#define LATENCY 500

#define SAVE_DIR "saves/"
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)


// DEFAULT SAVE SIZE
#define SAVE_SIZE 10

// DEFAULT MAP SIZE
#define MAP_SIZE 15

// DEFAULT NORMAL OBJECT BUFF VALUE
#define BUFF 5

// Player's Magia roster default size
#define SKILL_ROSTER 6
