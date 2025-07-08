#!/usr/bin/env python3
"""
DPVS Mbuf Seed Generator for AFL-Fuzz

This script generates seed files for AFL-fuzz testing based on the DPVS MbufMutator
protocol buffer definition. It creates binary protobuf messages with various
network packet structures.
"""

import os
import sys
import struct
import random
import argparse
from google.protobuf.message import Message

# Import the generated proto file
# Note: You need to compile the proto file first using:
# protoc --python_out=. dpvs.proto
try:
    from protocOut.fuzz.mbuf_mutator_pb2 import MbufMutator
except ImportError:
    print("Error: mbuf_mutator_pb2.py not found!")
    print("Please compile the proto file first using:")
    print("protoc --python_out=. dpvs.proto")
    sys.exit(1)

# Constants for packet generation
ETH_ALEN = 6  # Ethernet address length
ETH_HLEN = 14  # Ethernet header length
IPV4_HLEN = 20  # IPv4 header length (without options)
TCP_HLEN = 20  # TCP header length (without options)
UDP_HLEN = 8   # UDP header length

# Protocol numbers
IPPROTO_TCP = 6
IPPROTO_UDP = 17

# Ethernet types
ETH_P_IP = 0x0800
ETH_P_IPV6 = 0x86DD
ETH_P_VLAN = 0x8100

# Generate Ethernet header
def generate_eth_header(vlan=False):
    """Generate an Ethernet header with optional VLAN tag"""
    # Source MAC
    src_mac = bytes([random.randint(0, 255) for _ in range(ETH_ALEN)])
    # Destination MAC
    dst_mac = bytes([random.randint(0, 255) for _ in range(ETH_ALEN)])
    
    if vlan:
        # Ethernet header with VLAN tag
        eth_type = struct.pack("!H", ETH_P_VLAN)
        return dst_mac + src_mac + eth_type
    else:
        # Regular Ethernet header with IP ethertype
        eth_type = struct.pack("!H", ETH_P_IP)
        return dst_mac + src_mac + eth_type

# Generate IPv4 header
def generate_ipv4_header(protocol, total_length):
    """Generate an IPv4 header"""
    version_ihl = (4 << 4) | 5  # IPv4, header length 5 * 4 = 20 bytes
    tos = 0
    id = random.randint(0, 65535)
    flags_fragoffset = 0  # Don't fragment
    ttl = 64
    proto = protocol
    checksum = 0  # Will be computed by the network stack
    src_ip = bytes([random.randint(1, 254) for _ in range(4)])
    dst_ip = bytes([random.randint(1, 254) for _ in range(4)])
    
    header = struct.pack("!BBHHHBBH4s4s",
                         version_ihl,
                         tos,
                         total_length,
                         id,
                         flags_fragoffset,
                         ttl,
                         proto,
                         checksum,
                         src_ip,
                         dst_ip)
    return header

# Generate TCP header
def generate_tcp_header(data_length):
    """Generate a TCP header"""
    src_port = random.randint(1024, 65535)
    dst_port = random.randint(1, 1023)  # Well-known ports
    seq_num = random.randint(0, 0xFFFFFFFF)
    ack_num = random.randint(0, 0xFFFFFFFF)
    
    # Flags: SYN, ACK, PSH, etc.
    offset_flags = (5 << 12) | (1 << 1) | (1 << 4)  # 5 * 4 = 20 bytes header, ACK + PSH flags
    
    window = 65535
    checksum = 0  # Will be computed by the network stack
    urgent_ptr = 0
    
    header = struct.pack("!HHLLHHHH",
                         src_port,
                         dst_port,
                         seq_num,
                         ack_num,
                         offset_flags,
                         window,
                         checksum,
                         urgent_ptr)
    return header

# Generate UDP header
def generate_udp_header(data_length):
    """Generate a UDP header"""
    src_port = random.randint(1024, 65535)
    dst_port = random.randint(1, 1023)  # Well-known ports
    length = UDP_HLEN + data_length  # UDP header + data
    checksum = 0  # Will be computed by the network stack
    
    header = struct.pack("!HHHH",
                         src_port,
                         dst_port,
                         length,
                         checksum)
    return header

# Generate payload
def generate_payload(min_size=10, max_size=100):
    """Generate random payload data"""
    size = random.randint(min_size, max_size)
    return bytes([random.randint(0, 255) for _ in range(size)])

# Generate a complete mbuf
def generate_mbuf(include_vlan=False, protocol=IPPROTO_TCP):
    """Generate a complete mbuf structure"""
    mbuf = MbufMutator()
    
    # Generate payload first to determine sizes
    payload_data = generate_payload(10, 100)
    mbuf.payload = payload_data
    
    # Set basic mbuf fields
    mbuf.port = random.randint(0, 16)
    mbuf.nb_segs = 1
    mbuf.ol_flags = random.randint(0, 0xFFFFFFFF)
    
    # Add Ethernet header
    eth_header = generate_eth_header(include_vlan)
    mbuf.eth_header = eth_header
    
    # Add VLAN tag if required
    if include_vlan:
        mbuf.vlan_tci = random.randint(0, 4095)  # 12-bit VLAN ID
    
    # Add IP header
    transport_header_size = TCP_HLEN if protocol == IPPROTO_TCP else UDP_HLEN
    total_length = IPV4_HLEN + transport_header_size + len(payload_data)
    ip_header = generate_ipv4_header(protocol, total_length)
    mbuf.ip_header = ip_header
    
    # Add transport header
    if protocol == IPPROTO_TCP:
        transport_header = generate_tcp_header(len(payload_data))
    else:  # UDP
        transport_header = generate_udp_header(len(payload_data))
    mbuf.transport_header = transport_header
    
    # Set packet length and data length
    mbuf.pkt_len = len(eth_header) + (4 if include_vlan else 0) + total_length
    mbuf.data_len = mbuf.pkt_len
    
    return mbuf

def main():
    parser = argparse.ArgumentParser(description='Generate seed files for AFL-fuzz testing of DPVS')
    parser.add_argument('-o', '--output-dir', default='seeds', help='Directory to store the generated seed files')
    parser.add_argument('-n', '--num-seeds', type=int, default=2, help='Number of seed files to generate')
    args = parser.parse_args()
    
    # Create output directory if it doesn't exist
    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)
    
    # Generate seed files
    for i in range(args.num_seeds):
        include_vlan = random.choice([True, False])
        protocol = random.choice([IPPROTO_TCP, IPPROTO_UDP])
        
        mbuf = generate_mbuf(include_vlan, protocol)
        
        # Serialize to binary
        seed_data = mbuf.SerializeToString()
        
        # Write to file
        protocol_name = "tcp" if protocol == IPPROTO_TCP else "udp"
        vlan_str = "vlan" if include_vlan else "novlan"
        filename = f"{args.output_dir}/seed_{i:03d}_{protocol_name}_{vlan_str}.bin"
        
        with open(filename, 'wb') as f:
            f.write(seed_data)
        
        print(f"Generated seed file: {filename} ({len(seed_data)} bytes)")

if __name__ == "__main__":
    main()