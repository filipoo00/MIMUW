package Wyrazenia;

import java.util.List;

public abstract class Wyrazenie {

    public abstract int oblicz(List<Zmienna> zmienne);

    public abstract String toString();

    public abstract String getNazwa();

    public abstract void setWartosc(int wartosc);
}