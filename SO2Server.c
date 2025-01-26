#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

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

void print_board(wchar_t board[8][8], int is_white_turn)
{
    if (!is_white_turn)
    {
        for (int i = 0; i < 8; i++)
        {
            wprintf(L"%d ", 8 - i);
            for (int j = 0; j < 8; j++)
            {
                wprintf(L"%lc ", board[i][j]);
            }
            wprintf(L"\n");
        }
        wprintf(L"  a b c d e f g h\n");
    }
    else
    {
        for (int i = 7; i >= 0; i--)
        {
            wprintf(L"%d ", 8 - i);
            for (int j = 7; j >= 0; j--)
            {
                wprintf(L"%lc ", board[i][j]);
            }
            wprintf(L"\n");
        }
        wprintf(L"  h g f e d c b a\n");
    }
}

void serialize_board(wchar_t board[8][8], char *buffer, int is_white_turn)
{
    char *ptr = buffer;
    if (is_white_turn)
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                ptr += sprintf(ptr, "%lc ", board[i][j]);
            }
            ptr += sprintf(ptr, "\n");
        }
    }
    else
    {
        for (int i = 7; i >= 0; i--)
        {
            for (int j = 7; j >= 0; j--)
            {
                ptr += sprintf(ptr, "%lc ", board[i][j]);
            }
            ptr += sprintf(ptr, "\n");
        }
    }
}

int validate_pawn_move(wchar_t board[8][8], int src_x, int src_y, int dest_x, int dest_y)
{
    wchar_t piece = board[src_x][src_y];
    int direction = (piece == white_pawn) ? -1 : 1; // White pawns move up (-1), black move down (+1)

    // Single step forward
    if (dest_x == src_x + direction && dest_y == src_y && (board[dest_x][dest_y] == white_square || board[dest_x][dest_y] == black_square))
        return 1;

    // Capture diagonally
    if (dest_x == src_x + direction && abs(dest_y - src_y) == 1 && (board[dest_x][dest_y] != white_square || board[dest_x][dest_y] != black_square))
        return 1;

    // Double step forward (only on starting row)
    if ((src_x == 6 && piece == white_pawn) || (src_x == 1 && piece == black_pawn))
    {
        if (dest_x == src_x + 2 * direction && dest_y == src_y && board[src_x + direction][src_y] == white_square && board[dest_x][dest_y] == white_square)
            return 1;
    }

    return 0;
}

int validate_rook_move(wchar_t board[8][8], int src_x, int src_y, int dest_x, int dest_y)
{
    // rook: straight line horizontally/vertically
    if (src_x != dest_x && src_y != dest_y)
    {
        return 0; // invalid if not along same row or column
    }

    int step_x = (dest_x > src_x) ? 1 : (dest_x < src_x) ? -1 : 0;
    int step_y = (dest_y > src_y) ? 1 : (dest_y < src_y) ? -1 : 0;

    // check for obstacles along the rooks path
    int x = src_x + step_x;
    int y = src_y + step_y;
    while (x != dest_x || y != dest_y)
    {
        if (board[x][y] != white_square && board[x][y] != black_square)
            return 0; // path blocked by another piece
        x += step_x;
        y += step_y;
    }

    // destination square cannot contain a piece of the same color as the rook
    if ((board[src_x][src_y] <= white_pawn && board[dest_x][dest_y] <= white_pawn) || (board[src_x][src_y] >= black_king && board[dest_x][dest_y] >= black_king))
    {
        return 0;
    }
    return 1;
}

int validate_knight_move(wchar_t board[8][8], int src_x, int src_y, int dest_x, int dest_y)
{
    wchar_t piece = board[src_x][src_y];
    // knight moves in an 'L' shape
    int dx = abs(dest_x - src_x);
    int dy = abs(dest_y - src_y);

    if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2))
    {
        // destination square cannot contain a piece of the same color as the knight
        if ((piece <= white_pawn && board[dest_x][dest_y] <= white_pawn) || (piece >= black_king && board[dest_x][dest_y] >= black_king))
            return 0; // invalid move

        return 1;
    }
    return 0;
}

