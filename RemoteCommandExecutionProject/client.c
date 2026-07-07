#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int clientSocket;

    struct sockaddr_in serverAddr;

    char buffer[BUFFER_SIZE];

    char username[100];
    char password[100];

    // Create socket
    clientSocket = socket(AF_INET,
                          SOCK_STREAM,
                          0);

    if (clientSocket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    inet_pton(AF_INET,
              "127.0.0.1",
              &serverAddr.sin_addr);

    // Connect
    if (connect(clientSocket,
                (struct sockaddr *)&serverAddr,
                sizeof(serverAddr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Receive welcome message
    memset(buffer, 0, BUFFER_SIZE);

    recv(clientSocket,
         buffer,
         BUFFER_SIZE,
         0);

    printf("%s\n\n", buffer);

    /* -------- Login -------- */

    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    send(clientSocket,
         username,
         strlen(username),
         0);

    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    send(clientSocket,
         password,
         strlen(password),
         0);

    // Authentication response
    memset(buffer, 0, BUFFER_SIZE);

    recv(clientSocket,
         buffer,
         BUFFER_SIZE,
         0);

    if (strcmp(buffer, "AUTH_SUCCESS") != 0)
    {
        printf("\nAuthentication Failed!\n");

        close(clientSocket);

        return 0;
    }

    printf("\nLogin Successful!\n");

    /* -------- Command Loop -------- */

    while (1)
    {
        printf("\nEnter Command: ");

        fgets(buffer,
              BUFFER_SIZE,
              stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "exit") == 0)
            break;

        send(clientSocket,
             buffer,
             strlen(buffer),
             0);

        memset(buffer, 0, BUFFER_SIZE);

        recv(clientSocket,
             buffer,
             BUFFER_SIZE,
             0);

        printf("\nServer Output:\n%s\n", buffer);
    }

    close(clientSocket);

    return 0;
}