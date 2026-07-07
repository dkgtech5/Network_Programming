#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

#include "auth.h"
#include "command.h"
#include "logger.h"

#define PORT 8080
#define BUFFER_SIZE 1024

/* Prevent zombie processes */
void sigchld_handler(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main()
{
    int serverSocket, clientSocket;

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLength;

    signal(SIGCHLD, sigchld_handler);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(serverSocket,
               SOL_SOCKET,
               SO_REUSEADDR,
               &opt,
               sizeof(opt));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket,
             (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(serverSocket, 5);

    printf("=====================================\n");
    printf(" Remote Command Execution Server\n");
    printf(" Listening on Port %d\n", PORT);
    printf("=====================================\n");

    while (1)
    {
        clientLength = sizeof(clientAddr);

        clientSocket = accept(serverSocket,
                              (struct sockaddr *)&clientAddr,
                              &clientLength);

        if (clientSocket < 0)
        {
            perror("Accept failed");
            continue;
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("Fork failed");
            close(clientSocket);
            continue;
        }

        if (pid == 0)
        {
            /* Child Process */

            close(serverSocket);

            char buffer[BUFFER_SIZE];
            char output[4096];

            char username[100];
            char password[100];

            char logMessage[256];

            printf("Client Connected\n");
            logEvent("Client Connected");

            char welcome[] =
                "Connected to Remote Command Execution Server";

            send(clientSocket,
                 welcome,
                 strlen(welcome),
                 0);

            /* Authentication */

            memset(username, 0, sizeof(username));
            recv(clientSocket,
                 username,
                 sizeof(username),
                 0);

            memset(password, 0, sizeof(password));
            recv(clientSocket,
                 password,
                 sizeof(password),
                 0);

            if (authenticate(username, password))
            {
                char success[] = "AUTH_SUCCESS";

                send(clientSocket,
                     success,
                     strlen(success),
                     0);

                printf("Login Success : %s\n", username);

                sprintf(logMessage,
                        "Login Success : %s",
                        username);

                logEvent(logMessage);
            }
            else
            {
                char failed[] = "AUTH_FAILED";

                send(clientSocket,
                     failed,
                     strlen(failed),
                     0);

                sprintf(logMessage,
                        "Login Failed : %s",
                        username);

                logEvent(logMessage);

                close(clientSocket);
                exit(0);
            }

            /* Command Loop */

            while (1)
            {
                memset(buffer, 0, BUFFER_SIZE);

                int bytes = recv(clientSocket,
                                 buffer,
                                 BUFFER_SIZE,
                                 0);

                if (bytes <= 0)
                    break;

                printf("Command : %s\n", buffer);

                sprintf(logMessage,
                        "Command : %s",
                        buffer);

                logEvent(logMessage);

                memset(output, 0, sizeof(output));

                if (isAllowedCommand(buffer))
                {
                    executeCommand(buffer,
                                   output);
                }
                else
                {
                    strcpy(output,
                           "Command Not Allowed\n");

                    sprintf(logMessage,
                            "Blocked Command : %s",
                            buffer);

                    logEvent(logMessage);
                }

                send(clientSocket,
                     output,
                     strlen(output),
                     0);
            }

            printf("Client Disconnected\n");

            logEvent("Client Disconnected");

            close(clientSocket);

            exit(0);
        }

        /* Parent Process */

        close(clientSocket);
    }

    close(serverSocket);

    return 0;
}