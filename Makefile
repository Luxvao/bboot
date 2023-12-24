all :
	make try_all || make fail_clean

try_all :
	make img
	make boot
	make clean

qemu :
	make all
	qemu-system-x86_64 build/disk.img

qemu-cli :
	make all
	qemu-system-i386 build/disk.img -nographic -serial mon:stdio

debug :
	make all
	qemu-system-i386 -s -S build/disk.img

img :
	rm -fvr build/disk.img
	cp build/preset/disk.img build/disk.img
	losetup --partscan --show --find build/disk.img
	mount /dev/loop6p2 build/root/
	
boot : build/bootloader/boot.sec build/bootloader/stage1.boot
	dd if=build/bootloader/boot.sec of=/dev/loop6 bs=1 conv=notrunc
	dd if=build/preset/magic.bin of=/dev/loop6 bs=1 seek=510 count=2 conv=notrunc
	dd if=build/bootloader/stage1.boot of=/dev/loop6p1 bs=1 conv=notrunc

build/bootloader/boot.sec : src/bootloader/bootsec/boot.asm
	nasm -f bin src/bootloader/bootsec/boot.asm -o build/bootloader/boot.sec

build/bootloader/stage1.boot : src/bootloader/stage1/stage1.c src/bootloader/stage1/std.c src/bootloader/stage1/types.h src/bootloader/stage1/std.h
	clang -g -O0 -m16 -nostdlib -masm=intel -fno-stack-protector src/bootloader/stage1/*.c -o build/bootloader/stage1.boot
	
clean :
	umount build/root/
	losetup -d /dev/loop6

fail_clean :
	umount build/root/
	losetup -d /dev/loop6
	exit 1


