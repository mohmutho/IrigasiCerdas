'''
import os
import xlrd
sensorA = os.path.join(os.path.dirname(__file__),'data','Dataset sensor A.xlsx')
sensorB = os.path.join(os.path.dirname(__file__),'data','Dataset sensor B.xlsx')
wb = xlrd.open_workbook(sensorA)
sheet = wb.sheet_by_index(0)
dataA = []
for i in range(sheet.nrows):
    temp = [sheet.cell_value(i,j) for j in range(sheet.ncols)]
    dataA.append(temp)
print(dataA)
'''

import _thread
import serial
import serial.tools.list_ports
from serial.serialutil import SerialException
#========================================================
#==================KONFIGURASI===========================
PORT = 5000
HOST = '127.0.0.1'
DEBUG = True
BAUDRATE = 19200
#========================================================
def listeningArduino():
    ports = serial.tools.list_ports.comports(include_links=False)
    usb = ''
    for port in ports:
        ser = serial.Serial(port.device, baudrate=BAUDRATE,)
        if ser.isOpen():
            ser.close()
            usb = port.device
            break
    if usb != '':
        ser = serial.Serial(usb, baudrate=BAUDRATE,)
    while True:
        data = ''
        try:
            if usb != '':
                data = ser.readline()
                data = data.decode().replace('\r\n','')
                if 'Soil Moisture Value 1 ' in data:
                    print('data sensor A',int(data[23:]))
                else:
                    print('data sensor B',int(data[23:]))
            else:
                raise SerialException
        except SerialException:
            ports = serial.tools.list_ports.comports(include_links=False)
            for port in ports:
                ser = serial.Serial(port.device, baudrate=BAUDRATE,)
                if ser.isOpen():
                    usb = port.device
                    break
        
_thread.start_new(listeningArduino,())
while True:
    pass