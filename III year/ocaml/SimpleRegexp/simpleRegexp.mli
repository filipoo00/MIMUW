(** {2 Wyrażenia regularne nad abstrakcyjnym alfabetem}*)

(** Typ wyrażeń regularnych. W tej specyfikacji jest to typ abstrakcyjny, 
    ale powinien on być zaimplementowany jako typ z modułu {!SimpleRegexpDef}. 
    Alfabet jest na razie dowolnym typem ['c]. *)
type 'c reg

(** Upraszczanie wyrażeń regularnych używające prostych własności, 
    oczywiście bez zmiany języka akceptowanych słów.
    Początkowo może to być identyczność, potem można dodać takie uproszczenia jak
      - r | ∅ = r  
      - εr = r

    itp.
    *)
val simpl : 'c reg -> 'c reg

(** Czy ε należy do języka? *)
val nullable : 'c reg -> bool

(** Czy akceptowany język jest pusty? *)
val empty : 'c reg -> bool 

(** [der a r] to pochodna [r] względem [a], czyli wyrażenie regularne [r'], 
    które akceptuje takie słowa [w], że słowo [aw] jest akceptowane przez [r].
    Np. pochodna wyrażenia ab* po a to oczywiście b*, pochodna a* po a to a*, 
    a pochodna b po a to ∅ (wyrażenie nieakceptujące żadnego słowa). 
    Jaka jest pochodna a*bc po b ? A jaka a*ac po a ? A jaka (a*a|ab)c po a ? *)
val der : 'c -> 'c reg -> 'c reg

(** [ders v r] to pochodna [r] względem [v], czyli wyrażenie regularne [r'], 
    które akceptuje takie słowa [w], że słowo [vw] jest akceptowane przez [r].
    W implementacji należy użyć [der], niewątpliwie przyda się też [simpl]. *)
val ders : 'c list -> 'c reg -> 'c reg

(** Czy słowo jest akceptowane przez wyrażenie regularne?
    W implementacji należy użyć [ders] i [nullable]. *)
val accept : 'c reg -> 'c list -> bool


(** Prezentacja wyrażenia regularnego w postaci napisu. *)
val repr : ('c -> string) -> 'c reg -> string 



(** {2 Użyteczne funkcje dla [char reg]} *)

(** Wyrażenie regularne akceptujące język złożony z jednego jednoliterowego słowa. *)
val char : char -> char reg

(** Wyrażenie regularne akceptujące język złożony z jednego słowa. *)
val string : string -> char reg

(** Wyrażenie regularne akceptujace język złożony z jednoliterowych słów 
    dla wszystkich liter z podanego napisu. *)
val alts : string -> char reg

(** Specjalizacja accept dla [char reg] i [string]. *)
val accepts : char reg -> string -> bool

(** Zamiana [char reg] na napis. Proszę postarać się o niewstawianie niepotrzebnych nawiasów. *)
val to_string : char reg -> string

(** Zamiana napisu na wyrażenie regularne. Proszę zajrzeć do {!README.md}. *)
val parse : string -> char reg
