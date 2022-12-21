/*ref https://sourceware.org/binutils/docs-2.30/ld/Simple-Example.html*/

SECTIONS
{
  /* location counter offset */
  . = 0x0;

  /* take all text, data and bss sections */
  .text : { *(.text) }
  .data : { *(.data) }
  .bss : { *(.bss) }
}