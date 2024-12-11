#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>

#define white_king 0x2654   // ♔
#define white_queen 0x2655  // ♕
#define white_rook 0x2656   // ♖
#define white_bishop 0x2657 // ♗
#define white_knight 0x2658 // ♘
#define white_pawn 0x2659   // ♙
#define black_king 0x265A   // ♚
#define black_queen 0x265B  // ♛
#define black_rook 0x265C   // ♜
#define black_bishop 0x265D // ♝
#define black_knight 0x265E // ♞
#define black_pawn 0x265F   // ♟

#define black_square 0x25A0 // ◼
#define white_square 0x25A1 // ◻

void init(wchar_t board[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (i == 0)
            {
                if (j == 0 || j == 7)
                    board[i][j] = black_rook;
                else if (j == 1 || j == 6)
                    board[i][j] = black_knight;
                else if (j == 2 || j == 5)
                    board[i][j] = black_bishop;
                else if (j == 3)
                    board[i][j] = black_queen;
                else if (j == 4)
                    board[i][j] = black_king;
            }
            else if (i == 1)
            {
                board[i][j] = black_pawn;
            }
            else if (i == 6)
            {
                board[i][j] = white_pawn;
            }
            else if (i == 7)
            {
                if (j == 0 || j == 7)
                    board[i][j] = white_rook;
                else if (j == 1 || j == 6)
                    board[i][j] = white_knight;
                else if (j == 2 || j == 5)
                    board[i][j] = white_bishop;
                else if (j == 3)
                    board[i][j] = white_queen;
                else if (j == 4)
                    board[i][j] = white_king;
            }
            else
            {
                if ((i + j) % 2 == 0)
                {
                    board[i][j] = black_square;
                }
                else
                {
                    board[i][j] = white_square;
                }
            }
        }
    }
}

void print_board(wchar_t board[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] != 0)
            {
                wprintf(L"%lc ", board[i][j]);
            }
            else
            {
                wprintf(L". ");
            }
        }
        wprintf(L"\n");
    }
}

int chose_color()
{
    int color;
    printf("white = 1; black = 2\n");
    do
    {
        printf("chose your color: ");
        scanf("%d", &color);
    } while ((color != 1) && (color != 2));

    return color == 1;
}

int replay()
{
    int choise;
    printf("play again with same player = 1; quit = 2\n");
    do
    {
        printf("chose your answer: ");
        scanf("%d", &choise);
    } while ((choise != 1) && (choise != 2));

    return choise == 1;
}

void set_username(char username[32])
{
    printf("Type your username ");
    scanf("%31s", username);
}

int main()
{
    setlocale(LC_ALL, "");
    wchar_t board[8][8];
    init(board);
    print_board(board);
    return 0;
}