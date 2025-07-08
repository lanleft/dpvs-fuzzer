# test send one syn packet
from scapy.all import *

sport = 12348
dport = 8000
src = "10.0.2.1"
dst = "10.0.2.105"

# # start new connection with timeout 0.1s
# syn = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='S', seq=1000)
# send(syn)
# print("Sent SYN packet.")

# # wait for synack
# synack = sr1(syn, timeout=3, verbose=0)
# print(synack)
# if synack:
#     print("Received SYNACK packet.")
#     ack = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='A', seq=1001, ack=synack[TCP].seq+1)
#     send(ack)
#     print("Sent ACK packet.")
#     fin = IP(src=src, dst=dst)/TCP(sport=sport, dport=dport, flags='F', seq=1001, ack=synack[TCP].seq+1)
#     send(fin)
#     print("Sent FIN packets.")

ip = IP(src=src, dst=dst)
udp = UDP(
    sport=sport,
    dport=dport,
    # seq=1234,
    # ack=1,
    # flags="A",
)
payload = Raw(b"A" * 100)
packet1 = ip /  udp / payload
send(packet1)

