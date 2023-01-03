
; Flags
%define PAGE_PRESENT (1 << 0)
%define PAGE_WRITE (1 << 1)
%define PAGE_SIZE (1 << 7)

%define PAGE_LENGTH 0x1_000
; Memory defines
; https://wiki.osdev.org/Memory_Map_(x86) - free space (0x500 - 0x7FFFF) or above 0x100000
%define MEMORY_SIZE 2

; the paging takes 0x1_000 * (MEMORY_SIZE + 2)
%define FREE_SPACE_OFFSET 0x100_000
%define PML4_ENTRY ((FREE_SPACE_OFFSET + 0x1000) << 12) | PAGE_WRITE | PAGE_PRESENT 
%define LARGE_PAGE_SIZE 0x2_00000

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
    shl esi, 12
    or esi, edx
    store_qword_little ebx, 0x0, esi

    add edi, eax
    add ebx, 8
    loop setup_entry
popa
ret 
; Set up a four level page tables
; ebx - pml4 start
setup_pml4_map:

    ; setup pml4 entry
    lea edi, [ebx + PAGE_LENGTH] 
    mov ebx, ebx
    mov edx, (PAGE_WRITE | PAGE_PRESENT)
    mov ecx, 1
    mov eax, PAGE_LENGTH
    call _create_tables


    ; setup pdp entries
    lea edi, [ebx + (2 * PAGE_LENGTH)] 
    mov ebx, [ebx + PAGE_LENGTH]
    mov edx, (PAGE_WRITE | PAGE_PRESENT)
    mov ecx, MEMORY_SIZE ; must be lower then 513
    mov eax, PAGE_LENGTH
    call _create_tables

    ; setup pd entries 
    lea edi, 0x0 ; maps physical memory
    mov ebx, [ebx + (2 * PAGE_LENGTH)]
    mov edx, (PAGE_SIZE | PAGE_WRITE | PAGE_PRESENT)
    mov ecx, (MEMORY_SIZE << 9) ; for each pdp entry we need 512 pd entries
    mov eax, LARGE_PAGE_SIZE
    call _create_tables
    

ret 