# test-tcp2.py
# sending 2 tcp packets
from scapy.all import *

src = "10.0.2.1"
dst = "10.0.2.105"
sport = 12345
dport = 8000

# send 2 tcp packets
pkt1 = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='S', seq=1000) / Raw(b'Hello')
send(pkt1)
pkt2 = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='PA', seq=1001, ack=1000) / Raw(b'World')
send(pkt2)
print("Sent 2 tcp packets.")
