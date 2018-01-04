#include <planner/planner_nodelet.h>
#include <cstring>

PLUGINLIB_EXPORT_CLASS(parsian_agent::PlannerNodelet, nodelet::Nodelet);

using namespace parsian_agent;

void PlannerNodelet::onInit(){
    ROS_INFO("%s onInits", getName().c_str());

    nh = getNodeHandle();
    private_nh = getPrivateNodeHandle();

    debugger = new Debugger;
    drawer   = new Drawer;
    wm = new CWorldModel;

    QString name(getName().c_str());

    planner.reset(new CPlanner(name.split('_').at(1).toInt()));

    common_config_sub = nh.subscribe("/commonconfig/parameter_updates", 1000, &PlannerNodelet::commonConfigCb, this);
    world_model_sub   = nh.subscribe("world_model", 10000, &PlannerNodelet::wmCb, this);
    planner_sub       = nh.subscribe(QString("agent_%1/plan").arg(planner->getID()).toStdString(), 5, &PlannerNodelet::plannerCb, this);

    debug_pub = nh.advertise<parsian_msgs::parsian_debugs>("debugs", 1000);
    draw_pub  = nh.advertise<parsian_msgs::parsian_draw>("draws", 1000);
    planner->path_pub = private_nh.advertise<parsian_msgs::parsian_path>("path", 5);


    timer_ = nh.createTimer(ros::Duration(0.01), &PlannerNodelet::timerCb, this);
}

void PlannerNodelet::commonConfigCb(const dynamic_reconfigure::ConfigConstPtr &_cnf) {
    auto * a = const_cast<dynamic_reconfigure::Config*>(_cnf.get());
    conf->__fromMessage__(*a);
}

void PlannerNodelet::wmCb(const parsian_msgs::parsian_world_modelConstPtr& _wm) {
    wm->update(_wm);
}

void PlannerNodelet::timerCb(const ros::TimerEvent& event){
    if (debugger != nullptr) debug_pub.publish(debugger->debugs);
    if (drawer   != nullptr) {
        draw_pub.publish(drawer->draws);
        cleanDraws();
    }
}

void PlannerNodelet::cleanDraws() const {
    drawer->draws.texts.clear();
    drawer->draws.circles.clear();
    drawer->draws.segments.clear();
    drawer->draws.vectors.clear();
    drawer->draws.rects.clear();
}

void PlannerNodelet::plannerCb(const parsian_msgs::parsian_get_planConstPtr & _plan) {
    QList<int> ourRL;
    QList<int> oppRL;
    for (const auto& id : _plan->ourRelaxList) ourRL.append(id);
    for (const auto& id : _plan->oppRelaxList) oppRL.append(id);
    planner->initPathPlanner(Vector2D(_plan->goal), ourRL, oppRL, _plan->avoidPenaltyArea, _plan->avoidCenterCircle, _plan->ballObstacleRadius);
}
