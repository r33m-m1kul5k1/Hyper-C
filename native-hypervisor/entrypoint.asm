
section .text
    global _start

    _start:
        mov rdi, 0x1
        mov rsi, hello
        mov rdx, helloLen
        mov rax, 0x1
        syscall

        xor rdi, rdi
        mov rax, 0x3c
        syscall

section .data
    hello db "test makefile", 0xa
    helloLen equ $-hello