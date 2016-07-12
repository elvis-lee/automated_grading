#! /usr/bin/python
import serial
from timeit import Timer
import timeit
import string
import random

def id_generator(size=6, chars=string.ascii_uppercase + string.digits):
    return ''.join(random.choice(chars) for _ in range(size))

success=0
fail=0
ser = serial.Serial()
ser.port = "/dev/ttyUSB0"    
ser.baudrate = 1000000
ser.parity=serial.PARITY_NONE
ser.bytesize=serial.EIGHTBITS
ser.stopbits=serial.STOPBITS_ONE
ser.timeout=None
ser.writeTimeout=None
ser.open()  
#data = "S"
def com1():   
  global success
  global fail 
  global data
  for i in range(0,1000):
   print i
   data = id_generator(1,) 
   ser.write(data)    
   print "Time=%d  sent" %i  
   received=ser.read(1)
   print "Time=%d  received" %i
   if data==received:
    success=success+1
    print "Time=%d transfer success!" %i
   else:
    fail=fail+1
    print "Time=%d transfer fail!" %i
  return
 
t=Timer(com1,setup="") 
print t.timeit(1)    
print "success time = %d, fail time = %d" %(success,fail)
ser.close()
