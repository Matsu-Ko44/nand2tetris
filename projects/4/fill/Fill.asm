// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed,
// the screen should be cleared.

(LOOP)
    // if (KBD == 0) goto UNPRESSED
    @KBD
    D=M
    @UNPRESSED
    D;JEQ

    // n = 0
    @n
    M=0
(BLACK)
    // if (n == 512) goto LOOP
    @n
    D=M
    @512
    D=D-A
    @LOOP
    D;JEQ
    // *(SCREEN + n) = 0
    @SCREEN
    D=A
    @n
    A=D+M
    M=-1
    // n = n + 1
    @n
    M=M+1
    // goto BLACK
    @BLACK
    0;JMP

(UNPRESSED)
    // n = 0
    @n
    M=0
(WHITE)
    // if (n == 512) goto LOOP
    @n
    D=M
    @512
    D=D-A
    @LOOP
    D;JEQ
    // *(SCREEN + n) = 255
    @SCREEN
    D=A
    @n
    A=D+M
    M=0
    // n = n + 1
    @n
    M=M+1
    // goto WHITE
    @WHITE
    0;JMP
