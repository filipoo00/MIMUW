### Tworzenie parsera dla wyrażeń regularnych

Pliki `lexer.mll` i `parser.mly` służą do łatwego wyprodukowania parsera wyrażeń regularnych.
Należy mieć zainstalowane binarki `ocamlyacc` i `ocamllex` (które przychodzą razem z kompilatorem ocamla) i uruchomić je odpowiednio na dwóch wspomnianych plikach, najlepiej poprzez umieszczenie następujących poleceń w pliku `dune`:
```
(ocamlyacc parser)

(ocamllex lexer)
```
Aby uruchomić parser na jakimś napisie z poziomu kodu w ocamlu, wystarczy użyć następującej linijki:
```
let parse s = Parser.regex Lexer.token (Lexing.from_string s)
```
Więcej szczegółów na temat narzędzi `ocamlyacc` i `ocamllex` można znaleźć w [podręczniku OCamla](https://ocaml.org/manual/lexyacc.html), (niewielki) kompletny przykład (a jakże! Kalkulator ;) ze wszystkimi liniami poleceń jest w [sekcji 6](https://ocaml.org/manual/lexyacc.html#s%3Alexyacc-example).
