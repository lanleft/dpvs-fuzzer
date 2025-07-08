# test-race-refcnt.py
from scapy.all import *
import threading
import time
import socket

# src = "10.0.2.1"
src_hex = 0x0a000201
dst = "10.0.2.105"
dport = 8000
sport = 12346

def open_and_close():
    global sport
    global src_hex
    for _ in range(5):
        src_hex += 1
        src = socket.inet_ntoa(struct.pack('>I', src_hex))
        print(src)
        sport += 1
        syn = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='S', seq=1000)
        synack = sr1(syn, timeout=1, verbose=0)
        if synack and synack.haslayer(TCP) and synack[TCP].flags == 'SA':
            ack = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='A', seq=1001, ack=synack[TCP].seq+1)
            send(ack)
            fin = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='F', seq=1001, ack=synack[TCP].seq+1)
            send(fin)
        time.sleep(0.05)

threads = [threading.Thread(target=open_and_close) for _ in range(2)]
for t in threads:
    t.start()
for t in threads:
    t.join()