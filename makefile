# pattern rules - https://www.gnu.org/software/make/manual/make.html#Pattern-Rules

# --------------- LINKER --------------- #
LINKER 			= ld
LINKER_FLAGS    = -nostdlib \
				  -T scripts/linker.ld \
				  #-M # prints the memory map


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
		  -fno-stack-protector \
		  -g \
		  -mno-red-zone \
		  -I ./include
# ---------------------------------------------- #

OBJECT_DIR = build
# /iso/boot/grub is a folder to configure grub
GRUB_DIR = $(OBJECT_DIR)/iso/boot
SRC_DIR = src

C_SRC_FILES = $(wildcard $(SRC_DIR)/*/*.c)
ASM_SRC_FILES = $(wildcard $(SRC_DIR)/guest/*.asm) $(wildcard $(SRC_DIR)/vmm/*.asm) $(wildcard $(SRC_DIR)/boot/entrypoint.asm)

C_FILES = $(C_SRC_FILES:$(SRC_DIR)/%=%)
ASM_FILES = $(ASM_SRC_FILES:$(SRC_DIR)/%=%)


OBJECT_FILES = $(addprefix $(OBJECT_DIR)/,$(C_FILES:.c=.o)) \
			    $(addprefix $(OBJECT_DIR)/,$(ASM_FILES:.asm=.o)) \


.PHONY : clean

all : $(OBJECT_DIR)/hypervisor.iso 


# make iso
$(OBJECT_DIR)/hypervisor.iso : $(GRUB_DIR)/hypervisor
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
	rm -f $(OBJECT_DIR)/hypervisor.iso