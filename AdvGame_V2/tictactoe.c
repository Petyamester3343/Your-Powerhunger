#include <stdio.h>

static inline char gridChar_3T(const int i)
{
    char c;
    switch(i)
    {
    case -1:
        c = 'X';
    case 0:
        c = ' ';
    case 1:
        c = 'O';
    }
    return c;
}

static inline void drawBoard_3T(const int b[9])
{
    printf(" %c | %c | %c\n",gridChar_3T(b[0]),gridChar_3T(b[1]),gridChar_3T(b[2]));
    printf("---+---+---\n");
    printf(" %c | %c | %c\n",gridChar_3T(b[3]),gridChar_3T(b[4]),gridChar_3T(b[5]));
    printf("---+---+---\n");
    printf(" %c | %c | %c\n",gridChar_3T(b[6]),gridChar_3T(b[7]),gridChar_3T(b[8]));
}

// Determines if a player has won. Otherwise, it returns with 0.
static inline int checkWin_3T(const int board[9])
{
    unsigned wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    int i;
    for(i = 0; i < 8; ++i)
    {
        if(board[wins[i][0]] != 0 &&
                board[wins[i][0]] == board[wins[i][1]] &&
                board[wins[i][0]] == board[wins[i][2]])
            return board[wins[i][2]];
    }
    return 0;
}

// Determines the way the position is like for player (their turn) on board
static inline int minimax_3T(int board[9], const int player)
{
    int winner = checkWin_3T(board);
    if(winner != 0) return winner*player;

    int move = -1;
    int score = -2; //Losing moves are preferred to no move
    for(unsigned int i = 0; i < 9; ++i)   //For all moves,
    {
        if(board[i] == 0)   //If legal,
        {
            board[i] = player; //Try the move
            int thisScore = -minimax_3T(board, player*-1);
            if(thisScore > score)
            {
                score = thisScore;
                move = i;
            } //Pick the one that's worst for the opponent
            board[i] = 0; //Reset board after try
        }
    }
    if(move == -1) return 0;
    return score;
}

// Computer moves
static inline void computerMove_3T(int board[9])
{
    int move = -1;
    int score = -2;
    for(unsigned int i = 0; i < 9; ++i)
    {
        if(board[i] == 0)
        {
            board[i] = 1;
            int tempScore = -minimax_3T(board, -1);
            board[i] = 0;
            if(tempScore > score)
            {
                score = tempScore;
                move = i;
            }
        }
    }

    board[move] = 1; // Return with a score based on minimax tree at a given node.
}

// Player moves
static inline void playerMove_3T(int board[9])
{
    int move = 0;
    do
    {
        while(board[move] != 0)
        {
            printf("\nInput move ([0..8]): ");
            scanf("%d", &move);
            if(board[move] != 0)
            {
                printf("This position is already occupied!");
            }
        }
        printf("\n");
    }
    while (move >= 9 || move < 0 || board[move] != 0);
    board[move] = -1;
}

// The game itself
static inline void gameMech_3T()
{
    int board[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    //computer squares are 1, player squares are -1.
    printf("Computer: O, You: X\nPlay (1)st or (2)nd? ");
    int player = 0;
    scanf("%d",&player);
    printf("\n");
    unsigned turn;
    for(turn = 0; turn < 9 && checkWin_3T(board) == 0; ++turn)
    {
        if((turn+player) % 2 == 0)
            computerMove_3T(board);
        else
        {
            drawBoard_3T(board);
            playerMove_3T(board);
        }
    }
    switch(checkWin_3T(board))
    {
    case 0:
        printf("A draw... How droll...\n");
        break;
    case 1:
        drawBoard_3T(board);
        printf("I win... Hang your head not! I know you can do better than this.\n");
        break;
    case -1:
        printf("You've won. Now that's inconceivable!\n");
        break;
    }
}
