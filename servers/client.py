import zmq 

context = zmq.Context()
socket = context.socket(zmq.REQ)
hostname = "2.59.161.68"
#hostname = "127.0.0.1"
port = 12345
socket.connect(f"tcp://{hostname}:{port}")
while True:
    msg = input("Enter message to server: ")
    if msg == "quit": 
        break
    socket.send_string(msg)
    data = socket.recv_string()
    print("Server sent: ", data)
socket.close()