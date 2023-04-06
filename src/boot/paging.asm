; https://wiki.osdev.org/Page_Tables

[bits 32]
; Creates multiple paging tables
; Param:
; edi - physical address (or index to another table)
; ebx - table's base
; edx - flags
; ecx - number of entries to initiate
; eax - destination address jump 
_create_tables:
pusha
setup_entry:
    ; esi is a temp register
    mov esi, edi
    or esi, edx
    store_qword_little ebx, 0x0, esi

    add edi, eax
    add ebx, 8
    loop setup_entry
popa
ret

; Setup: one pml4, one pdpt, `MEMORY_SIZE` pds that maps to 2Mib memory chunks
; Total tables's size: 1 + 1 + MEMORY_SIZE
setup_pml4_map:

    ; setup pml4 entry
    mov edi, IA32e_PAGING_BASE + PAGE_LENGTH       ; PDPT
    mov ebx, IA32e_PAGING_BASE                     ; PML4
    mov edx, (PAGE_WRITE | PAGE_PRESENT)
    mov ecx, 1
    mov eax, PAGE_LENGTH
    call _create_tables

    ; setup pdpt entries
    mov edi, IA32e_PAGING_BASE + (2 * PAGE_LENGTH) ; PDT
    mov ebx, IA32e_PAGING_BASE + PAGE_LENGTH       ; PDPT
    mov edx, (PAGE_WRITE | PAGE_PRESENT)
    mov ecx, MEMORY_SIZE                    
    mov eax, PAGE_LENGTH
    call _create_tables
    
    ; setup pd entries
    mov edi, 0x0 ; maps physical memory
    mov ebx, IA32e_PAGING_BASE + (2 * PAGE_LENGTH) ; PDT
    mov edx, (PAGE_SIZE | PAGE_WRITE | PAGE_PRESENT)
    mov ecx, (MEMORY_SIZE << 9)
    mov eax, LARGE_PAGE_SIZE
    call _create_tables

ret