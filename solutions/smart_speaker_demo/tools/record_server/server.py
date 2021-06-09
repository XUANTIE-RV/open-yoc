#!/usr/bin/env python

from SimpleWebSocketServer.SimpleWebSocketServer import SimpleWebSocketServer, WebSocket

import sys
import os

if sys.argv[1] == "--help":
    print("Usage:")
    print("python server.py [interface]:[port] [/path/to/save]")
    print("eg. python server.py 192.168.1.102:9090 ./download")
    exit()

if sys.argv[1:]:
    address = sys.argv[1]
    if (':' in address):
        interface = address.split(':')[0]
        port = int(address.split(':')[1])
    else:
        interface = '0.0.0.0'
        port = int(address)
else:
    port = 9090
    interface = '0.0.0.0'

if sys.argv[2:]:
    os.chdir(sys.argv[2])

print(interface)
print(port)
# print(os.getcwd())

clients = []
class WsRec(WebSocket):
    def saveFile(self, data):
        fileName = None
        if not data:
            return
            
        #print("data type is", type(data))
        fileName, bindata = data.split(b"\0", 1)
        fileName = fileName.decode('utf-8')
        # print(fileName, ".")
        # print(bindata)
        if not fileName or not bindata:
            return
 
        if not self.fileCreate:
            self.fileCreate = True
            if os.path.exists(fileName):
                os.remove(fileName)
            
        with open(fileName,'ab+') as f:
            f.write(bindata)

    def handleMessage(self):
        # print("--->")
        for client in clients:
            if (client == self.address):
                # print(self.data)
                if (self.opcode == 0):
                    self.saveFile(self.data)

    def handleConnected(self):
        print(self.address, 'connected')
        # for client in clients:
        #     client.sendMessage(self.address[0] + u' - connected')
        #self.fileName = 'voice_factory_test.pcm'
        
        self.fileCreate = False
        clients.append(self.address)
        print("append ok")

    def handleClose(self):
        clients.remove(self.address)
        print(self.address, 'closed')
        # for client in clients:
        #     client.sendMessage(self.address[0] + u' - disconnected')

try:
    server = SimpleWebSocketServer(interface, port, WsRec)
    server.serveforever()

except KeyboardInterrupt:
    print('^C received, shutting down the web server')
    server.close()