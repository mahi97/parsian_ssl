//
// Created by parsian-ai on 10/3/17.
//

#ifndef PARSIAN_PROTOBUF_WRAPPER_PARSIAN_VISION_NODELET_H_H
#define PARSIAN_PROTOBUF_WRAPPER_PARSIAN_VISION_NODELET_H_H


#include <csignal>
#include <chrono>

#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <nodelet/nodelet.h>
#include <dynamic_reconfigure/server.h>
#include <parsian_protobuf_wrapper/ssl-vision/convert/convert_detection.h>
#include <parsian_protobuf_wrapper/ssl-vision/convert/convert_geometry.h>

#include <parsian_protobuf_wrapper/messages_robocup_ssl_wrapper.pb.h>
#include <parsian_protobuf_wrapper/common/net/robocup_ssl_client.h>

#include <parsian_protobuf_wrapper/visionConfig.h>


namespace parsian_protobuf_wrapper {

    class VisionNodelet : public nodelet::Nodelet {


    private:
        virtual void onInit();


        // Timer CallBack (to publish)
        void timerCb(const ros::TimerEvent& event);
        void reconnect();
        void configCb(const protobuf_wrapper_config::visionConfig &config , uint32_t level);

        ros::Publisher ssl_geometry_pub;
        ros::Publisher ssl_detection_pub;

        ros::Timer timer;


        bool isOurColorYellow = false;
        RoboCupSSLClient *vision;
        protobuf_wrapper_config::visionConfig visionConfig;
        SSL_WrapperPacket vision_packet;
        std::string teamColor;


    };
}

#endif //PARSIAN_PROTOBUF_WRAPPER_PARSIAN_VISION_NODELET_H_H
