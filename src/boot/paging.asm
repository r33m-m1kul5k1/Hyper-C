
; Entry defines
%define PAGE_PRESENT (1 << 0)
%define PAGE_WRITE (1 << 1)
%define PAGE_SIZE (1 << 7)

; Memory defines
; https://wiki.osdev.org/Memory_Map_(x86) - free space (0x500 - 0x7FFFF) or above 0x100000
%define MEMORY_SIZE 2
%define FREE_SPACE_OFFSET 0x1_0000

; the paging takes 0x1_000 * (MEMORY_SIZE + 2)
%define PML4_ENTRY (FREE_SPACE_OFFSET + 0x1000 | (PAGE_WRITE | PAGE_PRESENT)) ; <offset><flags>
%define LARGE_PAGE_SIZE 0x200_000

; stores a qword in little endian order
; %1 - address
; %2 - higher dword
; %3 - lower dword
%macro store_qword_little 3
    mov ebx, %1
    mov dword [ebx], %3
    mov dword [ebx + 4], %2
%endmacro

global setup_paging

section .text
setup_paging:

    ; initialize PML4
    store_qword_little FREE_SPACE_OFFSET, 0x0, PML4_ENTRY

    mov ecx, MEMORY_SIZE ; each loop allocates 1G
    ; initialize pdp table
    lea ebx, [FREE_SPACE_OFFSET + 0x1000] ; pdp pointer
    lea eax, [FREE_SPACE_OFFSET + 0x2000] ; pd pointer
        
setup_pdp_entries:

        mov edx, eax
        or edx, (PAGE_WRITE | PAGE_PRESENT)
        
        ; save pd entry to memory
        mov dword [ebx], edx
        mov dword [ebx + 4], 0x0

        ; advance entry
        add eax, 0x1000
        add ebx, 8

        loop setup_pdp_entries
        
    mov ecx, MEMORY_SIZE
    shl ecx, 9
    mov eax, 0 
    lea ebx, [FREE_SPACE_OFFSET + 0x2000] ; first pd pointer

    ; note that this loop will overflow to other pd tables
setup_pd_entries:
        
        mov edx, eax
        or edx, (PAGE_SIZE | PAGE_WRITE | PAGE_PRESENT) 

        ; save page table entry
        mov dword [ebx], edx
        mov dword [ebx + 4], 0x0

        add eax, LARGE_PAGE_SIZE
        add ebx, 8
        loop setup_pd_entries

setup_paging_end:
ret
