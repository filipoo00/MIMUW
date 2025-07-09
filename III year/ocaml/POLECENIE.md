# Zadanie zaliczeniowe o wyrażeniach regularnych

W niniejszym zadaniu pomijamy ważne praktyczne aspekty wyrażeń regularnych - grupy, zakotwiczenia, predefiniowane zbiory znaków itp. W dodatku będziemy tych wyrażeń używać do rozpoznawania słów, a nie do wyszukiwania podsłów.

Zadanie polega na zaimplementowaniu prostej biblioteki wyrażeń regularnych, funktora do testowania tychże oraz kilku modułów dostosowujących różne biblioteki wyrażeń regularnych do wymagań funktora testującego. 

Do budowania projektu należy używać narzędzia `dune`, umieszczając `(lang dune 3.10)` w pliku `dune-project`.


## 1. Prosta biblioteka wyrażeń regularnych

W katalogu [SimpleRegexp](SimpleRegexp/) zaimplementuj prostą bibliotekę (library) z operacjami na regexach reprezentowanych przez wariantowy typ danych. Definicja typu podana jest w pliku [simpleRegexpDef.ml](SimpleRegexp/simpleRegexpDef.ml), a specyfikacja wymaganych operacji w pliku [simpleRegexp.mli](SimpleRegexp/simpleRegexp.mli). Pliki [lexer.mll](SimpleRegexp/lexer.mll) i [parser.mly](SimpleRegexp/parser.mly) służą do stworzenia parsera wyrażeń regularnych jak opisano w pliku [README.md](SimpleRegexp/README.md).

**Składnia konkretna** wyrażeń regularnych, stosowana w tym zadaniu, używa dwóch specjalnych stałych ∅ i ε (do reprezentowania, odpowiednio, języka pustego i języka składającego się z jednego słowa: pustego), a poza tym postfiksowego operatora gwiazdki * do reprezentowania powtórzenia, pustego infiksowego operatora konkatenacji i infiksowego znaku | do reprezentowania alternatywy, przy czym * wiąże najsilniej, a | najsłabiej. Poza tym w wyrażeniach regularnych mogą występować litery ascii (duże i małe) oraz cyfry, odstępy nie mają znaczenia, do grupowania można używać nawiasów okrągłych. A zatem wyrażenie regularne, którego język to słowa nad alfabetem a, b, które zaczynają się na a i kończą na b to będzie "a(a|b)*b".


## 2. Funktor testujący

W katalogu [TestRe](TestRe/) zaimplementuj bibliotekę zawierającą funktor testujący poprawność biblioteki wyrażeń regularnych zgodnie z opisem w pliku [testRegexp.mli](TestRe/testRegexp.mli). Sygnatura modułu implementującego wyrażenia regularne jest w pliku [regexp.ml](TestRe/regexp.ml) i jak widać, trochę różni się od interfejsu zaimplementowanej wcześniej prostej biblioteki wyrażeń regularnych. A zatem, aby użyć funktora testującego do prostych wyrażeń regularnych należy jeszcze stworzyć niewielki moduł dopasowujący.

**Uwaga!** Testy o których piszemy w tym zadaniu nie używają funkcjonalności do testowania opartej na bibliotece Core prezentowanej na zajęciach. To ma być zwykły kod ocamlowy sprawdzający działania biblioteki wyrażeń regularnych na konkretnych (sprytnie dobranych ;) wyrażeniach regularnych, słowach i językach. 


## 3. Moduł dopasowujący

W katalogu [ReSimple](ReSimple/) umieść bibliotekę, dopasowującą Twoją prostą implementację wyrażeń regularnych do wymagań funktora testującego. Czyli musisz zaimplementować moduł ReSimple, którego specyfikacja znajduje się w pliku [reSimple.mli](ReSimple/reSimple.mli) (a tak naprawdę w pliku [TestRe/regexp.ml](TestRe/regexp.ml)). Każda z zaimplementowanych funkcji w tym module powinna mieć 1-2 linijki, gdyż tak naprawdę implementacja wszystkich elementów jest już zrobiona w [SimpleRegexp](SimpleRegexp/).


## 4. Moduł dopasowujący dla jednej z istniejących bibliotek (albo dwa takie :)

W katalogu [ReRe](ReRe/), [RePcre](RePcre/) lub [ReKit](ReKit/) umieść bibliotekę dopasowującą odpowiednią zewnętrzną bibliotekę [re](https://ocaml.org/p/re), [pcre](https://ocaml.org/p/pcre) lub [ocamlregextkit](https://ocaml.org/p/ocamlregextkit) - wszystkie dostępne poprzez `opam` - do wymagań funktora testującego. Stwórz odpowiednie pliki `.ml` i `.mli`, przy czym plik `.mli` powinien być identyczny z [ReSimple/reSimple.mli](ReSimple/reSimple.mli). 

Oczywiście implementacja każdej z funkcji wymaganych przez funktor powinna być krótka, używająca typów i funkcji dostarczonych przez daną bibliotekę zewnętrzną. 

Jeśli chodzi o parsowanie (funkcja `re`), to należy zwrócić uwagę, że dana biblioteka może używać ciut innej składni wyrażeń regularnych, np. inny może być symbol na sumę |, znaczące odstępy itp. Najprościej podmienić napis reprezentujący wyrażenie regularne przed przekazaniem go funkcji parsującej z biblioteki zewnętrznej. W przypadkach prostych zamian znaków można rozważyć użycie `String.map`, w przypadku bardziej skomplikowanych może być użyteczny moduł `Bytes` albo `Buffer`. Kwestię symboli ∅ i ε można zignorować, bo wyjątkowo rzadko występują one w prawdziwych wyrażeniach regularnych, w szczególności zapewne nie będzie ich w wyrażeniach pojawiających się w funktorze testującym. 

Przy implementacji funkcji `matches` należy upewnić się, że wybraliśmy odpowiednią funkcję z biblioteki podstawowej i/lub tak zmodyfikowaliśmy napis przekształcany w wyrażenie regularne w funkcji `re`, by dopasowany mógł być  wyłącznie **cały** podany ciąg znaków, a nie podsłowo. 

Funkcja `debug` może być kłopotliwa z tego względu, że niektóre z bibliotek zewnętrznych w ogóle nie dają możliwości wypisania już stworzonego wyrażenia regularnego. Należy wtedy typ `t` zdefiniować jako typ par zawierających oryginalny napis (z którego "powstało" wyrażenie regularne) oraz "prawdziwe" wyrażenie regularne zgodne z używaną biblioteką i wypisywać ten pierwszy element.

Mile widziana implementacja modułu dopasowującego dla więcej niż jednej biblioteki zewnętrznej.


## 5. Program główny

W katalogu [Main](Main/) zaimplementuj jednoplikowy program wykonywalny, w którym zaaplikujesz funktor testujący do poszczególnych modułów dostosowawczych i wywołasz funkcję `test` z każdego z nich, wypisując efekty końcowe.
Postaraj się, aby nie pisać wielokrotnie (prawie) takiego samego kodu.

Oczywiście (stosownie uszczuploną) wersję programu głównego należy napisać **przed** zakończeniem implementacji modułów opisanych powyżej, aby móc przyrostowo sprawdzać działanie poszczególnych implementowanych elementów.


## Wymagania

Program należy oddać na moodle w postaci archiwum zawierającego wszystkie pliki potrzebne do zbudowania projektu i uruchomienia pliku wykonywalnego.

**Termin oddania: niedziela 26.01.2025**

Prowadzący laboratorium może wymagać osobistego lub zdalnego spotkania w celu właściwego ocenienia projektu. 
