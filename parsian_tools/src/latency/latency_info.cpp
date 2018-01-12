//
// Created by ali on 1/11/18.
//
#include <latency/latency_info.h>
using namespace parsian_tools;

//
//template <class T>
//LatencyInfo<T>::LatencyInfo(/*ros::NodeHandle& nh,const char* name , const char* topic*/) {
//    ROS_INFO_STREAM("shit");
//    sum = 0;
//    max = 0;
//    min = 0xffffffff;
//    this->name = name;
//    ROS_INFO_STREAM("shit");
//
//    subscriber = nh.subscribe(topic, 20, &LatencyInfo::call_back, this);
//    ROS_INFO_STREAM("shit");
//}
//template <class T>
//void LatencyInfo<T>::call_back(boost::shared_ptr<T>& msg){
//    int latency = (ros::Time::now() - msg->header.stamp).nsec;
//    sum +=latency;
//    this->latency_queue.push_back(latency);
//
//    this->updateetMinMax(latency);
//
//    if(this->latency_queue.length() > buffSize) {
//        sum -= latency_queue.dequeue();
//        ROS_INFO_STREAM(this->info());
//    }
//
//}
//
//template <class T>
//std::string LatencyInfo<T>::info() {
//    return name + QString("'s delay: min: %1  , mid: %2   ,  max: %3")
//            .arg(min)
//            .arg(sum / buffSize)
//            .arg(max).toStdString();
//}
//
//template <class T>
//void LatencyInfo<T>::updateetMinMax(int value){
//    if (value < min)
//        min = value;
//    if (value > max)
//        max = value;
//}