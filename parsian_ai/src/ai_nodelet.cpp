#include <parsian_ai/ai_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_ai::AINodelet, nodelet::Nodelet);

using namespace parsian_ai;

  void AINodelet::onInit() {

      ros::NodeHandle &nh = getNodeHandle();
      ros::NodeHandle &private_nh = getPrivateNodeHandle();
      worldModelSub = nh.subscribe("/world_model", 1000, &AI::updateWM, &ai);
      robotStatusSub = nh.subscribe("/robot_status", 1000, &AI::updateRobotStatus, &ai);
      refereeSub = nh.subscribe("/referee", 1000,  &AI::updateReferee, &ai);

      drawPub = private_nh.advertise<parsian_msgs::parsian_draw>("/draws", 1000);
      debugPub = private_nh.advertise<parsian_msgs::parsian_debugs>("/debugs", 1000);
      timer_ = nh.createTimer(ros::Duration(.062), boost::bind(&AINodelet::timerCb, this, _1));

      //config server settings
      server.reset(new dynamic_reconfigure::Server<ai_config::aiConfig>);
      dynamic_reconfigure::Server<ai_config::aiConfig>::CallbackType f;
      f = boost::bind(&AINodelet::ConfigServerCallBack,this, _1, _2);
      server->setCallback(f);
//      ros::Publisher  statusPub  = n.advertise("/ai_status",1000);
//      ros::Publisher  gpPub      = n.advertise<parsian_msgs::gotoPoint>("/gotoPoint", 1000);
//      ros::Publisher  gpaPub     = n.advertise<parsian_msgs::gotoPointAvoid>("/gotoPointAvoid", 1000);
//      ros::Publisher  kickPub    = n.advertise<parsian_msgs::kick>("/kick", 1000);
//      ros::Publisher  recvPub    = n.advertise<parsian_msgs::receivePass>("/receivePass", 1000);



      drawer = new Drawer();
      debugger = new Debugger();


  }

  void AINodelet::timerCb(const ros::TimerEvent& event){

      ai.execute();
      drawPub.publish(drawer->draws);
      debugPub.publish(debugger->debugs);
//        ai.publish({&gpaPub, &gpaPub, &kickPub, &recvPub});



  }

  void AINodelet::ConfigServerCallBack(const ai_config::aiConfig &config, uint32_t level)
  {
    
  }
