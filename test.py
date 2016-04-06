import socket
import time

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect(('', 8080))
while(1):
    time.sleep(5)
    s.send("Hello world.")
time.sleep(0.1)
s.close()
