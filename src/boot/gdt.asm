; https://wiki.osdev.org/Global_Descriptor_Table


gdt:
    dq 0
.IA32e_code_segment: equ $ - gdt
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10011010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b00101111     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31
    
.IA32_code_segment: equ $ - gdt
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10011010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b01001111     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31

.real_mode_code_segment: equ $ - gdt
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10011010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b00000000     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31

.IA32e_data_segment: equ $ - gdt
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10010010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b00001111     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31

.IA32_data_segment: equ $ - gdt
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10010010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b01001111     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31

.real_mode_data_segment: equ $ - gdt
    dw 0xffff         ; limit 0-15
    dw 0x0000         ; base 0-15

    db 0x00           ; base 16-23
    db 0b10010010     ; P, DPL, S, E, DC, RW, A (access byte)
    db 0b00000000     ; G, DB, L, Reserved (limit 19-16 & flags)
    db 0x00           ; base 24-31

.pointer:
    dw .pointer - gdt - 1 ; limit (the size of our gdt) 
    dq gdt



