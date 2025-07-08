import sys
from google.protobuf import text_format
from scapy.all import IP, TCP, Raw, send, ICMP
from protocOut.mbuf_mutator_pb2 import Mbuf

# Maps enum values to real numbers for sending
SRC_ADDR_MAP = {
    0: "10.0.2.1",
    -1: "255.255.255.255",
}

DST_ADDR_MAP = {
    0: "10.0.2.105",
    -1: "255.255.255.255",
}

def convert_enum_src_ip(enum_value):
    return SRC_ADDR_MAP.get(enum_value, "10.0.2.1")  # fallback to localhost

def convert_enum_dst_ip(enum_value):
    return DST_ADDR_MAP.get(enum_value, "10.0.2.105")  # fallback to localhost

def reverse_ntohs(value):
    """
    Reverse the byte order of a 16-bit integer.
    """
    return (value >> 8) | ((value & 0xFF) << 8)

def reverse_ntohl(value):
    """
    Reverse the byte order of a 32-bit integer.
    """
    return (value >> 24) | ((value & 0xFF) << 24) | ((value >> 8) & 0xFF00) | ((value & 0xFF00) << 8)

def build_and_send_tcp_packet(mbuf_msg):

    ip4_hdr = tcp_packet.ip4_hdr
    tcp_hdr = tcp_packet.tcp_hdr
    payload = tcp_packet.data if tcp_packet.HasField("data") else b""

    # Convert proto values
    src_ip = convert_enum_src_ip(ip4_hdr.ip_src)
    dst_ip = convert_enum_dst_ip(ip4_hdr.ip_dst)

    pkt = IP(
        src=src_ip,
        dst=dst_ip,
        ttl=ip4_hdr.ip_ttl,
        id=reverse_ntohs(ip4_hdr.ip_id),
        tos=ip4_hdr.ip_tos,
        proto=ip4_hdr.ip_p,
        len=ip4_hdr.ip_len,
        frag=ip4_hdr.ip_off
    ) / TCP(
        sport=tcp_hdr.th_sport,
        dport=tcp_hdr.th_dport,
        seq=tcp_hdr.th_seq,
        ack=tcp_hdr.th_ack,
        flags="S",
        window=tcp_hdr.th_win,
        urgptr=tcp_hdr.th_urp
    )

    if payload:
        pkt = pkt / Raw(load=payload)

    print(f"Sending packet to {dst_ip}:{tcp_hdr.th_dport}")
    send(pkt)


def build_and_send_icmp_packet(icmp_msg):
    ip4_hdr = icmp_msg.ip4_hdr
    icmp_hdr = icmp_msg.icmp_hdr
    payload = icmp_msg.data if icmp_msg.HasField("data") else b""

    src_ip = convert_enum_src_ip(ip4_hdr.ip_src)
    dst_ip = convert_enum_dst_ip(ip4_hdr.ip_dst)

    # TODO: modify checksum when replicating testcase
    pkt = IP(
        src=src_ip,
        dst=dst_ip,
        ttl=ip4_hdr.ip_ttl,
        id=reverse_ntohs(ip4_hdr.ip_id),
        tos=ip4_hdr.ip_tos,
        proto=ip4_hdr.ip_p,
        len=ip4_hdr.ip_len,
        frag=ip4_hdr.ip_off
    ) / ICMP(
        type=icmp_hdr.icmp_type,
        code=icmp_hdr.icmp_code,
        id=icmp_hdr.icmp_id,
        seq=icmp_hdr.icmp_seq
    )

    if payload:
        pkt = pkt / Raw(load=payload)

    print(f"Sending ICMP packet to {dst_ip}")
    send(pkt)

def build_and_send_packet(mbuf_msg):
    if mbuf_msg.HasField("tcp_packet"):
        build_and_send_tcp_packet(mbuf_msg.tcp_packet)
    elif mbuf_msg.HasField("icmp_packet"):
        build_and_send_icmp_packet(mbuf_msg.icmp_packet)
    else:
        print("Unsupported packet type")

def main(textproto_file):
    msg = Mbuf()
    with open(textproto_file, "r") as f:
        text_format.Parse(f.read(), msg)

    build_and_send_packet(msg)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} file.textproto")
        sys.exit(1)

    main(sys.argv[1])
