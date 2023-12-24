# Bor's Bootloader
This project is meant to be a simple bootloader written in assembly and C targeting BIOS and x86 architecture.

# TODO
- Configuration
- Kernel loading
- 32 bit
- 64 bit

# Building
It supports a few different modes:
- Normal via `make all`
- Test through qemu via `make qemu`
- Test through qemu (without qemu window) via `make qemu-cli`
- Debugging through the qemu built-in gdb server via `make debug`