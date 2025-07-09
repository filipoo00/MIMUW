package Builder;

import Instrukcje.*;
import Wyrazenia.Wyrazenie;

import java.util.List;

public class ProceduraBuilder {

    private DeklaracjaProcedury deklaracjaProcedury;


    public ProceduraBuilder(String nazwa, List<String> parametry) {
       deklaracjaProcedury = new DeklaracjaProcedury(nazwa, parametry);
    }

    public ProceduraBuilder deklaracjaZmiennej(String nazwa, Wyrazenie wyrazenie) {
        DeklaracjaZmiennej deklaracjaZmiennej = new DeklaracjaZmiennej(nazwa, wyrazenie);
        deklaracjaProcedury.dajBlok().dodajDeklaracjeZmiennej(deklaracjaZmiennej);
        return this;
    }

    public ProceduraBuilder deklaracjaProcedury(DeklaracjaProcedury deklaracjaProcedury) {
        this.deklaracjaProcedury.dajBlok().dodajDeklaracjeProcedury(deklaracjaProcedury);
        return this;
    }

    public ProceduraBuilder przypisanie(String nazwa, Wyrazenie wyrazenie) {
        Przypisanie przypisanie = new Przypisanie(nazwa, wyrazenie);
        deklaracjaProcedury.dajBlok().dodajInstrukcje(przypisanie);
        return this;
    }

    public ProceduraBuilder print(Wyrazenie wyrazenie) {
        Print print = new Print(wyrazenie);
        deklaracjaProcedury.dajBlok().dodajInstrukcje(print);
        return this;
    }

    public ProceduraBuilder petla(Petla petla) {
        deklaracjaProcedury.dajBlok().dodajInstrukcje(petla);
        return this;
    }

    public ProceduraBuilder blok(Blok blok) {
        deklaracjaProcedury.dajBlok().dodajInstrukcje(blok);
        return this;
    }

    public ProceduraBuilder warunkowa(Warunkowa warunkowa) {
        deklaracjaProcedury.dajBlok().dodajInstrukcje(warunkowa);
        return this;
    }

    public ProceduraBuilder wykonajProcedure(String nazwaProcedury, List<Wyrazenie> wyrazenia) {
        WykonajProcedure wykonajProcedure = new WykonajProcedure(nazwaProcedury, wyrazenia);
        deklaracjaProcedury.dajBlok().dodajInstrukcje(wykonajProcedure);
        return this;
    }


    public DeklaracjaProcedury build() {
        return deklaracjaProcedury;
    }
}
