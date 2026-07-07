#include <stdio.h>
#include <string.h>

#include "auth.h"

/* Hardcoded credentials */
#define USERNAME "admin"
#define PASSWORD "admin123"

/* Verify username and password */
int authenticate(const char *username, const char *password)
{
    if (strcmp(username, USERNAME) == 0 &&
        strcmp(password, PASSWORD) == 0)
    {
        return 1;
    }

    return 0;
}