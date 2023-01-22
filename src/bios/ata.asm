; ATA - advance Technology Attachment

load_mbr:
	mov si, REAL_MODE_RELOCATION(DAP)
	mov ah, 0x42		                        ; read hard disk in LBA mode
	mov dl, 0x80		                        ; drive number 0 (A = 0, B = 1, C = 2)
	int 0x13



; Disk Address Packet
DAP:
	db	0x10
	db	0
sector_count:	dw	1		    ; int 13 resets this to # of blocks actually read/written
destination:	dd	0x7C00		; memory buffer destination address (0:7c00)
    			dw	0		    ; little endian
sector_address:	dd	0		    ; logical block address - value 0 is the first sector
    			dd	0		    ; more storage bytes only for big lba's ( > 4 bytes )

; https://en.wikipedia.org/wiki/Logical_block_addressing
; https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=42h:_Extended_Read_Sectors_From_Drive