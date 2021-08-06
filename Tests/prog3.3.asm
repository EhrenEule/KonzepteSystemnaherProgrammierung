//
// prog1.asm -- an assembler example with global variables
//

//
// compute the gcd of two positive numbers
//
// global Integer x;
// global Integer y;
// x = readInteger();
// y = readInteger();
//   if (x > y) {
//     x = x - y;
//   } else {
//     y = y - x;
//   }
// writeInteger(x);
// writeCharacter('\n');

	// x = readInteger();
	rdint
	popg	0
	// y = readInteger();
	rdint
	popg	1

	// if ...
	pushg	0
	pushg	1
	gt
	brf	L3
	// x = x - y
	pushg	0
	pushg	1
	sub
	popg	0
	jmp	L4
L3:
	// y = y - x
	pushg	1
	pushg	0
	sub
	popg	1
L4:
	// writeInteger(x);
	pushg	0
	wrint
	// writeCharacter('\n');
	pushc	'\n'
	wrchr
	halt


