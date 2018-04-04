#ifdef __linux
#include <unistd.h>
#else
#include <windows.h>
#endif

void sleep_ms(int miliseconds)
{
#ifdef __linux
    usleep(miliseconds * 1000);
#else
    Sleep(miliseconds);
#endif
}
