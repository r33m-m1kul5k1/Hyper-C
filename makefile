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
C_FLAGS = -c \
		  -ffreestanding \
		  -g \
		  -I./src/include # all includes will look at this dir
# ---------------------------------------------- #

OBJECT_DIR = build
# /iso/boot/grub is a folder to configure grub
OUTPUT_DIR = $(OBJECT_DIR)/iso/boot
SRC_DIR = src

# https://stackoverflow.com/questions/68215653/makefile-pattern-rules-on-multiple-subdirectories
BOOT_ASM_SOURCE_FILES		= $(addprefix boot/, $(shell find src/boot/ -maxdepth 1 -name '*.c' -printf '%f '))
VMM_C_SOURCE_FILES        	= $(addprefix vmm/, $(shell find src/vmm/ -maxdepth 1 -name '*.c' -printf '%f '))
DRIVERS_C_SOURCE_FILES      = $(addprefix drivers/, $(shell find src/drivers/ -maxdepth 1 -name '*.c' -printf '%f '))

OBJECT_FILES = $(addprefix $(OBJECT_DIR)/,$(VMM_C_SOURCE_FILES:.c=.o)) \
			   $(addprefix $(OBJECT_DIR)/,$(DRIVERS_C_SOURCE_FILES:.c=.o)) \
			   $(addprefix $(OBJECT_DIR)/,$(BOOT_ASM_SOURCE_FILES:.asm=.o)) \


.PHONY : clean

# make iso
hypervisor.iso : $(OUTPUT_DIR)/hypervisor
	grub-mkrescue -o $@ $(OBJECT_DIR)/iso

# link
$(OUTPUT_DIR)/hypervisor : $(OBJECT_FILES)
	$(LINKER) $(LINKER_FLAGS) -o $@ $^

# C
$(OBJECT_DIR)/%.o : $(SRC_DIR)/%.c # https://www.gnu.org/software/make/manual/make.html#Pattern-Rules
	$(C_COMPILER) $(C_FLAGS) -o $@ $^

# Asm
$(OBJECT_DIR)/%.o : $(SRC_DIR)/%.asm
	$(ASM_COMPILER) $(ASM_FLAGS) -o $@ $^

clean :
	@rm -f $(OUTPUT_DIR)/hypervisor
	@rm -f $(OBJECT_FILES)
	@rm -f hypervisor.iso