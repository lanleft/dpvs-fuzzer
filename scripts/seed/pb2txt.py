import sys
from google.protobuf import text_format
from protocOut.mbuf_mutator_pb2 import Mbuf

def convert_pb_to_textproto(pb_file, textproto_file):
    message = Mbuf()

    with open(pb_file, "rb") as f:
        message.ParseFromString(f.read())

    text_data = text_format.MessageToString(message)

    with open(textproto_file, "w") as f:
        f.write(text_data)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} input.pb output.textproto")
        sys.exit(1)

    pb_file = sys.argv[1]
    textproto_file = sys.argv[2]

    convert_pb_to_textproto(pb_file, textproto_file)
    print(f"Converted {pb_file} to {textproto_file}")
