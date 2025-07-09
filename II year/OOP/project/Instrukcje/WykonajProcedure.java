package Instrukcje;

import Debugger.Debugger;
import Srodowisko.Srodowisko;
import Wyrazenia.Wyrazenie;
import Wyrazenia.Zmienna;

import java.util.ArrayList;
import java.util.List;

import static java.lang.System.exit;

public class WykonajProcedure extends Instrukcja {


    private String nazwaProcedury;

    private List<Wyrazenie> wyrazenia;


    public WykonajProcedure(String nazwaProcedury, List<Wyrazenie> wyrazenia) {
        this.nazwaProcedury = nazwaProcedury;
        this.wyrazenia = wyrazenia;
    }




    @Override
    public void wykonajInstrukcje(Debugger debugger, Srodowisko srodowisko) {
        try {
            DeklaracjaProcedury deklaracjaProcedury = srodowisko.dajDeklaracjeProcedury(nazwaProcedury);
            if (deklaracjaProcedury == null) {
                throw new NullPointerException("Nie istnieje procedura o danej nazwie.");
            }
            List<Zmienna> zmienneSwoje = deklaracjaProcedury.dajZmienneZParametrow();
            przypiszWartoscZmiennym(zmienneSwoje, srodowisko);
            dodajDoSwojejListyZmienne(srodowisko, zmienneSwoje);

            srodowisko.dodajListeZmiennych(zmienneSwoje);

            deklaracjaProcedury.wykonajInstrukcje(debugger, srodowisko);

            srodowisko.usunOstatniaListeZmiennych();
        } catch (NullPointerException e) {
            System.out.println("Wystapil blad: " + e.getMessage());
            System.out.println("Wywolanie procedury");
            wypiszZmienne(srodowisko.dajOstatniaListe());
            exit(-1);
        }

    }

    public void dodajDoSwojejListyZmienne(Srodowisko srodowisko, List<Zmienna> zmienneSwoje) {
        for (Zmienna zmiennaOjca : srodowisko.dajOstatniaListe()) {
            int stop = 0;
            for (Zmienna zmiennaSwoja : zmienneSwoje) {
                if (zmiennaOjca.getNazwa().equals(zmiennaSwoja.getNazwa())) {
                    stop = 1;
                    break;
                }
            }
            if (stop == 0) {
                zmienneSwoje.add(zmiennaOjca);
            }
        }
    }

    public void przypiszWartoscZmiennym(List<Zmienna> zmienne, Srodowisko srodowisko) {
        try {
            if (wyrazenia.size() != (zmienne.size()))
                throw new IllegalArgumentException("Liczba parametrow sie rozni.");
        } catch (IllegalArgumentException e) {
            System.out.println("Wystapil blad: " + e.getMessage());
            System.out.println("Wywolanie procedury");
            wypiszZmienne(srodowisko.dajOstatniaListe());
            exit(-1);
        }
        int k = 0;
        for (Zmienna z : zmienne) {
            z.setWartosc(wyrazenia.get(0).oblicz(srodowisko.dajOstatniaListe()));
            k++;
        }
    }

    @Override
    public void wypiszInstrukcje() {
        System.out.println("Wykonanie procedury: " + nazwaProcedury);
    }

    public void wypiszZmienne(List<Zmienna> zmienne) {
        for (Zmienna z : zmienne) {
            System.out.println("Nazwa " + z.getNazwa() + "| Wartosc " + z.oblicz(zmienne));
        }
    }
}
