import Builder.*;
import Instrukcje.*;
import Program.Program;
import Wyrazenia.*;
import org.junit.jupiter.api.Test;


import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class TestyLinux {

    private final ByteArrayOutputStream outContent = new ByteArrayOutputStream();
    private final PrintStream originalOut = System.out;


    @Test
    public void testDodawanie() {
        Dodawanie d = new Dodawanie(Literal.of(5), Literal.of(10));
        assertEquals(15 ,d.oblicz(List.of()));
        assertEquals("5+10", d.toString());
    }

    @Test
    public void testOdejmowanie() {
        Odejmowanie o = new Odejmowanie(Literal.of(10), Literal.of(5));
        assertEquals(5 ,o.oblicz(List.of()));
        assertEquals("10-5", o.toString());
    }


    @Test
    public void testDzielenie() {
        Dzielenie dzielenie = new Dzielenie(Literal.of(10), Literal.of(5));
        assertEquals(2 ,dzielenie.oblicz(List.of()));
        assertEquals("10/5", dzielenie.toString());
    }

    @Test
    public void testLiteral() {
        Literal l = new Literal(5);
        assertEquals(5 ,l.oblicz(List.of()));
    }

    @Test
    public void testMnozenie() {
        Mnozenie mnozenie = new Mnozenie(Literal.of(10), Literal.of(5));
        assertEquals(50 ,mnozenie.oblicz(List.of()));
        assertEquals("10*5", mnozenie.toString());
    }

    @Test
    public void testModulo() {
        Modulo modulo = new Modulo(Literal.of(11), Literal.of(5));
        assertEquals(1 ,modulo.oblicz(List.of()));
        assertEquals("11%5", modulo.toString());
    }

    @Test
    public void testZmienna() {
        Zmienna x1 = new Zmienna("x");
        x1.setWartosc(5);
        assertEquals(5, x1.getWartosc());

        Zmienna x2= new Zmienna("x");
        assertEquals(5, x2.oblicz(List.of(x1)));
    }



    @Test
    public void testDeklaracjaZmiennej1(){
        System.setOut(new PrintStream(outContent));
        Blok blok  = new BlokBuilder()
                .deklaracjaZmiennej("a", Literal.of(5))
                .print(Zmienna.named("a"))
                .build();
        Program program = new Program(blok);
        program.wykonajBezDebugga();


        assertEquals("5\nNazwa a| Wartosc 5\n", outContent.toString());

        System.setOut(originalOut);
    }

    @Test
    public void testDeklaracjaZmiennej2(){
        System.setOut(new PrintStream(outContent));
        Blok blok  = new BlokBuilder()
                .deklaracjaZmiennej("a", Literal.of(1))
                .blok(new BlokBuilder()
                        .deklaracjaZmiennej("a", Literal.of(2))
                        .blok(new BlokBuilder()
                                .deklaracjaZmiennej("a", Literal.of(3))
                                .print(Zmienna.named("a"))
                                .build())
                        .print(Zmienna.named("a"))
                        .build())
                .print(Zmienna.named("a"))
                .build();
        Program program = new Program(blok);
        program.wykonajBezDebugga();


        assertEquals("3\n2\n1\nNazwa a| Wartosc 1\n", outContent.toString());

        System.setOut(originalOut);
    }


    @Test
    public void testDeklaracjaProcedury1(){
        System.setOut(new PrintStream(outContent));

        Blok blok  = new BlokBuilder()
                .deklaracjaProcedury(new ProceduraBuilder("test", List.of("a"))
                        .print(Zmienna.named("a"))
                        .build())
                .wykonajProcedure("test", List.of(Literal.of(100)))
                .build();

        Program program = new Program(blok);
        program.wykonajBezDebugga();


        assertEquals("100\n", outContent.toString());

        System.setOut(originalOut);
    }

    @Test
    public void testDeklaracjaProcedury2(){
        System.setOut(new PrintStream(outContent));

        Blok blok  = new BlokBuilder()
                .deklaracjaProcedury(new ProceduraBuilder("pierwszy", List.of())
                        .deklaracjaZmiennej("b", Literal.of(5))
                        .deklaracjaProcedury(new ProceduraBuilder("drugi", List.of())
                                .print(Zmienna.named("b"))
                                .build())
                        .wykonajProcedure("drugi", List.of())
                        .build())
                .wykonajProcedure("pierwszy", List.of())
                .build();

        Program program = new Program(blok);
        program.wykonajBezDebugga();


        assertEquals("5\n", outContent.toString());

        System.setOut(originalOut);
    }

    @Test
    public void testPetla() {
        System.setOut(new PrintStream(outContent));

        Blok blok = new BlokBuilder()
                .petla(new ForBuilder("i", Literal.of(2))
                        .print(Zmienna.named("i"))
                        .build())
                .build();

        Program program = new Program(blok);
        program.wykonajBezDebugga();

        assertEquals("0\n1\n", outContent.toString());

        System.setOut(originalOut);
    }


    @Test
    public void testPrzypisanie() {
        System.setOut(new PrintStream(outContent));

        Blok blok = new BlokBuilder()
                .deklaracjaZmiennej("a", Literal.of(5))
                .przypisanie("a", Literal.of(10))
                .print(Zmienna.named("a"))
                .build();

        Program program = new Program(blok);
        program.wykonajBezDebugga();

        assertEquals("10\nNazwa a| Wartosc 10\n", outContent.toString());

        System.setOut(originalOut);
    }

    @Test
    public void testWarunkowa1() {
        System.setOut(new PrintStream(outContent));

        Blok blok = new BlokBuilder()
                .warunkowa(new WarunkowaBuilder(Literal.of(1), Literal.of(0), ">")
                        .printP(Literal.of(1))
                        .printF(Literal.of(0))
                        .build())
                .build();

        Program program = new Program(blok);
        program.wykonajBezDebugga();

        assertEquals("1\n", outContent.toString());

        System.setOut(originalOut);
    }

    @Test
    public void testWarunkowa2() {
        System.setOut(new PrintStream(outContent));

        Blok blok = new BlokBuilder()
                .warunkowa(new WarunkowaBuilder(Literal.of(0), Literal.of(1), ">")
                        .printP(Literal.of(1))
                        .printF(Literal.of(0))
                        .build())
                .build();

        Program program = new Program(blok);
        program.wykonajBezDebugga();

        assertEquals("0\n", outContent.toString());

        System.setOut(originalOut);
    }

}
