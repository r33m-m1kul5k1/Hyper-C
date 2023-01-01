%define MUTLIBOOT_MAGIC 0xE85250D6
%define I386 0

extern _start
section .multiboot

multiboot2_header_start:

    .magic dd MUTLIBOOT_MAGIC 
    .architecture dd I386 
    .length dd (multiboot2_header_end - multiboot2_header_start)
    .checksum dd -(MUTLIBOOT_MAGIC + I386 + (multiboot2_header_end - multiboot2_header_start))
    
        entry_tag_start:
            .type dw 3 
            .flags dw 0 
            .size dd entry_tag_end - entry_tag_start
            .entry_addr dd _start
        entry_tag_end:
    
        ; terminating tag
        dd 0
        dd 0
        dw 8
multiboot2_header_end: