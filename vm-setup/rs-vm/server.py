import socket
import threading

HOST = '::'  # IPv6 any-address (also accepts IPv4 if dual-stack is supported)
PORT = 8000

def tcp_server_ipv6():
    with socket.socket(socket.AF_INET6, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_V6ONLY, 0)  # Enable dual-stack
        s.bind((HOST, PORT))
        s.listen()
        print(f"[TCP] Listening on [::]:{PORT}")
        while True:
            conn, addr = s.accept()
            with conn:
                print(f"[TCP] Connection from {addr}")
                data = conn.recv(1024)
                if data:
                    print(f"[TCP] Received: {data.decode()}")
                    conn.sendall(b"Echo from TCP IPv6 server")

def udp_server_ipv6():
    with socket.socket(socket.AF_INET6, socket.SOCK_DGRAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_V6ONLY, 0)  # Enable dual-stack
        s.bind((HOST, PORT))
        print(f"[UDP] Listening on [::]:{PORT}")
        while True:
            data, addr = s.recvfrom(1024)
            print(f"[UDP] Received from {addr}: {data.decode()}")
            s.sendto(b"Echo from UDP IPv6 server", addr)

# Start both servers in background threads
threading.Thread(target=tcp_server_ipv6, daemon=True).start()
threading.Thread(target=udp_server_ipv6, daemon=True).start()

# Keep the main thread running
try:
    while True:
        pass
except KeyboardInterrupt:
    print("Shutting down...")