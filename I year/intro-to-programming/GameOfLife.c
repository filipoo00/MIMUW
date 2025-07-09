#include <stdio.h>
#include <stdlib.h>

#ifndef WIERSZE
#define WIERSZE 22
#endif

#ifndef KOLUMNY
#define KOLUMNY 80
#endif

struct lista {
    int nr_kolumny;
    int nr_wiersza;
    int zywi_sasiedzi;
    struct lista *nast;
};
typedef struct lista Tlista;

int sprawdzaj_czy_w_liscie(Tlista **l, int i, int j) {
    Tlista *pom = *l;
    if(pom == NULL) {
        return 0;
    }
    while(pom != NULL && (pom -> nr_wiersza != i || pom -> nr_kolumny != j)) {
        pom = pom -> nast;
    }
    if(pom != NULL) return 1;
    return 0;
}

void wstaw_na_koniec(Tlista **l, int i, int j, int k) {
    Tlista *pom = (Tlista*)malloc(sizeof(Tlista));
    pom -> nr_wiersza = i;
    pom -> nr_kolumny = j;
    pom -> zywi_sasiedzi = k;
    pom -> nast = NULL;
    if(*l == NULL) {
        *l = pom;
    }
    else {
        Tlista *ostatni = *l;
        while(ostatni -> nast != NULL) {
            ostatni = ostatni -> nast;
        }
        ostatni -> nast = pom;
    }
}

void usun_liste(Tlista **l) {
    Tlista *pom = *l;
    Tlista *p = NULL;
    while(pom != NULL) {
        p = pom;
        pom = pom -> nast;
        free(p);
    }
}

void wczytaj(Tlista **l) {
    int c = getchar();
    c = getchar();
    while(c != '\n') {
        ungetc(c, stdin);
        scanf("%d", &c);
        int k = getchar();
        while(k != '\n') {
            if(k == ' ') scanf("%d", &k);
            wstaw_na_koniec(l, c, k, 0);
            k = getchar();
        }
        c = getchar();
        c = getchar();
        if(c == '\n') break;
    }
}

int zliczanie_zywych_sasiadow(Tlista **l, int i, int j) {
    int licznik = 0;
    Tlista *pom = *l;
    while(pom != NULL) {
        if((pom -> nr_wiersza == i - 1 && pom -> nr_kolumny == j - 1) ||
            (pom -> nr_wiersza == i - 1 && pom -> nr_kolumny == j) ||
            (pom -> nr_wiersza == i - 1 && pom -> nr_kolumny == j + 1) ||
            (pom -> nr_wiersza == i && pom -> nr_kolumny == j - 1) ||
            (pom -> nr_wiersza == i && pom -> nr_kolumny == j + 1) ||
            (pom -> nr_wiersza == i + 1 && pom -> nr_kolumny == j - 1) ||
            (pom -> nr_wiersza == i + 1 && pom -> nr_kolumny == j) ||
            (pom -> nr_wiersza == i + 1 && pom -> nr_kolumny == j + 1)) {
                if(pom -> zywi_sasiedzi != -1) {
                    licznik++;
                }
        }
        pom = pom -> nast;
    }
    return licznik;
}

int sprawdz_czy_martwa(Tlista **l, int i, int j) {
    int czy_martwa = 0;
    Tlista *pom = *l;
    while(pom != NULL) {
        if(pom -> nr_wiersza == i && pom -> nr_kolumny == j) {
            czy_martwa = pom -> zywi_sasiedzi;
        }
        pom = pom -> nast;
    }
    return czy_martwa;
}

void dodaj_zywych_sasiadow(Tlista **l, int i, int j, int sasiedzi) {
    Tlista *pom = *l;
    while(pom != NULL) {
        if(pom -> nr_wiersza == i && pom -> nr_kolumny == j) {
            pom -> zywi_sasiedzi = sasiedzi;
            break;
        }
        pom = pom -> nast;
    }
}

