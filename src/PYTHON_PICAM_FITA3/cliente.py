#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  cliente.py
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

s = socket.socket()             # Create a socket object
host = "localhost"  #Ip address that the TCPServer  is there
port = 50000                     # Reserve a port for your service every new transfer wants a new port or you must wait.

s.connect((host, port))
s.send("Conexión establecida con el servidor!")

with open('solar_panel_image', 'wb') as f:
    print 'Fichero abierto'
    while True:
        print('Recibiendo datos...')
        data = s.recv(1024)
        #print('data=%s', (data))
        if not data:
            break
        # write data to a file
        f.write(data)

f.close()
print('Obtención del archivo exitoso')
s.close()
print('Conexión cerrada')
