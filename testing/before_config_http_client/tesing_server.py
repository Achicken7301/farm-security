import socket
import struct

SERVER_PORT = 8844
# BUFFER_SIZE = 1<<24
BUFFER_SIZE = 1<<16

def start_server():
    # Create a TCP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Bind the socket to a specific address and port
    server_socket.bind(('0.0.0.0', SERVER_PORT))

    # Listen for incoming connections
    server_socket.listen(1)
    print(f"Server listening on port {SERVER_PORT}")

    while True:
        # Wait for a connection
        print("Waiting for a connection...")
        client_socket, client_address = server_socket.accept()
        print(f"Accepted connection from {client_address}")

        try:
            # Receive the file size
            file_size_bytes = client_socket.recv(8)
            # file_size = struct.unpack('Q', file_size_bytes)[0]
            file_size = int(file_size_bytes.decode('utf8'))
            print(f"Receiving file with size {file_size} bytes")

            # Receive the file content
            total_bytes_received = 0
            num_of_package = 0
            with open('received_picture.jpg', 'wb') as file:
                while total_bytes_received < file_size:
                    data = client_socket.recv(BUFFER_SIZE)
                    file.write(data)
                    num_of_package+=1
                    total_bytes_received += len(data)

            print(f"File received successfully. Total bytes received: {total_bytes_received}, number of packages: {num_of_package}")

        finally:
            # Close the connection
            client_socket.close()

if __name__ == "__main__":
    start_server()

