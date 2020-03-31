#!/usr/bin/python
import os
import sys
import socket
import thread
import time
import serial

server_address = '/tmp/mysocket'

# Make sure the socket does not already exist
try:
    os.unlink(server_address)
except OSError:
    if os.path.exists(server_address):
        raise

sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

# Bind the socket to the port
print >>sys.stderr, 'starting up on %s' % server_address
sock.bind(server_address)

# Listen for incoming connections
sock.listen(1)

changingstate = 0

# global variable for connection
connection = 0


# Forward all received lines to the socket client
def thread_seriallogger_reader(arg):

    with serial.Serial('/dev/ttyUSB0', 921600, timeout=2) as ser:
        while(True):
            line = ser.readline()   # read a '\n' terminated line
            if len(line) == 0:
                continue

            try:
                connection.sendall(line)
            except:
                print 'no client.  sleeping a bit'
                time.sleep(1)
                continue


thread.start_new_thread(thread_seriallogger_reader, ('arrrr',))



# Accept a connection and do nothing
while True:
    # Wait for a connection
    print >>sys.stderr, 'waiting for a connection'
    connection, client_address = sock.accept()
    try:
        print >>sys.stderr, 'connection from', client_address

        while True:
            try:
                connection.sendall("DATA FROM SERVER %d" % changingstate)
            except :
                break
            changingstate = changingstate + 1
            time.sleep(1)

    finally:
        # Clean up the connection
        connection.close()

