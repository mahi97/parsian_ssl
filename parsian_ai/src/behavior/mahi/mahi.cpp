#include <behavior/mahi/mahi.h>

BehaviorMahi::BehaviorMahi()
: Behavior("Mahi", "describe me") {

}

BehaviorMahi::~BehaviorMahi() {

}

int BehaviorMahi::execute() {
    return 0;
}

double BehaviorMahi::eval(parsian_msgs::parsian_behaviorPtr _behav) {
    _behav = nullptr;
    return 0.0;
}
