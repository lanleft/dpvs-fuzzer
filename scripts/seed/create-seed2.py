from protocOut.fuzz.mbuf_mutator_pb2 import MbufMutator
from google.protobuf import text_format

mbuf = MbufMutator()

# Set fields
mbuf.pkt_len = 128
mbuf.data_len = 128
mbuf.port = 0
mbuf.nb_segs = 1
mbuf.ol_flags = 0
# eth_header size is 14 bytes
mbuf.eth_header = b'\xAA\xBB\xCC\xDD\xEE\xFF\x11\x22\x33\x44\x55\x66\x08\x00'
# ip_header size is 20 bytes
mbuf.ip_header = b'\x45' * 20
# transport_header size is 20 bytes
mbuf.transport_header = b'\x50' * 20
# payload size is 10 bytes
mbuf.payload = b'hello_data'

# Serialize to TEXT instead of binary
with open('input-seeds/seed0', 'w') as f:
    f.write(text_format.MessageToString(mbuf))
