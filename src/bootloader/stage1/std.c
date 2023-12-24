#include "std.h"
#include "types.h"

void* alloca(uint32_t size) {
    uint16_t dest = 0;

    asm("\
    sub si, %[size] \n\
    mov %[out], si \n\
    "
    : [size] "=r" (dest)
    : [out] "m" (size)
    );

    return (void*)dest;
}

void* memcpy(void* dest, const void* src,  uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        ((char*)dest)[i] = ((char*)src)[i];
    }

    return dest;
}

bool memcmp(const void* src1, const void* src2,  uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        if (((char*)src1)[i] != ((char*)src2)[i]) {
            return false;
        }
    }

    return true;
}

void* memset(void* ptr, int x, uint32_t n) {
    uint8_t* arr = ptr;

    for (int i = 0; i < n; i++) {
        arr[i] = x;
    }

    return ptr;
}

uint32_t strlen(const char *input) {
    uint32_t counter = 0;
    while (input[counter] != '\0') {
        counter++;
    }

    return counter;
}

void putchar(char input) {
    asm("\
    mov ah, 0x0e \n\
    mov al, %[input] \n\
    int 0x10 \n\
    "
    :
    : [input] "r" (input)
    );
}

void putint(uint32_t input) {
    char rev_str[11] = {0};
    char str[11] = {0};

    uint32_t i = 0;

    while (input != 0) {
        char digit = (input % 10) + 48;
        input = input / 10;

        rev_str[i] = digit;
        i++;
    }

    revstr(rev_str, str);

    putstr(str);
}

void putstr(char input[]) {
    for (uint32_t i = 0; i < strlen(input); i++) {
        putchar(input[i]);
    }
}

void print(char* str, ...) {
    char** args = &str + 1;

    bool fmt = false;

    bool end = false;

    for (uint32_t i = 0; i < strlen(str); i++)  {
        if (fmt) {
            if (str[i] == 's') {
                end = true;
                fmt = false;

                putstr(*args);
                args++;

                continue;
            }
            else if (str[i] == 'i') {
                end = true;
                fmt = false;

                putint(*(uint32_t*)args);
                args += 4;
            }
            else if (str[i] == '{') {
                putchar('{');
                end = true;
                fmt = false;
                continue;
            }
            else {
                putstr("ERROR: EXPECTED FORMAT");
                break;
            }
        }
        else if (end) {
            if (str[i-1] == '{') {
                if (str[i] == '}') {
                    continue;
                }
                else {
                    nl();
                    putstr("ERROR: EXPECTED }}");
                    break;
                }
            }
            else if (str[i] == '}') {
                end = false;
                continue;
            }
            else {
                nl();
                putstr("ERROR: EXPECTED }");
                break;
            }
        }
        else {
            if (str[i] != '{') {
                putchar(str[i]);
                continue;
            }
            else {
                fmt = true;
                continue;
            }
        }
    }
}

void nl() {
    putstr("\r\n");
}

void revstr(char* input, char* output) {
    for (uint32_t i = 1; i < strlen(input) + 1; i++) {
        output[i-1] = input[strlen(input) - i];
    }

    output[strlen(input)] = '\0';
}

void make_dap(void* dest, uint16_t sectors, uint32_t lba, DAP* dap) {
    dap->size = 0x10;

    dap->padding = 0;

    dap->sectors = sectors;

    dap->dest = (int)dest;

    dap->lowerLBA = lba;

    dap->upperLBA = 0;
}

void* read_lba(DAP* dap) {
    void* addy = (void*)dap->dest;
    const uint32_t SPR = 17;

    if (dap->sectors > SPR) {
        dap->sectors = SPR;

        uint32_t sectors = dap->sectors;
        uint32_t address = dap->dest;
        uint32_t lowerLBA = dap->lowerLBA;

        while (sectors > SPR) {
            dap->dest = address;
            dap->lowerLBA = lowerLBA;

            read_lba(dap);

            sectors -= SPR;
            address += SPR * 512;
            lowerLBA += SPR;
        }

        dap->sectors = sectors;

        read_lba(dap);
    }
    else {
        asm("\
        mov ah, 0x42 \n\
        mov dl, 0x80 \n\
        mov si, %[dap] \n\
        int 0x13 \n\
        "
        :
        : [dap] "m" (dap)
        );
    }

    return addy;
}

void panic(char panic[]) {
    nl();
    nl();

    print("BOOT PANICKED! ERROR: {s}", panic);

    nl();

    asm("\
    cli \n\
    hlt \n\
    "
    :
    :
    );
}

void* read_chain(const uint32_t cluster, const void* dest, const FATEntry* FAT, const uint32_t csize, const  uint32_t partlba) {
    uint32_t currentCluster = cluster;
    uint32_t address = (uint32_t)dest;
    FATEntry* entries = FAT;

    do {
        DAP* dap = (DAP*)419430;
        make_dap((void*)address, csize, partlba + currentCluster * csize, dap);

        read_lba(&dap);

        currentCluster = entries[currentCluster].Entry;
        address += csize * 512;
    } while (currentCluster != 0xFFFFFF8 && currentCluster != 0x0FFFFFFF && currentCluster != 0xFFFF && currentCluster != 0x0FFF);

    return dest;
}

uint32_t chain_size(const uint32_t cluster, const FATEntry* FAT) {
    uint32_t currentCluster = cluster;
    FATEntry* entries = FAT;
    uint32_t size = 0;

    do {
        size++;
        print("this ran\r\n");
        currentCluster = entries[currentCluster].Entry;
    } while(currentCluster != 0xFFFFFF8 && currentCluster != 0xFFFFFFFF && currentCluster != 0x0FFFFFFF && currentCluster != 0xFFFF && currentCluster != 0x0FFF);

    return size;
}

int32_t read_dir_entry(const DirectoryEntry* dirEntry, char* fileName) {
    uint32_t currentEntry = 0;

    for ( ; ; ) {
        if (memcmp(dirEntry[currentEntry].FileName, fileName, strlen(fileName))) {
            return dirEntry[currentEntry].ClusterLower + (dirEntry[currentEntry].ClusterUpper << 16);
        }
        else if (dirEntry[currentEntry].FileName[0] == 0x0) {
            return -1;
        }

        currentEntry++;
    }
}