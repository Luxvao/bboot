#include "types.h"
#include "std.h"

void load_kernel_fat32(VBR* vbr, MBR* mbr) {
    print("INFO: Loading the FAT from FAT32\r\n");

    uint8_t fat[5 * 512];
    memset(&fat, 0, 5 * 512);

    FATEntry* entries = (FATEntry*)fat;

    DAP fat_dap;
    memset(&fat_dap, 0, sizeof(DAP));

    make_dap(&fat, 5, mbr->part2.LBA + vbr->ReservedSectors, &fat_dap);

    read_lba(&fat_dap);

    DirectoryEntry* root_dir = (DirectoryEntry*)alloca(chain_size(vbr->StartRootDir, entries) * vbr->SectorsPerCluster * 512);
    memset(root_dir, 0, chain_size(vbr->StartRootDir, entries) * vbr->SectorsPerCluster * 512);

    panic("NORMAL1");
}

void load_kernel_fat16(VBR* vbr, MBR* mbr) {
    panic("FAT16 isn't supported yet!\r\n");
}

void load_kernel_fat12(VBR* vbr, MBR* mbr) {
    panic("FAT12 isn't supported yet!\r\n");
}

uint8_t index(uint8_t* input, uint32_t index) {
    return *(input + index);
}

void _start(void) {
    for (int i = 0; i < 10; i++) {
        print("test\r\n");
    } 

    print("INFO: Stage 1 Loaded!\r\n");

    print("INFO: Loading FAT VBR into RAM...\r\n");

    MBR* mbr = (MBR*)0x7c00;

    VBR vbr;
    memset(&vbr, 0, sizeof(VBR));

    DAP vbr_dap;
    memset(&vbr_dap, 0, sizeof(DAP));

    make_dap(&vbr, 1, mbr->part2.LBA, &vbr_dap);

    read_lba(&vbr_dap);

    print("INFO: VBR Loaded!\r\n");

    print("INFO: Checking Filesystem...\r\n");

    char FATName[9];
    memset(FATName, 0, 8);

    memcpy(FATName, vbr.FATName, 8);

    if (memcmp(FATName, "FAT", 3)) {
        print("CHECK: Successful!\r\nINFO: Filesystem: {s}\r\n", FATName);
    }
    else {
        print("CHECK: Failed!\r\n");
        panic("Filesystem not recognized!\r\n");
    }

    if (memcmp(FATName, "FAT32", 5)) load_kernel_fat32(&vbr, mbr);
    if (memcmp(FATName, "FAT16", 5)) load_kernel_fat16(&vbr, mbr);
    if (memcmp(FATName, "FAT12", 5)) load_kernel_fat12(&vbr, mbr);

    panic("Unknown FAT type!");
}
