from scapy.all import *

# IPv6 basic header
ip6 = IPv6(
    src="2001:db8:10::1",
    dst="2001:db8:10::105",
    hlim=64
)

# 16 bytes of data = (2 + 1) * 8 - 8 = 16 bytes for options
opt_data = b"\x00" * 16

hbh = IPv6ExtHdrHopByHop(
    nh=58,                     # ICMPv6 is next
    len=0xff,                     # (2 + 1) * 8 = 24 bytes total
    options=[PadN(optdata=opt_data)]
)

# ICMPv6 Echo Request
icmp6 = ICMPv6EchoRequest(
    id=0x1234,
    seq=1
)

# Optional payload
payload = Raw(b"A"*40)

# Full packet: IPv6 -> Hop-by-Hop -> ICMPv6 -> Payload
packet = ip6 / hbh / icmp6 / payload

# Show the packet structure
packet.show()

# Send the packet
send(packet)
