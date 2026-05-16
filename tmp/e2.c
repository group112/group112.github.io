#include <stdio.h>
#include <math.h>

double fast_pow(double x, int n);

int main(void) {
    for(int n = 0; n < 710; ++ n) {
        double y = fast_pow(M_E, n);
        printf("%4d %.15e %.15e %.15e\n", n, y, fabs(y - exp(n)), fabs(y - exp(n)) / y);
    }
}

double fast_pow(double x, int n) {
    double r = 1.;

    for(int f = 0; n; f = 1, n >>= 1) {
        if(f) {
            x *= x;
        } 
        if(n & 1) {
            r *= x;
        }
    }

    return r;
}

