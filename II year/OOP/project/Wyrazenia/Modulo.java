package Wyrazenia;

import java.util.List;

public class Modulo extends Operator {
    public Modulo(Wyrazenie lewe, Wyrazenie prawe) {
        super(lewe, prawe);
    }

    public static Modulo modulo(Wyrazenie lewe, Wyrazenie prawe) {
        return new Modulo(lewe, prawe);
    }

    public int oblicz(List<Zmienna> zmienne) {
        if (lewe == null || prawe == null) {
            throw new NullPointerException("Blad wyliczenia.");
        }
        int lewaWartosc = lewe.oblicz(zmienne);
        int prawaWartosc = prawe.oblicz(zmienne);
        if (prawaWartosc == 0) {
            throw new ArithmeticException("Dzielenie przez zero");
        }
        int wynik = lewaWartosc % prawaWartosc;

        return wynik;
    }

    @Override
    public String getSymbol() {
        return "%";
    }
}
