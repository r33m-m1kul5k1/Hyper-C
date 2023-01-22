; ATA - advance Technology Attachment

load_mbr:
	mov si, REAL_MODE_RELOCATION(DAPACK)		; address of "disk address packet"
	mov ah, 0x42		                        ; read hard disk in LBA mode
	mov dl, 0x80		                        ; drive number 0
	int 0x13

ret

DAPACK:
	db	0x10
	db	0
blkcnt:	dw	1		    ; int 13 resets this to # of blocks actually read/written
db_add:	dw	0x7C00		; memory buffer destination address (0:7c00)
    	dw	0		    ; in memory page zero
d_lba:	dd	1		    ; put the lba to read in this spot
    	dd	0		    ; more storage bytes only for big lba's ( > 4 bytes )
 