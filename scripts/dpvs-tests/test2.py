from scapy.all import *

# Basic HTTP GET packet
pkt = Ether()/IP(dst="10.0.2.105")/TCP(dport=8000, sport=12345, flags="PA")/Raw(load="GET /index.html HTTP/1.0\r\n\r\n")

# Send it without checksum calculated
sendp(pkt, iface="br0")
# sendp(pkt)

# receive the packet
pkt = sniff(iface="br0", count=1)
print(pkt)
