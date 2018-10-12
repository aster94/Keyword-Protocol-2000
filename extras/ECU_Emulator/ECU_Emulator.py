# -*- coding: utf-8 -*-
"""
Created on Fri Aug  4 12:39:40 2017

@author: Vincenzo G.
"""

import serial, time, sys
from random import randint

startCom = ["0x81", "0x12", "0xf1", "0x81", "0x5"]
startCom_ok = [0x80, 0xF1, 0x12, 0x03, 0xC1, 0xEA, 0x8F, 0xC0]
requestSens = ["0x80", "0x12", "0xf1", "0x2", "0x21", "0x8", "0xae"]
fakeSens = [0x80, 0xF1, 0x12, 0x34]
request = list()
ECUConnected = False
last_byte = 0
last_data_received = 0
default_port = '/dev/ttyUSB1' # ' ' for windows

ser = serial.Serial(
    port=default_port,
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout = 2
    #timeout = None
)

# send an array of int as binary data
def send (toSend):
    #print("Sending:\t" + str([hex(x) for x in toSend]))
    for i in toSend:
        ser.write(i.to_bytes(1,byteorder='little'))
        time.sleep(0.01)
    time.sleep(0.04)

# compare two string list
def compare(received,correct):
    #print ("\nReceived:\t" + str(received) + "\nCorrect:\t" + str(correct))
    return all(item==check for item,check in zip(received, correct))

# calculate the checksum
def calc_cs (arr):
    return sum(arr) & 0xFF
    
# listen from the board 
def listen():
    global last_data_received
    byte = 0
    last_byte = -1
    request.clear()
    start_time = time.time()
    elapsed_time = 0

    while (ser.in_waiting > 0):
        elapsed_time = time.time() - start_time
        
        while (elapsed_time < 2.0):
            in_bin = ser.read()
            in_hex = hex(int.from_bytes(in_bin, byteorder='little'))       
            #print ("Incoming byte " + str(byte) + ": " + in_hex)
            request.append(in_hex)
            
            if byte == 0:
                if in_hex == '0x81':
                    # we will receive only 4 more bytes
                    last_byte = 4
                elif in_hex == '0x80':
                    # we don't know yet
                    last_byte = -1
            
            if byte == 3 and last_byte == -1:
                last_byte = int(in_hex, 0) + 4 #3 headers + 1 checksum
                
            if (byte == last_byte):
                # check the checksum
                cs = hex(calc_cs(int(x,0) for x in request[0:byte]))
                if in_hex == cs:
                    #print("request received: " + str(request))
                    last_data_received = time.time()
                    return True
                else:
                    print("wrong checksum!")
                    print(request)
                    print(cs)
                    return False
            byte += 1
    return False



# choose serial port
'''
print ("default port is: '" + default_port + "' write if different")
user_input = input("Serial:") or default_port
print('Choosed port:', user_input)
'''

# start
print("Starting ECU Emulator")

try:
    if (ser.is_open):
        print("port already opened")
    else:
        ser.open()
    
    print("Port Info:\n"+str(ser))
except Exception as e:
    print ("error open serial port: " + str(e))
    sys.exit()


while 1:
    # connection expired
    if (time.time() - last_data_received > 2):
        if ECUConnected == True: print("Connection expired")
        ECUConnected = False
        
    if (listen()):
        # we received something
        if ECUConnected == False:
            if compare(request,startCom):
                send(startCom_ok)
                print('\nConnection established \n')
                ECUConnected = True
                
            else:
                print('you should send me the init sequence!\n')
                
        elif ECUConnected:
            if compare(request,startCom):
                print("already connected, we are waiting for the sensor request!")
            
            elif compare(request,requestSens):
                print("\nSending fake sensor data \n")
                for i in range(0,52):
                    fakeSens.append(randint(0,255))
                fakeSens.append(calc_cs(fakeSens))
                send(fakeSens)
                del fakeSens[4:]
                
            else:
                print("unkown request\n")
