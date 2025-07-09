package Instrukcje;
import Wyrazenia.*;
import Srodowisko.*;
import Debugger.*;

import java.util.List;

import static java.lang.System.exit;


public class Przypisanie extends Instrukcja {

    private String nazwa;
    private Wyrazenie wyrazenie;

    public Przypisanie(String nazwa, Wyrazenie wyrazenie) {
        this.nazwa = nazwa;
        this.wyrazenie = wyrazenie;
    }


    @Override
    public void wykonajInstrukcje(Debugger debugger, Srodowisko srodowisko) {
        try {
            int wartosc = wyrazenie.oblicz(srodowisko.dajOstatniaListe());
            int stop = 0;
            for (Zmienna z : srodowisko.dajOstatniaListe()) {
                if (z.getNazwa().equals(nazwa)) {
                    z.setWartosc(wartosc);
                    stop = 1;
                    break;
                }
            }
            if (stop == 0)
                throw new IllegalArgumentException("Podano nieprawidlowa nazwe.");
        } catch (ArithmeticException | IllegalArgumentException | NullPointerException e) {
            System.out.println("Wystapil blad: " + e.getMessage());
            System.out.println("Przypisanie");
            for (Zmienna z : srodowisko.dajOstatniaListe()) {
                System.out.println("Nazwa " + z.getNazwa() + "| Wartosc " + z.oblicz(srodowisko.dajOstatniaListe()));
            }
            exit(-1);
        }
    }

    public void wypiszInstrukcje() {
        System.out.println("Przypisanie: " + nazwa + " := " + wyrazenie.toString());
    }

}
