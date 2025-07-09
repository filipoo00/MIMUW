package Srodowisko;

import Debugger.Debugger;
import Instrukcje.DeklaracjaProcedury;
import Wyrazenia.Zmienna;

import java.util.ArrayList;
import java.util.List;

public class Srodowisko {

    private Debugger debugger;

    private List<List<Zmienna>> listaListZmiennych;

    private List<List<DeklaracjaProcedury>> listaListaProcedur;

    public Srodowisko() {
        listaListZmiennych = new ArrayList<>();
        listaListaProcedur = new ArrayList<>();
    }

    public void ustawDebugger(int obecnyKrok, int ileKrokowWykonac, int poziomZagniezdzenia) {
        debugger = new Debugger(obecnyKrok, ileKrokowWykonac, poziomZagniezdzenia);
    }

    public Debugger getDebugger() {
        return debugger;
    }

    public List<List<Zmienna>> dajListeListZmiennych() {
        return listaListZmiennych;
    }
    public void dodajListeZmiennych(List<Zmienna> zmienne) {
        listaListZmiennych.add(zmienne);
    }

    public List<Zmienna> dajOstatniaListe() {
        int size = listaListZmiennych.size();
        if (size == 0) {
            return new ArrayList<>();
        }
        return listaListZmiennych.get(size-1);
    }

    public void usunOstatniaListeZmiennych() {
        listaListZmiennych.remove(listaListZmiennych.size()-1);
    }

    public int dajRozmiarListy() {
        return listaListZmiennych.size();
    }

    public List<Zmienna> dajListePodIndeksem(int i) {
        return listaListZmiennych.get(i);
    }


    public void dodajListeProcedur(List<DeklaracjaProcedury> deklaracjaProcedury) {
        listaListaProcedur.add(deklaracjaProcedury);
    }

    public void usunOstatniaListeProcedur() {
        listaListaProcedur.remove(listaListaProcedur.size()-1);
    }

    public DeklaracjaProcedury dajDeklaracjeProcedury(String nazwaProcedury) {
        for (int i = listaListaProcedur.size()-1; i>=0; i--) {
            for (DeklaracjaProcedury deklaracjaProcedury : listaListaProcedur.get(i)) {
                if (deklaracjaProcedury.dajNazwaProcedury().equals(nazwaProcedury)) {
                    return deklaracjaProcedury;
                }
            }
        }
        return null;
    }

    public boolean czyJestDeklaracjaODanejNazwie(List<DeklaracjaProcedury> dp, String nazwa) {
        for (DeklaracjaProcedury deklaracjaProcedury : dp) {
            if (deklaracjaProcedury.dajNazwaProcedury().equals(nazwa)) {
                return true;
            }
        }
        return false;
    }

    public List<DeklaracjaProcedury> dajWidoczneDeklaracje() {
        List<DeklaracjaProcedury> listaDeklaracjiProcedur = new ArrayList<>();
        for (int i = listaListaProcedur.size()-1; i>=0; i--) {
            for (DeklaracjaProcedury deklaracjaProcedury : listaListaProcedur.get(i)) {
                if (!czyJestDeklaracjaODanejNazwie(listaDeklaracjiProcedur, deklaracjaProcedury.dajNazwaProcedury())) {
                    listaDeklaracjiProcedur.add(deklaracjaProcedury);
                }
            }
        }
        return listaDeklaracjiProcedur;
    }



}
