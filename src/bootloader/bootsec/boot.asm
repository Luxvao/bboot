[bits 16]
[org 0x7c00]

%define MBR 0x7c00
%define PartitionEntryAddy MBR + 446
%define Partition1SectorCount [PartitionEntryAddy + 0x0c]
%define Partition1LBA [PartitionEntryAddy + 0x08]

%define Partition1Head byte [PartitionEntryAddy + 0x01]
%define Partition1Sector byte [PartitionEntryAddy + 0x02]
%define Partition1Cylinder byte [PartitionEntryAddy + 0x03]

%define Partition1LBA word [PartitionEntryAddy + 0x08]

%define Stage1Location 0x7e00


; First read

mov word [sectors], 60
mov dword [dest], Stage1Location
mov ax, Partition1LBA
mov word [lba], ax

mov ah, 0x42
mov dl, 0x80
mov si, stage1_dap
int 0x13

; Second read

mov word [sectors], 100
mov dword [dest], 0xf600
mov ax, Partition1LBA
add ax, 60
mov [lba], ax

mov ah, 0x42
mov dl, 0x80
mov si, stage1_dap
int 0x13

; Jump to stage 1

mov ax, [Stage1Location + 0x18]
add ax, Stage1Location

jmp ax

stage1_dap:
db 0x10
db 0
sectors: dw 0
dest: dd 0
lba: dd 0
dd 0

