%define MUTLIBOOT_MAGIC 0xE85250D6
%define I386 0
%define HEADER_ADDRESS 0x10_0000

extern _start


multiboot2_header_start:

    .magic dd MUTLIBOOT_MAGIC 
    .architecture dd I386 
    .length dd (multiboot2_header_end - multiboot2_header_start)
    .checksum dd -(MUTLIBOOT_MAGIC + I386 + (multiboot2_header_end - multiboot2_header_start))

        address_tag_start:
            .type dw 2
            .flags dw 0
            .size dd address_tag_end - address_tag_start
            .header_addr dd HEADER_ADDRESS
            .load_addr dd -1    ; load .text after header
            .load_end_addr dd 0 ; text segment and data segment occupy the whole OS image
            .bss_end_addr dd 0  ; no bss segment
        address_tag_end:
    
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