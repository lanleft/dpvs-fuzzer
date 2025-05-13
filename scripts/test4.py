from scapy.all import *

ip = IP(src="10.0.2.1", dst="10.0.2.105")

tcp = TCP(
    sport=12345,
    dport=8000,
    seq=222222,
    ack=0,
    flags="S",  # SYN
    window=4444,
    urgptr=0,
    # dataofs=20,  # TCP header length in 32-bit words
)

# Manually set 'doff'
# 'doff = 6' → means header is 24 bytes long 
tcp.dataofs = 15  # (tcph->doff << 2) - 20
tcp.options = [('MSS', 1460)]

# Prevent automatic recalculation
tcp.auto_checksum = False

# Optional: manually set checksum too
tcp.chksum = 18700  # = 18700 in decimal

# Append payload → so pkt_len > header length
payload = Raw(b"A" * 40)  # 40 bytes of extra data

packet = ip / tcp /payload

# Send the crafted packet
send(packet)
