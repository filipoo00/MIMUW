package Instrukcje;
import Debugger.Debugger;
import Wyrazenia.*;
import Srodowisko.*;

import java.util.List;

import static java.lang.System.exit;

public class Print extends Instrukcja {

    private Wyrazenie wyrazenie;

    public Print(Wyrazenie wyrazenie) {
        this.wyrazenie = wyrazenie;
    }


    public void ustawWartoscZmiennej(List<Zmienna> zmienne) {
        int stop = 0;
        for (Zmienna zmienna : zmienne) {
            if (zmienna.getNazwa().equals(wyrazenie.getNazwa())) {
                wyrazenie.setWartosc(zmienna.oblicz(zmienne));
                stop = 1;
                break;
            }
        }
        if (stop == 0)
            wyrazenie = null;
    }

    @Override
    public void wykonajInstrukcje(Debugger debugger, Srodowisko srodowisko) {
        try {
            if (wyrazenie.getNazwa() != null) {
                ustawWartoscZmiennej(srodowisko.dajOstatniaListe());
            }
            if (wyrazenie == null) {
                throw new IllegalArgumentException("Nieprawidlowa podana nazwa.");
            }

            int wartosc = wyrazenie.oblicz(srodowisko.dajOstatniaListe());
            System.out.println(wartosc);
        } catch (ArithmeticException | NullPointerException | IllegalArgumentException e){
            System.out.println("Wystapil blad: " + e.getMessage());
            System.out.println("Print");
            drukujZmienne(srodowisko.dajOstatniaListe());
            exit(-1);
        }
    }

    public void wypiszInstrukcje() {
        System.out.println("Print: " + wyrazenie.toString());
    }

    public void drukujZmienne(List<Zmienna> zmienne) {
        for (Zmienna z : zmienne) {
            System.out.println("Nazwa " + z.getNazwa() + "| Wartosc " + z.oblicz(zmienne));
        }
    }
}
