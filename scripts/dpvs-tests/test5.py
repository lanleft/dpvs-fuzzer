from scapy.all import *


def reverse_ntohs(value):
    """
    Reverse the byte order of a 16-bit integer.
    """
    return (value >> 8) | ((value & 0xFF) << 8)

# Create the IP header
ip = IP(
    src="10.0.2.1",
    dst="10.0.2.105",
    ihl=5,             # IP header length in 32-bit words (5 = 20 bytes)
    ttl=1,            # Time to live
    id=reverse_ntohs(4444),          # Identification field
    flags="DF",        # Don't Fragment flag
    frag=0,            # Fragment offset
    proto=89,           # Protocol: TCP # IPPROTO_OSPF = 89
    tos=0x1C,        # TOS (e.g., DSCP=7, ECN=0 → binary: 00011100) 
)

# TCP header
tcp = TCP(
    sport=12345,
    dport=8000,
    seq=222222,
    ack=0,
    flags="S",         # SYN
    window=4444,
    urgptr=0,
    dataofs=5          # TCP header size (5 * 4 = 20 bytes)
)

# Optional payload to increase pkt_len > header len
# payload = Raw(b"A" * 40)  # Add 40 bytes of payload
payload = Raw(b"A" * 1)  # 40 bytes of extra data

# Optional: disable auto checksum and set custom values
# len = ntohs(iph->total_length);
ip.len = 24 # ntohs -> 3333               # Let Scapy auto-calculate total length
ip.chksum = None  # Let Scapy auto-calculate checksum
tcp.auto_checksum = False
tcp.chksum = 1111      # Set custom TCP checksum (optional)

# Build full packet
packet = ip / tcp / payload

# Send packet
send(packet)
