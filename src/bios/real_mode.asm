
; copy real mode code to the lower memory
real_mode_start:


;----------------------------------------------------
; Calls the real mode callback function
; rdi - the callback function pointer
[bits 64]
real_mode_callback:
    
    sub rdi, real_mode_start
    add rdi, REAL_MODE_BASE_ADDRESS
    
    mov rax, REAL_MODE_RELOCATION(call_real_mode_function)
    call rax
    ret
;----------------------------------------------------

%include "src/boot/gdt.asm"
%include "src/bios/switch_modes.asm"
%include "src/bios/vga.asm"
%include "src/bios/ata.asm"
real_mode_end: