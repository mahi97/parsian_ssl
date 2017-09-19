//
// Created by parsian-ai on 9/19/17.
//

#include "parsian_world_model/wm/worldmodel.h"


CWorldModel::CWorldModel() {
    qDebug() << "Worldmodel initilizer";
}

CWorldModel::~CWorldModel() {

}

void CWorldModel::updateDetection(const parsian_msgs::ssl_vision_detection &) {


}

void CWorldModel::updateGeom(const parsian_msgs::ssl_vision_geometry &) {

}


void CWorldModel::execute(parsian_msgs::parsian_world_model &rosWM) {

}
