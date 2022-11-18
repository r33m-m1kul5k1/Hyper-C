
global _start

; double word - 32
; word - 16
; byte - 8
[BITS 32]
section .text

multiboot2_header_start:
     
    dd 0xE85250D6 ; magic
    dd 0 ; architecture => 32-bit (protected) mode of i386
    dd (multiboot2_header_end - multiboot2_header_start) ; header_length
    dd -(0xE85250D6 + 0 + (multiboot2_header_end - multiboot2_header_start)) ; checksum
    
        multiboot2_tag_start:
        ; this doesn't work
        dw 3 ; tage type 
        dw 0 ; and it's flags 
        dd multiboot2_tag_end - multiboot2_tag_start ; size
        dd _start ; entry addresss
        multiboot2_tag_end:
    
        ; terminating tag
        dd 0
        dd 0
        dw 8
multiboot2_header_end:

    _start:
        mov ax, 42
        jmp $
