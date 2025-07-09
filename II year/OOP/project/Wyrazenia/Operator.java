package Wyrazenia;

public abstract class Operator extends Wyrazenie {

    public Wyrazenie lewe;
    public Wyrazenie prawe;
    public Operator(Wyrazenie lewe, Wyrazenie prawe) {
        this.lewe = lewe;
        this.prawe = prawe;
    }

    @Override
    public String toString() {
        String str = "";
        if (lewe == null || prawe == null) {
            throw new NullPointerException("Nie mozna wypisac nastepnej instrukcji bo jest blad.");
        }
        str += lewe.toString();
        str += getSymbol();
        str += prawe.toString();
        return str;
    }

    public abstract String getSymbol();

    @Override
    public String getNazwa() {
        return null;
    }

    @Override
    public void setWartosc(int wartosc) {

    }
}
