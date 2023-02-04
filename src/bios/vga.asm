; Prints a smile using the VGA buffer and the BIOS
real_mode_smile:

    mov ax, 0 ; initalize VGA in text mode
    int 0x10 

    mov ah, 0x0e    ; function number = 0Eh : Display Character
    mov al, ':'     ; AL = code of character to display
    int 0x10
    
    mov ah, 0x0e    
    mov al, ')'     
    int 0x10

ret