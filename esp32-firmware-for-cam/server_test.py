import json
from http.server import BaseHTTPRequestHandler, HTTPServer

# Dữ liệu JSON mẫu
data = {"message": "Hello World!"}
data_post = {"message from post": "Hello World!"}

class MyRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # Gửi dữ liệu JSON cho yêu cầu GET
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())

    def do_POST(self):
        # In dữ liệu nhận được từ yêu cầu POST
        content_length = int(self.headers['Content-Length'])
        print(f"headers: {self.headers}")
        post_data = self.rfile.read(content_length)

        post_type, post_format = self.headers['Content-Type'].split('/')

        if(post_type == "application"):
            print(f"type application, file format {post_format}.")
            print(f"Receiving data: {post_data}")

        if(post_type == "image"):
            print(f"type image, file format {post_format}.")
            file_name = f"receiving_image.{post_format}"
            with open(file_name, 'wb') as file:
                file.write(post_data)

        # Trả về HTTP 200
        self.send_response(200)
        self.end_headers()

        

# Khởi động server
port = 8080
server = HTTPServer(('192.168.1.13', port), MyRequestHandler)
print(f"Server đang chạy trên cổng {port}")
server.serve_forever()

