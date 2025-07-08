from scapy.all import *

def reverse_ntohs(value):
    """
    Reverse the byte order of a 16-bit integer.
    """
    return (value >> 8) | ((value & 0xFF) << 8)


src = "10.0.2.1"
dst = "10.0.2.105"
SPORT = reverse_ntohs(0x1111)
DPORT = 8000

# 1. TCP handshake for one connection
syn = IP(src=src, dst=dst)/TCP(sport=SPORT, dport=DPORT, flags='S', seq=1000)
synack = sr1(syn, timeout=2, verbose=0)
if not synack or not synack.haslayer(TCP) or synack[TCP].flags != 'SA':
    print("No SYN-ACK received")
    exit(1)
ack = IP(src=src, dst=dst)/TCP(sport=SPORT, dport=DPORT, flags='A', seq=1001, ack=synack[TCP].seq+1)
send(ack)

print("TCP connection established.")

# 2. Send a TCP data packet
data_pkt = IP(src=src, dst=dst)/TCP(sport=SPORT, dport=DPORT, flags='PA', seq=1001, ack=synack[TCP].seq+1)/Raw(b'Hello')
send(data_pkt)
print("TCP data packet sent.")

# # 3. Send ICMP Destination Unreachable referencing the data packet
# def make_icmp_error(orig_pkt, error_type=3, error_code=1):
#     # inner_ip = bytes(orig_pkt)[:28]  # IP header (20) + 8 bytes of TCP header
#     inner_ip = IP(src=src, dst=dst)/TCP(sport=reverse_ntohs(0x1111), dport=DPORT)
#     return IP(src="8.8.8.8", dst=dst) / ICMP(type=error_type, code=error_code) / Raw(inner_ip)

# icmp_err = make_icmp_error(data_pkt)
# send(icmp_err)
# print("ICMP error sent.")

# 4. Interact with the connection after ICMP error (optional)
data_again = IP(src=src, dst=dst)/TCP(sport=SPORT, dport=DPORT, flags='PA', seq=1006, ack=synack[TCP].seq+1)/Raw(b'World')
send(data_again)
print("Sent data after established connection.")
