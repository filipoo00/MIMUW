global _start

SYS_EXIT    equ 60
SYS_OPEN    equ 2
SYS_WRITE   equ 1
SYS_CLOSE   equ 3
SYS_READ    equ 0

mod         equ 0644o                    ; Uprawnienia -rw-r--r--
flag1       equ 0000                     ; O_RDONLY (do odczytu).
flag2       equ 0100 | 0001 | 0200       ; O_CREAT | O_WRONLY | O_EXCL (Tworzy plik. Jeśli już istnieje to błąd).

section .bss

buffer1     resb 1024                    ; Rezerwacja 1024 bajtów.
buffer2     resb 1538                    ; Rezerwacja 1538 bajtów (odpowiednio dużo: 3/2*buffer1 + 2).

section .text

_start:
    mov     rcx, [rsp]                   ; Ładuj do rcx liczbę argumentów.
    cmp     rcx, 3                       ; sprawdź czy liczba arg to 3,
    jne     exit_code1                   ; jeśli nie to kończymy program z kodem 1.

    mov     rax, SYS_OPEN                ; Otwieramy plik do odczytu.
    mov     rdi, [rsp + 16]              ; Adres pierwszego parametru (nazwa pierwszego pliku).
    mov     rsi, flag1                   ; Tryb otwarcia pliku,
    xor     rdx, rdx                     ; uprawnienia do pliku (domyślnie ustawiana na 0).
    syscall
    cmp     rax, -4096                   ; Sprawdź czy w rax jest wartość pomiędzy -1 a -4095,
    ja      exit_code1                   ; jeśli tak to kończymy program z kodem 1.

    mov     r8, rax                      ; Deskryptor pliku pierwszego zapisujemy w r8.

    mov     rax, SYS_OPEN                ; Tworzymy plik do zapisu.
    mov     rdi, [rsp + 24]              ; Adres drugiego parametru (nazwa drugiego pliku).
    mov     rsi, flag2                   ; Tryb otwarcia pliku,
    mov     rdx, mod                     ; uprawnienia do pliku.
    syscall
    cmp     rax, -4096
    ja      error_close_file1            ; Jeśli błąd wywołania to zamykamy plik pierwszy i kończymy z kodem 1.
   
    mov     r9, rax                      ; Deskryptor pliku drugiego zapisujemy w r9.

    xor     r12, r12 
    xor     r13, r13                     ; r13 - będzie służył jako check, czy koniec pętli read, czy nie.
    xor     r14, r14                     ; r14 - licznik wpisanych(tych, które chcemy) bajtów po buforze2.
read_loop:
    xor     r10, r10                     ; Zerujemy r10, licznik pętli po buforze1.
    mov     rax, SYS_READ                ; Odczytujemy znak z pliku.
    mov     rdi, r8                      ; Deskryptor pliku pierwszego,
    mov     rsi, buffer1                 ; bufor na znaki,
    mov     rdx, 1024                    ; liczba bajtów do odczytu.
    syscall
    cmp     rax, -4096
    ja      error_close_file12           ; Jeśli błąd to zamykamy plik 1, 2 i kończymy z kodem 1.
    
    cmp     rax, 0                       ; Sprawdzamy czy koniec pliku (czy nic nieprzeczytaliśmy),
    je      read_done                    ; jeśli tak to skocz do etykiety read_done.

    mov     rbx, rax                     ; Liczbę przeczytanych bajtów zapisujemy w rbx.
    jmp     buffer_loop

read_done:
    mov     r13, 1                       ; Koniec czytania, więc check ustawiamy na 1.
    mov     r14, 2                       ; Jeśli r12 będzie różne od 0, to znaczy, że,
    mov     qword [buffer2], r12         ; będziemy chcieli coś jeszcze zapisać w pliku (2 bajty).
    cmp     r12, 0                       ; Zatem r14 ustawiamy na 2. Pod adres buffer2 zapisujemy wartość r12.
    jne     write_to_file

