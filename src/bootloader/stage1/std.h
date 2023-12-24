#ifndef BSTD_H
#define BSTD_H

#include "types.h"

void* memcpy(void* dest, const void* src,  uint32_t count);

void* memset(void* ptr, int x, uint32_t n);

void* alloca(uint32_t size);

uint32_t strlen(const char *input);

void putchar(char input);

void putint( uint32_t input);

void putstr(char input[]);

void print(char* str, ...);

void nl();

void revstr(char* input, char* output);

void make_dap(void* dest, uint16_t sectors, uint32_t lba, DAP* dap);

void* read_lba(DAP* dap);

void panic(char panic[]);

bool memcmp(const void* src1, const void* src2,  uint32_t count);

void* read_chain(const uint32_t cluster, const void* dest, const FATEntry* FAT, const uint32_t csize, const uint32_t partlba);

uint32_t chain_size(const uint32_t cluster, const FATEntry* FAT);

int32_t read_dir_entry(const DirectoryEntry* dirEntry, char fileName[8]);

#endif