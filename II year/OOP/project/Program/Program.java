package Program;

import Instrukcje.Blok;
import Srodowisko.Srodowisko;
import Wyrazenia.*;

import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class Program {

    private Blok blok_glowny;

    private Srodowisko srodowisko;

    public Program(Blok blok_glowny) {
        this.blok_glowny = blok_glowny;
    }

    public void wykonaj() {
        Scanner scanner = new Scanner(System.in);
        String polecenie;
        int stop = 0;
        while (stop == 0) {
            System.out.println("Wprowadz polecenie (0 - bez debuggera, 1 - z debuggerem):");
            polecenie = scanner.nextLine();
            if (polecenie.equals("0")) {
                wykonajBezDebugga();
                stop = 1;
            }
            else if (polecenie.equals("1")) {
                wykonajZDebuggerem();
                stop = 1;
            }
            else {
                System.out.println("Nieprawidlowe polecenie.");
            }
        }
    }

    public void wykonajZDebuggerem() {
        srodowisko = new Srodowisko();
        srodowisko.ustawDebugger(0, 0, 0);
        blok_glowny.wykonajInstrukcje(srodowisko.getDebugger(), srodowisko);
        System.out.println("Program sie zakonczyl");
    }

    public void wykonajBezDebugga() {
        srodowisko = new Srodowisko();
        srodowisko.ustawDebugger(0, -1, 0);
        blok_glowny.wykonajInstrukcje(srodowisko.getDebugger(), srodowisko);
    }
}