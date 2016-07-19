#! /usr/bin/python
import serial
from timeit import Timer
import timeit
import string
import random
import time

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
 for i in range(1,11001):      #Sending out 10MB in total
  data = id_generator(10,) 
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
