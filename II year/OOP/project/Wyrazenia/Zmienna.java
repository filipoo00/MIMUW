package Wyrazenia;


import java.util.List;


public class Zmienna extends Wyrazenie {

    private String nazwa;
    private int wartosc;

    public Zmienna(String nazwa) {
        try {
            if (nazwa == null || nazwa.length() != 1 || !Character.isLowerCase(nazwa.charAt(0))) {
                throw new IllegalArgumentException("Nazwa zmiennej musi byc jednoliterowa od 'a' do 'z'.");
            }
            this.nazwa = nazwa;
        } catch (IllegalArgumentException e) {
            System.out.println("Blad wykonania: " + e.getMessage());

        }
    }

    public static Zmienna named(String nazwa) {
        return new Zmienna(nazwa);
    }

    public void ustawWartoscZmiennej(List<Zmienna> zmienne) {
        int stop = 0;
        for (Zmienna zmienna : zmienne) {
            if (zmienna.getNazwa().equals(nazwa)) {
                this.wartosc = zmienna.getWartosc();
                stop = 1;
                break;
            }
        }
        if (stop == 0)
            nazwa = null;
    }

    @Override
    public int oblicz(List<Zmienna> zmienne) {
        ustawWartoscZmiennej(zmienne);
        if (nazwa == null) {
            throw new NullPointerException("Podana nieprawidlowa nazwe.");
        }
        return wartosc;
    }

    public void setWartosc(int wartosc) {
        this.wartosc = wartosc;
    }

    public String getNazwa() {
        return nazwa;
    }

    @Override
    public String toString() {
        return nazwa;
    }

    public int getWartosc() {
        return wartosc;
    }
}
