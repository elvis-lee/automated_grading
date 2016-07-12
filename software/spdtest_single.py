#! /usr/bin/python
import serial
from timeit import Timer
import timeit
import string
import random

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
 for i in range(1,10):      #Sending out 10MB in total
  data = id_generator(1048576)  #1048576Bytes=1MB
  ser.write(data)
  print "Time=%d Sent=%dMB" %(i,i)     
 return


    
t=Timer(com1,setup="") 
t1=t.timeit(1)
print "Total Time=%fs" %(t1) 
print "Baud Rate=%d" %(BAUDRATE)  
print "Throughout=%fKB/s" %(10240/t1)
ser.close()
