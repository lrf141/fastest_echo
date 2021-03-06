#!/usr/bin/env python
# -*- coding: utf-8 -*-
#This code was written with reference to this page: http://rabbitfoot141.hatenablog.com/entry/2018/01/22/111550

import socket

target = "localhost"
port = 8888
message = "hello,world!"

#using IPv4 and TCP/IP
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#connecting
client_socket.connect((target, port))

#data send
client_socket.send(message.encode('utf-8'))

#get data
response_data = client_socket.recv(4096) 

print(response_data.decode(encoding='utf-8'))
