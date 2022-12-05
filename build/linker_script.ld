/*ref https://sourceware.org/binutils/docs-2.30/ld/Simple-Example.html*/

SECTIONS
{
  /* location counter offset */
  . = 0x10000;

  /* take all text, data and bss sections */
  .text : { *(.text) }
  . = 0x8000000;
  .data : { *(.data) }
  .bss : { *(.bss) }
}