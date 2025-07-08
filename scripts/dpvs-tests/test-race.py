import threading
import time
from scapy.all import *

src = "10.0.2.1"
dst = "10.0.2.105"
sport = 12345
dport = 8000

# 1. Establish TCP connection (3-way handshake)
syn = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='S', seq=1000)
synack = sr1(syn, timeout=2, verbose=0)
if not synack or not synack.haslayer(TCP) or synack[TCP].flags != 'SA':
    print("No SYN-ACK received")
    exit(1)
ack = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='A', seq=1001, ack=synack[TCP].seq+1)
send(ack)
print("TCP connection established.")

# Shared state for sequence/ack numbers
seq = 1001
ack_num = synack[TCP].seq+1

print("Sending FIN to close connection.")
fin = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='F', seq=seq, ack=ack_num)
send(fin)

# time.sleep(2)

# def close_connection():
#     global seq, ack_num
#     # Wait a moment to increase race chance
#     time.sleep(0.1)
#     fin = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='FA', seq=seq, ack=ack_num)
#     send(fin)
#     print("[Thread 1] Sent FIN to close connection.")

# def use_connection():
#     global seq, ack_num
#     # Try to send data while the other thread is closing
#     for i in range(5):
#         data = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='PA', seq=seq, ack=ack_num)/Raw(f"data-{i}".encode())
#         send(data)
#         print(f"[Thread 2] Sent data packet {i}.")
#         seq += len(f"data-{i}")
#         time.sleep(0.05)  # Small delay to interleave with FIN

# # 2. Start both threads
# t1 = threading.Thread(target=close_connection)
# t2 = threading.Thread(target=use_connection)

# t1.start()
# t2.start()

# t1.join()
# t2.join()

# print("Race test complete.")