
; copy real mode code to the lower memory
real_mode_start:
%include "src/bios/switch_modes.asm"
real_mode_end: