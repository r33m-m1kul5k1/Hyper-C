
;----------------------------------------------------
; Calls the real mode callback function
; rdi - the callback function pointer
[bits 64]
real_mode_callback:
    enter 0, 0
    ; the stack pointer must be accessable from real mode
    mov rsp, LOWER_MEMORY_STACK_TOP
    
    sub rdi, real_mode_start
    add rdi, REAL_MODE_BASE_ADDRESS

    ; far call
    mov rax, REAL_MODE_RELOCATION(call_real_mode_function)
    call rax

    leave
    ret
;----------------------------------------------------

; copy real mode code to the lower memory
real_mode_start:
%include "src/bios/switch_modes.asm"
%include "src/bios/ata.asm"
%include "src/bios/vga.asm"
%include "src/boot/paging.asm"
real_mode_end: