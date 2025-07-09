package Instrukcje;
import Debugger.Debugger;
import Srodowisko.Srodowisko;


public abstract class Instrukcja {

    public abstract void wykonajInstrukcje(Debugger debugger, Srodowisko srodowisko);

    public abstract void wypiszInstrukcje();

}