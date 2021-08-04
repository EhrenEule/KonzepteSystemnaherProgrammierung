# KonzepteSystemnaherProgrammierung

gcc -g -Wall -std=c99 -pedantic

Aufgabe 0: finished
Aufgabe 1: finished (Bis auf Stacküber- und unterlauf und SIGN_EXTENDED(i) noch verstehen)
Aufgabe 2: finished 
Aufgabe 3:
Aufgabe 4:
Aufgabe 5:
Aufgabe 6:
Aufgabe 7:
Aufgabe 8:


Debugger commands:

gcc program.c -g
Then use gdb:

$ gdb ./a.out
(gdb) run
<segfault happens here>
(gdb) backtrace
<offending code is shown here>

Datei ausführbar machen
chmod +x njvm