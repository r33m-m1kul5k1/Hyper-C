# --------------- LINKER --------------- #
LINKER 			= ld
LINKER_FLAGS    = -nostdlib \
				  --oformat elf64-x86-64 \
				  -n # do not page align data

# --------------- ASM COMPILER --------------- #
ASM_COMPILER 	= nasm
ASM_FLAGS 		= -f elf64 \
				  -g


BUILD_DIR = build
# /iso/boot/grub is a folder to configure grub
OUTPUT_DIR = $(BUILD_DIR)/iso/boot
SRC_DIR = native-hypervisor

.PHONY : clean

hypervisor.iso : $(OUTPUT_DIR)/entrypoint
	grub-mkrescue -o $@ $(BUILD_DIR)/iso

$(OUTPUT_DIR)/entrypoint : $(BUILD_DIR)/entrypoint.o
	$(LINKER) -o $@ $^
	
$(BUILD_DIR)/entrypoint.o : $(SRC_DIR)/entrypoint.asm
	$(ASM_COMPILER) $(ASM_FLAGS) -o $@ $^

clean :
	rm $(OUTPUT_DIR)/entrypoint
	rm $(BUILD_DIR)/entrypoint.o
	rm hypervisor.iso