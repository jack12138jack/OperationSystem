#!/usr/bin/env python3
"""Assemble boot sector and kernel into a bootable disk image."""
from __future__ import annotations

import math
import os
import struct
import subprocess
import sys

BOOT_ORIGIN = 0x7C00


def find_symbol_address(elf_path: str, symbol: str) -> int:
    """Return the absolute address of *symbol* within the ELF file."""
    try:
        output = subprocess.check_output(["nm", "-a", "--numeric-sort", elf_path], text=True)
    except subprocess.CalledProcessError as exc:  # pragma: no cover - defensive
        raise RuntimeError(f"failed to invoke nm: {exc}") from exc

    for line in output.splitlines():
        parts = line.strip().split()
        if len(parts) == 3 and parts[2] == symbol:
            return int(parts[0], 16)
    raise ValueError(f"symbol '{symbol}' not found in {elf_path}")


def patch_word(buf: bytearray, offset: int, value: int) -> None:
    if not (0 <= value < (1 << 16)):
        raise ValueError("value out of range for 16-bit word")
    if offset < 0 or offset + 2 > len(buf):
        raise ValueError("offset out of bounds")
    buf[offset:offset + 2] = struct.pack('<H', value)


def main(argv: list[str]) -> int:
    if len(argv) != 5:
        print(f"usage: {argv[0]} <boot.elf> <boot.bin> <kernel.bin> <output>", file=sys.stderr)
        return 1

    boot_elf, boot_bin_path, kernel_bin_path, out_path = argv[1:]

    with open(boot_bin_path, "rb") as fh:
        boot_sector = bytearray(fh.read())
    with open(kernel_bin_path, "rb") as fh:
        kernel_image = bytearray(fh.read())

    if len(boot_sector) > 512:
        raise ValueError("boot sector exceeds 512 bytes")

    # Ensure boot sector is exactly 512 bytes.
    if len(boot_sector) < 512:
        boot_sector.extend(b"\0" * (512 - len(boot_sector)))

    kernel_sectors = math.ceil(len(kernel_image) / 512) or 1

    symbol_address = find_symbol_address(boot_elf, "kernel_sectors")
    offset = symbol_address - BOOT_ORIGIN
    if offset < 0 or offset + 2 > len(boot_sector):
        raise ValueError("kernel_sectors symbol offset invalid")

    patch_word(boot_sector, offset, kernel_sectors)

    # Pad kernel to whole sectors for a clean image.
    remainder = len(kernel_image) % 512
    if remainder:
        kernel_image.extend(b"\0" * (512 - remainder))

    with open(out_path, "wb") as out:
        out.write(boot_sector)
        out.write(kernel_image)

    image_size = os.path.getsize(out_path)
    print(f"[build_image] kernel sectors: {kernel_sectors}, image size: {image_size} bytes")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