int validate_bishop_move(wchar_t board[8][8], int src_x, int src_y, int dest_x, int dest_y)
{
    // Bishop moves diagonally, so the difference in x and y coordinates must be the same
    if (abs(dest_x - src_x) != abs(dest_y - src_y))
        return 0;

    int step_x = (dest_x > src_x) ? 1 : (dest_x < src_x) ? -1 : 0;
    int step_y = (dest_y > src_y) ? 1 : (dest_y < src_y) ? -1 : 0;

    // check for obstacles along the bishops path
    int x = src_x + step_x, y = src_y + step_y;
    while (x != dest_x || y != dest_y)
    {
        if (board[x][y] != white_square && board[x][y] != black_square)
            return 0; // path blocked by another piece
        x += step_x;
        y += step_y;
    }

    // destination square cannot contain a piece of the same color as the bishop
    wchar_t piece = board[src_x][src_y];
    if ((piece <= white_pawn && board[dest_x][dest_y] <= white_pawn) ||
        (piece >= black_king && board[dest_x][dest_y] >= black_king))
        return 0;

    return 1;
}

int validate_queen_move(wchar_t board[8][8], int src_x, int src_y, int dest_x, int dest_y)
{
    // queen moves like a rook or a bishop
    if (src_x == dest_x || src_y == dest_y)
    {
        return validate_rook_move(board, src_x, src_y, dest_x, dest_y);
    }
    else if (abs(dest_x - src_x) == abs(dest_y - src_y))
    {
        return validate_bishop_move(board, src_x, src_y, dest_x, dest_y);
    }

    return 0; // invalid move
}

int validate_king_move(wchar_t board[8][8], int src_x, int src_y, int dest_x, int dest_y)
{
    int dx = abs(dest_x - src_x);
    int dy = abs(dest_y - src_y);

    // king moves 1 square in any direction
    if (dx <= 1 && dy <= 1)
    {
        // destination square cannot contain a piece of the same color as the king
        wchar_t piece = board[src_x][src_y];
        if ((piece <= white_pawn && board[dest_x][dest_y] <= white_pawn) || (piece >= black_king && board[dest_x][dest_y] >= black_king))
            return 0; // invalid move

        return 1;
    }

    return 0;
}

int is_valid_move(wchar_t board[8][8], int src_x, int src_y, int dest_x, int dest_y, int is_white_turn)
{
    wchar_t piece = board[src_x][src_y];

    if (piece == black_square || piece == white_square)
        return 0;

    if (is_white_turn)
    {
        if (piece > white_pawn || piece < white_king)
            return 0;
    }
    else
    {
        if (piece > black_pawn || piece < black_king)
            return 0;
    }

    if (src_x < 0 || src_x >= 8 || src_y < 0 || src_y >= 8 || dest_x < 0 || dest_x >= 8 || dest_y < 0 || dest_y >= 8)
        return 0;

    switch (piece)
    {
        case white_pawn:
        case black_pawn:
            return validate_pawn_move(board, src_x, src_y, dest_x, dest_y);
        case white_rook:
        case black_rook:
            return validate_rook_move(board, src_x, src_y, dest_x, dest_y);
        case white_knight:
        case black_knight:
            return validate_knight_move(board, src_x, src_y, dest_x, dest_y);
        case white_bishop:
        case black_bishop:
            return validate_bishop_move(board, src_x, src_y, dest_x, dest_y);
        case white_queen:
        case black_queen:
            return validate_queen_move(board, src_x, src_y, dest_x, dest_y);
        case white_king:
        case black_king:
            return validate_king_move(board, src_x, src_y, dest_x, dest_y);
        default:
            return 0;
    }
}

int make_move(wchar_t board[8][8], int src_x, int src_y, int dest_x, int dest_y, int is_white_turn)
{
    if (!is_valid_move(board, src_x, src_y, dest_x, dest_y, is_white_turn))
        return 0;
    board[dest_x][dest_y] = board[src_x][src_y];
    board[src_x][src_y] = ((src_x + src_y) % 2 == 0) ? black_square : white_square;
    return 1;
}

void parse_move(const char *input, int *src_x, int *src_y, int *dest_x, int *dest_y)
{
    *src_y = input[0] - 'a';       // Convert column letter to index
    *src_x = 8 - (input[1] - '0'); // Convert row number to index
    *dest_y = input[3] - 'a';
    *dest_x = 8 - (input[4] - '0');
}

