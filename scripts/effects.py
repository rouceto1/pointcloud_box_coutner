#!/usr/bin/env python
import rospy
import collections
from std_msgs.msg import Float32
import numpy as np
import json
import socket
import random
rolling_window_size = 10

rolling_1 = collections.deque(maxlen=rolling_window_size)
rolling_2 = collections.deque(maxlen=rolling_window_size)
old_avg1 = old_avg2 = 0.0

UDP_IP = "192.168.8.100"
UDP_PORT = 6006

new_data1=0
new_data2=0


state1 = 0
state2 = 0
msgId = 0


def callback1(data):
    global new_data1
    rolling_1.append(data.data)
    new_data1 = 1
    #print(data.data)
 
def callback2(data):
    global new_data2
    rolling_2.append(data.data)
    new_data2 = 1
    #print(data.data)


def make_test_json(i,width_of_wave,speed_of_wave,rgb):
    global msgId
#    width_of_wave = 3 #[0,22]
#    speed_of_wave = dif_2 #[5,150]
#    rgb= [dif_1,dif_1,dif_1]
    out = []
           
    #wave + stripindex[0,3] + ;  + width of wave 
    msg = "wave:" + str(i) + ";" + str(width_of_wave)  + ";" + str(speed_of_wave) + ";" + str(rgb[0]) + ";" + str(rgb[1]) + ";" + str(rgb[2]) + ";" + str(msgId) + ";\0" 
    out.append(msg)
    msgId +=1
    return out

def send_pulse(rgb,avg,dif):
	
#	out_files,msgId = make_test_json(no1,no2,avg1,avg2,dif1,dif2,msgId)
	i = random.randint(0,2)	
	width_of_wave = int(avg/100);	
	speed_of_wave = int(dif)
	out_files = make_test_json(i,width_of_wave,speed_of_wave,rgb)

	for out in out_files:
		sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		sock.sendto(bytes(out,"utf-8"), (UDP_IP, UDP_PORT))
		print(out)
	

def make_json(r_1,r_2, avg_1,avg_2,dif_1,dif_2):
    x = {
        "place1_raw": r_1,
        "place2_raw": r_2,
        "place1_avg": avg_1,
        "place2_avg": avg_2,
        "place1_diff": dif_1,
        "place2_diff": dif_2
    }
    json_file = json.dumps(x, indent=4)
    return json_file


def listener():
    global old_avg1 
    global old_avg2 
    global new_data1
    global UDP_PORT
    global new_data2
    global state1
    global state2


    rospy.init_node('effects', anonymous=True)

    rospy.Subscriber("/box1/lidar/count", Float32, callback1)
    rospy.Subscriber("/box2/lidar/count", Float32, callback2)
    # spin() simply keeps python from exiting until this node is stopped
    pub = rospy.Publisher('a1', Float32, queue_size=10)
    pub2 = rospy.Publisher('a2', Float32, queue_size=10)
    pub3 = rospy.Publisher('d1', Float32, queue_size=10)
    pub4 = rospy.Publisher('d2', Float32, queue_size=10)
    rate = rospy.Rate(10)
    msgId = 0
    while not rospy.is_shutdown():
        send1 = 0
        send2 = 0

        #UDP_PORT +=1
        if new_data1 :
            send1 = 1
            new_data1 = 0
            no1=np.array(list(rolling_1))
            avg1 = no1.mean()
            dif1 = avg1-old_avg1
            old_avg1 = avg1
            pub.publish(Float32(avg1))
            pub3.publish(Float32(abs(dif1)))

        if new_data2:
            send2 = 1 
            new_data2 = 0
            no2=np.array(list(rolling_2))
            avg2 = no2.mean()
            dif2 = avg2-old_avg2
            old_avg2 = avg2
            pub2.publish(Float32(avg2))
            pub4.publish(Float32(abs(dif2)))
#        if send1:
#            th = 10
#            if ((dif1>=th) and (state1==0)):
#                send_pulse([255,0,0],avg1,dif1)
#                state1 = 1
#            if ((dif1<th) and (state1==1)):
#                send_pulse([255,0,0],avg1,dif1)
#                state1 = 0
#        if send2:
#            th = 10           
#            if ((dif2>=th) and (state2==0)):
#                send_pulse([0,0,255],avg2,dif2)
#                state2 = 1
#            if ((dif2<th) and (state2==1)):
#                send_pulse([0,0,255],avg2,dif2)
#                state2 = 0

        if send1:
            th = 10
            if (dif1>=th): 
                send_pulse([255,0,0],avg1,dif1)
        if send2:
            th = 10           
            if (dif2>=th): 
                send_pulse([0,0,255],avg2,dif2)



        else:
            print("nothing new")

        rate.sleep()

if __name__ == '__main__':
    listener()
