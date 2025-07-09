package Builder;

import Instrukcje.*;
import Wyrazenia.Wyrazenie;

import java.util.List;

public class ForBuilder {

    private Petla petla;

    public ForBuilder(String nazwa, Wyrazenie wyrazenie) {
        petla = new Petla(nazwa, wyrazenie);
    }


    public ForBuilder wykonajProcedure(String nazwaProcedury, List<Wyrazenie> wyrazenia) {
        WykonajProcedure wykonajProcedure = new WykonajProcedure(nazwaProcedury, wyrazenia);
        petla.dodajInstrukcje(wykonajProcedure);
        return this;
    }

    public ForBuilder przypisanie(String nazwa, Wyrazenie wyrazenie) {
        Przypisanie przypisanie = new Przypisanie(nazwa, wyrazenie);
        petla.dodajInstrukcje(przypisanie);
        return this;
    }

    public ForBuilder print(Wyrazenie wyrazenie) {
        Print print = new Print(wyrazenie);
        petla.dodajInstrukcje(print);
        return this;
    }

    public ForBuilder petla(Petla petla) {
        this.petla.dodajInstrukcje(petla);
        return this;
    }

    public ForBuilder blok(Blok blok) {
        petla.dodajInstrukcje(blok);
        return this;
    }

    public ForBuilder warunkowa(Warunkowa warunkowa) {
        petla.dodajInstrukcje(warunkowa);
        return this;
    }

    public ForBuilder deklaracjaProcedury(DeklaracjaProcedury deklaracjaProcedury) {
        petla.dodajInstrukcje(deklaracjaProcedury);
        return this;
    }

    public Petla build() {
        return petla;
    }
}
