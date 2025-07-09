open Regexp

module Test(Re : REGEXP) : sig

    (** {2 Elementy pomocnicze} *)
    
    (** Język to zbiór słów. Użyj modułu [Set] lub podobnego. *)
    type lang

    (** Tworzenie losowego zbioru słów z podanych liter. Do losowania należy 
        używać modułu [Random], parametr liczbowy powinien jakoś określać rozmiar 
        zbioru (im większa liczba tym większy zbiór). Może to być po prostu 
        liczba elementów zbioru, ale niekoniecznie. *)
    val make_lang : string -> int -> lang 


    (** Wybieranie z języka tych słów, które są akceptowane przez dane 
        wyrażenie regularne. *)
    val select_accepted : Re.t -> lang -> lang


    (** Porównanie dwóch wyrażeń regularnych z użyciem podanego języka.
        Procedura powinna wybierać z języka słowa akceptowane przez każde 
        z wyrażeń regularnych za pomocą [select_accepted], a
        następnie porównywać uzyskane zbiory za pomocą [eq] z modułu [Set].
        Wynikiem powinna być liczba błędów (dowolnie liczona, byle dodatnia 
        jeśli wynik jest nieprawidłowy) oraz czas łącznego wykonania obu wywołań
        [select_accepted]. Do liczenia czasu można używać [Sys.time]. 
        Ostatni parametr boolowski może służyć zwiększeniu poziomu wypisywania 
        szczegółów działania (debug on/off). *)
    val test_two : Re.t -> Re.t -> lang -> bool -> int * float

    
    (** {2 Głowna funkcja testująca} *)

    (** Najważniejsza funkcja w funktorze testującym. Uruchamia wszystkie testy 
        (patrz niżej), w wyniku daje łączną liczbę błędów i czas działania, 
        w miarę możliwości ograniczony do wywołań funkcji na wyrażeniach 
        regularnych (bez np. konstrukcji zbioru). *)
    val test : unit -> int * float 


    (** {2 Jakie testy należy przeprowadzić?} *)

    (** Należy przeprowadzić cztery grupy testów:
    
      1. Złośliwe testy na pojedynczych napisach, np.
      - Czy ba*b akceptuje baabaabaaab i baaaaaaab ?
      - Czy a*b akceptuje aaaaaaa i aaaaab ? 
        (tu można użyć naprawdę długich napisów, np. takich na 1000000 znaków)
    
      2. Porównanie r*r i rr* za pomocą [test_two]:
      - a*a i aa* na języku złożonym z liter a b 
      - (a|b)*(a|b) i (a|b)(a|b)* na języku złożonym z a b c

      3. Porównanie r1|r2 i r2|r1 za pomocą [test_two]:
      - a(a|b)*|(a|b)*b oraz (a|b)*b|a(a|b)* na napisach złożonych z a b
      
      4. Jedna lub dwie własne nietrywialne propozycje.
    *)
end
