package Instrukcje;
import Debugger.Debugger;
import Wyrazenia.*;
import Srodowisko.*;

import java.util.ArrayList;
import java.util.List;

public class DeklaracjaZmiennej extends Instrukcja {

    private String nazwa;
    private Wyrazenie wyrazenie;

    public DeklaracjaZmiennej(String nazwa, Wyrazenie wyrazenie) {
        this.nazwa = nazwa;
        this.wyrazenie = wyrazenie;
    }

    public int getWartosc() {
        List<Zmienna> z = new ArrayList<>();
        return wyrazenie.oblicz(z);
    }

    public String getNazwa() {
        if (nazwa == null || nazwa.length() != 1 || !Character.isLowerCase(nazwa.charAt(0))) {
            throw new IllegalArgumentException("Nazwa zmiennej musi byc jednoliterowa od 'a' do 'z'.");
        }
        else {
            return nazwa;
        }
    }

    public void wypiszInstrukcje() {
        System.out.println("Deklaracja: " + nazwa + " : " + wyrazenie.toString());
    }


    @Override
    public void wykonajInstrukcje(Debugger debugger, Srodowisko srodowisko) {

    }
}
