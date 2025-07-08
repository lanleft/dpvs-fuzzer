from scapy.all import *

def reverse_ntohs(value):
    """
    Reverse the byte order of a 16-bit integer.
    """
    return (value >> 8) | ((value & 0xFF) << 8)

# Craft an IP header with custom IHL (header length)
ip = IP(
    src="10.0.2.1",
    dst="10.0.2.105",
    ihl=5,           # 6 * 4 = 24 bytes (needs 4 bytes of options!)
    tos=0x10,
    id=reverse_ntohs(4444),
    ttl=64,
    flags="DF",
    frag=0,            # Fragment offset
)

# # Add 4 bytes of IP options to match ihl=6
# ip.options = IPOption('\x01\x01\x01\x01')  # Just dummy NOPs

# ICMP packet (standard)
icmp = ICMP(type=8, code=0, id=0x4321, seq=1)
payload = Raw(b'A' * 0x10)  # 40 bytes of payload

# Let Scapy auto-fix checksums
ip.chksum = None

# Build and send packet
packet = ip / icmp / payload
send(packet)
