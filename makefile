
OBJDIR = build
OUTPUT_DIR = iso
SRC_DIR = native-hypervisor

.PHONY : clean

$(OUTPUT_DIR)/entrypoint : $(OBJDIR)/entrypoint.o
	ld -o $@ $^
	
$(OBJDIR)/entrypoint.o : $(SRC_DIR)/entrypoint.asm
	nasm -f elf64 -o $@ $^

clean :
	rm $(OUTPUT_DIR)/entrypoint
	rm $(OBJDIR)/entrypoint.o