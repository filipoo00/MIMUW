package Wyrazenia;

import java.util.List;

public class Odejmowanie extends Operator {
    public Odejmowanie(Wyrazenie lewe, Wyrazenie prawe) {
        super(lewe, prawe);
    }

    public static Odejmowanie odejmij(Wyrazenie lewe, Wyrazenie prawe) {
        return new Odejmowanie(lewe, prawe);
    }

    public int oblicz(List<Zmienna> zmienne) {
        if (lewe == null || prawe == null) {
            throw new NullPointerException("Blad wyliczenia.");
        }
        int lewaWartosc = lewe.oblicz(zmienne);
        int prawaWartosc = prawe.oblicz(zmienne);
        int wynik = lewaWartosc - prawaWartosc;

        return wynik;
    }
    @Override
    public String getSymbol() {
        return "-";
    }
}
