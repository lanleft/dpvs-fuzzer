#!/usr/bin/env python3
# test-sctp-init.py

from scapy.all import send, IP
from scapy.layers.sctp import SCTP, SCTPChunkInit

# Configuration: update as needed
dst_ip = "10.0.2.105"   # DPVS virtual IP
src_ip = "10.0.2.1"     # Your local IP
src_port = 12345
dst_port = 8000

# Create the SCTP INIT chunk
sctp_init = SCTPChunkInit(
    init_tag=0x12345678,  # Verification tag
    a_rwnd=1500,          # Advertised receiver window credit
    n_out_streams=1,                 # Outbound streams
    n_in_streams=1,                # Inbound streams
    init_tsn=0x1       # Starting TSN
)

# Create the SCTP header with INIT chunk
sctp = SCTP(sport=src_port, dport=dst_port, tag=0) / sctp_init

# Wrap it in an IP packet
ip_pkt = IP(src=src_ip, dst=dst_ip) / sctp

# Send the packet
send(ip_pkt, verbose=True)
print("✅ Sent SCTP INIT packet.")
