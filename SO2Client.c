#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

int chat_with_server(int sockfd)
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        printf("You: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (write(sockfd, buffer, strlen(buffer)) < 0)
        {
            perror("Failed to send message");
            exit(EXIT_FAILURE);
        }
        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Exiting chat.\n");
            return 0;
        }
        memset(buffer, 0, sizeof(buffer));
        if (read(sockfd, buffer, sizeof(buffer)) > 0)
        {
            if(strncmp(buffer,"GAME OVER",9)==0)
            {
                printf("%s\n",buffer);
                break;
            }
            printf("Board:\n%s", buffer);
        }
    }
    return 1;
}

void receive_initial_board(int sockfd, char *buffer)
{
    memset(buffer, 0, sizeof(buffer));
    if (read(sockfd, buffer, sizeof(buffer)) > 0)
    {
        printf("%s\n", buffer);
    }
    else
    {
        perror("Failed to receive the initial board");
        exit(EXIT_FAILURE);
    }
}

void client_color(int sockfd)
{
    int color;
    printf("Choose your color:1 for Black, 2 for White\n");
    scanf("%d",&color);
    send(sockfd, &color, sizeof(color), 0);
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

int main()
{
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4555);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection to the server failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server. Type 'exit' to quit.\n");
    client_color(sockfd);
    receive_initial_board(sockfd,buffer);
    while(1)
    {
        if(chat_with_server(sockfd))
        {
            printf("Do you wish to play again with the same gplayer? Type 'yes' if you do.\n");
            fgets(buffer,sizeof(buffer),stdin);
            if(strcmp(buffer, "yes") != 0)
            {
                break;
            }
            if (write(sockfd, buffer, strlen(buffer)) < 0)
            {
                perror("Failed to send message");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            break;
        }
    }
    close(sockfd);
    return 0;
}
