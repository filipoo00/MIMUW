package Instrukcje;
import Debugger.Debugger;
import Wyrazenia.*;
import Srodowisko.*;

import java.util.ArrayList;
import java.util.List;

import static java.lang.System.exit;

public class Warunkowa extends Instrukcja {

    private Wyrazenie leweWyrazenie;
    private Wyrazenie praweWyrazenie;
    private String operator;
    private List<Instrukcja> prawdaInstrukcje;
    private List<Instrukcja> falszInstrukcje;


    public Warunkowa(Wyrazenie leweWyrazenie, Wyrazenie praweWyrazenie, String operator) {
        this.leweWyrazenie = leweWyrazenie;
        this.praweWyrazenie = praweWyrazenie;
        this.operator = operator;
        prawdaInstrukcje = new ArrayList<>();
        falszInstrukcje = new ArrayList<>();
    }

    public void dodajPrawda(Instrukcja i) {
        prawdaInstrukcje.add(i);
    }

    public void dodajFalsz(Instrukcja i) {
        falszInstrukcje.add(i);
    }


    public void wykonajInstrukcjePF(Debugger debugger, Srodowisko srodowisko, boolean prawda) {
        List<Instrukcja> ins;
        if (prawda) {
            ins = prawdaInstrukcje;
        }
        else {
            ins = falszInstrukcje;
        }
        for (Instrukcja instrukcja : ins) {
            debugger.debugger(instrukcja, srodowisko);
            instrukcja.wykonajInstrukcje(debugger, srodowisko);
        }
    }
    @Override
    public void wykonajInstrukcje(Debugger debugger, Srodowisko srodowisko) {
        try {
            int lewaWartosc = leweWyrazenie.oblicz(srodowisko.dajOstatniaListe());
            int prawaWartosc = praweWyrazenie.oblicz(srodowisko.dajOstatniaListe());
            boolean warunek;
            switch (operator) {
                case "=":
                    warunek = lewaWartosc == prawaWartosc;
                    break;
                case "<>":
                    warunek = lewaWartosc != prawaWartosc;
                    break;
                case "<":
                    warunek = lewaWartosc < prawaWartosc;
                    break;
                case ">":
                    warunek = lewaWartosc > prawaWartosc;
                    break;
                case "<=":
                    warunek = lewaWartosc <= prawaWartosc;
                    break;
                case ">=":
                    warunek = lewaWartosc >= prawaWartosc;
                    break;
                default:
                    throw new IllegalArgumentException("Nieprawidlowy operator: " + operator);
            }
            if (warunek) {
                wykonajInstrukcjePF(debugger, srodowisko, true);
            }
            else {
                wykonajInstrukcjePF(debugger, srodowisko, false);
            }
        } catch (ArithmeticException | IllegalArgumentException | NullPointerException e) {
            System.out.println("Wystapil blad: " + e.getMessage());
            System.out.println("Warunkowa");
            drukujZmienne(srodowisko.dajOstatniaListe());
            exit(-1);
        }
    }

    public void wypiszInstrukcje() {
        System.out.println("Warunkowa: " + leweWyrazenie.toString() + " " + operator + " " + praweWyrazenie.toString());
    }

    public void drukujZmienne(List<Zmienna> zmienne) {
        for (Zmienna z : zmienne) {
            System.out.println("Nazwa " + z.getNazwa() + "| Wartosc " + z.oblicz(zmienne));
        }
    }
}