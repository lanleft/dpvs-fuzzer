

gen:
	protoc --python_out=./protocOut --proto_path=../../fuzz/  mbuf_mutator.proto

pb2txt:
	python3 pb2txt.py $(ip) $(op)


txt2pb:
	python3 txt2pb.py $(ip) $(op)



