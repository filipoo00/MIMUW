import Instrukcje.*;
import Program.Program;
import Wyrazenia.*;
import Builder.*;


import java.util.List;

public class Main {
    public static void main(String[] args) {

        Blok blok = new BlokBuilder()
                .deklaracjaZmiennej("x", Literal.of(101))
                .deklaracjaZmiennej("y", Literal.of(1))
                .deklaracjaProcedury(new ProceduraBuilder("out", List.of("a"))
                        .print(Dodawanie.dodaj(Zmienna.named("a"), Zmienna.named("x")))
                        .build())
                .przypisanie("x", Odejmowanie.odejmij(Zmienna.named("x"), Zmienna.named("y")))
                .wykonajProcedure("out", List.of(Zmienna.named("x")))
                .wykonajProcedure("out", List.of(Literal.of(100)))
                .blok(new BlokBuilder()
                        .deklaracjaZmiennej("x", Literal.of(10))
                        .wykonajProcedure("out", List.of(Literal.of(100)))
                        .build())
                .build();

        Program p = new Program(blok);
        p.wykonaj();
    }
}