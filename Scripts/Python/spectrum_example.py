import zmq
import random

context = zmq.Context()

print("Connecting via PUSH..")

socket = context.socket(zmq.PUSH)	
socket.bind("tcp://127.0.0.1:5551")


r = []

r = random.sample(xrange(1, 101), 11)

request = 0


print("Sending Data...")
print unicode(r)
socket.send_string(unicode(r))

