#include <parsian_ai/ai_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_ai::AINodelet, nodelet::Nodelet);

using namespace parsian_ai;

  void AINodelet::onInit() {

      ros::NodeHandle &nh = getNodeHandle();
      ros::NodeHandle &private_nh = getPrivateNodeHandle();
      worldModelSub = nh.subscribe("/world_model", 1000, &AINodelet::worldModelCallBack, this);
      robotStatusSub = nh.subscribe("/robot_status", 1000, &AINodelet::robotStatusCallBack, this);
      refereeSub = nh.subscribe("/referee", 1000,  &AINodelet::refereeCallBack, this);

      drawPub = private_nh.advertise<parsian_msgs::parsian_draw>("/draws", 1000);
      debugPub = private_nh.advertise<parsian_msgs::parsian_debugs>("/debugs", 1000);
      timer_ = nh.createTimer(ros::Duration(.062), boost::bind(&AINodelet::timerCb, this, _1));

      //config server settings
      server.reset(new dynamic_reconfigure::Server<ai_config::aiConfig>);
      dynamic_reconfigure::Server<ai_config::aiConfig>::CallbackType f;
      f = boost::bind(&AINodelet::ConfigServerCallBack,this, _1, _2);
      server->setCallback(f);

      for (int i = 0; i < _MAX_NUM_PLAYERS; ++i) {
          std::string topic("robot_tsk_"+std::to_string(i));
          robTask[i] =
                  nh.advertise<parsian_msgs::parsian_robot_task>(topic, 1000);
      }

    drawer = new Drawer();
    debugger = new Debugger();

}

void AINodelet::timerCb(const ros::TimerEvent& event) {

//      ai.execute();
    drawPub.publish(drawer->draws);
    debugPub.publish(debugger->debugs);
//        ai.publish({&gpaPub, &gpaPub, &kickPub, &recvPub});
}


void AINodelet::worldModelCallBack(const parsian_msgs::parsian_world_modelConstPtr &_wm) {
    ai.updateWM(_wm);
    ai.execute();

//    for(int i=0; i<wm->our.activeAgentsCount(); i++) {
    robTask[wm->our.activeAgentID(0)].publish(ai.getTask(wm->our.activeAgentID(0)));
//    }

}
void AINodelet::refereeCallBack(const parsian_msgs::ssl_refree_wrapperConstPtr & _ref) {
    ai.updateReferee(_ref);
}

void AINodelet::robotStatusCallBack(const parsian_msgs::parsian_robotConstPtr & _rs) {
    ai.updateRobotStatus(_rs);
}



void AINodelet::ConfigServerCallBack(const ai_config::aiConfig &config, uint32_t level)
{
    ROS_INFO_STREAM("callback called! with" << config.test_param);
}
