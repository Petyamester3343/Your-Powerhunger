// Definitions of the Main Game methods' return values
#define     PWRHNGR_DEF         static inline void
#define     PWRHNGR_BOOLDEF     static inline bool
#define     PWRHNGR_IDEF        static inline int
#define     PWRHNGR_UIDEF       static inline uint
#define     PWRHNGR_POSDEF      static inline Pos
#define     PWRHNGR_PLAYERDEF   static inline Player*


// Deinitions of the linked list-related methods' return values
#define     PWRHNGR_OLLDEF      static inline OBJ_DLL*
#define     PWRHNGR_OBJDEF      static inline ObjNode*

#define     PWRHNGR_FLLDEF      static inline FOE_DLL*
#define     PWRHNGR_FOEDEF      static inline FoeNode*


// Definitions of some non-method value types
#define     PWRHNGR_UINT        static uint
#define     PWRHNGR_STR         static const char*
#define     PWRHNGR_STRUCT      typedef struct

// Definitions of the Secret Game methods' return values
#define     T3_DECL_CHAR        static inline char
#define     T3_DECL_INT         static inline int
#define     T3_DECL_VOID        static inline void


// Mathematical formula for the Player's level up requirement
// Once reached or exceeded, their level would increment
// (as long as their XP exceeds or reaches the requirement below)
#define     LV_UP_REQ(lv)       ((lv)*(lv)*25)

// Multi-platform directory management for the save files
#define     SAVE_DIR            "saves/"

#ifdef _WIN32
#include <windows.h> // for Sleep()
#include <direct.h> // for _mkdir()
#define     SLEEP_MS(ms)        Sleep(ms)
#else
#include <sys/stat.h>
#include <unistd.h>
#define     SLEEP_MS(ms)        usleep((ms) * 1000) // usleep takes microseconds (us)
#endif // _WIN32


// Definition of the "bzero" method
#define     bzero(b,len)        (memset((b), '\0', (len)), (void) 0)


// DEFAULT SAVE SIZE
#define     SAVE_SIZE           10

// DEFAULT MAP SIZE
#define     MAP_SIZE            15

// DEFAULT NORMAL OBJECT BUFF VALUE
#define     OBJ_DEF_BUFF        5

// DEFAULT PERMANENT UPGRADE BUFF VALUE
#define     PERM_BUFF           30

// PLAYER'S MAGIA ROSTER DEFAULT SIZE
#define     SKILL_ROSTER        6

// DEFAULT STRING SIZE
#define     STR_SIZE            100

// PLAYER'S SURROUNDINGS' RADIUS VALUE
#define     PLAYER_R            1


// DEFAULT MAX HEADCOUNTS FOR FOES
#define     MAX_SLIME           3
#define     MAX_GOBLIN          3
#define     MAX_ACOLYTE         2
#define     MAX_THW_SELF        2
#define     MAX_GOLEM           1

// Aesthetic latency between printing texts
#define     LATENCY             500

// A simplification of output coloring double words used at setting the console text's attribute
#define     FOREGROUND_CYAN     FOREGROUND_BLUE|FOREGROUND_GREEN
#define     FOREGROUND_PURPLE   FOREGROUND_BLUE|FOREGROUND_RED
#define     FOREGROUND_YELLOW   FOREGROUND_GREEN|FOREGROUND_RED

// Other helping definitions of data types
#define     uint                unsigned int
#define     myStr               const char*
