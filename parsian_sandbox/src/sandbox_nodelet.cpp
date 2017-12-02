#include <parsian_sandbox/sandbox_nodelet.h>

PLUGINLIB_EXPORT_CLASS(parsian_sandbox::SandBoxNodelet, nodelet::Nodelet);

using namespace parsian_sandbox;
void SandBoxNodelet::onInit(){
    ROS_INFO("oninit");

    debugger = new Debugger;
    drawer   = new Drawer;
    wm = new CWorldModel;

    ros::NodeHandle& nh = getNodeHandle();
    ros::NodeHandle& private_nh = getPrivateNodeHandle();
    sandBox.reset(new SandBoxMahi);




}
void SandBoxNodelet::modeCb(const std_msgs::UInt8ConstPtr &_id) {
    switch (_id->data) {
        case 0:
            sandBox.reset(new SandBoxMahi);
            sandBox->init();
        case 1:
        default:break;
    }
}

void SandBoxNodelet::timerCb(const ros::TimerEvent& event){
    sandBox->execute();
}

