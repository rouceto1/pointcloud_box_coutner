#!/usr/bin/env python
import rospy
from std_msgs.msg import Float32

def callback1(data):
    print(data.data)
 
def callback2(data):
    print(data.data)
    
def listener():

    rospy.init_node('effects', anonymous=True)

    rospy.Subscriber("/box1/lidar/count", Float32, callback1)

    rospy.Subscriber("/box2/lidar/count", Float32, callback2)
    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()
