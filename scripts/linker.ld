/*
ref https://sourceware.org/binutils/docs-2.30/ld/Simple-Example.html
https://wiki.osdev.org/Memory_Map_(x86) - free space (0x500 - 0x7DFF and 0x7E00 - 0x7FFF) or above 0x100000
*/

HEADER_START = 0x100000;


SECTIONS
{
  /* location counter offset */
  . = HEADER_START;

  /* take all text, data and bss sections */
  .multiboot :  { *(.multiboot) }
  .text :       { *(.text) }
  .data :       { *(.data) }
  .bss :        { *(.bss) }  
  .rodata :     { *(.rodata) }
}