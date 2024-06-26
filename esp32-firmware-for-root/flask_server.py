from flask import Flask, request, jsonify
import datetime
import socket

app = Flask(__name__)
APP = "application"
IMAGE = "image"


@app.route("/api/get", methods=["GET"])
def api_get():
    # Return a JSON response for GET request
    now_date_time = datetime.datetime.now().strftime("%d-%m-%Y %H:%M:%S")

    data = {f"Datetime": str(now_date_time), "ROUTER_SSID": "", "ROUTER_PASS": "", "SERVER_IPV4": ""}
    return jsonify(data)


@app.route("/api/post", methods=["POST"])
def api_post():
    if request.method == "POST":
        # Assuming the incoming data is in JSON format
        request_data = request.get_json()
        header_type, header_format = request.headers["Content-Type"].split("/")
        print(request.headers)

        now_date = datetime.datetime.now().strftime("%d-%m-%Y")
        now_date_time = datetime.datetime.now().strftime("%d-%m-%Y %H:%M:%S")

        if header_type == APP:
            # Open the file in append mode ('a')
            with open(f'{now_date}.txt', 'a') as file:
                # Write the desired text to the end of the file
                file.write(f'[{now_date_time}] {request.data.decode()}\n')



        # Process the data or perform any necessary operations
        # For demonstration purposes, let's just echo the received data
        # response_data = {
            # "message": "API POST request successful",
        #     "data_received": request_data,
        # }
        return "Server reiceve success"


@app.route("/api/upload", methods=["POST", "GET"])
def api_upload():
    # print(request.headers)
    header_type, header_format = request.headers["Content-Type"].split("/")
    # print(f"type: {header_type}, format: {header_format}")

    if header_type == APP:
        print(request.data)
        return "This is not url for post use /api/post"
    elif header_type == IMAGE:
        now = datetime.datetime.now().strftime("%d-%m-%Y %H:%M:%S")
        file_format = f"{now}.{header_format}"
        # print(f"Saved {file_format}")
        print(f"Receiving {len(request.data)} bytes")
        with open(file_format, "wb") as f:
            f.write(request.data)

    return "upload_success"


@app.route("/")
def index():
    return "index page"


if __name__ == "__main__":
    hostname = socket.gethostname()
    IPAddr = socket.gethostbyname(hostname)
    app.run(debug=True, host=IPAddr, port=8080)
