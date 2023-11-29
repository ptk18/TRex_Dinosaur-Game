#include <time.h>
int getTime()
{
    time_t t;
    struct tm* current_time;
    t = time(NULL);
    current_time = localtime(&t);
    int total_sec = current_time->tm_hour * 3600 + current_time->tm_min * 60 + current_time->tm_sec;
    return total_sec;
}
