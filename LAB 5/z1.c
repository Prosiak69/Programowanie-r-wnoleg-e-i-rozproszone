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
    double a, b, h, wynik;
    int NR_PROC=atoi(argv[1]);
    srand((unsigned int)time(NULL));
    pid_t pid;

    printf("%f\n", a);
    printf("%f\n", b);

    printf("Moj PID = %d\n", getpid());

    for (i = 1; i <= NR_PROC; i++){

        a=0.0;
        b=0.0;
        while(a>=b){
            a=rand()%10+1;
            b=rand()%10+1;
            printf("%f\n", a);
            printf("%f\n", b);
        }

        n=rand()%10+1;

        switch (pid = fork()) {
            case -1: 
                printf("Error in fork\n");
            case 0:

                    h=(b-a)/n;

                    double Xi[n-1];
                    double tab[n+1];

                    int i;
                    for(i=1;i<n;i++) {
                        Xi[i-1]=a+((i/(double)n)*(b-a));
                    }

                    tab[0]=(4*pow(a,2)-6*a+5)/2;
                    tab[n]=(4*pow(b,2)-6*b+5)/2;


                    for(i=1;i<n;i++){
                        tab[i]=(4*pow(Xi[i-1],2.0)-6*Xi[i-1]+5);
                    }

                    double sum=0;
                    for(i=0;i<n+1;i++){
                        sum=sum+tab[i];
                    }

                    wynik=sum*h;
                    printf("Jestem procesem potomnym. Moj PID = %d\n", getpid());
                    printf("A = %.2lf, B = %.2lf, n = %d. Wartość całki = %.3lf\n", a, b, n, wynik);

                return 0;
        }
    }
    
    /* czeka na zakonczenie procesow potomnych */
    for (i = 1; i <= NR_PROC; i++) {
        if (wait(0) == -1) { 
            printf("Error in wait\n");
        }
    }
    
    return 0;
}
