#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"
#include <math.h>

int main (int argc, char *argv[]){
    int i, n;
    int NR_PROC=atoi(argv[1]);
    srand((unsigned int)time(NULL));
    pid_t pid;
    double pi;


    printf("Moj PID = %d\n", getpid());

    for (i = 1; i <= NR_PROC; i++){

        n=rand()%1000+1;

        switch (pid = fork()) {
            case -1: 
                printf("Error in fork\n");
            case 0:
                pi=1.0;
                int i;
                int N;
                for (i=3, N=2*n+1; i<=N; i+=2)
                pi += ((i&2) ? -1.0 : 1.0) / i;
                printf("%f\n",4*pi);
                   
                return 0;
        }
    }
    
    for (i = 1; i <= NR_PROC; i++) {
        if (wait(0) == -1) { 
            printf("Error in wait\n");
        }
    }
    
    return 0;
}
