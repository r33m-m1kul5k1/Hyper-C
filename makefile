# pattern rules - https://www.gnu.org/software/make/manual/make.html#Pattern-Rules

# --------------- LINKER --------------- #
LINKER 			= ld
LINKER_FLAGS    = -nostdlib \
				  --oformat elf64-x86-64 \
				  -T build/linker_script.ld \
				  -n # do not page align data

# --------------- ASM COMPILER --------------- #
ASM_COMPILER 	= nasm
ASM_FLAGS 		= -f elf64 \
				  -g 
				  
				  

# --------------- C COMPILER ------------------- #
C_COMPILER = gcc
# the red zone is a signal-protected area above the stack pointer which can be use for temporary variables without adjusting SP.
C_FLAGS = -c \
		  -Wall \
		  -ffreestanding \
		  -g \
		  -mno-red-zone \
		  -I ./include
# ---------------------------------------------- #

OBJECT_DIR = build
# /iso/boot/grub is a folder to configure grub
GRUB_DIR = $(OBJECT_DIR)/iso/boot
SRC_DIR = src

BOOT_ASM_SOURCE_FILES		= $(addprefix boot/, $(shell find src/boot/ -maxdepth 1 -name '*.asm' -printf '%f '))
VMM_C_SOURCE_FILES        	= $(addprefix vmm/, $(shell find src/vmm/ -maxdepth 1 -name '*.c' -printf '%f '))
DRIVERS_C_SOURCE_FILES      = $(addprefix drivers/, $(shell find src/drivers/ -maxdepth 1 -name '*.c' -printf '%f '))

OBJECT_FILES = $(addprefix $(OBJECT_DIR)/,$(VMM_C_SOURCE_FILES:.c=.o)) \
			   $(addprefix $(OBJECT_DIR)/,$(DRIVERS_C_SOURCE_FILES:.c=.o)) \
			   $(addprefix $(OBJECT_DIR)/,$(BOOT_ASM_SOURCE_FILES:.asm=.o)) \


.PHONY : clean

all : hypervisor.iso

# make iso
hypervisor.iso : $(GRUB_DIR)/hypervisor
	grub-mkrescue -o $@ $(OBJECT_DIR)/iso

# link
$(GRUB_DIR)/hypervisor : $(OBJECT_FILES)
	$(LINKER) $(LINKER_FLAGS) -o $@ $^

# C

$(OBJECT_DIR)/%.o : $(SRC_DIR)/%.c
	$(C_COMPILER) $(C_FLAGS) -o $@ $^

# Asm
$(OBJECT_DIR)/%.o : $(SRC_DIR)/%.asm
	$(ASM_COMPILER) $(ASM_FLAGS) -o $@ $^

clean :
	rm -f $(GRUB_DIR)/hypervisor
	rm -f $(OBJECT_FILES)
	rm -f hypervisor.iso