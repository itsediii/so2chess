#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

void chat_with_server(int sockfd)
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
            break;
        }
        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Exiting chat.\n");
            break;
        }
        memset(buffer, 0, sizeof(buffer));
        if (read(sockfd, buffer, sizeof(buffer)) > 0)
        {
            printf("Board:\n%s", buffer);
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

int main()
{
    int sockfd;
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
    chat_with_server(sockfd);
    close(sockfd);
    return 0;
}
