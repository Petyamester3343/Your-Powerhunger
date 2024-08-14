#include <stdio.h>
#include "definitions.h"

// Helper method for drawing the board
T3_DECL_CHAR gridChar_3T(const int i)
{
    char c;
    switch (i)
    {
    case -1:
        c = 'X';
        break;
    case 0:
        c = ' ';
        break;
    case 1:
        c = 'O';
        break;
    }
    return c;
}

// Draws the board
T3_DECL_VOID drawBoard_3T(const int b[9])
{
    printf(" %c | %c | %c\n", gridChar_3T(b[0]), gridChar_3T(b[1]), gridChar_3T(b[2]));
    printf("---+---+---\n");
    printf(" %c | %c | %c\n", gridChar_3T(b[3]), gridChar_3T(b[4]), gridChar_3T(b[5]));
    printf("---+---+---\n");
    printf(" %c | %c | %c\n", gridChar_3T(b[6]), gridChar_3T(b[7]), gridChar_3T(b[8]));
}

// Determines if a player has won. Otherwise, it returns with 0.
T3_DECL_INT checkWin_3T(const int board[9])
{
    unsigned wins[8][3] = { {0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6} };
    for (uint i = 0; i < 8; ++i)
    {
        if (board[wins[i][0]] != 0 &&
                board[wins[i][0]] == board[wins[i][1]] &&
                board[wins[i][0]] == board[wins[i][2]])
            return board[wins[i][2]];
    }
    return 0;
}

// Determines the way the position is like for player (their turn) on board
T3_DECL_INT minimax_3T(int board[9], const int player)
{
    int victor = checkWin_3T(board);
    if (victor != 0)
        return victor * player;

    int move = -1;
    int score = -2; // Losing moves are preferred to no move
    for (uint i = 0; i < 9; ++i)   // For all moves,
    {
        if (board[i] == 0)   // If the move is legal,
        {
            // The computer tries it
            board[i] = player;
            int thisScore = -minimax_3T(board, player * -1);
            if (thisScore > score)
            {
                score = thisScore;
                move = i;
            } // It picks the one that's worst for the opponent
            board[i] = 0; // Reset board after try
        }
    }
    if (move == -1) return 0;
    return score;
}

// Computer moves
T3_DECL_VOID cpuMove_3T(int board[9])
{
    int move = -1;
    int score = -2;
    for (uint i = 0; i < 9; ++i)
    {
        if (board[i] == 0)
        {
            board[i] = 1;
            int tempScore = -minimax_3T(board, -1);
            board[i] = 0;
            if (tempScore > score)
            {
                score = tempScore;
                move = i;
            }
        }
    }

    board[move] = 1; // Return with a score based on minimax tree at a given node.
}

// Player moves
T3_DECL_VOID playerMove_3T(int board[9])
{
    int move = -1;
    do
    {
        printf("\nInput move ([0...8]): ");
        fflush(stdin);
        scanf("%d", &move);

        if (move < 0 || move >= 9)
            printf("Invalid move! Please enter a number BETWEEN 0 and 8.\n");
        else if (board[move] != 0)
            printf("This position is already occupied!\n");
    }
    while (move >= 9 || move < 0 || board[move] != 0);
    board[move] = -1;
}

// The game itself
T3_DECL_VOID gameMech_3T()
{
    int board[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // Computer squares are 1, player squares are -1
    uint player = 0;
    while(player != 1 && player != 2)
    {
        printf("Computer: O, You: X\nPlay (1)st or (2)nd? ");
        scanf("%d", &player);
        printf("\n");
        if(player != 1 && player != 2)
            printf("Invalid choice!\n");
    }
    for (unsigned turn = 0; turn < 9 && checkWin_3T(board) == 0; ++turn)
    {
        if ((turn + player) % 2 == 0)
        {
            cpuMove_3T(board);
            drawBoard_3T(board);
            printf("\n");
        }
        else
        {
            playerMove_3T(board);
            printf("\n");
        }
    }
    switch (checkWin_3T(board))
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
