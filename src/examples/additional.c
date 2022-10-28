/*Pintos 1_User program_Fibonacci, Max --------------------------------- STARTS HERE*/
#include <stdio.h>
#include <syscall.h>
#include <string.h>

int main (int argc, char* argv[]) {
    int num[4] = { 0 };

    for (int i = 0; i < 4; i++) {
        int len = strlen(argv[i]);
        for (int j = 0; j < len; j++) {
            num[i] *= 10;
            num[i] += argv[i][j] - '0';
        }
    }
    int fib, max = num[0];

    //fibonacci
    if (num[0] <=2) fib = 1;
    else {
        int fib_1 = 1, fib_2 = 1;
        int temp;
        
        for (int i = 3; i <= num[0]; i++) {
            temp = fib_1 + fib_2;
            fib_1 = fib_2;
            fib_2 = temp;
        }

        fib = fib_2;
    }

    //max
    for (int i = 1; i < 4; i++) {
        max = num[i] > max ? num[i] : max;
    }

    printf("%d %d\n", fib, max);

    return EXIT_SUCCESS;
}
/*Pintos 1_User program_Fibonacci, Max --------------------------------- ENDS HERE*/
