package Instrukcje;
import Debugger.Debugger;
import Wyrazenia.*;
import Srodowisko.*;

import java.util.ArrayList;
import java.util.List;

import static java.lang.System.exit;

public class Petla extends Instrukcja {

    private String nazwa;
    private Wyrazenie wyrazenie;
    private List<Instrukcja> instrukcje;

    public Petla(String nazwa, Wyrazenie wyrazenie) {
        this.nazwa = nazwa;
        this.wyrazenie = wyrazenie;
        this.instrukcje = new ArrayList<>();
    }

    public void dodajInstrukcje(Instrukcja i) {
        instrukcje.add(i);
    }


    public List<Zmienna> stworzListeZmiennych(Srodowisko srodowisko) {
        List<Zmienna> zmienne = new ArrayList<>();
        Zmienna nowa = new Zmienna(nazwa);
        nowa.setWartosc(0);
        zmienne.add(nowa);
        for (Zmienna zmiennaOjca : srodowisko.dajOstatniaListe()) {
            int stop = 0;
            for (Zmienna zmiennaSwoja : zmienne) {
                if (zmiennaOjca.getNazwa().equals(zmiennaSwoja.getNazwa())) {
                    stop = 1;
                    break;
                }
            }
            if (stop == 0)
                zmienne.add(zmiennaOjca);
        }
        return zmienne;
    }

    public void wykonajInstrukcje(Debugger debugger, Srodowisko srodowisko) {
        try {
            if (nazwa == null || nazwa.length() != 1 || !Character.isLowerCase(nazwa.charAt(0))) {
                throw new IllegalArgumentException("Nazwa zmiennej musi byc jednoliterowa od 'a' do 'z'.");
            }
            List<Zmienna> zmienneSwoje = stworzListeZmiennych(srodowisko);

            int wartosc = wyrazenie.oblicz(zmienneSwoje);

            srodowisko.dodajListeZmiennych(zmienneSwoje);


            if (wartosc > 0) {
                for (int i = 0; i < wartosc; i++) {
                    for (Zmienna z : zmienneSwoje) {
                        if (z.getNazwa().equals(nazwa)) {
                            z.setWartosc(i);
                            break;
                        }
                    }
                    for (Instrukcja instrukcja : instrukcje) {
                        debugger.debugger(instrukcja, srodowisko);
                        instrukcja.wykonajInstrukcje(debugger, srodowisko);
                    }
                }
            }

            srodowisko.usunOstatniaListeZmiennych();

        } catch (ArithmeticException | NullPointerException | IllegalArgumentException e) {
            System.out.println("Wystapil blad: " + e.getMessage());
            System.out.println("Petla");
            wypiszZmienne(srodowisko.dajOstatniaListe());
            exit(-1);
        }
    }

    public void wypiszInstrukcje() {
        System.out.println("Petla: " + nazwa + " " + wyrazenie.toString());
    }

    public void wypiszZmienne(List<Zmienna> zmienne) {
        for (Zmienna z : zmienne) {
            System.out.println("Nazwa " + z.getNazwa() + "| Wartosc " + z.oblicz(zmienne));
        }
    }
}