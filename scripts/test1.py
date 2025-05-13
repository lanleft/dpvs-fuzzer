# inject_packet.py
import os
import struct

TAP_DEV = "/dev/tap0"

# Construct virtio_net_hdr: flags=0x01 (NEEDS_CSUM), csum_start=34, csum_offset=2
hdr = struct.pack("BBHHiHH", 1, 0, 0, 0xffff, 2, 0, 0)  # 10-byte version

# Basic Ethernet frame (fill in your own MAC/IP headers here)
ethernet_frame = b'\xff'*14 + b'\x45\x00' + b'\x00'*46  # placeholder

pkt = hdr + ethernet_frame

with open(TAP_DEV, 'wb') as f:
    f.write(pkt)
