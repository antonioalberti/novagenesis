import zmq
import sys
context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect("tcp://192.168.1.214:5555")
#socket.connect("tcp://127.0.0.1:5555")
 
for i in range(10):
	#msg = "msg %s" % i
	#msg = "ON 802.15.4"
	msg=sys.argv[1]
	socket.send(msg)
	#print "Message ", msg
	 
	msg_in = socket.recv()
	print(msg_in)
	break
