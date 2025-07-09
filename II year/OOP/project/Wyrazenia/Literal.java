package Wyrazenia;

import java.util.List;

public class Literal extends Wyrazenie {
    private int wartosc;

    public Literal(int wartosc) {
        this.wartosc = wartosc;
    }

    public static Literal of(int wartosc) {
        return new Literal(wartosc);
    }

    public int oblicz(List<Zmienna> zmienne) {
        return wartosc;
    }

    @Override
    public String toString() {
        return "" + wartosc;
    }

    @Override
    public String getNazwa() {
        return null;
    }
    @Override
    public void setWartosc(int wartosc) {
        this.wartosc = wartosc;
    }
}