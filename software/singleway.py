#! /usr/bin/python
import serial
from timeit import Timer
import timeit
import string
import random
import time
import ctypes
import struct

BAUDRATE=1000000

def id_generator(size=6, chars=string.ascii_uppercase + string.digits):
    return ''.join(random.choice(chars) for _ in range(size))


ser = serial.Serial()
ser.port = "/dev/ttyUSB0"    
ser.baudrate = BAUDRATE    
ser.parity=serial.PARITY_NONE
ser.bytesize=serial.EIGHTBITS
ser.stopbits=serial.STOPBITS_ONE
ser.timeout=None
ser.writeTimeout=None
ser.open()  
def com1():
 tvalue = 110050 
 for i in range(1,6001):  
  tvalue = tvalue - 1;   
  tsend =  struct.pack('I',(ctypes.c_uint32(tvalue).value))
  if (i%2):
   v = b'\xFF\x80'
  else:
   v = b'\x00\x00'
  data = 'ST'
  print data
  ser.write(data)
  data = tsend
  print data
  ser.write(data)
  data = v
  print data
  ser.write(data)
  data = 'CE'
  print data
  ser.write(data)
  print "Time=%d Sent=%d packets" %(i,i) 
 received = ser.read(8)
 print received 
    
 return


    
t=Timer(com1,setup="") 
t1=t.timeit(1)
print "Total Time=%fs" %(t1) 
print "Baud Rate=%d" %(BAUDRATE)  

ser.close()