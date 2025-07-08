from scapy.all import *
import time

src = "10.0.2.1"
dst = "10.0.2.105"
sport = 12345
dport = 8000

# 1. Establish TCP connection (3-way handshake)
syn = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='S', seq=1000)
synack = sr1(syn, timeout=0.1, verbose=0)
if not synack or not synack.haslayer(TCP) or synack[TCP].flags != 'SA':
    print("No SYN-ACK received")
    exit(1)
ack = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='A', seq=1001, ack=synack[TCP].seq+1)
send(ack)
print("TCP connection established.")


time.sleep(2)
# send second packet
data_pkt = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='PA', seq=1001, ack=synack[TCP].seq+1)/Raw(b'Hello')
send(data_pkt)
print("TCP data packet sent.")

# Send FIN to close connection.
fin = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='F', seq=1001, ack=synack[TCP].seq+1)
send(fin)