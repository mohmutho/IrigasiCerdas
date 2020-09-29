import os
import xlrd
import xlwt
from xlutils.copy import copy as xlcopy
from flask import Flask, request, render_template
import json
from optimize import calculate
import threading
import serial
import serial.tools.list_ports
from serial.serialutil import SerialException
import time
#========================================================
#==================KONFIGURASI===========================
PORT = 5000
HOST = '127.0.0.1'
DEBUG = True
BAUDRATE = 115200
#========================================================
def listeningArduino():
    ports = serial.tools.list_ports.comports(include_links=False)
    usb = ''
    for port in ports:
        ser = serial.Serial(port.device, baudrate=BAUDRATE,timeout=None)
        if ser.isOpen():
            usb = port.device
            break
    if not os.path.isfile(os.path.join('data','Sensor Arduino A.xlsx')):
        
        workbook = xlwt.Workbook()
        sheet = workbook.add_sheet('data', cell_overwrite_ok=True)
        sheet.write_merge(0,0,0,6,'Sensor A')
        sheet.write_merge(1,2,0,0,'Jam ke-')
        sheet.write_merge(1,1,1,6,'12 Jam ke-')
        for i in range(12):
            sheet.write(3+i,0,i+1)
        workbook.save(os.path.join('data','Sensor Arduino A.xlsx'))
        
        workbook = xlwt.Workbook()
        sheet = workbook.add_sheet('data', cell_overwrite_ok=True)
        sheet.write_merge(0,0,0,6,'Sensor B')
        sheet.write_merge(1,2,0,0,'Jam ke-')
        sheet.write_merge(1,1,1,6,'12 Jam ke-')
        for i in range(12):
            sheet.write(3+i,0,i+1)
        workbook.save(os.path.join('data','Sensor Arduino B.xlsx'))

    def writeXlsx(target,data,tipe,ser):
        workbook = xlrd.open_workbook(os.path.join('data',f'{target}.xlsx'))
        sheet = workbook.sheet_by_index(0)
        x = 3
        y = 1
        berubah = False
        alldata = []
        try:
            while sheet.cell_value(x,y) is not None and sheet.cell_value(x,y) != '':
                x+=1
                if x > 14:
                    x = 3
                    y += 1
                    berubah = True
                    alldata.append([data])
                else:
                    berubah = False
                    alldata.append([float(sheet.cell_value(x,y))])
        except:
            pass
        if berubah:
            gb = calculate(alldata[-12:])
            ser.write(f"{tipe}:{gb[0][0]}\n".encode())
        wb = xlcopy(workbook)
        ws = wb.get_sheet(0)
        ws.write(2,y,y)
        ws.write(x,y,data)
        ws.write_merge(0,0,0,y,'Sensor B')
        ws.write_merge(1,2,0,0,'Jam ke-')
        ws.write_merge(1,1,1,y,'12 Jam ke-')
        wb.save(os.path.join('data',f'{target}.xlsx'))
    data = ''
    while True:
        
        try:
            while ser.in_waiting > 0:
                t = ser.readline()
                data = t.decode()
                print(data)
                data = data.replace('\r\n','')
                if 'Soil Moisture Value 1 ' in data:
                    print('data sensor A',float(data[23:]))
                    writeXlsx('Sensor Arduino A',float(data[23:]),'A',ser)
                elif 'Soil Moisture Value 2 ' in data:
                    print('data sensor B',float(data[23:]))
                    writeXlsx('Sensor Arduino B',float(data[23:]),'B',ser)
                    
                else:
                    pass
        except OSError as e:
            ports = serial.tools.list_ports.comports(include_links=False)
            for port in ports:
                ser = serial.Serial(port.device, baudrate=BAUDRATE,timeout=0)
                if ser.isOpen():
                    usb = port.device
                    break
            print(e)
        except SerialException as e:
            if not ser.is_open:
                ports = serial.tools.list_ports.comports(include_links=False)
                for port in ports:
                    ser = serial.Serial(port.device, baudrate=BAUDRATE,timeout=0)
                    if ser.isOpen():
                        usb = port.device
                        break
        except ValueError as e:
            print('serial error',e)
            pass
        time.sleep(1)
x = threading.Thread(target=listeningArduino, args=())
x.start()
def resourceFiles():
    return [i for i in os.listdir(os.path.join(os.path.dirname(__file__),'data')) if '.xlsx' in i and len(i.split('.')) == 2]

def updateParameter(wb,index,data):
    rdata = [wb,
        index,
        calculate(data)
    ]
    with open(os.path.join(os.path.dirname(__file__),'output','1.json'),'w') as f:
        json.dump(rdata,f)
def retrieveParameter():
    with open(os.path.join(os.path.dirname(__file__),'output','1.json'),'r') as f:
        rdata=json.load(f)
    return rdata
app = Flask(__name__)

