#!/usr/bin/env python3
"""Read-only PE32 inventory for establishing reverse-engineering baselines."""
import argparse
import hashlib
import json
from pathlib import Path
import struct


def inspect(path, root):
    data = path.read_bytes()
    def unpack(fmt, offset):
        return struct.unpack_from('<' + fmt, data, offset)
    if data[:2] != b'MZ':
        raise ValueError(f'{path}: missing DOS signature')
    pe, = unpack('I', 0x3c)
    if data[pe:pe + 4] != b'PE\0\0':
        raise ValueError(f'{path}: missing PE signature')
    machine, count, timestamp, _, _, optional_size, flags = unpack('HHIIIHH', pe + 4)
    opt = pe + 24
    magic, = unpack('H', opt)
    if magic != 0x10b:
        raise ValueError(f'{path}: expected PE32, got {magic:#x}')
    entry, = unpack('I', opt + 16)
    base, = unpack('I', opt + 28)
    sections = []
    for i in range(count):
        offset = opt + optional_size + i * 40
        name = data[offset:offset + 8].rstrip(b'\0').decode('ascii', errors='replace')
        size, rva, raw_size, raw = unpack('IIII', offset + 8)
        sections.append(dict(name=name, virtual_size=size, rva=rva,
                             raw_size=raw_size, raw_offset=raw))
    def file_offset(rva):
        for section in sections:
            delta = rva - section['rva']
            if 0 <= delta < section['raw_size']:
                return section['raw_offset'] + delta
        raise ValueError(f'{path}: unmapped RVA {rva:#x}')
    imports = []
    directory_count, = unpack('I', opt + 92)
    if directory_count > 1:
        import_rva, import_size = unpack('II', opt + 104)
        if import_rva:
            start = file_offset(import_rva)
            for offset in range(start, start + import_size, 20):
                descriptor = unpack('IIIII', offset)
                if not any(descriptor):
                    break
                name_offset = file_offset(descriptor[3])
                end = data.index(b'\0', name_offset)
                imports.append(data[name_offset:end].decode('ascii'))
    return dict(path=str(path.relative_to(root)), bytes=len(data),
                sha256=hashlib.sha256(data).hexdigest(),
                format='PE32', machine=hex(machine), image_base=hex(base),
                entry_point=hex(base + entry), imports=imports, sections=sections)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('game_dir', type=Path)
    args = parser.parse_args()
    root = args.game_dir.resolve()
    paths = [root / name for name in ('terran.exe', 'terranx.exe',
             '_backup_v2.1/terran.exe', '_backup_v2.1/terranx.exe')]
    print(json.dumps({'binaries': [inspect(p, root) for p in paths if p.exists()]}, indent=2))


if __name__ == '__main__':
    main()
