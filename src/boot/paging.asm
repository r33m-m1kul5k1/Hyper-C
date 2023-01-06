
; Entry
%define PAGE_PRESENT (1 << 0)
%define PAGE_WRITE (1 << 1)
%define PAGE_SIZE (1 << 7)

; Control Registers
%define LONG_MODE (1 << 8)
%define PAGING (1 << 31)
%define PAE (1 << 5)

; Memory defines
%define PAGE_LENGTH 0x1_000
%define MEMORY_SIZE 2
%define LARGE_PAGE_SIZE (1 << 30)
; after 20 Kib of .text section
%define PML4_ADDRESS 0x5000
%define PDP_ADDRESS PML4_ADDRESS + 0x1000

; MSRs
%define EFER_MSR 0xC0000080

; stores a qword in little endian order
; %1 - address
; %2 - higher dword
; %3 - lower dword
%macro store_qword_little 3
    mov ebx, %1
    mov dword [ebx], %3
    mov dword [ebx + 4], %2
%endmacro

global setup_pml4_map
global enabling_paging_mode

section .text
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

; Setup a four level page tables
; ebx - pml4 start
setup_pml4_map:

    ; setup pml4 entry
    mov edi, PDP_ADDRESS
    mov ebx, PML4_ADDRESS
    mov edx, (PAGE_WRITE | PAGE_PRESENT)
    mov ecx, 1
    mov eax, PAGE_LENGTH
    call _create_tables

    ; setup pdp entries
    mov edi, 0x0 ; maps physical memory
    mov ebx, PDP_ADDRESS
    mov edx, (PAGE_SIZE | PAGE_WRITE | PAGE_PRESENT)
    mov ecx, MEMORY_SIZE ; must be lower then 513
    mov eax, LARGE_PAGE_SIZE
    call _create_tables

ret 

enabling_paging_mode:
    ; disable previous paging
    mov eax, cr0
    and eax, ~PAGING
    mov cr0, eax
    
    ; enable PAE
    mov eax, cr4
    or eax, PAE 
    mov cr4, eax

    ; initialize PML4 pointer
    mov eax, PML4_ADDRESS
    mov cr3, eax

    mov ecx, EFER_MSR          
    rdmsr
    or eax, LONG_MODE               
    wrmsr
    
    mov eax, cr0
    or eax, PAGING
    mov cr0, eax
ret