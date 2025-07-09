package Builder;

import Instrukcje.*;
import Wyrazenia.Wyrazenie;

import java.util.List;

public class WarunkowaBuilder {

    private Warunkowa warunkowa;

    public WarunkowaBuilder(Wyrazenie leweWyrazenie, Wyrazenie praweWyrazenie, String operator) {
        warunkowa = new Warunkowa(leweWyrazenie, praweWyrazenie, operator);
    }



    public WarunkowaBuilder wykonajProcedureF(String nazwaProcedury, List<Wyrazenie> wyrazenia) {
        WykonajProcedure wykonajProcedure = new WykonajProcedure(nazwaProcedury, wyrazenia);
        warunkowa.dodajFalsz(wykonajProcedure);
        return this;
    }

    public WarunkowaBuilder przypisanieF(String nazwa, Wyrazenie wyrazenie) {
        Przypisanie przypisanie = new Przypisanie(nazwa, wyrazenie);
        warunkowa.dodajFalsz(przypisanie);
        return this;
    }

    public WarunkowaBuilder printF(Wyrazenie wyrazenie) {
        Print print = new Print(wyrazenie);
        warunkowa.dodajFalsz(print);
        return this;
    }

    public WarunkowaBuilder petlaF(Petla petla) {
        warunkowa.dodajFalsz(petla);
        return this;
    }

    public WarunkowaBuilder blokF(Blok blok) {
        warunkowa.dodajFalsz(blok);
        return this;
    }

    public WarunkowaBuilder warunkowaF(Warunkowa warunkowa) {
        this.warunkowa.dodajFalsz(warunkowa);
        return this;
    }

    public WarunkowaBuilder deklaracjaProceduryF(DeklaracjaProcedury deklaracjaProcedury) {
        warunkowa.dodajFalsz(deklaracjaProcedury);
        return this;
    }

    public WarunkowaBuilder wykonajProcedureP(String nazwaProcedury, List<Wyrazenie> wyrazenia) {
        WykonajProcedure wykonajProcedure = new WykonajProcedure(nazwaProcedury, wyrazenia);
        warunkowa.dodajPrawda(wykonajProcedure);
        return this;
    }

    public WarunkowaBuilder przypisanieP(String nazwa, Wyrazenie wyrazenie) {
        Przypisanie przypisanie = new Przypisanie(nazwa, wyrazenie);
        warunkowa.dodajPrawda(przypisanie);
        return this;
    }

    public WarunkowaBuilder printP(Wyrazenie wyrazenie) {
        Print print = new Print(wyrazenie);
        warunkowa.dodajPrawda(print);
        return this;
    }

    public WarunkowaBuilder petlaP(Petla petla) {
        warunkowa.dodajPrawda(petla);
        return this;
    }

    public WarunkowaBuilder blokP(Blok blok) {
        warunkowa.dodajPrawda(blok);
        return this;
    }

    public WarunkowaBuilder warunkowaP(Warunkowa warunkowa) {
        this.warunkowa.dodajPrawda(warunkowa);
        return this;
    }

    public WarunkowaBuilder deklaracjaProceduryP(DeklaracjaProcedury deklaracjaProcedury) {
        warunkowa.dodajPrawda(deklaracjaProcedury);
        return this;
    }

    public Warunkowa build() {
        return warunkowa;
    }
}
