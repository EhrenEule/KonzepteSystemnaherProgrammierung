# KonzepteSystemnaherProgrammierung

gcc -g -Wall -std=c99 -pedantic

Current TODO: Being able to print the static data and then completing the debug console menu

Aufgabe 0: finished

Aufgabe 1: finished (Bis auf Stacküber- und unterlauf und SIGN_EXTENDED(i) noch verstehen)

Aufgabe 2: finished 

Aufgabe 3: finished 

Aufgabe 4:

Aufgabe 5:

Aufgabe 6:

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

//Debugger Spezifikation
