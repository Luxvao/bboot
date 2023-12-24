#ifndef BTYPES_H
#define BTYPES_H

#define true 1
#define false 0

typedef unsigned short int uint16_t;
typedef signed short int int16_t;
typedef int bool;
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned int uint32_t;
typedef int int32_t;


typedef struct __attribute__((packed)) {
    uint8_t Head;
    uint8_t Sector;
    uint8_t Cylinder;
} CHS;

typedef struct __attribute__((packed)) {
    uint8_t size;
    uint8_t padding;
    uint16_t sectors;
    uint32_t dest;
    uint32_t lowerLBA;
    uint32_t upperLBA;
} DAP;

typedef struct __attribute__((packed)) {
    char FileName[8];
    char FileExtension[3];
    uint8_t FileAttributes;
    uint8_t padding1[6];
    uint16_t ClusterUpper;
    uint8_t padding2[4];
    uint16_t ClusterLower;
    uint32_t FileSize;
} DirectoryEntry;


typedef struct __attribute__((packed)) {
    uint8_t DriveAttributes;
    CHS CHS;
    uint8_t PartitionType;
    CHS LastCHS;
    uint32_t LBA;
    uint32_t Sectors;
} PartitionEntry;

typedef struct __attribute((packed)) {
    uint32_t Entry;
} FATEntry;

typedef struct __attribute__((packed)) {
    uint8_t jump[3];
    uint8_t OEMName[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t NumCopiesFAT;
    uint16_t padding1[2];
    uint8_t MediaDescriptor;
    uint16_t padding2;
    uint16_t SectorsPerTrack;
    uint16_t NumHeads;
    uint32_t NumHiddenSectorsInPartition;
    uint32_t NumSectorsInPartition;
    uint32_t NumSectorsPerFAT;
    uint16_t Flags;
    uint16_t padding3;
    uint32_t StartRootDir;
    uint8_t padding4[34];
    uint8_t FATName[8];
    uint8_t bootcode[420];
    uint8_t magic[2];
} VBR;

typedef struct __attribute__((packed)) {
    uint8_t bootcode[446];
    PartitionEntry part1;
    PartitionEntry part2;
    PartitionEntry part3;
    PartitionEntry part4;
    uint8_t magic[2];
} MBR;

#endif