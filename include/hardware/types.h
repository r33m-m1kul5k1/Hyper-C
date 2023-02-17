#pragma once
#include <stdint.h>
// https://codingart.readthedocs.io/en/latest/c/Naming.html#typedef

typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint32_t dword_t;
typedef uint64_t qword_t;

/*
https://en.wikipedia.org/wiki/Logical_block_addressing
https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=42h:_Extended_Read_Sectors_From_Drive
https://wiki.osdev.org/Disk_access_using_the_BIOS_(INT_13h)
*/
struct {
    byte_t size;
    byte_t unused;
    word_t sector_count;
    word_t destination_addr;
    word_t destination_segment;
    qword_t sector_lba;

} typedef disk_address_packet_t;