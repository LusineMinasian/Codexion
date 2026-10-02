#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

long long now_ms() 
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main()
{
    printf("%lld\n", now_ms());
    return 0;
}