@app.route('/')
def index():
    files = []
    data = []
    for i in resourceFiles():
        sensor = os.path.join(os.path.dirname(__file__),'data',i)
        files.append(i.split('.')[0])
        wb = xlrd.open_workbook(sensor)
        sheet = wb.sheet_by_index(0)
        temp2 = []
        for i in range(sheet.nrows):
            temp = [sheet.cell_value(i,j) for j in range(sheet.ncols)]
            temp2.append(temp)
        data.append(temp2)
    return render_template('index.html',data=data, files=files, title='Penampil Algoritma Particle Swarm Optimization',sidebar=0)

@app.route('/parameter',methods=['GET','POST'])
def parameter():
    if request.method == 'GET' and os.path.isfile(os.path.join(os.path.dirname(__file__),'output','1.json')):
        sensor,index,data = retrieveParameter()
    else:
        sensor = request.form.get('sensor',resourceFiles()[0])
        index = int(request.form.get('index','0'))
    source_file = os.path.join(os.path.dirname(__file__),'data',sensor)
    wb = xlrd.open_workbook(source_file)
    sheet = wb.sheet_by_index(0)
    data = []
    for i in range(sheet.nrows):
        temp = [sheet.cell_value(i,j) for j in range(sheet.ncols)]
        data.append(temp)
    alldata = []
    for i in resourceFiles():
        sensor = os.path.join(os.path.dirname(__file__),'data',i)
        wb = xlrd.open_workbook(sensor)
        sheet = wb.sheet_by_index(0)
        temp2 = []
        for i in range(sheet.nrows):
            temp = [sheet.cell_value(i,j) for j in range(sheet.ncols)]
            temp2.append(temp)
        alldata.append(temp2)
    if request.method == 'POST':
        updateParameter(sensor,index,[[i[index+1]] for i in data[3:]])
        print([[i[index+1]] for i in data[3:]])
    return render_template('parameter.html',title='Penampil Algoritma Particle Swarm Optimization',sidebar=1, alldata=alldata, files=resourceFiles(),data=data, index=index, sensor=os.path.basename(sensor))

@app.route('/perhitungan', methods=['GET','POST'])
def perhitungan():
    calculation = None
    if request.method == 'GET' and os.path.isfile(os.path.join(os.path.dirname(__file__),'output','1.json')):
        sensor,index,calculation = retrieveParameter()
    else:
        sensor = request.form.get('sensor',resourceFiles()[0])
        index = int(request.form.get('index','0'))
    source_file = os.path.join(os.path.dirname(__file__),'data',sensor)
    if calculation is None:
        wb = xlrd.open_workbook(source_file)
        sheet = wb.sheet_by_index(0)
        data = []
        for i in range(sheet.nrows):
            temp = [sheet.cell_value(i,j) for j in range(sheet.ncols)]
            data.append(temp)
        updateParameter(sensor,index,[[i[index+1]] for i in data[3:]])
        sensor,index,calculation = retrieveParameter()
    return render_template('perhitungan.html',title='Penampil Algoritma Particle Swarm Optimization',sidebar=1,data=calculation[-1])
@app.route('/visualisasi', methods=['GET','POST'])
def visualisasi():
    calculation = None
    if request.method == 'GET' and os.path.isfile(os.path.join(os.path.dirname(__file__),'output','1.json')):
        sensor,index,calculation = retrieveParameter()
    else:
        sensor = request.form.get('sensor',resourceFiles()[0])
        index = int(request.form.get('index','0'))
    source_file = os.path.join(os.path.dirname(__file__),'data',sensor)
    if calculation is None:
        wb = xlrd.open_workbook(source_file)
        sheet = wb.sheet_by_index(0)
        data = []
        for i in range(sheet.nrows):
            temp = [sheet.cell_value(i,j) for j in range(sheet.ncols)]
            data.append(temp)
        updateParameter(sensor,index,[[i[index+1]] for i in data[3:]])
        sensor,index,calculation = retrieveParameter()
    local_best = []
    local_value = []
    global_best = []
    global_value = []
    for i in calculation[-1]:
        for j in i[0]:
            t = []
            for k in j:
                t.append(k[0])
            local_best.append(t)
        for j in i[1]:
            local_value.append(j)
        for j in i[-2]:
            global_best.append(j)
        global_value.append(i[-1])
    label = '[1,2]'
    dataset = []
    dataset2 = []
    for index,g in enumerate(global_best):
        data_pertama = []
        for j,jval in enumerate(local_best[index]):
            data_pertama.append("{x: %f, value: %f}" % (jval,local_value[index][j]))
        data_pertama = '['+",".join(data_pertama)+']'
        data_kedua = '[{x: %f, value: %f}]' % (g,global_value[index])
        dataset2.append((data_pertama, data_kedua))
        dataset.append([
            label,[local_best[index],[g]]
        ])
    return render_template('visualisasi.html',title='Penampil Algoritma Particle Swarm Optimization',sidebar=1,data=dataset2,global_best=calculation[0])

if __name__ == "__main__":
    app.run(HOST,PORT, debug=DEBUG)