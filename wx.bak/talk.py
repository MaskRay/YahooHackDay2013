#coding:utf8
import socket  
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
sock.connect(('127.0.0.1',5000))  
import time  
time.sleep(2)  
sock.send("hello")
print sock.recv(1024)  
sock.close()  

