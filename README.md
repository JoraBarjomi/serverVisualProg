# serverPython
Simple TCP server and client in Python, Go, and C. The servers listen on port 12345 and perform a simple string manipulation on the received data.

- The **Python server** reverses the received string.
- The **Go server** converts the received string to uppercase.
- The **C server** converts the received string to uppercase.

The clients connect to a hardcoded IP address and send user-provided messages to the server.

There are also Dockerfiles that can be used to build and run each of the servers in a container.

# Building and Running

## Python

To run the Python server:
```bash
python3 server.py
```

To run the Python client:
```bash
python3 client.py
```

## Go

To run the Go server:
```bash
go run server.go
```
As there is no `client.go` file, there are no instructions on how to run a Go client.

## C

To build and run the C server:
```bash
gcc -o serverC server.c
./serverC
```

To build and run the C client:
```bash
gcc -o clientC client.c
./clientC
```

## Docker

To build and run the C server in a Docker container:
```bash
docker build -t server-c -f DockerServerC .
docker run -p 12345:12345 server-c
```

To build and run the Go server in a Docker container:
```bash
docker build -t server-go -f DockerServerGo .
docker run -p 12345:12345 server-go
```

To build and run the Python server in a Docker container:
```bash
docker build -t server-python -f DockerServerPython .
docker run -p 12345:12345 server-python
```