int choose_color(int fd1, int fd2)
{
    int color1 = 0, color2 = 0;
    recv(fd1, &color1, sizeof(color1), 0);
    recv(fd2, &color2, sizeof(color2), 0);
    if (color1 == color2)
    {
        srand(time(NULL));
        color1 = (rand() %2) + 1;
        color2 = (color1 == 1) ? 2 : 1;
    }
    else if (color1 != 1 && color1 != 2 && color2 != 1 && color2 != 2)
    {
        color1 = 1;
        color2 = 2;
    }
    else if (color1 != 1 && color1 != 2)
    {
        color1 = (color2 == 1) ? 2 : 1;
    }
    else if (color2 != 1 && color2 != 2)
    {
        color2 = (color1 == 1) ? 2 : 1;
    }
    /*char message1[25], message2[25];
    snprintf(message1, sizeof(message1), "You are assigned %s.\n", color1 == 1 ? "White" : "Black");
    snprintf(message2, sizeof(message2), "You are assigned %s.\n", color2 == 1 ? "White" : "Black");
    write(fd1, message1, strlen(message1)+1);
    write(fd2, message2, strlen(message2)+1);*/
    return (color1==1);
}


int replay(int fd1, int fd2)
{
    char response1[16], response2[16];

    read(fd1, response1, sizeof(response1));
    response1[strcspn(response1, "\n")] = 0;

    read(fd2, response2, sizeof(response2));
    response2[strcspn(response2, "\n")] = 0;

    if (strcmp(response1, "yes") == 0 && strcmp(response2, "yes") == 0)
    {
        return 1;
    }
    return 0;
}

int end_game(wchar_t board[8][8], int fd1, int fd2)
{
    int whiteking=0;
    int blackking=0;
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            if(board[i][j]==black_king)
            {
                blackking=1;
            }
            if(board[i][j]==white_king)
            {
                whiteking=1;
            }
        }
    }
    if(!whiteking)
    {
        return 1;
    }
    if(!blackking)
    {
        return 2;
    }
    return 0;
}

int main()
{
    setlocale(LC_ALL, "");
    wchar_t board[8][8];
    char message_buffer[256];
    char board_buffer[1024];
    int is_white_turn = 0;
    int winner=0;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(4555);
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    if (listen(sockfd, 2) < 0)
    {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for player 1 to connect...\n");
    int fd1 = accept(sockfd, (struct sockaddr *)&addr, &addr_len);
    if (fd1 < 0)
    {
        perror("accept player 1");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Player 1 connected!\n");

    printf("Waiting for player 2 to connect...\n");
    int fd2 = accept(sockfd, (struct sockaddr *)&addr, &addr_len);
    if (fd2 < 0)
    {
        perror("accept player 2");
        close(fd1);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Player 2 connected!\n");

    printf("Two players connected!\n");

    while(1)
    {
        init(board);
        if(!choose_color(fd1,fd2))
        {
            //fd1 will always be white for the game logic. if fd1 picks black we will have to switch them
            int tmp=fd1;
            fd1=fd2;
            fd2=tmp;
        }
        serialize_board(board, board_buffer, is_white_turn);
        write(fd1, board_buffer, strlen(board_buffer));
        serialize_board(board, board_buffer, !is_white_turn);
        write(fd2, board_buffer, strlen(board_buffer));

        while (1)
        {
            memset(message_buffer, 0, sizeof(message_buffer));

            int current_fd = is_white_turn ? fd1 : fd2;
            int mademove=0;

            if (read(current_fd, message_buffer, sizeof(message_buffer)) > 0)
            {
                printf("Player %s: %s", is_white_turn ? "White" : "Black", message_buffer);

                int src_x, src_y, dest_x, dest_y;
                parse_move(message_buffer, &src_x, &src_y, &dest_x, &dest_y);

                while(!mademove)
                {
                    if (make_move(board, src_x, src_y, dest_x, dest_y, is_white_turn))
                    {
                        if((winner=end_game(board,fd1,fd2))>0)
                        {
                            break;
                        }
                        mademove=1;
                        char board_buffer[1024];
                        serialize_board(board, board_buffer, is_white_turn);
                        write(fd1, board_buffer, strlen(board_buffer));
                        is_white_turn = !is_white_turn;
                        write(fd2, board_buffer, strlen(board_buffer));
                    }
                    else
                    {
                        const char *error_msg = "Invalid move. Try again.\n";
                        write(current_fd, error_msg, strlen(error_msg));
                    }
                }
                if(winner==2)
                {
                    write(fd1,"GAME OVER, BLACK HAS WON!",25);
                    write(fd2,"GAME OVER, BLACK HAS WON!",25);
                    break;
                }
                else if(winner)
                {
                    write(fd1,"GAME OVER, WHITE HAS WON!",25);
                    write(fd2,"GAME OVER, WHITE HAS WON!",25);
                    break;
                }
            }
        }
        if(!replay(fd1,fd2))
        {
            break;
        }
    }
    close(fd1);
    close(fd2);
    close(sockfd);
    return 0;
}
//todo: redo pawn movement
// test other pieces validation
// allow more than just two players to play at the same time