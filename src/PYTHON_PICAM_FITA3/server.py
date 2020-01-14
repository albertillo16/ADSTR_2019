#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  server.py
#  
#  Copyright 2019 Alberto <alberto@alberto-VirtualBox>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  
import socket                   # Import socket module
import os
from picamera import picamera
from time import sleep

port = 50000                    # Reserve a port for your service every new transfer wants a new port or you must wait.
s = socket.socket()             # Create a socket object
host = "localhost"   # Get local machine name
s.bind((host, port))            # Bind to the port
s.listen(1)                     # Now wait for client connection.

print 'Servidor escuchando'


while True:
	
    conn, addr = s.accept()     # Establish connection with client.
    print 'Conexión de', addr
    data = conn.recv(1024)
    print('Servidor recibido', repr(data))
    
    camera.start_preview()
    
    for i in range(1):
		sleep(0.5)
		camera.capture('/home/pi/Desktop/sockets_pyton/image.jpg')
    
    camera.stop_preview()
   	
    filename='/home/pi/Desktop/sockets_pyton/image.jpg.jpg' #In the same folder or path is this file running must the file you want to tranfser to be
    f = open(filename,'rb')
    l = f.read(1024)
    while (l):
       conn.send(l)
       print('Enviando ',repr(l))
       l = f.read(1024)
    f.close()
	
	
    print('Enviado')
    conn.send('Gracias por la conexión')
    conn.close()
