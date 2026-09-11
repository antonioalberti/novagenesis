import os
import zmq
import sys
context = zmq.Context()
socket = context.socket(zmq.REQ)
endpoint = os.environ.get("ZMQ_ENDPOINT")
if not endpoint:
    raise SystemExit("Set ZMQ_ENDPOINT, for example tcp://<host>:5555")
socket.connect(endpoint)
 
for i in range(10):
	#msg = "msg %s" % i
	#msg = "ON 802.15.4"
	msg=sys.argv[1]
	socket.send(msg)
	#print "Message ", msg
	 
	msg_in = socket.recv()
	print(msg_in)
	break
