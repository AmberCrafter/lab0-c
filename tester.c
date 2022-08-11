#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

int main() {
    srand(time(NULL));

    double x = (double) rand() / (RAND_MAX + 1.0);
    int y = (int) (x * 20.0);

    printf("rand: %f\n", x);
    printf("rand: %d\n", y);
    return 0;
}