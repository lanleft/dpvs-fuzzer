from scapy.all import *

# Outer IP header (encapsulating packet)
outer_ip = IP(
    src="10.0.2.1",
    dst="10.0.2.105",
    proto=47,   # Protocol number for GRE
    id=2222,
    ttl=64,
    flags="DF"
)

# GRE header (no options, standard GRE)
gre = GRE()

# Inner IP packet to encapsulate (can be any valid IP packet, like an ICMP ping)
inner_ip = IP(src="192.168.1.1", dst="8.8.8.8") / ICMP(type=8, code=0) / Raw(b'A' * 16)

# Compose full GRE packet
packet = outer_ip / gre / inner_ip

# Send the packet
send(packet)
