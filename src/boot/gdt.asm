; https://wiki.osdev.org/Global_Descriptor_Table

%define GDT_SIZE gdt_end - gdt_start - 1

gdt_start:
    dq 0
IA32E_CODE_SEGMENT: equ $ - gdt_start
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10011010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b10101111     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31
    
IA32_CODE_SEGMENT: equ $ - gdt_start
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10011010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b11001111     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31

REAL_MODE_CODE_SEGMENT: equ $ - gdt_start
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10011010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b10000000     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31

IA32E_DATA_SEGMENT: equ $ - gdt_start
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10010010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b10001111     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31

IA32_DATA_SEGMENT: equ $ - gdt_start
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10010010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b11001111     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31

REAL_MODE_DATA_SEGMENT: equ $ - gdt_start
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10010010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b10000000     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31
gdt_end:



IA32_gdtr: 
    dw GDT_SIZE
    dd gdt_start

IA32e_gdtr: 
    dw GDT_SIZE
    dq gdt_start


