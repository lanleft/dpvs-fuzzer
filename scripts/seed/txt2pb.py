
# generate protocOut: protoc --python_out=./protocOut --proto_path=../../fuzz/  mbuf_mutator.proto
# python3 generate-seed.py ../../input-text-seeds/seed0.pb ../../input-seeds/seed0
import sys
from google.protobuf import text_format
from protocOut.mbuf_mutator_pb2 import Mbuf


def convert_textproto_to_pb(textproto_file, pb_file):
    message = Mbuf()

    with open(textproto_file, "r") as f:
        text_data = f.read()

    text_format.Parse(text_data, message)

    with open(pb_file, "wb") as f:
        f.write(message.SerializeToString())

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} input.textproto output.pb")
        sys.exit(1)
    
    textproto_file = sys.argv[1]
    pb_file = sys.argv[2]

    convert_textproto_to_pb(textproto_file, pb_file)
    print(f"Converted {textproto_file} to {pb_file}")
