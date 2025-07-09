package Builder;

import Instrukcje.*;
import Wyrazenia.Wyrazenie;

import java.util.List;

public class BlokBuilder {

    private Blok blok;

    public BlokBuilder() {
        blok = new Blok();
    }

    public BlokBuilder deklaracjaZmiennej(String nazwa, Wyrazenie wyrazenie) {
        DeklaracjaZmiennej deklaracjaZmiennej = new DeklaracjaZmiennej(nazwa, wyrazenie);
        blok.dodajDeklaracjeZmiennej(deklaracjaZmiennej);
        return this;
    }

    public BlokBuilder wykonajProcedure(String nazwaProcedury, List<Wyrazenie> wyrazenia) {
        WykonajProcedure wykonajProcedure = new WykonajProcedure(nazwaProcedury, wyrazenia);
        blok.dodajInstrukcje(wykonajProcedure);
        return this;
    }

    public BlokBuilder przypisanie(String nazwa, Wyrazenie wyrazenie) {
        Przypisanie przypisanie = new Przypisanie(nazwa, wyrazenie);
        blok.dodajInstrukcje(przypisanie);
        return this;
    }

    public BlokBuilder print(Wyrazenie wyrazenie) {
        Print print = new Print(wyrazenie);
        blok.dodajInstrukcje(print);
        return this;
    }

    public BlokBuilder petla(Petla petla) {
        blok.dodajInstrukcje(petla);
        return this;
    }

    public BlokBuilder blok(Blok blok) {
        this.blok.dodajInstrukcje(blok);
        return this;
    }

    public BlokBuilder warunkowa(Warunkowa warunkowa) {
        blok.dodajInstrukcje(warunkowa);
        return this;
    }

    public BlokBuilder deklaracjaProcedury(DeklaracjaProcedury deklaracjaProcedury) {
        blok.dodajDeklaracjeProcedury(deklaracjaProcedury);
        return this;
    }

    public Blok build() {
        return blok;
    }
}
