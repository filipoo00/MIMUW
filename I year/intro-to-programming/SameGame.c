#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#ifndef WIERSZE
# define WIERSZE 10
#endif

#ifndef KOLUMNY
# define KOLUMNY 15
#endif

#ifndef RODZAJE
# define RODZAJE 4
#endif

#define PUSTE '.'

void czytaj_plansze(char a[][KOLUMNY], int d[], int n) {
    int w = 0;
    int c = getchar();
    while(w < n) {
        d[w] = 0;
        while(d[w] < KOLUMNY) {
            a[w][d[w]] = (char) c;
            d[w]++;
            c = getchar();
        }
        w++;
        if(w < n) {
            c = getchar();
        }
    }
}

void pisz_plansze(char a[][KOLUMNY], int d[], int n) {
    int w = 0;
    while(w < n) {
        d[w] = 0;
        while(d[w] < KOLUMNY) {
            putchar(a[w][d[w]]);
            d[w]++;
        }
        putchar('\n');
        w++;
    }
}

int sprawdz_czy_grupa(char a[][KOLUMNY], int n, int w, int k) {
    if(((k + 1 < KOLUMNY) && (a[w][k] == a[w][k+1])) ||
        ((w + 1 < n) && (a[w][k] == a[w+1][k])) ||
        ((k - 1 >= 0) && (a[w][k] == a[w][k-1])) ||
        ((w - 1 >= 0) && (a[w][k] == a[w-1][k]))) {
            return 1;
    }
    else {
        return 0;
    }
}

void usun_grupe(char a[][KOLUMNY], int n, int w, int k, int klocek) {
    a[w][k] = PUSTE;
    if((w + 1 < n) && (a[w+1][k] == klocek)) {
        usun_grupe(a, n, w + 1, k, klocek);
    }
    if((w - 1 >= 0) && (a[w - 1][k] == klocek)) {
        usun_grupe(a, n, w - 1, k, klocek);
    }
    if((k + 1 < KOLUMNY) && (a[w][k+1] == klocek)) {
        usun_grupe(a, n, w, k + 1, klocek);
    }
    if((k - 1 >= 0) && (a[w][k-1] == klocek)) {
        usun_grupe(a, n, w, k - 1, klocek);
    }
}

void zamien(char *x, char *y) {
    char z = *x;
    *x = *y;
    *y = z;
}

void przesun_w_dol(char a[][KOLUMNY], int n) {
    for(int k = 0; k < KOLUMNY; k++) {
        for(int w = n - 1; w > 0; w--) {
            if(a[w][k] == PUSTE) {
                int nowe_w = w;
                while((nowe_w > 0) && (a[nowe_w-1][k] == PUSTE)) {
                    nowe_w--;
                }
                if(nowe_w != 0) {
                    zamien(&a[w][k], &a[nowe_w-1][k]);
                }
            }
        }
    }
}

void przesun_w_lewo(char a[][KOLUMNY], int n) {
    int w = n - 1;
    for(int k = 0; k < KOLUMNY - 1; k++) {
        if(a[w][k] == PUSTE) {
            int nowe_k = k;
            while((nowe_k < KOLUMNY - 1) && (a[w][nowe_k+1] == PUSTE)) {
                nowe_k++;
            }
            if(nowe_k != KOLUMNY - 1) {
                for(int i = 0; i < n; i++) {
                    zamien(&a[i][k], &a[i][nowe_k+1]);
                }
            }
        }
    }
}

void porzadkuj_plansze(char a[][KOLUMNY], int n) {
    przesun_w_dol(a, n);
    przesun_w_lewo(a, n);
}

void wykonaj(char a[][KOLUMNY], int d[], int n, int w, int k) {
    czytaj_plansze(a, d, n);
    if(a[w][k] == '.') {
        pisz_plansze(a, d, n);
    }
    else {
        int sprawdzaj = sprawdz_czy_grupa(a, n, w, k);
        if(sprawdzaj == 0) {
            pisz_plansze(a, d, n);
        }
        else {
            int klocek = a[w][k];
            usun_grupe(a, n, w, k, klocek);
            porzadkuj_plansze(a, n);
            pisz_plansze(a, d, n);
        }
    }
}

int main(int argc, char *argv[]) {
    char a[WIERSZE][KOLUMNY];
    int d[WIERSZE];
    assert(argc == 3);
    int w = atoi(argv[1]);
    int k = atoi(argv[2]);
    wykonaj(a, d, WIERSZE, w, k);
    return 0;
}
