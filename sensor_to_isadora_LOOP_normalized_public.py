"""Small example OSC client

This program sends 10 random values between 0.0 and 1.0 to the /filter address,
waiting for 1 seconds between each value.
https://pypi.org/project/python-osc/
"""
import argparse
import random
import time
import requests
import json
import pandas as pd
import numpy as np
import math

from pythonosc import udp_client
from pythonosc.dispatcher import Dispatcher
from pythonosc import osc_bundle_builder
from pythonosc import osc_message_builder

ip="127.0.0.1"
port=1234
client = udp_client.SimpleUDPClient(ip, port)

""" sensor list array to hold list of sensors to look up. cycles through one at a time to avoid read throttling. alterantely use a csv file
""" #100000 series prototype, 200000 is artist proof, 300000 is first production run
sensor_list = [100070, 100072, 100200, 100203, 100205] 
#sensor_list=[]
url='https://data.domesticlight.art/api/dynamodb/dy/excute'
headers = {
    'Content-type': 'application/json',
    'origin':'*',
}

data_keys=['F1_415','F2_445','F3_480','F4_515','F5_555',
    'F6_590','F7_630','F8_680','F9_VIS','F10_NIR',
    'ASTEP','ATIME','GAIN','RTCTemp','UUID','MAC_ID','unixtime',]
## 'MAC_ID', 'unixtime' removed

data_key_types={'F1_415': 'N',
    'F2_445': 'N',
    'F3_480': 'N',
    'F4_515': 'N',
    'F5_555': 'N',
    'F6_590': 'N',
    'F7_630': 'N',
    'F8_680': 'N',
    'F9_VIS': 'N',
    'F10_NIR': 'N',
    'ASTEP': 'N',
    'ATIME': 'N',
    'GAIN': 'N',
    'RTCTemp': 'N',
    'UUID': 'S',
    'device_id': 'N',
    'MAC_ID': 'S',
    'sample_time': 'N',
    'unixtime': 'N'}
  
    ##  'MAC_ID': 'S',
    ##  'unixtime': 'N',

def convert_number(s):
    if '.' in s:
        return float(s)
    else:
        return int(s)
    
def parse_data(raw_data):
    d0={}
    for idx, key in enumerate(data_keys):
        d_type=data_key_types[key]
        val = raw_data['device_data']['M'][key][d_type]
        d0[key]= val
    # d0=[{key: raw_data['device_data']['M'][key][data_key_types[idx]]} for idx, key in enumerate(data_keys)]
    d0['device_id']=raw_data['device_id']['N']
    d0['sample_time']=raw_data['sample_time']['N']
    for key in d0.keys():
        val=d0[key]
        d0[key]=convert_number(val) if(data_key_types[key]=='N') else val
    return d0

def getGainVal(gain):
    return 0.5 * pow(2, gain)

# Correct raw data by gain and integration
def getBasicCounts(row):
    gainVal = getGainVal(int(row.GAIN))
    integrationTime=(int(row.ATIME) + 1) * (int(row.ASTEP) + 1) * 2.78 / 1000
    keys=["F1_415","F2_445","F3_480","F4_515","F5_555","F6_590","F7_630","F8_680","F9_VIS","F10_NIR"]
    return [int(row[key])/(gainVal*integrationTime) for key in keys]

# Interesting removing baseline after correcting for gain and integration. 
def getCorectSensorData(basicCounts):
    offsets=[0.00197,0.00725,0.00319,0.00131,0.00147,
        0.00186,0.00176,0.00522,0.00300,0.00100]
    factors=[1.02811,1.03149,1.03142,1.03125,1.03390,
        1.03445,1.03508,1.03359,1.23384,1.26942]
    return [(val-offsets[i])*factors[i] for (i, val) in enumerate(basicCounts)]

def basicCounts2XYZ(counts):
    mat=np.array([[0.39814,1.29540,0.36956,0.10902,0.71942,
        1.78180,1.10110,-0.03991,-0.27597,-0.02347],
        [0.01396,0.16748,0.23538,1.42750,1.88670,
        1.14200,0.46497,-0.02702,-0.24468,-0.01993],
        [1.95010,6.45490,2.78010,0.18501,0.15325,
        0.09539,0.10563,0.08866,-0.61140,-0.00938]])
    vec = np.array(getCorectSensorData(counts))
    return np.matmul(mat, vec)

def raw2XYZ(raw):
    mat=np.array([[0.39814,1.29540,0.36956,0.10902,0.71942,
        1.78180,1.10110,-0.03991,-0.27597,-0.02347],
        [0.01396,0.16748,0.23538,1.42750,1.88670,
        1.14200,0.46497,-0.02702,-0.24468,-0.01993],
        [1.95010,6.45490,2.78010,0.18501,0.15325,
        0.09539,0.10563,0.08866,-0.61140,-0.00938]])
    vec = np.array(getCorectSensorData(getBasicCounts(raw)))
    return np.matmul(mat, vec)

