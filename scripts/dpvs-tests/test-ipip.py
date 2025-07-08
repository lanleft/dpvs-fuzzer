from scapy.all import *

# Outer IP header (encapsulating IP packet)
outer_ip = IP(
    src="10.0.2.1",
    dst="10.0.2.105",
    proto=4,   # Protocol number for IP-in-IP
    id=1234,
    ttl=64,
    flags="DF"
)

# Inner IP packet (can be any valid IP packet; here we use ICMP for demo)
inner_ip = IP(src="192.168.1.1", dst="8.8.8.8") / ICMP(type=8, code=0) / Raw(b'HelloIPIP')

# Compose full IPIP packet
packet = outer_ip / inner_ip

# Send the packet
send(packet)
