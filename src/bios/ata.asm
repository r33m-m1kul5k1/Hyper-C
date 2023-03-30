; ATA - advance Technology Attachment
%define DAP_ADDRESS 0x500
%define DRIVE_NUMBER_ADDRESS DAP_ADDRESS + 0x100

read_disk:
	mov si, DAP_ADDRESS
	mov ah, 0x42						; read hard disk in LBA mode
	mov dl, [DRIVE_NUMBER_ADDRESS]		; drive number 0 (A = 0, B = 1, C = 2)
	int 0x13
finished_read:
    ret