def XYZ2RGBI(xyz):
    XYZ2RBG_Mat=np.array([[ 3.2406, -1.5372, -0.4986 ],
        [-0.9689,  1.8758,  0.0415 ],
        [ 0.0557, -0.2040,  1.0570 ]])
    rgb01=np.matmul(XYZ2RBG_Mat, xyz)
    rgb02=[v*12.92 if v<=0.0031308 else (1.055 * pow(v, (1 / 2.4)) - 0.055)
        for v in rgb01]
    # let's normalize rgb02
    level=math.sqrt(sum([pow(v,2) for v in rgb02]))
    rgbi=[min(max(v/level, 0), 1) for v in rgb02] + [level]
    # rgbi= [min(max(v, 0), 1) for v in rgb02] + [1.0]
    # print(rgbi)
    return rgbi
    

def get_latest_sensor_reading(sensor):
    """ trying to use string concatenation to define the device_id as a variable fromt the sensor list to iterate over. Breaks here
    """
    ctime=round(time.time()*1000)
    data={
        "Statement": "SELECT * FROM DL_data where device_id = " + str(sensor) + " AND sample_time > ?",
        "Parameters": [{"N": f"{ctime-1*10*1000}"}],
        "ConsistentRead": True,
    }

    data_json=json.dumps(data)
    data_json

    response=requests.post(url, data=data_json, headers=headers)
    # print("status_code", response.status_code)
    # print("text", response.text)
    
    if(response.status_code!=200):
        print('error:', response.status_code)
        return ""

    res=response.json()['data']['Items']

    res_data = response.json()['data']['Items']
    if(len(res_data)==0):
        # print('!!! no data during last 1 minute !!!')
        return pd.DataFrame([])

    d0=pd.DataFrame([parse_data(raw) for raw in res_data])
    # print(d0.shape[0], 'devices found last 1 minute')
    # Find latest reading for each device_id
    d1=d0.loc[d0.groupby("device_id")["sample_time"].idxmax()].copy().reset_index(drop=True)
    # print(d1)
    # d1['R']=d1.apply(lambda r: XYZ2RGB(raw2XYZ(r))[0], axis=1)
    # d1['G']=d1.apply(lambda r: XYZ2RGB(raw2XYZ(r))[1], axis=1)
    # d1['B']=d1.apply(lambda r: XYZ2RGB(raw2XYZ(r))[2], axis=1)
    d1['R']=d1.apply(lambda r: XYZ2RGBI(raw2XYZ(r))[0], axis=1)
    d1['G']=d1.apply(lambda r: XYZ2RGBI(raw2XYZ(r))[1], axis=1)
    d1['B']=d1.apply(lambda r: XYZ2RGBI(raw2XYZ(r))[2], axis=1)
    d1['I']=d1.apply(lambda r: XYZ2RGBI(raw2XYZ(r))[3], axis=1)
    # print(d1['R'], d1['G'], d1['B'], d1['I'])
    print(d1[['R', 'G', 'B', 'I']])
    return d1


def sensor_to_isadora():

    """ adding for loop here to iterate over the sensor list
    """
    for sensor in sensor_list:
     
        d1=get_latest_sensor_reading(sensor)
        num_records=d1.shape[0]
        # print('num_records:', num_records )
        records=d1.to_dict('records')
        # records

        for x in range(num_records):
            record=records[x]
            address = "/"+str(sensor)
            print("sending device_id:", record['device_id'], 'to', address)
            bundle = osc_bundle_builder.OscBundleBuilder(
            osc_bundle_builder.IMMEDIATELY)
            msg = osc_message_builder.OscMessageBuilder(address=address)

            for key in ['R', 'G', 'B', 'I',]+ data_keys:
                msg.add_arg(record[key])
                bundle.add_content(msg.build())

            sub_bundle = bundle.build()
            # Now add the same bundle inside itself.
            bundle.add_content(sub_bundle)
            # The bundle has 5 elements in total now.

            bundle = bundle.build()
            # You can now send it via a client as described in other examples.

            # client.send_message("/filter", random.random())
            client.send(bundle)

           # time.sleep(.02)

if __name__ == "__main__":
  # parser = argparse.ArgumentParser()
  # parser.add_argument("--ip", default="127.0.0.1",
  #     help="The ip of the OSC server")
  # parser.add_argument("--port", type=int, default=1234,
  #     help="The port the OSC server is listening on")
  # args = parser.parse_args()

  # client = udp_client.SimpleUDPClient(args.ip, args.port)

    for x in range(86400):
        print(x)
        try:
            sensor_to_isadora()
        except ValueError as v:
            print("no readings. DataFrame is empty", v)
            
        except Exception as inst:
            print(type(inst))
            print(inst.args)  
            print(inst) 
            print("Something else went wrong line 215, most likely no connection") 
        # time.sleep(.01)
