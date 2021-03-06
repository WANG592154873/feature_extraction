/*
 * Header file for feature extraction node
 *
 * Dan Pierce
 * 2017-06-01
 */
#ifndef _FEATURE_EXTRACTION_NODE_H_
#define _FEATURE_EXTRACTION_NODE_H_
// ROS
#include <ros/ros.h>
#include <pcl_ros/point_cloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/Imu.h>
#include <pcl_conversions/pcl_conversions.h>
 #include "tf/transform_datatypes.h"
// STL
#include <iostream>
// PCL
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/filters/conditional_removal.h>
#include <pcl/common/transforms.h>
#include <pcl/filters/passthrough.h>

#include <pcl/segmentation/extract_clusters.h>

#include <pcl/common/time.h>

#include <pcl/features/3dsc.h>

namespace pcl
{

struct PointDescriptor
{
  PCL_ADD_POINT4D;                  // preferred way of adding a XYZ+padding
  float intensity;
  float descriptor[1980];
  float rf[9];
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW   // make sure our new allocators are aligned
} EIGEN_ALIGN16;                    // enforce SSE padding for correct memory alignment

}

POINT_CLOUD_REGISTER_POINT_STRUCT (PointDescriptor,           // here we assume a XYZ + "test" (as fields)
                                   (float, x, x)
                                   (float, y, y)
                                   (float, z, z)
                                   (float, intensity, intensity)
                                   (float[1980], descriptor, shape_context)
                                   (float[9], rf, rf)
)

/*! /brief Primary class for the feature extraction node class
*
*/
class FeatureExtractionNode
{

  public:
    
    typedef std::vector<pcl::PointIndices> IndicesClusters;
    typedef boost::shared_ptr<IndicesClusters> IndicesClustersPtr;

    typedef pcl::PointXYZI Point;
    typedef pcl::PointCloud<Point> PointCloud;

    typedef pcl::Normal Normal;
    typedef pcl::PointCloud<Normal> NormalCloud;

    typedef pcl::PointNormal PointNormal;
    typedef pcl::PointCloud<PointNormal> PointNormalCloud;

    typedef pcl::ShapeContext1980 Descriptor;
    typedef pcl::PointCloud<Descriptor> DescriptorCloud;

    typedef pcl::PointDescriptor PointDescriptor;
    typedef pcl::PointCloud<PointDescriptor> PointDescriptorCloud;

    FeatureExtractionNode();
    ~FeatureExtractionNode();

    void filterCloud (PointCloud::Ptr cloud);

  private:

    void printRosParameters (void);

    void getElevationAngles (PointCloud::Ptr cloud);

    void rotateCloud (PointCloud::Ptr cloud);

    void estimateKeypoints (const PointCloud::Ptr cloud, PointCloud::Ptr keypoints, PointCloud::Ptr keypoint_cloud);

    void getCylinderSegments (const PointCloud::Ptr cloud, PointCloud::Ptr keypoints, PointCloud::Ptr keypoint_cloud);

    void estimateDescriptors (const PointCloud::Ptr cloud, const PointCloud::Ptr keypoints, DescriptorCloud::Ptr descriptors);

    void cloudCallback(const sensor_msgs::PointCloud2ConstPtr& msg);
    void imuCallback(const sensor_msgs::ImuConstPtr& msg);
    
    // --- Publisher
    ros::Publisher feature_pub;         // Feature publisher
    ros::Publisher kp_pub;              // Keypoint publisher
    ros::Publisher kpc_pub;              // Keypoint cloud publisher
    ros::Publisher cloud_pub;            // Filtered point cloud publisher

    // --- Subscribers
    ros::Subscriber pc_sub;             // Point cloud subscriber
    ros::Subscriber imu_sub;            // IMU subscriber (for roll/pitch)
    
    // --- Class variables
    // Pass through filter
    double zMin,zMax,xMin,xMax,yMin,yMax; // Bounds of point cloud pass through filter
    double roll,pitch;                  // Roll/pitch estimate for rotating point cloud to local-level frame

    bool levelCloud;
    // Detector
    double clusterTolerance;            // Tolerance for point cloud segmentation (as a measure in L2 Euclidean space)
    int clusterMinCount;                 // Minimum number of points in a cluster
    int clusterMaxCount;                // Maximum number of points in a cluster
    double clusterRadiusThreshold;      
    int detectionChannelThreshold;
    // Descriptor
    double descriptorRadius;            // search radius used to build descriptor
    bool descriptorEstimation;          // whether or not to estimate and publish descriptor

    // Other
    bool init;
    
};

#endif