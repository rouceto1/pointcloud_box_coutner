#include <ros/ros.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <boost/foreach.hpp>
#include <pcl/common/transforms.h>
#include <std_msgs/Float32.h>
#include <tf/transform_listener.h>
#include <pcl_ros/transforms.h>
#include <pcl/filters/passthrough.h>
typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;

float box_size = 3;
float max_z = 10.0;
float min_z = -10.0;


ros::Publisher pub;
ros::Publisher pub2;
ros::Publisher pub3;
ros::Publisher pub4;

tf::TransformListener *tf_listener; 
#include <atomic>

std::atomic<float> count = 0;

void callback_points(const PointCloud::ConstPtr& msg)//const PointCloud::ConstPtr& msg)
{
  //pcl::PCLPointCloud2 pcl_pc2;
  //pcl_conversions::toPCL(*msg,pcl_pc2);
  //pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_in(new pcl::PointCloud<pcl::PointXYZ>);
  //pcl::fromPCLPointCloud2(pcl_pc2,*cloud_in);
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>);
  pcl::IndicesPtr indices_x (new pcl::Indices ());
  pcl::IndicesPtr indices_xz (new pcl::Indices ());
  //pcl::PointIndices::Ptr indices_x (new pcl::PointIndices ());
  //pcl::PointIndices::Ptr indices_xz (new pcl::PointIndices ());
  // The indices_x array indexes all points of cloud_in that have x between 0.0 and 1000.0
  // tf_listener->waitForTransform("pupek", "test", "test1", ros::Duration(5.0));


  const pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_transformed (new pcl::PointCloud<pcl::PointXYZ>);
  const std::string test1 = "cloud";
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
  ros::NodeHandle nh;
  ros::Subscriber sub = nh.subscribe("ouster/points", 1, callback_points);
  ros::Subscriber sub2 = nh.subscribe("ouster/points", 1, callback_points);
  pub = nh.advertise<sensor_msgs::PointCloud2> ("points2", 10000);
  pub2 = nh.advertise<std_msgs::Float32> ("counts", 1);
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
