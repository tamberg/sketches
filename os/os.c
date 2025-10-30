// based on https://stackoverflow.com/questions/142508

#include <stdio.h>

int main(void) {
#if __CYGWIN__
    printf("Windows\n");
#elif __MACH__
    printf("MacOS (mach)\n");
#elif __APPLE__
    printf("MacOS\n");
#elif __unix__
    printf("Unix\n");
#elif __FreeBSD__
    printf("FeeBSD\n");
#elif __ANDROID__
    printf("Android\n");
#elif __linux__
    printf("Linux\n");
#endif
    return 0;
}
