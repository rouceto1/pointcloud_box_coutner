#include <ros/ros.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <boost/foreach.hpp>
#include <pcl/common/transforms.h>
#include <std_msgs/Float32.h>
#include <tf/transform_listener.h>
#include <pcl_ros/transforms.h>
#include <pcl/filters/passthrough.h>
#include <atomic>
typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;

float box_size;


ros::Publisher pub;
ros::Publisher pub2;
ros::Publisher pub3;
ros::Publisher pub4;

tf::TransformListener *tf_listener; 
std::string box_frame;
std::atomic<float> count = 0;

void callback_points(const PointCloud::ConstPtr& msg)//const PointCloud::ConstPtr& msg)
{
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>);
  pcl::IndicesPtr indices_x (new pcl::Indices ());
  pcl::IndicesPtr indices_xz (new pcl::Indices ());

  const pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_transformed (new pcl::PointCloud<pcl::PointXYZ>);
  const std::string test1 = box_frame;
  pcl_ros::transformPointCloud(test1,
                           *msg,
                           *cloud_transformed,
                           *tf_listener);
  
  pcl::PassThrough<pcl::PointXYZ> ptfilter (false); // Initializing with true will allow us to extract the removed indices
  ptfilter.setInputCloud (cloud_transformed);

  ptfilter.setFilterFieldName ("x");
  ptfilter.setFilterLimits (0, box_size);
  ptfilter.filter (*indices_x);

  ptfilter.setIndices (indices_x);
  ptfilter.setFilterFieldName ("z");
  ptfilter.setFilterLimits (0, box_size);
  ptfilter.setNegative (false);
  ptfilter.filter (*indices_xz);

  ptfilter.setIndices (indices_xz);
  ptfilter.setFilterFieldName ("y");
  ptfilter.setFilterLimits (0,box_size);
  ptfilter.setNegative (false);
  ptfilter.filter (*cloud_filtered);

  //cloud_filtered->frame_id = "shit"
  float local_count = 0;
  BOOST_FOREACH (const pcl::PointXYZ& pt, cloud_filtered->points){
    float coef = 1 + (pt.z);
    local_count = local_count+coef;
  }

   pub.publish (cloud_filtered);
   count = count + local_count;
}



int main(int argc, char** argv)
{
  ros::init(argc, argv, "lidar_exctract");
  ros::NodeHandle nh("~");
  std::string count_topic_name, cloud_topic_name;
  nh.param<std::string>("cloud_out", cloud_topic_name, "points");
  nh.param<std::string>("cont_out", count_topic_name, "count");
  nh.param<std::string>("box_frame", box_frame, "box");
  std::string lidar_1,lidar_2, lidar_3, lidar_4;
  nh.param<std::string>("lidar_topic_1", lidar_1, "ouster/points");
  nh.param<std::string>("lidar_topic_2", lidar_2, "os2");
  nh.param<std::string>("lidar_topic_3", lidar_3, "os3");
  nh.param<std::string>("lidar_topic_4", lidar_4, "os4");
  nh.param<float>("box_size", box_size, 3.0);

  std::cout << box_frame << std::endl;
  ros::Subscriber sub1 = nh.subscribe(lidar_1, 1, callback_points);
  ros::Subscriber sub2 = nh.subscribe(lidar_2, 1, callback_points);
  ros::Subscriber sub3 = nh.subscribe(lidar_3, 1, callback_points);
  ros::Subscriber sub4 = nh.subscribe(lidar_4, 1, callback_points);
  pub = nh.advertise<sensor_msgs::PointCloud2> (cloud_topic_name, 10000);
  pub2 = nh.advertise<std_msgs::Float32> (count_topic_name, 1);
  tf_listener    = new tf::TransformListener();
  ros::Rate rate(20); // ROS Rate at 5Hz
  while (ros::ok()) {
    std::cout << count << std::endl;
    std_msgs::Float32 outCount;
    outCount.data = count;
    if (count != 0 )
      pub2.publish(outCount);
    count = 0;
    ros::spinOnce ();
    rate.sleep();
    }
}
