entrypoint : entrypoint.o
	ld -o entrypoint entrypoint.o
	
entrypoint.o : entrypoint.asm
	nasm -f elf64 entrypoint.asm
	