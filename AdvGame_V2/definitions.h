// Definitions of the Main Game methods' return values
#define PWRHNGR_DEF static inline void
#define PWRHNGR_POSDEF static inline Pos
#define PWRHNGR_OLLDEF static inline OBJ_DLL*
#define PWRHNGR_OBJDEF static inline ObjNode*
#define PWRHNGR_BOOLDEF static inline bool
#define PWRHNGR_PLAYERDEF static inline Player*
#define PWRHNGR_IDEF static inline int

// Definitions of some non-method value types
#define PWRHNGR_UINT static unsigned int
#define PWRHNGR_STR static const char*
#define PWRHNGR_STRUCT typedef struct

// Definitions of the Secret Game methods' return values
#define T3_DECL_CHAR static inline char
#define T3_DECL_INT static inline int
#define T3_DECL_VOID static inline void


// Mathematical formula for the Player's level up requirement
// Once reached or exceeded, their level would increment
// (as long as their XP exceeds or reaches the requirement below)
#define LV_UP_REQ(lv) ((lv)*(lv)*25)

// multi-platform directory management for the save files
#define SAVE_DIR "saves/"
#ifdef _WIN32
    #include <windows.h> // for Sleep()
    #include <direct.h> // for _mkdir()
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000) // usleep takes microseconds
#endif // _WIN32


// Definition of the "bzero" method
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)


// DEFAULT SAVE SIZE
#define SAVE_SIZE 10

// DEFAULT MAP SIZE
#define MAP_SIZE 15

// DEFAULT NORMAL OBJECT BUFF VALUE
#define BUFF 5

// Player's Magia roster default size
#define SKILL_ROSTER 6

// Default string size
#define STR_SIZE 100


// DEFAULT MAX HEADCOUNTS FOR FOES
#define MAX_SLIME 3
#define MAX_GOBLIN 3
#define MAX_ACOLYTE 2
#define MAX_THW_SELF 2
#define MAX_GOLEM 1

// Aesthetic latency between printing texts
#define LATENCY 500

#define FOREGROUND_CYAN FOREGROUND_BLUE|FOREGROUND_GREEN
#define FOREGROUND_PURPLE FOREGROUND_BLUE|FOREGROUND_RED
#define FOREGROUND_YELLOW FOREGROUND_GREEN|FOREGROUND_RED
