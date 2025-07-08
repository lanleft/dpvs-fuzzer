# test-malformed-header.py
from scapy.all import *

ip = IP(src="10.0.2.1", dst="10.0.2.105")
tcp = TCP(
    sport=12348,
    dport=8000,
    seq=1234,
    ack=0,
    flags="S",
    window=1024
)
payload = Raw(b"A" * 100)
packet = ip / tcp / payload
send(packet)
print("Sent malformed TCP header packet.")