# KonzepteSystemnaherProgrammierung

gcc -g -Wall -std=c99 -pedantic

gcc -g -Wall -std=c99 -pedantic -I../bigint/build/include -L../bigint/build/lib njvm.c -lbigint

Questions: Bei vergleichsoperatoren das Ergebniss als einfaches value auf den stack legen?
           Immer checken ob objRef oder number?

           not too sure about objRef *variable_memory ( if pointer is correct)

           Modulo mit BigInt Bib?

           Div Remainder beachten?

Current Bugs: Cant print static_data
                

Aufgabe 0: finished

Aufgabe 1: finished (Bis auf Stacküber- und unterlauf und SIGN_EXTENDED(i) noch verstehen)

Aufgabe 2: finished 

Aufgabe 3: finished 

Aufgabe 4: finished

Aufgabe 5: finished

Aufgabe 6: modulo still missing

Aufgabe 7:

Aufgabe 8:

--map gibt die immediate werte der sprung befehle aus

//Debugger commands:

gcc program.c -g
Then use gdb:

$ gdb ./a.out
(gdb) run
<segfault happens here>
(gdb) backtrace
<offending code is shown here>

//Datei ausführbar machen
chmod +x njvm