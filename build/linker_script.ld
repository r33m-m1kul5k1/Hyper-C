/*
 ref https://sourceware.org/binutils/docs-2.30/ld/Simple-Example.html 
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
}