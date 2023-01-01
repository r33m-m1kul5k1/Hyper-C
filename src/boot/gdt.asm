

section .data

gdt:
    dq 0
.code64: equ $ - gdt
    ; type c/d   |  present  | read/write| executable |  64-bit
    dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 43) | (1 << 53)

.code32: equ $ - gdt
    ; type c/d   |  present  | read/write| executable| 32-bit
    dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 43) | (1 << 54)

.code16: equ $ - gdt
    ; type c/d   |  present  | read/write| executable
    dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 43) 

.data32: equ $ - gdt
    ; type c/d   | present   | read write| 32-bit    
    dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 54)

.data: equ $ - gdt
    ; type c/d   | present   | read write  
    dq (1 << 44) | (1 << 47) | (1 << 41)

.pointer:
    dw .pointer - gdt - 1 ; limit (the size of our gdt) 
    dq gdt

ivt_pointer:
    dw 0x3ff
    dd 0x0 ; the IVT location is 0x0000