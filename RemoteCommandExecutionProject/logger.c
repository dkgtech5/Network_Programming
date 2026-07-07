#include <stdio.h>
#include <time.h>

#include "logger.h"

void logEvent(const char *event)
{
    FILE *fp;

    time_t now;
    struct tm *timeInfo;

    now = time(NULL);
    timeInfo = localtime(&now);

    fp = fopen("logs/server.log", "a");

    if (fp == NULL)
    {
        return;
    }

    fprintf(fp,
            "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            timeInfo->tm_year + 1900,
            timeInfo->tm_mon + 1,
            timeInfo->tm_mday,
            timeInfo->tm_hour,
            timeInfo->tm_min,
            timeInfo->tm_sec,
            event);

    fclose(fp);
}