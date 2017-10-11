//
// Created by parsian-ai on 9/19/17.
//

#include "parsian_world_model/wm/worldmodel.h"


CWorldModel::CWorldModel() {
}

CWorldModel::~CWorldModel() {

}

void CWorldModel::updateDetection(const parsian_msgs::ssl_vision_detectionConstPtr&) {

}

void CWorldModel::updateGeom(const parsian_msgs::ssl_vision_geometryConstPtr &) {

}


void CWorldModel::execute() {

}

parsian_msgs::parsian_world_model CWorldModel::getParsianWorldModel() const {
    return rosWM;
}
