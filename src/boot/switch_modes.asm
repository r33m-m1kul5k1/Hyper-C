%define PROTECTION_ENABLE (1 << 0)

; long_mode:
;     ; https://forum.nasm.us/index.php?topic=1474.0 
;     push gdt.IA32_code_segment
;     push compatibility_mode
;     retfq



; [bits 32]
; compatibility_mode:

;     mov eax, gdt.IA32_data_segment
;     mov ss, eax
;     mov ds, eax
;     mov es, eax
;     mov fs, eax
; 	mov gs, eax

;     mov eax, cr0
;     and eax, ~(PAGING)
;     mov cr0, eax

;     ; note that I stay with the same page tables
;     mov eax, PDP_LOCATION
;     mov cr3, eax

;     mov ecx, EFER_MSR          
;     rdmsr
;     and eax, ~LONG_MODE               
;     wrmsr

;     mov eax, cr0
;     or eax, PAGING
;     mov cr0, eax

;     hlt

;     jmp gdt.IA32_code_segment:protected_mode

    

; [BITS 32]
; protected_mode:

;     cli
;     ; paging must be identity mapped
;     mov eax, cr0
;     and eax, ~(PAGING)
;     mov cr0, eax

;     hlt