void usun_zywe_komorki(Tlista **l) {
    Tlista atrapa;
    atrapa.nast = *l;
    Tlista *p = NULL;
    Tlista *pom = &atrapa;
    while(pom -> nast != NULL) {
        if((pom -> nast ->  zywi_sasiedzi < 2 && pom -> nast -> zywi_sasiedzi != -1) || 
            (pom -> nast -> zywi_sasiedzi > 3)) {

            p = pom -> nast;
            pom -> nast = p -> nast;
            free(p);
        }
        else if(pom -> nast -> zywi_sasiedzi == -1) {
            pom -> nast -> zywi_sasiedzi = 3;
            pom = pom -> nast;
        }
        else {
            pom = pom -> nast;
       }
    }
    *l = atrapa.nast;
}

void usun_wszystkie_wskazane(Tlista **l, int i, int j) {
    Tlista atrapa;
    atrapa.nast = *l;
    Tlista *pom = &atrapa;
    Tlista *p = NULL;
    while(pom -> nast != NULL) {
        if(pom -> nast -> nr_wiersza == i && pom -> nast -> nr_kolumny == j) {
            p = pom -> nast;
            pom -> nast = p -> nast;
            free(p);
        }
        else {
            pom = pom -> nast;
        }
    }
    *l = atrapa.nast;
}

int zliczanie(Tlista **l) {
    int licznik = 0;
    Tlista *pom = *l;
    while(pom != NULL) {
        licznik++;
        pom = pom -> nast;
    }
    return licznik;
}

void nowa_generacja_lista(Tlista **l) {
    int licznik = zliczanie(l);
    int liczenie = 0;
    Tlista *pom = *l;
    while(pom != NULL && liczenie != licznik) {
        int i = pom -> nr_wiersza;
        int j = pom -> nr_kolumny;
        pom = pom -> nast;
        for(int w = i - 1; w <= i + 1; w++) {
            for(int k = j - 1; k <= j + 1; k++) {
                    int sprawdzaj = sprawdzaj_czy_w_liscie(l, w, k);
                    if(sprawdzaj == 0) {
                        wstaw_na_koniec(l, w, k, -1);
                }
            }
        }
        int zywi_sasiedzi = zliczanie_zywych_sasiadow(l, i, j);
        int martwa = sprawdz_czy_martwa(l, i, j);
        if(martwa != -1) {
            dodaj_zywych_sasiadow(l,i,j, zywi_sasiedzi);
        }
        if(martwa == -1 && zywi_sasiedzi != 3) {
            usun_wszystkie_wskazane(l, i, j);
        }
        liczenie++;
    }
    while(pom != NULL) {
        int i = pom -> nr_wiersza;
        int j = pom -> nr_kolumny;
        pom = pom -> nast;
        int zywi_sasiedzi = zliczanie_zywych_sasiadow(l, i, j);
        int martwa = sprawdz_czy_martwa(l, i, j);
        if(martwa != -1) {
            dodaj_zywych_sasiadow(l,i,j, zywi_sasiedzi);
        }
        if(martwa == -1 && zywi_sasiedzi != 3) {
            usun_wszystkie_wskazane(l, i, j);
        }
    }
    usun_zywe_komorki(l);
}

void posortuj_liste_wiersze(Tlista **l) {
    Tlista *pom = *l;
        while(pom != NULL) {
            Tlista *pom2 = pom;
            Tlista *pom3 = pom -> nast;
            while(pom3 != NULL) {
                if(pom2 -> nr_wiersza > pom3 -> nr_wiersza) {
                    pom2 = pom3;
                }
                pom3 = pom3 -> nast;
            }
            int x = pom -> nr_wiersza;
            int y = pom -> nr_kolumny;
            pom -> nr_wiersza = pom2 -> nr_wiersza;
            pom2 -> nr_wiersza = x;
            pom -> nr_kolumny = pom2 -> nr_kolumny;
            pom2 -> nr_kolumny = y;
            pom = pom -> nast;
        }
}

void posortuj_liste_kolumny(Tlista **l) {
    Tlista *pom = *l;
    while(pom != NULL) {
        int x = pom -> nr_wiersza;
        Tlista *pom2 = pom;
        Tlista *pom3 = pom -> nast;
        while(pom3 != NULL && pom3 -> nr_wiersza == x) {
            if(pom2 -> nr_kolumny > pom3 -> nr_kolumny) {
                pom2 = pom3;
            }
            pom3 = pom3 -> nast;
        }
        int y = pom -> nr_kolumny;
        pom -> nr_kolumny = pom2 -> nr_kolumny;
        pom2 -> nr_kolumny = y;
        pom = pom -> nast;
    }
}

void zrzut_aktualnej_generacji(Tlista **l) {
    posortuj_liste_wiersze(l);
    posortuj_liste_kolumny(l);
    Tlista *pom = *l;
    int pusta = 1;
    if(pom != NULL) {
        int a = pom -> nr_wiersza;
        int b = pom -> nr_kolumny;
        putchar('/');
        printf("%d", a);
        putchar(' ');
        printf("%d", b);
        pusta = 0;
    }
    while(pom != NULL) {
        int c;
        int d;
        int a = pom -> nr_wiersza;
        pom = pom -> nast;
        if (pom != NULL) {
            c = pom -> nr_wiersza;
            d = pom -> nr_kolumny;
            if(a == c) {
                putchar(' ');
                printf("%d", d);
            }
            else if(a != c) {
                putchar('\n');
                putchar('/');
                printf("%d", c);
                putchar(' ');
                printf("%d", d);
            }
        }
    }
    if(pusta == 0) {
        putchar('\n');
    }
    putchar('/');
    putchar('\n');
}

void wypelnij(int a[][KOLUMNY]) {
    for(int i = 0; i < WIERSZE; i++) {
        for(int j = 0; j < KOLUMNY; j++) {
            a[i][j] = '.';
        }
    }
}

void drukuj(int a[][KOLUMNY]) {
    for(int i = 0; i < WIERSZE; i++) {
        for(int j = 0; j < KOLUMNY; j++) {
            putchar(a[i][j]);
        }
        putchar('\n');
    }
    for(int j = 0; j < KOLUMNY; j++) {
        putchar('=');
    }
    putchar('\n');
}

void drukowanie_generacji(Tlista **l, int i, int j) {
    int a[WIERSZE][KOLUMNY];
    wypelnij(a);
    Tlista *pom = *l;
    while(pom != NULL) {
        if((pom -> nr_wiersza >= i) && 
            (pom -> nr_wiersza <= i + WIERSZE - 1) && 
            (pom -> nr_kolumny >= j) && 
            (pom -> nr_kolumny <= j + KOLUMNY - 1)) {

            int wiersz = pom -> nr_wiersza - i;
            int kolumna = pom -> nr_kolumny - j;
            a[wiersz][kolumna] = '0';
        }
        pom = pom -> nast;
    }
    drukuj(a);
}

void graj(Tlista **l) {
    int w = 1;
    int k = 1;
    drukowanie_generacji(l, w, k);
    int c = getchar();
    while(c != '.') {
        if(c == '\n') {
            nowa_generacja_lista(l);
            drukowanie_generacji(l, w, k);
            c = getchar();
        }
        else {
            ungetc(c, stdin);
            int a;
            scanf("%d", &a);
            c = getchar();
            if(c == ' ') {
                int b;
                scanf("%d", &b);
                w = a;
                k = b;
                drukowanie_generacji(l, w, k);
                c = getchar();
                c = getchar();
            }
            else {
                if(a == 1) {
                    nowa_generacja_lista(l);
                    drukowanie_generacji(l, w, k);
                    c = getchar();
                }
                else if(a == 0) {
                    zrzut_aktualnej_generacji(l);
                    drukowanie_generacji(l, w, k);
                    c = getchar();
                }
                else {
                    for(int i = 0; i < a; i++) {
                        nowa_generacja_lista(l);
                    }
                    drukowanie_generacji(l, w, k);
                    c = getchar();
                }
            }
        }
    }
}

int main(void) {
    Tlista *l = NULL;
    wczytaj(&l);
    graj(&l);
    usun_liste(&l);
    return 0;
}