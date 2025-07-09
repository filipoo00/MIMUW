package Instrukcje;
import Debugger.Debugger;
import Srodowisko.Srodowisko;
import Wyrazenia.*;


import java.util.ArrayList;
import java.util.List;

import static java.lang.System.exit;


public class Blok extends Instrukcja {

    private List<DeklaracjaZmiennej> deklaracjaZmiennych;
    private List<Instrukcja> instrukcje;
    private List<DeklaracjaProcedury> deklaracjaProcedur;

    public Blok() {
        this.deklaracjaProcedur = new ArrayList<>();
        this.deklaracjaZmiennych = new ArrayList<>();
        this.instrukcje = new ArrayList<>();
    }

    public void dodajInstrukcje(Instrukcja i) {
        instrukcje.add(i);
    }

    public void dodajDeklaracjeZmiennej(DeklaracjaZmiennej deklaracjaZmiennej) {
        deklaracjaZmiennych.add(deklaracjaZmiennej);
    }

    public void dodajDeklaracjeProcedury(DeklaracjaProcedury deklaracjaProcedury) {
        deklaracjaProcedur.add(deklaracjaProcedury);
    }

    public List<Zmienna> deklarujZmienne(Debugger debugger, Srodowisko srodowisko) {
        List<Zmienna> noweZmienne = new ArrayList<>();
        try {
            for (DeklaracjaZmiennej deklaracjaZmiennej : deklaracjaZmiennych) {
                debugger.debugger(deklaracjaZmiennej, srodowisko);
                Zmienna z = new Zmienna(deklaracjaZmiennej.getNazwa());
                z.setWartosc(deklaracjaZmiennej.getWartosc());
                for (Zmienna zmienna : noweZmienne) {
                    if (zmienna.getNazwa().equals(z.getNazwa()))
                        throw new IllegalArgumentException("Zmienna o danej nazwie juz istnieje.");
                }
                noweZmienne.add(z);
            }
        } catch (ArithmeticException | IllegalArgumentException e) {
            System.out.println("Wystapil blad: " + e.getMessage());
            System.out.println("Deklaracja zmiennej");
            wypiszZmienne(srodowisko.dajOstatniaListe());
            exit(-1);
        }
        return noweZmienne;
    }

    public void dodajDoSwojejListyZmienne(Srodowisko srodowisko, List<Zmienna> zmienneSwoje) {
        if (srodowisko.dajRozmiarListy() != 0) {
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
    }

    public void deklarujProcedury(Debugger debugger, Srodowisko srodowisko) {
        List<DeklaracjaProcedury> deklaracje = new ArrayList<>();
        try {
            for (DeklaracjaProcedury deklaracjaProcedury : deklaracjaProcedur) {
                debugger.debugger(deklaracjaProcedury, srodowisko);
                if (!deklaracjaProcedury.dajNazwaProcedury().matches("[a-z]+"))
                    throw new IllegalArgumentException("Nieprawidlowa nazwa procedury.");
                if (!deklaracjaProcedury.sprawdzCzyDobreParametry())
                    throw new IllegalArgumentException("Nieprawidlowe parametry.");
                for (DeklaracjaProcedury dp : deklaracje) {
                    if (deklaracjaProcedury.dajNazwaProcedury().equals(dp.dajNazwaProcedury()))
                        throw new IllegalArgumentException("Deklaracja o danej nazwie juz istnieje.");
                }
                deklaracje.add(deklaracjaProcedury);
            }
        } catch (IllegalArgumentException e) {
            System.out.println("Wystapil blad: " + e.getMessage());
            System.out.println("Deklaracja procedury");
            wypiszZmienne(srodowisko.dajOstatniaListe());
            exit(-1);
        }
    }


    public void wykonajInstrukcje(Debugger debugger, Srodowisko srodowisko) {
        List<Zmienna> zmienneSwoje = deklarujZmienne(debugger, srodowisko);
        dodajDoSwojejListyZmienne(srodowisko, zmienneSwoje);


        srodowisko.dodajListeZmiennych(zmienneSwoje);
        srodowisko.dodajListeProcedur(deklaracjaProcedur);

        deklarujProcedury(debugger, srodowisko);

        for (Instrukcja i : instrukcje) {
            debugger.debugger(i, srodowisko);
            i.wykonajInstrukcje(debugger, srodowisko);
        }

        srodowisko.usunOstatniaListeZmiennych();
        srodowisko.usunOstatniaListeProcedur();

        if (srodowisko.dajRozmiarListy() == 0) {
            wypiszZmienne(zmienneSwoje);
        }
    }

    public void wypiszZmienne(List<Zmienna> zmienne) {
        for (Zmienna z : zmienne) {
            System.out.println("Nazwa " + z.getNazwa() + "| Wartosc " + z.oblicz(zmienne));
        }
    }

    public void wypiszInstrukcje() {
        System.out.println("Blok");
    }
}