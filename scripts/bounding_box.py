#!/usr/bin/env python
import rospy
from jsk_recognition_msgs.msg import BoundingBoxArray, BoundingBox

from tf.transformations import *
rospy.init_node("bbox")
pub = rospy.Publisher("bbox", BoundingBoxArray, queue_size=2)
boxes = rospy.get_param("~frames")
print(boxes)
r = rospy.Rate(24)
counter = 0
size = 3.0
def make_box(i,frame, size):
    box_a = BoundingBox()
    box_a.label = i
    now = rospy.Time.now()
    box_a.header.stamp = now
    box_a.header.frame_id = frame
    ##q = quaternion_about_axis((counter % 100) * math.pi * 2 / 100.0, [0, 0, 1]) this rotates the fucking box
    #box_a.pose.orientation.x = q[0]
    #box_a.pose.orientation.y = q[1]
    #box_a.pose.orientation.z = q[2]
    #box_a.pose.orientation.w = q[3]
    box_a.pose.position.x = box_a.pose.position.y = box_a.pose.position.z = size/2.0
    box_a.dimensions.x  = box_a.dimensions.y = box_a.dimensions.z = size
    #box_a.value = (counter % 100) / 100.0
    return box_a

while not rospy.is_shutdown():
    box_arr = BoundingBoxArray()
    box_arr.header.frame_id = "pupek"
    now = rospy.Time.now()
    box_arr.header.stamp = now
    for index, frames in enumerate (boxes):
        box_arr.boxes.append(make_box(index,frames, size))
    pub.publish(box_arr)
    r.sleep()
    counter = counter + 1
