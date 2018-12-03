import serial
import csv
import json
import glob
import os.path
from os import path
from datetime import datetime



# serialport = serial.Serial("/dev/cu.usbmodem14101", 9600, timeout=0.5)

# if(path.exists("/Users/DylanMyers/Desktop/projects/droneApplication/data.csv") == True):
# 	f = open('/Users/DylanMyers/Desktop/projects/droneApplication/data.csv', 'a')
# 	f.write("\n");
# else:
# 	f = open('/Users/DylanMyers/Desktop/projects/droneApplication/data.csv', 'w')
# 	f.write("Datetime,Latitude,Longitude,Altitude\n")
# while True:
#     command = serialport.readline().decode()
#     f.write(str(datetime.now()))
#     f.write(',')
#     f.write(str(command))
#     print(str(command))
#     f.write("\n")
#     f.flush()
#     # print(str(datetime.now()))
    
# f.close()

for filename in glob.glob('/Users/DylanMyers/Desktop/projects/droneApplication/data.csv'):
	csvfile = os.path.splitext(filename)[0]
	jsonfile = csvfile + '.json'

	with open(csvfile+'.csv') as f:
	    reader = csv.DictReader(f)
	    rows = list(reader)
	with open(jsonfile, 'w') as f:
		json.dump(rows, f)