back_read_done:
    mov     rax, SYS_CLOSE               ; Zamykamy plik pierwszy.
    mov     rdi, r8
    syscall
    cmp     rax, -4096                   ; Jeśli błąd to,
    ja      error_close_file2            ; próbujemy zamknąć drugi plik i kończymy program z kodem 1.

    mov     rax, SYS_CLOSE               ; Zamykamy plik drugi.
    mov     rdi, r9
    syscall
    cmp     rax, -4096                   ; Jeśli błąd to,
    ja      exit_code1                   ; kończymy program z kodem 1.

    xor     rdi, rdi
    jmp     exit                         ; Wszystko się udało, kończymy program z kodem 0.

buffer_loop:                             ; Pętla po buforze1.
    cmp     r10, rbx                     ; Jeśli koniec pętli,
    je      write_to_file                ; to zapisujemy do pliku drugiego to co jest w buffer2.

    cmp     byte [buffer1 + 1*r10], 's'  ; Porównujemy wartość bajtu pod danym adresem buffer1 z wartością s.
    je      write_sS_to_buffer

    cmp     byte [buffer1 + 1*r10], 'S'  ; Porównujemy wartość bajtu pod danym adresem buffer1 z wartością S.
    je      write_sS_to_buffer

    inc     r12                          ; Jeśli nie s i S to zwiększamy licznik naszego ciągu maks...

buffer_loop_back:
    inc     r10
    jmp     buffer_loop

write_sS_to_buffer:
    cmp     r12, 0                       ; Jeśli licznik ciągu maks... jest większy od 0,
    jne     write_not_sS_to_buffer       ; to przed zapisem s lub S chcemy zapisać do buffera nasz licznik r12.

back_write_sS_to_buffer:                 ; Wpisujemy pod dany adres buffer2 to co jest pod danym adresem buffer1,
    mov     rcx, [buffer1 + 1*r10]       ; czyli albo s albo S.
    mov     qword [buffer2 + 1*r14], rcx
    inc     r14                          ; Zwiększamy nasz licznik wpisanych bajtów.
    jmp     buffer_loop_back

write_not_sS_to_buffer:
    mov     [buffer2 + 1*r14], r12       ; Pod dany adres buffer2 zapisujemy nasz licznik r12.
    xor     r12,r12                      
    add     r14, 2                       ; Do licznika wpisanych bajtów (te które chcemy zliczać),
    jmp     back_write_sS_to_buffer      ; dodajemy 2, ponieważ tylko 2 pierwsze bajty z r12 nas obchodzą.

write_to_file:                             
    mov     rax, SYS_WRITE               ; Piszemy do pliku drugiego.
    mov     rdi, r9                      ; Deskryptor pliku drugiego.
    mov     rsi, buffer2         
    mov     rdx, r14                     ; Liczba bajtów, ile chcemy wpisać.
    syscall
    cmp     rax, -4096
    ja      error_close_file12

    xor     r14, r14                     ; Zerujemy licznik bajtów do wpisania z buffer2.
    cmp     r13, 0                       ; Sprawdzamy nasz check,
    je      read_loop                    ; jeśli check = 0, to znaczy, że wracamy do read_loop i dalej czytamy plik.

    jmp     back_read_done               ; Jeśli check = 1, to znaczy, że już skończyliśmy czytać plik.

error_close_file1:                       ; Zamyka pierwszy plik i kończy program z kodem 1.
    mov     rax, SYS_CLOSE
    mov     rdi, r8
    syscall

    jmp     exit_code1

error_close_file2:                       ; Zamyka drugi plik i kończy program z kodem 1.
    mov     rax, SYS_CLOSE
    mov     rdi, r9
    syscall

    jmp     exit_code1

error_close_file12:                      ; Zamyka pierwszy i drugi plik i kończy program z kodem 1.
    mov     rax, SYS_CLOSE
    mov     rdi, r8
    syscall

    mov     rax, SYS_CLOSE
    mov     rdi, r9
    syscall

    jmp     exit_code1

exit_code1:                              ; Kończy program z kodem 1.
    mov     rdi, 1

exit:
    mov      rax, SYS_EXIT
    syscall