package Debugger;

import Instrukcje.DeklaracjaProcedury;
import Instrukcje.Instrukcja;
import Wyrazenia.Zmienna;
import Srodowisko.*;

import java.io.*;
import java.util.InputMismatchException;
import java.util.List;
import java.util.Scanner;

import static java.lang.System.exit;

public class Debugger {

    private int obecnyKrok;

    private int ileKrokowWykonac;

    private int poziomZagniezdzenia;

    private int zrzut = 0;

    private String sciezkaDoPliku;

    public Debugger(int obecnyKrok, int ileKrokowWykonac, int poziomZagniezdzenia) {
        this.obecnyKrok = obecnyKrok;
        this.ileKrokowWykonac = ileKrokowWykonac;
        this.poziomZagniezdzenia = poziomZagniezdzenia;
    }

    public void debugger(Instrukcja instrukcja, Srodowisko srodowisko) {
        if (obecnyKrok == ileKrokowWykonac) {
            if (ileKrokowWykonac > 0) {
                try {
                    instrukcja.wypiszInstrukcje();
                } catch (NullPointerException e) {
                    System.out.println(e.getMessage());
                }
            }
            debugKomenda();
            while (obecnyKrok == 0 && ileKrokowWykonac == 0) {
                if (zrzut == 1) {
                    zrzut(srodowisko);
                }
                if (poziomZagniezdzenia == -1) {
                    try {
                        instrukcja.wypiszInstrukcje();
                    } catch (NullPointerException e) {
                        System.out.println(e.getMessage());
                    }
                }
                else {
                    if (srodowisko.dajRozmiarListy() - 1 - poziomZagniezdzenia >= 0) {
                        List<Zmienna> z = srodowisko.dajListePodIndeksem(srodowisko.dajRozmiarListy() - 1 - poziomZagniezdzenia);
                        for (Zmienna zmienna : z) {
                            System.out.println("Nazwa " + zmienna.getNazwa() + "| Wartosc " + zmienna.oblicz(z));
                        }
                    } else {
                        System.out.println("Podana liczba jest wieksza niz poziom zagniezdzenia biezacej instrukcji programu.");
                    }
                }
                debugKomenda();
            }
        }
        obecnyKrok++;
    }

    public void debugKomenda() {
        Scanner scanner = new Scanner(System.in);
        String komenda;
        String sciezkaDoPliku = null;
        int stop = 0;
        while (stop == 0) {
            System.out.println("Wprowadz polecenie: ");
            komenda = scanner.nextLine();
            int liczba = 0;
            if (komenda.trim().startsWith("s")) {
                String liczbaTekst = komenda.substring(1).trim();
                try {
                    liczba = Integer.parseInt(liczbaTekst);
                    komenda = "s";
                } catch (NumberFormatException e) {
                    komenda = "x";
                }
            }
            if (komenda.trim().startsWith("d")) {
                String liczbaTekst = komenda.substring(1).trim();
                try {
                    liczba = Integer.parseInt(liczbaTekst);
                    komenda = "d";
                } catch (NumberFormatException e) {
                    komenda = "x";
                }
            }
            if (komenda.trim().startsWith("m")) {
                sciezkaDoPliku = komenda.substring(1).trim();
                File file = new File(sciezkaDoPliku);
                if (file.exists() && file.isFile() && file.canWrite()) {
                    komenda = "m";
                } else {
                    komenda = "x";
                }
            }
            try {
                switch (komenda) {
                    case "c":
                        ileKrokowWykonac = -1;
                        zrzut = 0;
                        stop = 1;
                        break;
                    case "s":
                        if (liczba >= 0) {
                            ileKrokowWykonac = liczba;
                            obecnyKrok = 0;
                            poziomZagniezdzenia = -1;
                            zrzut = 0;
                            stop = 1;
                        }
                        else {
                            System.out.println("Nieprawidlowe polecenie.");
                        }
                        break;
                    case "d":
                        if (liczba >= 0) {
                            obecnyKrok = 0;
                            ileKrokowWykonac = 0;
                            poziomZagniezdzenia = liczba;
                            zrzut = 0;
                            stop = 1;
                        }
                        else {
                            System.out.println("Nieprawidlowe polecenie.");
                        }
                        break;
                    case "e":
                        exit(0);
                        break;
                    case "m":
                        obecnyKrok = 0;
                        ileKrokowWykonac = 0;
                        poziomZagniezdzenia = -1;
                        zrzut = 1;
                        this.sciezkaDoPliku = sciezkaDoPliku;
                        stop = 1;
                        break;
                    default:
                        throw new InputMismatchException("Nieprawdilowe polecenie.");
                }
            } catch (InputMismatchException e) {
                System.out.println(e.getMessage());
            }
        }
    }

    public void zrzut(Srodowisko srodowisko) {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(sciezkaDoPliku))) {
            for (DeklaracjaProcedury deklaracjaProcedury : srodowisko.dajWidoczneDeklaracje()) {
                writer.write(deklaracjaProcedury.toString() + deklaracjaProcedury.dajListaParametrow());
                writer.newLine();
            }

            for (Zmienna z : srodowisko.dajOstatniaListe()) {
                writer.write("Nazwa " + z.getNazwa() + "| Wartosc " + z.oblicz(srodowisko.dajOstatniaListe()));
                writer.newLine();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
