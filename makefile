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
		  -ffreestanding

BUILD_DIR = build
# /iso/boot/grub is a folder to configure grub
OUTPUT_DIR = $(BUILD_DIR)/iso/boot
SRC_DIR = native-hypervisor

OBJECT_FILES = $(BUILD_DIR)/initialize_machine.o $(BUILD_DIR)/entrypoint.o $(BUILD_DIR)/serial_print.o


.PHONY : clean

# make iso
hypervisor.iso : $(OUTPUT_DIR)/hypervisor
	grub-mkrescue -o $@ $(BUILD_DIR)/iso

# link
$(OUTPUT_DIR)/hypervisor : $(OBJECT_FILES)
	$(LINKER) -o $@ $^

# C
$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c # https://www.gnu.org/software/make/manual/make.html#Pattern-Rules
	$(C_COMPILER) $(C_FLAGS) -o $@ $^

# Asm
$(BUILD_DIR)/%.o : $(SRC_DIR)/%.asm
	$(ASM_COMPILER) $(ASM_FLAGS) -o $@ $^

clean :
	rm $(OUTPUT_DIR)/hypervisor
	rm $(BUILD_DIR)/*.o
	rm hypervisor.iso