#include <stdio.h>
#include <math.h>

double fast_pow(double x, int n);

int main(void) {

    for(int n = 0; n < 710; ++ n) {
        printf("%4d  %.15e\n", n, fast_pow(M_E, n));
    }

    /*
    for(int n = 0; n < 40; ++ n) {
        printf("%3d  %.1f\n", n, fast_pow(2., n));
    }
    */
}

double fast_pow(double x, int n) {
    double r = 1.;

    while(n) {
        if(n & 1) {
            r *= x;
        }
        x *= x;
        n >>= 1;
    }

    return r;
}

