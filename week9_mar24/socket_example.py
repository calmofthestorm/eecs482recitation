#!/usr/bin/env python
# encoding: utf-8

import socket
import struct
import sys

INTEGER_SIZE = len(struct.pack('i', 0))

def server():
  connection = None
  try:
    my_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    my_socket.bind((sys.argv[2], int(sys.argv[3])))
    my_socket.listen(10)

    while 1:
      connection, (remote_host, remote_port) = my_socket.accept()
      buf = []

      # Read string length (native int)
      olen = length = struct.unpack('i', connection.recv(INTEGER_SIZE))[0]

      # Read string.
      while length > 0:
        tmp = connection.recv(length)
        if tmp:
          buf.append(tmp)
          length -= len(tmp)

      # Convert to string
      buf = ''.join(buf)
      assert(len(buf) == olen)

      # Print string.
      print('%s:%i - %s' % (remote_host, remote_port, buf))

      # Send response -- number bytes read.
      connection.send(struct.pack('i', len(buf)))
  finally:
    # We're done here.
    if connection is not None:
      connection.close()

def client():
  my_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  my_socket.connect((sys.argv[2], int(sys.argv[3])))

  # Read string from stdin
  data = sys.stdin.read()

  # Send header
  my_socket.send(struct.pack('i', len(data)))

  # Send data
  count = 0
  while count < len(data):
    count += my_socket.send(data[count:])

  # Receive confirmation
  conf = struct.unpack('i', my_socket.recv(INTEGER_SIZE))[0]

  if conf == len(data):
    print("Success -- sent %i bytes to server." % conf)
  else:
    print("Server reported %i bytes but we sent %i." % (conf, len(data)))

if __name__ == '__main__':
  if len(sys.argv) != 4 or sys.argv[1] not in ("client", "server"):
    print("Usage: %s client|server host/interface port" % sys.argv[0])
  elif sys.argv[1] == "client":
    client()
  elif sys.argv[1] == "server":
    server()
