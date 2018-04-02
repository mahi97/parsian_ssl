#include <parsian_ai/ai_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_ai::AINodelet, nodelet::Nodelet);

using namespace parsian_ai;

void AINodelet::onInit() {

    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();
    ai.reset(new AI());
    ROS_INFO("init2");
    robTask = new ros::Publisher[_MAX_NUM_PLAYERS];
    for (int i = 0; i < _MAX_NUM_PLAYERS; ++i) {
        std::string topic(QString("/agent_%1/task").arg(i).toStdString());
        robTask[i] = nh.advertise<parsian_msgs::parsian_robot_task>(topic, 10);
    }
    drawer = new Drawer();

    worldModelSub = nh.subscribe("/world_model", 10, &AINodelet::worldModelCallBack, this);
    robotStatusSub = nh.subscribe("/robot_status", 100, &AINodelet::robotStatusCallBack, this);
    refereeSub = nh.subscribe("/referee", 100,  &AINodelet::refereeCallBack, this);
    teamConfSub = nh.subscribe("/team_config", 100, &AINodelet::teamConfCb, this);
    behaviorSub = nh.subscribe("/behavior", 100, &AINodelet::behaviorCb, this);
    mousePosSub = nh.subscribe("/mousePos", 100, &AINodelet::mousePosCb, this);
    forceRefereeSub = nh.subscribe("/force_referee", 100, &AINodelet::forceRefereeCallBack, this);
    robotfaultSub = nh.subscribe("/autofault", 100, &AINodelet::faultdetectionCallBack, this);

    drawPub = nh.advertise<parsian_msgs::parsian_draw>("/draws", 1000);
    timer_ = nh.createTimer(ros::Duration(0.1), boost::bind(&AINodelet::timerCb, this, _1));

    plan_client = nh.serviceClient<parsian_msgs::plan_service> ("/get_plans", true);

    behaviorPub = private_nh.advertise<parsian_msgs::parsian_ai_status>("/status", 10);

    ai->getSoccer()->getCoach()->setPlanClient(plan_client);
    ai->getSoccer()->getCoach()->setBehaviorPublisher(behaviorPub);
    //config server settings
    server.reset(new dynamic_reconfigure::Server<ai_config::aiConfig>(private_nh));
    dynamic_reconfigure::Server<ai_config::aiConfig>::CallbackType f;
    f = boost::bind(&AINodelet::ConfigServerCallBack, this, _1, _2);
    server->setCallback(f);

}
void AINodelet::mousePosCb(const parsian_msgs::vector2DConstPtr &_mousePos) {
    mousePos.assign(_mousePos.get()->x ,_mousePos.get()->y );
}
void AINodelet::teamConfCb(const parsian_msgs::parsian_team_configConstPtr& _conf) {
    teamConfig = *_conf;
}

void AINodelet::timerCb(const ros::TimerEvent& event){

    drawer->draws.texts.clear();
    if (drawer != nullptr)   drawPub.publish(drawer->draws);
    drawer->draws.circles.clear();
    drawer->draws.segments.clear();
    drawer->draws.polygons.clear();
    drawer->draws.rects.clear();
    drawer->draws.vectors.clear();
}

void AINodelet::worldModelCallBack(const parsian_msgs::parsian_world_modelConstPtr &_wm) {
    ai->updateWM(_wm);
    ROS_INFO("wm");
    ai->execute();
//
    for (int i = 0; i < wm->our.activeAgentsCount(); i++) {
        robTask[wm->our.activeAgentID(i)].publish(ai->getTask(wm->our.activeAgentID(i)));

    }
//
    parsian_msgs::plan_serviceResponse lastPlan = ai->getSoccer()->getCoach()->getLastPlan();
    ROS_INFO_STREAM("HSHM: last plan name: " << lastPlan.the_plan.planFile);

}

void AINodelet::refereeCallBack(const parsian_msgs::ssl_refree_wrapperConstPtr & _ref) {
    ai->updateReferee(_ref);
}

void AINodelet::faultdetectionCallBack(const parsian_msgs::parsian_robot_fault & _rs) {
    ai->updateRobotFaults(_rs);
}

void AINodelet::forceRefereeCallBack(const parsian_msgs::ssl_refree_commandConstPtr & _command){
    ai->forceUpdateReferee(_command);
}

void AINodelet::robotStatusCallBack(const parsian_msgs::parsian_robotConstPtr & _rs) {
    ai->updateRobotStatus(_rs);
}

void AINodelet::ConfigServerCallBack(const ai_config::aiConfig &config, uint32_t level) {
    ROS_INFO("MAHICALLING BACK");
    conf = config;
}

void AINodelet::behaviorCb(const parsian_msgs::parsian_behaviorConstPtr &_behavior) {
    ROS_INFO_STREAM("behavior " << _behavior->name << " received !");
    soccer->coach->updateBehavior(_behavior);
}
