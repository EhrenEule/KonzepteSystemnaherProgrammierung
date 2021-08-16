//
// version
//
	.vers	4

//
// execution framework
//
__start:
	call	_main
	call	_exit
__stop:
	jmp	__stop

//
// Integer readInteger()
//
_readInteger:
	asf	0
	rdint
	popr
	rsf
	ret

//
// void writeInteger(Integer)
//
_writeInteger:
	asf	0
	pushl	-3
	wrint
	rsf
	ret

//
// Character readCharacter()
//
_readCharacter:
	asf	0
	rdchr
	popr
	rsf
	ret

//
// void writeCharacter(Character)
//
_writeCharacter:
	asf	0
	pushl	-3
	wrchr
	rsf
	ret

//
// Integer char2int(Character)
//
_char2int:
	asf	0
	pushl	-3
	popr
	rsf
	ret

//
// Character int2char(Integer)
//
_int2char:
	asf	0
	pushl	-3
	popr
	rsf
	ret

//
// void exit()
//
_exit:
	asf	0
	halt
	rsf
	ret

//
// void main()
//
_main:
	asf	2
	pushc	100
	popl	0
	pushl	0
	call	_fakulteat
	drop	1
	pushr
	call	_writeInteger
	drop	1
	pushc	10
	call	_writeCharacter
	drop	1
__0:
	rsf
	ret

//
// Integer fakulteat(Integer)
//
_fakulteat:
	asf	0
	pushl	-3
	pushc	1
	gt
	brf	__2
	pushl	-3
	pushl	-3
	pushc	1
	sub
	call	_fakulteat
	drop	1
	pushr
	mul
	popr
	jmp	__1
	jmp	__3
__2:
	pushc	1
	popr
	jmp	__1
__3:
__1:
	rsf
	ret
               
