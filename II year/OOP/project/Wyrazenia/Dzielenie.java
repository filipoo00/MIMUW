package Wyrazenia;

import java.util.List;

public class Dzielenie extends Operator {
    public Dzielenie(Wyrazenie lewe, Wyrazenie prawe) {
        super(lewe, prawe);
    }

    public static Dzielenie dziel(Wyrazenie lewe, Wyrazenie prawe) {
        return new Dzielenie(lewe, prawe);
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
        int wynik = lewaWartosc/prawaWartosc;

        return wynik;
    }

    @Override
    public String getSymbol() {
        return "/";
    }
}