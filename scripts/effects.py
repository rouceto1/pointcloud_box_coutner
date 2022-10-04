#!/usr/bin/env python
import rospy
from std_msgs.msg import Int32

def callback(data):
    print(data.data)
    
def listener():

    rospy.init_node('effects', anonymous=True)

    rospy.Subscriber("counts", Int32, callback)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()
