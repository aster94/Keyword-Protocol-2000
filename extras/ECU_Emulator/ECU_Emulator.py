# -*- coding: utf-8 -*-
"""
Created on Fri Aug  4 12:39:40 2017

@author: Vincenzo G.
"""

import serial, time, sys
from random import randint

default_port = '/dev/ttyUSB1' # 'COMx' for windows '/dev/ttyUSBx' for linux
this_address = 0xf1
ecu_address = 0x12
P2m = 50
P2M = 3000
P3m = 80
P3M = 2000
P4m = 10


start_com = [0x81, ecu_address, this_address, 0x81, 0x5]
start_com_ok = [0x80, this_address, ecu_address, 0x03, 0xC1, 0xEA, 0x8F]

stop_com = [[0x81, ecu_address, this_address, 0x82, 0x6],
            [0x80, ecu_address, this_address, 0x1, 0x82, 0x6]]
stop_com_ok = [0x80, this_address, ecu_address, 0x01, 0xC2]

atp_current = [[0x80, ecu_address, this_address, 0x2, 0x83, 0x02, 0xa],
               [0x82, ecu_address, this_address, 0x83, 0x02, 0xa]]
atp_current_ok = [0x80, this_address, ecu_address, 0x07 , 0xC3, 0x2, P2m, int(P2M / 25), P3m, int(P3M / 25), P4m]

atp_limits = [[0x80, ecu_address, this_address, 0x2, 0x83, 0x00, 0x8],
              [0x82, ecu_address, this_address, 0x83, 0x00, 0x8]]
atp_limits_ok = [0x80, this_address, ecu_address, 0x07 , 0xC3, 0x0, 1, 2, 3, 4, 1]

request_sens = [[0x80, ecu_address, this_address, 0x2, 0x21, 0x8, 0xae],
                [0x82, ecu_address, this_address, 0x21, 0x8, 0xae]]
fake_sens = [0x80, this_address, ecu_address, 0x34]

reject_request = [0x80, this_address, ecu_address, 0x01, 0x7F]

request = list()
ECU_connected = False
ATP_setted = False
ATP_asked = False
last_byte = 0
last_data_received = 0

ser = serial.Serial(
    port=default_port,
    baudrate=115200,
    parity=serial.PARITY_ODD,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout = 2
    #timeout = None
)


# send an array of int as binary data
def send (to_send):
    to_send.append(calc_cs(to_send))
    #print("Sending:\t" + str([hex(x) for x in to_send]))
    for i in to_send:
        ser.write(i.to_bytes(1,byteorder='little'))
        time.sleep(P4m/1000.0)
    time.sleep(P2m/1000.0)


# compare two list
def compare(received,correct):
    #print (received)
    temp = list()
    if (type(correct[0]) != list):
        temp.append(correct)
    else:
        temp = correct
    for element in temp:
        coincident = 0
        l = len(element)
        for n in range(0,l):
            try:
                #print (element[n], int(received[n], 16))
                if (element[n] == int(received[n], 16)):
                    coincident += 1
            except IndexError:
                next
        if (coincident == l):
            return True
    return False


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
        
        while (elapsed_time < (P3M/1000.0)):
            in_bin = ser.read()
            in_hex = hex(int.from_bytes(in_bin, byteorder='little'))       
            #print ("Incoming byte " + str(byte) + ": " + in_hex)
            request.append(in_hex)
            
            # send back the byte we just received
            ser.write(in_bin)
            
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
                    time.sleep(P2m/1000.0)
                    return True
                else:
                    print("wrong checksum!")
                    print(request)
                    print(cs)
                    time.sleep(P2m/1000.0)
                    return False
            
            byte += 1
    return False


################### START ###################

print("Starting ECU Emulator")
try:
    if (ser.is_open):
        print("Port already opened")
    else:
        ser.open()
    print("Port Info:\n", str(ser))
except Exception as e:
    print ("Error open serial port: " + str(e))
    sys.exit()

while 1:
    # connection expired
    if (time.time() - last_data_received > P3M/1000.0):
        if ECU_connected == True: print("Connection expired")
        ECU_connected = False
        ATP_setted = False
        request.clear()
        
    if (listen()): # we received something
        
        if not ECU_connected:
            if compare(request,start_com):
                print('\nConnection established\n')
                send(start_com_ok)
                ECU_connected = True   

            else:
                print('You should send me the init sequence! not this:')
                print(request)
                
        elif ECU_connected and not ATP_asked and not ATP_setted:
            if compare(request,atp_limits):
                print('ATP limits asked\n')
                send(atp_limits_ok)
                ATP_asked = True
                
            elif compare (request, stop_com):
                print("The tester closed the link")
                send(stop_com_ok)
                ECU_connected = False

            else:
                print('You should ask me the ATP limits! not this:')
                print(request)
        
        elif ECU_connected and ATP_asked and not ATP_setted:
            if compare(request,atp_current):
                print('Sending ATP response\n')
                send(atp_current_ok)
                ATP_setted = True #or does the ecu answer back?
                
            elif compare (request, stop_com):
                print("The tester closed the link")
                send(stop_com_ok)
                ECU_connected = False
                
            else:
                print("Unkown request:")
                print(request)
        
        elif ECU_connected and ATP_setted:
            if compare(request,start_com):
                print("Already connected")
            
            elif compare(request,request_sens):
                print("Sending fake sensor data\n")
                for i in range(0,52):
                    fake_sens.append(randint(0,255))
                send(fake_sens)
                del fake_sens[4:]
            
            elif compare (request, stop_com):   
                print("The tester closed the link")
                send(stop_com_ok)
                ECU_connected = False
            
            else:
                print("Unkown request:")
                print(request)
