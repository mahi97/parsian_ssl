#include "ros/ros.h"
#include <dynamic_reconfigure/server.h>
#include <parsian_global_config/parsianGlobalConfig.h>


void callback(parsian_global_config::parsianGlobalConfig  &config, uint32_t level) {
   ROS_INFO("Reconfigure Request: %d", config.Team_Color);
}


int main(int argc, char **argv)
{

  ros::init(argc, argv, "parsian_global_config");
  ros::NodeHandle n;
  ros::Rate loop_rate(10);

    dynamic_reconfigure::Server<parsian_global_config::parsianGlobalConfig> server;
    dynamic_reconfigure::Server<parsian_global_config::parsianGlobalConfig>::CallbackType f;

    f = boost::bind(&callback, _1, _2);
    server.setCallback(f);

  while (ros::ok())
  {
      ros::spinOnce();

      loop_rate.sleep();

  }
  return 0;
}
