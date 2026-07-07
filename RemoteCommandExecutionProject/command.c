#include <stdio.h>
#include <string.h>
#include "command.h"

#define OUTPUT_SIZE 4096

/* List of allowed commands */
const char *allowedCommands[] = {
    "pwd",
    "ls",
    "date",
    "whoami",
    "hostname",
    "uptime",
    "uname",
    NULL
};

/* Check if command is allowed */
int isAllowedCommand(const char *command)
{
    int i = 0;

    while (allowedCommands[i] != NULL)
    {
        if (strcmp(command, allowedCommands[i]) == 0)
        {
            return 1;
        }

        i++;
    }

    return 0;
}

/* Execute command */
void executeCommand(const char *command, char *output)
{
    FILE *fp;

    output[0] = '\0';

    fp = popen(command, "r");

    if (fp == NULL)
    {
        strcpy(output, "Failed to execute command.\n");
        return;
    }

    char line[256];

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        strcat(output, line);
    }

    pclose(fp);
}