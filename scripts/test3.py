import socket

# Target server and port
HOST = '10.0.2.105'
PORT = 8008

# HTTP request data (equivalent to curl)
http_request = (
    "GET /index.html HTTP/1.1\r\n"
    f"Host: {HOST}:{PORT}\r\n"
    "Connection: close\r\n"
    "\r\n"
)

# Create TCP socket and connect to server
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.connect((HOST, PORT))
    print(f"Connected to {HOST}:{PORT}")

    # Send HTTP request
    sock.sendall(http_request.encode())
    print("HTTP request sent.")

    # Receive and print response
    response = b''
    while True:
        data = sock.recv(4096)
        if not data:
            break
        response += data

print("HTTP response received:\n")
print(response.decode())
