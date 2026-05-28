#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <unistd.h>
#include <sys/times.h>

int to_double(const char *s, double *p);
int to_double(const char *s, double *p) {
    char *e;

    errno = 0;
    *p = strtod(s, &e);
    
    if(errno || *e) {
        return -1;
    }

    return 0;
}

int to_int(const char *s, int *p);
int to_int(const char *s, int *p) {
    char *e;
    long l;     

    errno = 0;
    l = strtol(s, &e, 10);
    
    if(!(errno || *e)) {
        *p = (int)l;
        if(l == *p) {
            return 0;
        }
    }

    return -1;
}

void print_matrix(double *a, int n);
void print_matrix(double *a, int n)  {
    for(int i = 0; i < n; ++ i) {
        for(int j = 0; j < n; ++ j) {
            printf(" %.1f", a[i * n + j]);
        }
        printf("\n");
    }
}

void find_max_and_swap(double *a, int *k, int n, int i);
void find_max_and_swap(double *a, int *k, int n, int i) {
    int mi = i, t;
    double mv = fabs(a[k[mi] * n + i]);

    for(int i_ = i + 1; i_ < n; ++ i_) {
        if(fabs(a[k[i_] * n + i]) > mv) {
            mi = i_;
            mv = fabs(a[k[mi] * n + i]);
        }
    }

    t = k[i];
    k[i] = k[mi];
    k[mi] = t;
}

int calc_inverse_matrix(double *a, double *b, double *x, int *k, int n);
int calc_inverse_matrix(double *a, double *b, double *x, int *k, int n) {
    // pre.

    for(int i = 0; i < n; ++ i) {
        for(int j = 0; j < n; ++ j) {
            b[i * n + j] = (i == j) ? 1. : 0.;
        }
        k[i] = i;
    }

    // down.
   
    for(int i = 0; i < n; ++ i) {
        double a_ii;

        //find_max_and_swap(a, k, n, i);

        a_ii = a[k[i] * n + i];
        if(!fabs(a_ii)) {
            return -1;
        }

        for(int j = i; j < n; ++ j) {
            a[k[i] * n + j] /= a_ii;
        }

        for(int j = 0; j < n; ++ j) {
            b[k[i] * n + j] /= a_ii;
        }

        for(int i_ = i + 1; i_ < n; ++ i_) {
            double a_ii_ = a[k[i_] * n + i];

            for(int j = i; j < n; ++ j) {
                a[k[i_] * n + j] -= a_ii_ * a[k[i] * n + j];
            }

            for(int j = 0; j < n; ++ j) {
                b[k[i_] * n + j] -= a_ii_ * b[k[i] * n + j];
            }
        }
    }

    // up.

    for(int j_ = 0; j_ < n; ++ j_) {
        x[(n - 1) * n + j_] = b[k[n - 1] * n + j_];
    }

    for(int i = n - 2; i >= 0; -- i) {
        for(int j_ = 0; j_ < n; ++ j_) {
            x[i * n + j_] = b[k[i] * n + j_];

            for(int j = i + 1; j < n; ++ j) {
                x[i * n + j_] -= a[k[i] * n + j] * x[j * n + j_];
            }
        }
    }

    return 0;
}

void fill_matrix(double c, double d, double *a, int n) {
    for(int i = 0; i < n; ++ i) {
        for(int j = 0; j < n; ++ j) {
            if(i == j) {
                a[i * n + j] = d;
            } else if(j == i - 1 || j == i + 1) {
                a[i * n + j] = c;

            } else {
                a[i * n + j] = 0.;
            }
        }
    }
}

double calc_accuracy(double *a, double *x, double *b, int n);
double calc_accuracy(double *a, double *x, double *b, int n) {
    double e;

    for(int i = 0; i < n; ++ i) {
        for(int j = 0; j < n; ++ j) {
            b[i * n + j] = (i == j) ? -1. : 0.;
            for(int k = 0; k < n; ++ k) {
                b[i * n + j] += a[i * n + k] * x[k *n + j];   
            }
        }
    }

    for(int j = 0; j < n; ++ j) {
        double s = 0.;

        for(int i = 0; i < n; ++ i) {
            s += fabs(b[i * n + j]);
        }

        if(j == 0 || s > e) {
            e = s;
        }
    }
    
    return e;
}

int run_test(double c, double d, double *a, int *k, int n, double *t, double *t_, double *e);
int run_test(double c, double d, double *a, int *k, int n, double *t, double *t_, double *e) {
    double *a_, *b, *x;
    int r;
    clock_t st, en;
    struct tms st_, en_;

    a_ = a  + n * n;
    b  = a_ + n * n;
    x  = b  + n * n;

    fill_matrix(c, d, a, n);
    memcpy(a_, a, n * n * sizeof(double));

    st = clock();
    times(&st_);
    r = calc_inverse_matrix(a, b, x, k, n);
    times(&en_);
    en = clock();

    if(r) {
        return r;
    }

    *t  = (en - st) / (double) CLOCKS_PER_SEC;
    *t_ = (en_.tms_utime - st_.tms_utime) / (double) sysconf(_SC_CLK_TCK);


    *e = calc_accuracy(a_, x, b, n);

    return 0;
}


int main(int ac, char *av[]) {
    double c, d, *a, t1, t1_, t2, t2_, e1, e2;
    int n, *k, m; 

    if(ac != 4) {
        fprintf(stderr, "Usage: %s <c> <d> <n>\n", av[0]);
        return -1;
    }

    if(to_double(av[1], &c) == -1) {
        fprintf(stderr, "Wrong parametr c ...\n");
        return -1;
    }

    if(to_double(av[2], &d) == -1) {
        fprintf(stderr, "Wrong parametr d ...\n");
        return -1;
    }

    if(to_int(av[3], &n) == -1) {
        fprintf(stderr, "Wrong parametr n ...\n");
        return -1;
    }

    m = 4 * (2 * n * 2 * n);

    a = malloc(m * sizeof(double) + 2 * n * sizeof(int));
    if(!a) {
        fprintf(stderr, "Memory allocation error ...\n");
        return -1;
    }

    k = (int *)(a + m);

    if(run_test(c, d, a, k, n, &t1, &t1_, &e1) == -1) {
        fprintf(stderr, "Can't inverse matrix n x n ...\n");
        return -1;
    }

    printf("t1: %.15e %.15e\ne1: %.15e\n", t1, t1_, e1);

    if(run_test(c, d, a, k, 2 * n, &t2, &t2_, &e2) == -1) {
        fprintf(stderr, "Can't inverse matrix 2n x 2n ...\n");
        return -1;
    }

    printf("t2: %.15e %.15e\ne2: %.15e\n", t2, t2_, e2);
    
    if(t1) {
        printf("%.15e\n", t2 / t1);
    }

    if(t1_) {
        printf("%.15e\n", t2_ / t1_);
    }
}

